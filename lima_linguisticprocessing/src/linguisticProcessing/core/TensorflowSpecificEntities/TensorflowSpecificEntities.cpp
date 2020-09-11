// includes from project
#include "TensorflowSpecificEntities.h"
#include "nerUtils.h"

#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "common/tools/FileUtils.h"
#include "common/time/traceUtils.h"
#include "common/time/timeUtilsController.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/core/SpecificEntities/SpecificEntitiesMicros.h"

// includes system
#include <QString>
#include <QDir>
#include <QRegularExpression>
#include <QStringList>
#include <memory>
#include <queue>
#include <map>


// declaration of using namespace
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace tensorflow;

#define EIGEN_DONT_VECTORIZE

namespace Lima {
namespace LinguisticProcessing {
namespace TensorflowSpecificEntities {

SimpleFactory<MediaProcessUnit,TensorflowSpecificEntities> tensorflowSpecificEntitiesFactory(TENSORFLOWSPECIFICENTITIES_CLASSID);

class TensorflowSpecificEntitiesPrivate
{
  friend class TensorflowSpecificEntities;
  ~TensorflowSpecificEntitiesPrivate();
  TensorflowSpecificEntitiesPrivate():m_microAccessor(nullptr),m_sp(nullptr),m_session(nullptr){}
  TensorflowSpecificEntitiesPrivate(MediaId language);
  std::string m_graph;
  std::map<LinguisticGraphVertex,QString> m_matchingVertextoEntity;
  std::vector<LinguisticGraphVertex> m_visitedVertex;
  MediaId m_language;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
  FsaStringsPool* m_sp;
  std::map<QString,int> m_vocabWords;
  std::map<QChar,int> m_vocabChars;
  std::map<unsigned int,QString> m_vocabTags;
  Session* m_session;
  std::shared_ptr<Status> m_status;
  GraphDef m_graphDef;
  int m_batchSizeMax;
};

TensorflowSpecificEntitiesPrivate::TensorflowSpecificEntitiesPrivate(
    MediaId language) :
  m_language(language),
  m_session(nullptr)
{
  m_microAccessor = &(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));
  m_sp = &(Common::MediaticData::MediaticData::changeable().stringsPool(language));
}

TensorflowSpecificEntitiesPrivate::~TensorflowSpecificEntitiesPrivate()
{
  // 8. Free any resources used by the session
  m_status.reset(new Status());
  *m_status = m_session->Close();

  if (!m_status->ok())
  {
    TFSELOGINIT;
    LERROR << m_status->ToString();
  }

  delete m_session;
}

TensorflowSpecificEntities::TensorflowSpecificEntities() :
    m_d(new TensorflowSpecificEntitiesPrivate())
{
}

TensorflowSpecificEntities::~TensorflowSpecificEntities()
{
  delete m_d;
}

void TensorflowSpecificEntities::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  TFSELOGINIT;
  m_d->m_language = manager->getInitializationParameters().media;
  m_d->m_microAccessor = &(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_d->m_language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));
  m_d->m_sp = &(Common::MediaticData::MediaticData::changeable().stringsPool(m_d->m_language));

  // Load parameters required for running the graph
  // std::string graph;
  try
  {
    m_d->m_graph = Common::Misc::findFileInPaths(
      QString::fromStdString(Common::MediaticData::MediaticData::single().getResourcesPath()),
      QString::fromStdString(unitConfiguration.getParamsValueAtKey("graphOutputFile"))).toStdString();
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'graphOutputFile' in TensorflowSpecificEntities group for m_d->m_language " << (int) m_d->m_language;
    throw InvalidConfiguration();
  }

  QString fileChars,fileWords,fileTags;
  try
  {
    fileChars = Common::Misc::findFileInPaths(
      QString::fromStdString(Common::MediaticData::MediaticData::single().getResourcesPath()),
      QString::fromStdString(unitConfiguration.getParamsValueAtKey("charValuesFile")));
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'charValuesFile' in TensorflowSpecificEntities group for m_d->m_language "
            << (int) m_d->m_language;
    throw InvalidConfiguration();
  }

  try
  {
      fileWords = Common::Misc::findFileInPaths(
        QString::fromStdString(Common::MediaticData::MediaticData::single().getResourcesPath()),
        QString::fromStdString(unitConfiguration.getParamsValueAtKey("wordValuesFile")));
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'wordValuesFile' in TensorflowSpecificEntities group for m_d->m_language " << (int) m_d->m_language;
    throw InvalidConfiguration();
  }

  try
  {
      fileTags = Common::Misc::findFileInPaths(
        QString::fromStdString(Common::MediaticData::MediaticData::single().getResourcesPath()),
        QString::fromStdString(unitConfiguration.getParamsValueAtKey("tagValuesFile")));
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'tagValuesFile' in TensorflowSpecificEntities group for m_d->m_language " << (int) m_d->m_language;
    throw InvalidConfiguration();
  }

  //Minibatching (group of max 20 sentences of different size) is used in order to amortize the cost of loading the network weights from CPU/GPU memory across many inputs.
    //and to take advantage from parallelism.
//     std::string::size_type sz;
  try
  {
    m_d->m_batchSizeMax = QString::fromStdString(
      unitConfiguration.getParamsValueAtKey("batchSizeMax")).toInt();
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'batchSizeMax' in TensorflowSpecificEntities group for m_d->m_language "
            << (int) m_d->m_language;
    throw InvalidConfiguration();
  }

  try
  {
    m_d->m_vocabWords = loadFileWords(fileWords);
    if(m_d->m_vocabWords.empty())
    {
      throw LimaException();
    }
    m_d->m_vocabChars = loadFileChars(fileChars);
    if(m_d->m_vocabChars.empty())
    {
      throw LimaException();
    }
    m_d->m_vocabTags = loadFileTags(fileTags);
    if(m_d->m_vocabTags.empty())
    {
      throw LimaException();
    }
  }
  catch(const BadFileException& e)
  {
    TFSELOGINIT;
    LERROR << e.what();
    throw LimaException(e.what());
  }

  // Initialize a tensorflow session
  m_d->m_status.reset(new Status(NewSession(SessionOptions(), &m_d->m_session)));
  if (!m_d->m_status->ok())
  {
    TFSELOGINIT;
    LERROR << m_d->m_status->ToString();
    throw LimaException(m_d->m_status->ToString());
  }

  // Read in the protobuf graph we have exported
  *m_d->m_status = ReadBinaryProto(Env::Default(), m_d->m_graph,
                                   &m_d->m_graphDef);
  if (!m_d->m_status->ok())
  {
    TFSELOGINIT;
    LERROR << m_d->m_status->ToString();
    throw LimaException(m_d->m_status->ToString());
  }

  // Add the graph to the session
  *m_d->m_status = m_d->m_session->Create(m_d->m_graphDef);
  if (!m_d->m_status->ok())
  {
    TFSELOGINIT;
    LERROR << m_d->m_status->ToString();
    throw LimaException(m_d->m_status->ToString());
  }
}

LimaStatusCode TensorflowSpecificEntities::process(
  AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("TensorflowSpecificEntities");
  // Start named-entity recognition here !
  TFSELOGINIT;
  LINFO << "start TensorflowSpecificEntities";

  // Get sentence bounds
  auto sb = static_cast<SegmentationData*>(analysis.getData("SentenceBoundariesForSE"));
  if(sb == nullptr)
  {
    TFSELOGINIT;
    LERROR << "TensorflowSpecificEntities::process: no sentence bounds defined ! abort";
    return MISSING_DATA;
  }
  if (sb->getGraphId() != "AnalysisGraph")
  {
    TFSELOGINIT;
    LERROR << "TensorflowSpecificEntities::process: SentenceBounds are computed on graph '"
            << sb->getGraphId() << "'";
    LERROR << "can't compute TensorflowSpecificEntities on graph AnalysisGraph !";
    return INVALID_CONFIGURATION;
  }
  //Check if there are setences.
  if(sb->getSegments().size() == 0)
  {
    TFSELOGINIT;
    LERROR << "No sentences to analyze.";
    return UNKNOWN_ERROR;
  }
  auto boundItr = (sb->getSegments()).cbegin();

  auto tokenList = static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  auto g = tokenList->getGraph();
  auto tokenMap = get(vertex_token,*g);

  //save LinguisticGraphVertex visited following sentences' order

  /* The end of a segment is the beginning of the next one.
  So the beginning has not to be considered in order to build exactly the original sentences
  Performs could be degraded since viterbi algorithm is applied on sentences at the end of the net.
  */
  auto endPrecedentSentence = boundItr->getFirstVertex();

  while(boundItr != (sb->getSegments()).cend())
  {
    std::vector<std::vector<int>> wordIds(m_d->m_batchSizeMax); //list of identifiers of each word in each sentence from the batch
    std::vector<std::vector<std::vector<int>>> charIds(m_d->m_batchSizeMax);//list of identifiers of each character from each word in each sentence from the batch
    int batchSize = 0;
    m_d->m_visitedVertex.erase(m_d->m_visitedVertex.begin(),
                               m_d->m_visitedVertex.end());
    m_d->m_matchingVertextoEntity.erase(m_d->m_matchingVertextoEntity.begin(),
                                        m_d->m_matchingVertextoEntity.end());

    while(batchSize < m_d->m_batchSizeMax
      && boundItr != (sb->getSegments()).cend())
    {
      m_d->m_visitedVertex.reserve(boundItr->getLength()+m_d->m_visitedVertex.size());
      QStringList wordsRaw; //batch of sentences


      /*a segment is characterized by the first and the last vertex. Thanks to the boost library,
      it is easy to get all the vertex from the same sentence by following edges until endSentence vertex is reached*/

      auto beginSentence = boundItr->getFirstVertex();
      auto endSentence = boundItr->getLastVertex();
      std::queue<LinguisticGraphVertex> toVisit;
      toVisit.push(beginSentence);

      while(!toVisit.empty())
      {
        auto currentVertex = toVisit.front();
        toVisit.pop();
        if(currentVertex != tokenList->lastVertex())
        {
          if(currentVertex != tokenList->firstVertex()
            && currentVertex != endPrecedentSentence)
          {
            auto currentToken = tokenMap[currentVertex];
            if(currentToken != nullptr && !currentToken->stringForm().isEmpty())
            {
              m_d->m_visitedVertex.push_back(currentVertex);
              wordsRaw << currentToken->stringForm();
            }
          }

          if(currentVertex != endSentence)
          {
            LinguisticGraphOutEdgeIt outEdge, outEdge_end;
            boost::tie(outEdge,outEdge_end) = boost::out_edges(currentVertex, *g);
            toVisit.push(boost::target(*outEdge, *g));
            ++outEdge;
            if(outEdge != outEdge_end)
            {
              TFSELOGINIT;
              LERROR << "TensorflowSpecificEntities::process: at this step, it is supposed that each vertex has only one neighbourhood. abort";
              return OUT_OF_RANGE_ERROR;
            }
          }
          else
          {
            endPrecedentSentence = currentVertex;
          }
        }
      }

#ifdef DEBUG_LP
      TFSELOGINIT;
      std::ostringstream oss;
      for(auto itSequenceBegin = wordsRaw.cbegin();
          itSequenceBegin != wordsRaw.cend(); ++itSequenceBegin)
      {
        oss << (*itSequenceBegin).toStdString() << " ";
      }
      LDEBUG << "Sentence evaluated:" << oss.str();
#endif

      if(wordsRaw.size() == 0)
      {
        return UNKNOWN_ERROR;
      }

      //1. Transform words into ids and split all the characters and identify them
      std::vector< std::pair<std::vector<int>, int> > textConverted;
      textConverted.reserve(wordsRaw.size());
      for(auto it = wordsRaw.cbegin(); it != wordsRaw.cend(); ++it)
      {
        try
        {
          textConverted.push_back(getProcessingWord(*it,
                                                    m_d->m_vocabWords,
                                                    m_d->m_vocabChars,
                                                    true,
                                                    true));
          if(!std::get<1>(textConverted.back()))
          {
            return MISSING_DATA;
          }
        }
        catch(const UnknownWordClassException& e)
        {
          TFSELOGINIT;
          LERROR << e.what();
          return UNKNOWN_ERROR;
        }
      }

      //2. Gather ids of words and ids of sequences of characters according to the order of words

      wordIds[batchSize].resize(wordsRaw.size());
      charIds[batchSize].resize(wordsRaw.size());
      for(auto i=0; i < textConverted.size(); ++i)
      {
        charIds[batchSize][i].resize(textConverted[i].first.size());
        charIds[batchSize][i] = textConverted[i].first;
        wordIds[batchSize][i] = textConverted[i].second;
      }
      ++batchSize;
      ++boundItr;
    }

    //3.Resize data if current batch size is fewer than m_d->m_batchSizeMax
    if(batchSize < m_d->m_batchSizeMax)
    {
      wordIds.resize(batchSize);
      charIds.resize(batchSize);
    }

    //4. Check if characters have been identified, if all batch's characters
    // have not been identified, i.e lists are empty, model will not be run
    // over this batch
    auto itBatchChars = charIds.cbegin();
    auto itSqChars = itBatchChars->cbegin();
    while(itBatchChars != charIds.cend())
    {
      while(itSqChars != itBatchChars->cend() && itSqChars->size() == 0)
      {
        itSqChars++;
      }
      if(itSqChars != itBatchChars->cend())
      {
        break;
      }
      itBatchChars++;
      itSqChars = itBatchChars->cbegin();
    }

    if(itBatchChars != charIds.cend() || itSqChars != itBatchChars->cend())
    {
      //5. Predict entity
      std::vector<Eigen::MatrixXi> result(batchSize);
      if(predictBatch(m_d->m_status, m_d->m_session,
          batchSize, charIds, wordIds, result) == NERStatusCode::MISSING_DATA)
      {
        return MISSING_DATA;
      }
      auto itVisited = m_d->m_visitedVertex.cbegin();

      //6. Store results in a map, mapping LinguisticGraphVertex to its entity, following sentence's order
      for( auto i = 0; i <result.size(); ++i)
      {
        for(auto j = 0; j<result[i].size(); ++j)
        {
          m_d->m_matchingVertextoEntity[*itVisited] = m_d->m_vocabTags[result[i](j)];
          ++itVisited;
        }
      }

#ifdef DEBUG_LP
      TFSELOGINIT;
      std::ostringstream oss;
      for(auto itBegin = m_d->m_visitedVertex.cbegin();
          itBegin != m_d->m_visitedVertex.cend(); ++itBegin)
      {
        oss << tokenMap[*itBegin]->stringForm().toStdString()
            <<"  " << m_d->m_matchingVertextoEntity[*itBegin].toStdString()
            << "\n";
      }
      LDEBUG << "Entities found :\n" << oss.str();
#endif
    //7. Update Graphs
      updateAnalysisData(analysis);
    }
    //Continue to the next batch
  }

  return SUCCESS_ID;
}

bool TensorflowSpecificEntities::updateAnalysisData(AnalysisContent& analysis) const
{
//     LinguisticGraphVertex previous;
  auto analysisGraph = static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));

  auto lingGraph = const_cast<LinguisticGraph*>(analysisGraph->getGraph());

  auto itVisited = m_d->m_visitedVertex.cbegin();
  while(itVisited != m_d->m_visitedVertex.cend())
  {
    // Look for entities
    if(m_d->m_matchingVertextoEntity[*itVisited] != "O")
    {
      // Create a specific object to encapsulate LinguisticGraphVertex which forms the entity
      Automaton::RecognizerMatch entityFound(analysisGraph, *itVisited, true);
      itVisited++;
      auto entityBegin = entityFound.getBegin();
      //        Look for words from the same entity
      //        [eng] It doesn't begin by B-XXX but by I-XXX according to
      //          CoNLL format. The tag XXX has to be the same between words
      //          from the same entity
      while(itVisited != m_d->m_visitedVertex.cend() &&
        m_d->m_matchingVertextoEntity[*itVisited][0] != 'B' &&
        m_d->m_matchingVertextoEntity[*itVisited].endsWith(m_d->m_matchingVertextoEntity[entityBegin].mid(2)))
      {
        entityFound.addBackVertex(*itVisited);
        itVisited++;
      }
      EntityType seType = Common::MediaticData::MediaticData::single().getEntityType(m_d->m_matchingVertextoEntity[entityFound.getBegin()].remove(QRegularExpression("^[BI]-")));
      entityFound.setType(seType);
#ifdef DEBUG_LP
      TFSELOGINIT;
      auto tokenMap = get(vertex_token, *lingGraph);
      std::ostringstream oss;
      for(auto itBeginEntity = entityFound.cbegin();
          itBeginEntity != entityFound.cend(); ++itBeginEntity)
      {
        oss << tokenMap[(*itBeginEntity).m_elem.first]->stringForm().toStdString()
            <<"  "
            << Common::MediaticData::MediaticData::single().getEntityName(seType).toStdString()
            << " ; ";
      }
      LDEBUG << "Entity found :" << oss.str();
#endif
      // Update AnalysisGraph
      if(!createSpecificEntity(entityFound, analysis))
      {
        return false;
      }
//         previous=entityFound.getEnd();
    }
    else
    {
//         previous=*itVisited;
      itVisited++;
    }
  }
  return true;
}

bool TensorflowSpecificEntities::createSpecificEntity(
  Automaton::RecognizerMatch& entityFound,
  AnalysisContent& analysis) const
{
  if(entityFound.empty())
  {
    return false;
  }

#ifdef DEBUG_LP
  TFSELOGINIT;
  LDEBUG << "CreateSpecificEntity: create entity of type " << entityFound.getType() << " on vertices " << entityFound;
#endif

  auto analysisGraph = static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  auto annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));

  if (annotationData == 0)
  {
    return false;
  }

  // Do not create annotation if annotation of same type exists
  if (entityFound.size() == 1)
  {
    auto matches = annotationData->matches(analysisGraph->getGraphId(),
                                           entityFound.getBegin(), "annot");
    for (auto it = matches.cbegin(); it != matches.cend(); it++)
    {
      if (annotationData->hasAnnotation(*it, QString::fromUtf8("SpecificEntity"))
          && annotationData->annotation(*it, QString::fromUtf8("SpecificEntity"))
          .pointerValue<SpecificEntityAnnotation>()->getType() == entityFound.getType() )
      {
        return false;
      }
    }
  }

  // Create a specific annotation
  if (annotationData->dumpFunction("SpecificEntity") == 0)
  {
    annotationData->dumpFunction("SpecificEntity", new DumpSpecificEntityAnnotation());
  }

  auto lingGraph = const_cast<LinguisticGraph*>(analysisGraph->getGraph());
  auto tokenMap = get(vertex_token, *lingGraph);
  auto dataMap = get(vertex_data, *lingGraph);

  SpecificEntityAnnotation annot(entityFound,*m_d->m_sp);
  std::ostringstream oss;
  annot.dump(oss);
#ifdef DEBUG_LP
  LDEBUG << "CreateSpecificEntity: annot =  " << oss.str();
#endif
  auto head = annot.getHead();
  auto dataHead = dataMap[head];

  // Prepare a new Token and a new MorphoSyntacticData for the new Vertex built basing on entity's head from specificentityannotation data
  auto seFlex = annot.getString();
  auto seLemma = annot.getNormalizedString();
  //No features with this method
  auto seNorm = annot.getNormalizedForm();

  // creata a new MorphoSyntacticData
  auto newMorphData = new MorphoSyntacticData();

  // all linguisticElements of this morphosyntacticData share common SE information
  LinguisticElement elem;
  elem.inflectedForm = seFlex; // StringsPoolIndex
  elem.lemma = seLemma; // StringsPoolIndex
  elem.normalizedForm = seNorm; // StringsPoolIndex
  elem.type = SPECIFIC_ENTITY; // MorphoSyntacticType

  auto seType = entityFound.getType();
  //useful to get micros categories linked
  const auto& resourceName = Common::MediaticData::MediaticData::single().getEntityGroupName(seType.getGroupId())+"Micros";

#ifdef DEBUG_LP
  LDEBUG << "Entities resource name is : " << resourceName;
#endif
  auto res = LinguisticResources::single().getResource(m_d->m_language,
                                                       resourceName.toStdString());
  if (res != nullptr)
  {
    auto entityMicros = static_cast<SpecificEntities::SpecificEntitiesMicros*>(res);
    auto micros = entityMicros->getMicros(seType);
#ifdef DEBUG_LP
    if (logger.isDebugEnabled())
    {
      std::ostringstream oss;
      for (auto it = micros->cbegin(), it_end=micros->cend(); it != it_end; it++)
      {
        oss << (*it) << ";";
      }
      LDEBUG << "CreateSpecificEntity, micros are " << oss.str();
    }
#endif

    //create a set of linguisticElements. Each LinguisticElement is linked to a
    //LinguisticCode from the entity
    addMicrosToMorphoSyntacticData(newMorphData, dataHead, *micros, elem);
  }
  else
  {
    TFSELOGINIT;
    // cannot find micros for this type: error
    LERROR << "TensorflowSpecificEntities::createSpecificEntity: Missing resource "
            << resourceName ;
    delete newMorphData;
    return false;
  }
  const auto& sp = *m_d->m_sp; //match id to string
  auto newToken = new Token(
      seFlex,
      sp[seFlex],
      entityFound.positionBegin(),
      entityFound.length());
  auto tStatus = tokenMap[head]->status();
  if(tokenMap[entityFound.getEnd()]->status().isAlphaPossessive())
  {
    tStatus.setAlphaPossessive(true);
  }
  newToken->setStatus(tStatus);

  if (newMorphData->empty())
  {
    TFSELOGINIT;
    LERROR << "TensorflowSpecificEntities::createSpecificEntity : Found no morphosyntactic  data for new vertex. Abort.";
    delete newToken;
    delete newMorphData;
    assert(false);
    return false;
  }

  // Create the new LinguisticGraphVertex and two edges
#ifdef DEBUG_LP
  LDEBUG<<"Setting Annotation and dependency.";
#endif
  // Update SyntacticGraph
  auto syntacticData = static_cast<SyntacticData*>(analysis.getData("SyntacticData"));
  LinguisticGraphVertex newVertex;
  DependencyGraphVertex newDepVertex = 0;
  if (syntacticData != 0)
  {
    boost::tie (newVertex, newDepVertex) = syntacticData->addVertex();
  }
  else
  {
    newVertex = add_vertex(*lingGraph);
  }

  // Update AnnotationGraph : create a new vertex and annotation
  auto agv =  annotationData->createAnnotationVertex();
  annotationData->addMatching(analysisGraph->getGraphId(), newVertex,
                              "annot", agv);
  annotationData->annotate(agv,
                           QString::fromStdString(analysisGraph->getGraphId()),
                           newVertex);
  tokenMap[newVertex] = newToken;
  dataMap[newVertex] = newMorphData;
  GenericAnnotation ga(annot);
  annotationData->annotate(agv, QString::fromUtf8("SpecificEntity"), ga);

#ifdef DEBUG_LP
  LDEBUG << "      - new vertex " << newVertex
          << "("<<analysisGraph->getGraphId()<<"), " << newDepVertex
          << "(dep), " << agv << "(annot) added";
#endif

  // Link vertex before and after the entity to the entity vertex
  // Clear edges between old vertex which are now gathered in the entity vertex

  LinguisticGraphInEdgeIt inEdgeIt, inEdgeItEnd;
  boost::tie(inEdgeIt,inEdgeItEnd) = boost::in_edges(head, *lingGraph);
  bool success;
  LinguisticGraphEdge e;
  if(inEdgeIt != inEdgeItEnd)
  {
    LinguisticGraphVertex previous = boost::source(*inEdgeIt, *lingGraph);
    boost::remove_edge(head, previous, *lingGraph);
    boost::tie(e, success) = boost::add_edge(previous, newVertex, *lingGraph);

    if (success)
    {
#ifdef DEBUG_LP
      LDEBUG << "        - in edge " << e.m_source << " -> " << e.m_target << " added";
#endif
    }
    else
    {
      TFSELOGINIT;
      LERROR << "        - in edge " << previous << " ->" << newVertex << " NOT added";
    }

    clearUnreachableVertices(analysisGraph, previous);
    clearUnreachableVertices(analysisGraph, head);
  }

  inEdgeIt++;
  //It is supposed that only one path in the graph exists before this module. Necessarily, only one edge in and out have to be updated
  if(inEdgeIt != inEdgeItEnd)
  {
    return false;
  }

  LinguisticGraphOutEdgeIt outEdgeIt, outEdgeItEnd;
  boost::tie(outEdgeIt, outEdgeItEnd) = boost::out_edges(entityFound.getEnd(),
                                                         *lingGraph);
  if(outEdgeIt != outEdgeItEnd)
  {
    auto next = boost::target(*outEdgeIt, *lingGraph);
    boost::remove_edge(entityFound.getEnd(), next, *lingGraph);
    boost::tie(e, success) = boost::add_edge(newVertex, next, *lingGraph);

    if (success)
    {
#ifdef DEBUG_LP
      LDEBUG << "        - out edge " << e.m_source << " -> " << e.m_target << " added";
#endif
    }
    else
    {
      TFSELOGINIT;
      LERROR << "        - out edge " << newVertex << " ->" << next << " NOT added";
    }

    clearUnreachableVertices(analysisGraph, entityFound.getEnd());
    clearUnreachableVertices(analysisGraph, next);
  }

  outEdgeIt++;
  if(outEdgeIt != outEdgeItEnd)
  {
    return false;
  }

  // Finalysing cleaning
  // TO DO : Check if it is useful
  auto entityFoundIt = entityFound.cbegin();
  auto entityFoundItEnd = entityFound.cend();
  for (; entityFoundIt != entityFoundItEnd; entityFoundIt++)
  {
    clearUnreachableVertices(analysisGraph, (*entityFoundIt).getVertex());
  }

  return true;
}

void TensorflowSpecificEntities::addMicrosToMorphoSyntacticData(
    LinguisticAnalysisStructure::MorphoSyntacticData* newMorphData,
    const LinguisticAnalysisStructure::MorphoSyntacticData* oldMorphData,
    const std::set<LinguisticCode>& micros,
    LinguisticAnalysisStructure::LinguisticElement& elem) const
{
  // try to filter existing microcategories
  for (auto it = oldMorphData->cbegin(), it_end = oldMorphData->cend();
       it!=it_end; it++)
  {
    if (micros.find(m_d->m_microAccessor->readValue((*it).properties)) !=
        micros.end())
    {
      elem.properties = (*it).properties;
      newMorphData->push_back(elem);
    }
  }
  // if no categories kept : assign all micros to keep
  if (newMorphData->empty())
  {
    for (auto it = micros.cbegin(), it_end = micros.cend(); it!=it_end; it++)
    {
      elem.properties = *it;
      newMorphData->push_back(elem);
    }
  }
}

void TensorflowSpecificEntities::clearUnreachableVertices(
  AnalysisGraph* anagraph,
  LinguisticGraphVertex from) const
{
#ifdef DEBUG_LP
  TFSELOGINIT;
  LDEBUG << "RecognizerData: clearing unreachable vertices from " << from;
#endif
  auto& g = *(anagraph->getGraph());

  std::queue<LinguisticGraphVertex> verticesToCheck;
  verticesToCheck.push( from );
  while (! verticesToCheck.empty() )
  {
#ifdef DEBUG_LP
    LDEBUG << "    vertices to check size = " << verticesToCheck.size();
#endif
    LinguisticGraphVertex v = verticesToCheck.front();
    verticesToCheck.pop();
    bool toClear = false;
#ifdef DEBUG_LP
    LDEBUG << "  out degree of " << v << " is " << out_degree(v, g);
#endif
    if (out_degree(v, g) == 0 && v != anagraph->lastVertex())
    {
      toClear = true;
      LinguisticGraphInEdgeIt it, it_end;
      boost::tie(it,it_end) = in_edges(v,g);
      for (; it != it_end; it++)
      {
        verticesToCheck.push(source(*it,g));
      }
    }
#ifdef DEBUG_LP
    LDEBUG << "  in degree of " << v << " is " << in_degree(v, g);
#endif
    if (in_degree(v, g) == 0 && v != anagraph->firstVertex())
    {
      toClear = true;
      LinguisticGraphOutEdgeIt it, it_end;
      boost::tie(it, it_end) = out_edges(v, g);
      for (; it != it_end; it++)
      {
        verticesToCheck.push(target(*it, g));
      }
    }
    if (toClear)
    {
#ifdef DEBUG_LP
      LDEBUG << "  clearing vertex " << v;
#endif
      boost::clear_vertex(v, g);
    }
  }
}

} // TensorflowSpecificEntities
} // LinguisticProcessing
} // Lima
