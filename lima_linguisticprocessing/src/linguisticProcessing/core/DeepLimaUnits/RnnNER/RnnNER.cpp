// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <QtCore/QTemporaryFile>
#include <QtCore/QRegularExpression>
#include <QDir>


#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/tools/FileUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/timeUtilsController.h"

#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/common/helpers/ConfigurationHelper.h"
#include "linguisticProcessing/common/helpers/LangCodeHelpers.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/core/SpecificEntities/SpecificEntitiesMicros.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"



#include "RnnNER.h"
#include "deeplima/token_sequence_analyzer.h"
#include "deeplima/token_type.h"
#include "deeplima/dumper_conllu.h"
#include "helpers/path_resolver.h"

#define DEBUG_THIS_FILE true

using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::PropertyCode;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::AnnotationGraphs;
using namespace deeplima;


namespace Lima::LinguisticProcessing::DeepLimaUnits::RnnNER
{

#if defined(DEBUG_LP) && defined(DEBUG_THIS_FILE)
#define LOG_MESSAGE(stream, msg) stream << msg;
#define LOG_MESSAGE_WITH_PROLOG(stream, msg) PTLOGINIT; LOG_MESSAGE(stream, msg);
#else
    #define LOG_MESSAGE(stream, msg) ;
#define LOG_MESSAGE_WITH_PROLOG(stream, msg) ;
#endif

static SimpleFactory<MediaProcessUnit, RnnNER> rnnnerFactory(RNNNER_CLASSID); // clazy:exclude=non-pod-global-static

CONFIGURATIONHELPER_LOGGING_INIT(PTLOGINIT);

class RnnNERPrivate: public ConfigurationHelper
{
public:
  RnnNERPrivate();
  ~RnnNERPrivate() = default;
  void init(GroupConfigurationStructure& unitConfiguration);
  void tagger(std::vector<segmentation::token_pos>& buffer);
  void insertTags(TokenSequenceAnalyzer<>::TokenIterator &ti);
  void addMicrosToMorphoSyntacticData(LinguisticAnalysisStructure::MorphoSyntacticData* newMorphData,
          const LinguisticAnalysisStructure::MorphoSyntacticData* oldMorphData,
          const std::set<LinguisticCode>& micros,
          LinguisticAnalysisStructure::LinguisticElement& elem) const;

  std::map<QString,QString> loadFileTags(const QString& filepath);

  void clearUnreachableVertices(
          AnalysisGraph* anagraph,
          LinguisticGraphVertex from) const;

  dumper::AnalysisToConllU<TokenSequenceAnalyzer<>::TokenIterator> m_dumper;

  MediaId m_language;
  FsaStringsPool* m_stringsPool;
  QString m_data;
  std::shared_ptr< TokenSequenceAnalyzer<> > m_tag;
  std::function<void()> m_load_fn;
  StringIndex m_stridx;
  PathResolver m_pResolver;
  std::vector<std::string> m_tags;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
  std::map<QString, QString> m_typeMap;
  bool m_loaded;
};

RnnNERPrivate::RnnNERPrivate():
    ConfigurationHelper("RnnNERPrivate", THIS_FILE_LOGGING_CATEGORY()),
    m_stringsPool(nullptr), m_tag(nullptr), m_stridx(), m_tags(), m_microAccessor(nullptr), m_loaded(false)
{
}



RnnNER::RnnNER(): m_d(new RnnNERPrivate()) {

}

RnnNER::~RnnNER()
{
  delete m_d;
}

void RnnNER::init(GroupConfigurationStructure &unitConfiguration, Manager *manager)
{
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "RnnNER::init");

  m_d->m_language = manager->getInitializationParameters().media;
  m_d->m_stringsPool = &MediaticData::changeable().stringsPool(m_d->m_language);

  m_d->init(unitConfiguration);
}

Lima::LimaStatusCode
RnnNER::process(Lima::AnalysisContent &analysis) const
{
  TimeUtils::updateCurrentTime();
  TimeUtilsController RnnNERProcessTime("RnnNER");
  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "start RnnNER");
  auto anagraph = std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("AnalysisGraph"));
  if (anagraph == nullptr)
  {
      PTLOGINIT;
      LERROR << "Can't Process RnnNER: missing data 'AnalysisGraph'";
      return MISSING_DATA;
  }
  auto srcgraph = anagraph->getGraph();
  auto endVx = anagraph->lastVertex();
  /// Creates the posgraph with the second parameter (deleteTokenWhenDestroyed)
  /// set to false as the tokens are owned by the anagraph
  /// @note : tokens newly created later will be owned by their creator and have
  /// to be deleted by this one
  auto posgraph = std::make_shared<LinguisticAnalysisStructure::AnalysisGraph>(
      "PosGraph", m_d->m_language, false, true);
  analysis.setData("PosGraph", posgraph);
  const auto& propertyCodeManager = dynamic_cast<const LanguageData&>(
          MediaticData::single().mediaData(m_d->m_language)).getPropertyCodeManager();
  // const auto& microManager = propertyCodeManager.getPropertyManager("MICRO");
  /** Creation of an annotation graph if necessary*/
  auto annotationData = std::dynamic_pointer_cast< AnnotationData >(analysis.getData("AnnotationData"));
  if (annotationData==nullptr)
  {
      annotationData = std::make_shared<AnnotationData>();
      /** Creates a node in the annotation graph for each node of the
      * morphosyntactic graph. Each new node is annotated with the name mrphv and
      * associated to the morphosyntactic vertex number */
      if (std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("AnalysisGraph")) != nullptr)
      {
          std::dynamic_pointer_cast<AnalysisGraph>(
                  analysis.getData("AnalysisGraph"))->populateAnnotationGraph(
                  annotationData.get(),
                  "AnalysisGraph");
      }
      analysis.setData("AnnotationData", annotationData);
  }
  if (num_vertices(*srcgraph)<=2)
  {
      return SUCCESS_ID;
  }

  VertexTokenPropertyMap vTokens = get(vertex_token, *srcgraph);
  auto currentVx = anagraph->firstVertex();
  LinguisticGraph* resultgraph=posgraph->getGraph();
  remove_edge(posgraph->firstVertex(),posgraph->lastVertex(),*resultgraph);

  std::vector<segmentation::token_pos> buffer;
  std::vector< LinguisticGraphVertex > anaVertices;
  std::vector<std::string> v;

  while(currentVx != endVx){
      if (currentVx != 0 && vTokens[currentVx] != nullptr) {
          const auto& src = vTokens[currentVx];
          v.push_back(src->stringForm().toStdString());
          buffer.emplace_back();
          anaVertices.push_back(currentVx);
      }
      LinguisticGraphOutEdgeIt it, it_end;
      boost::tie(it, it_end) = boost::out_edges(currentVx, *srcgraph);
      if (it != it_end)
      {
          currentVx = boost::target(*it, *srcgraph);
      }
      else
      {
          currentVx = endVx;
      }
  }
  /**
    * Construction of the tokens used in the tagger.
    */
  for(unsigned long k=0;k<anaVertices.size();k++){
      currentVx = anaVertices[k];
      if (currentVx != 0 && vTokens[currentVx] != nullptr) {
          const auto& src = vTokens[currentVx];
          auto& token = buffer[k];
          token.m_offset = src->position();
          token.m_len = src->length();
          token.m_pch = v[k].c_str();
          token.m_flags = segmentation::token_pos::flag_t(src->status().getStatus() & StatusType::T_SENTENCE_BRK);
      }
  }
  m_d->tagger(buffer);
  LOG_MESSAGE(LDEBUG, "tag size: " << m_d->m_tags.size());
  std::vector<LinguisticGraphVertex>::size_type anaVerticesIndex = 0;
  // LinguisticGraphVertex previousPosVertex = posgraph->firstVertex();
  /*
    * Here we add the part of speech data to the tokens
    * Adding link beetween the node in the analysis graph and the pos graph.
    */
  std::shared_ptr<Automaton::RecognizerMatch> entityFound;
  QString prev_tag = "O";
  bool isTagged = false;
  while (anaVerticesIndex < anaVertices.size())
  {
    auto anaVertex = anaVertices[anaVerticesIndex];
/*           auto newVx = boost::add_vertex(*resultgraph);
    auto agv =  annotationData->createAnnotationVertex();
    annotationData->addMatching("PosGraph", newVx, "annot", agv);
    annotationData->addMatching("AnalysisGraph", anaVertex, "PosGraph", newVx);
    annotationData->annotate(agv, QString::fromUtf8("PosGraph"), newVx);*/
    auto morphoData = get(vertex_data,*srcgraph,anaVertex);
    // auto srcToken = get(vertex_token,*srcgraph,anaVertex);

    if (morphoData!=nullptr)
    {
      auto entityTag = QString::fromUtf8(m_d->m_tags[anaVerticesIndex].c_str());
      if((anaVerticesIndex>0 && entityTag != prev_tag) || (entityTag[0]!="B" && prev_tag != "O") )
      {
        LinguisticGraphVertex newVertex = anagraph->firstVertex();
        if (entityFound->size() == 1)
        {
          auto matches = annotationData->matches(anagraph->getGraphId(),
                                                  entityFound->getBegin(), "annot");
          for (auto it = matches.cbegin(); it != matches.cend(); it++)
          {
            if (annotationData->hasAnnotation(*it, QString::fromUtf8("SpecificEntity"))
                && annotationData->annotation(*it, QString::fromUtf8("SpecificEntity"))
                            .pointerValue<SpecificEntities::SpecificEntityAnnotation>()->getType() == entityFound->getType() )
            {
              entityFound = nullptr;
              isTagged = true;
            }
          }
        }
      if(isTagged)
      {
          if (annotationData->dumpFunction("SpecificEntity") == nullptr)
          {
            annotationData->dumpFunction("SpecificEntity",
                                          new SpecificEntities::DumpSpecificEntityAnnotation());
          }

          auto lingGraph = const_cast<LinguisticGraph*>(anagraph->getGraph());
          auto tokenMap = get(vertex_token, *lingGraph);
          auto dataMap = get(vertex_data, *lingGraph);

          SpecificEntities::SpecificEntityAnnotation annot(*entityFound,*m_d->m_stringsPool);
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
          elem.inflectedForm = seFlex;  // StringsPoolIndex
          elem.lemma = seLemma;         // StringsPoolIndex
          elem.normalizedForm = seNorm; // StringsPoolIndex
          elem.type = SPECIFIC_ENTITY;  // MorphoSyntacticType

          auto seType = entityFound->getType();
          const auto& resourceName = Common::MediaticData::MediaticData::single().getEntityGroupName(seType.getGroupId())+"Micros";
          auto res = LinguisticResources::single().getResource(m_d->m_language,
                                                                resourceName.toStdString());
          if (res != nullptr)
          {
            auto entityMicros = static_cast<SpecificEntities::SpecificEntitiesMicros*>(res);
            auto micros = entityMicros->getMicros(seType);
            //create a set of linguisticElements. Each LinguisticElement is linked to a
            //LinguisticCode from the entity
            m_d->addMicrosToMorphoSyntacticData(newMorphData, dataHead, *micros, elem);
          }

          const auto& sp = *m_d->m_stringsPool; //match id to string
          auto newToken = new Token(
                  seFlex,
                  sp[seFlex],
                  entityFound->positionBegin(),
                  entityFound->length());
          auto tStatus = tokenMap[head]->status();

          auto syntacticData = std::dynamic_pointer_cast<SyntacticAnalysis::SyntacticData>(analysis.getData("SyntacticData"));
          //LinguisticGraphVertex newVertex;
          DependencyGraphVertex newDepVertex = 0;
          if (syntacticData != nullptr)
          {
            boost::tie (newVertex, newDepVertex) = syntacticData->addVertex();
          }
          else
          {
            newVertex = add_vertex(*lingGraph);
          }

          // Update AnnotationGraph : create a new vertex and annotation
          auto agv =  annotationData->createAnnotationVertex();
          annotationData->addMatching(anagraph->getGraphId(), newVertex,
                                      "annot", agv);
          annotationData->annotate(agv,
                                    QString::fromStdString(anagraph->getGraphId()),
                                    newVertex);
          tokenMap[newVertex] = newToken;
          dataMap[newVertex] = newMorphData;
          GenericAnnotation ga(annot);
          annotationData->annotate(agv, QString::fromUtf8("SpecificEntity"), ga);

          LinguisticGraphInEdgeIt inEdgeIt, inEdgeItEnd;
          boost::tie(inEdgeIt,inEdgeItEnd) = boost::in_edges(head, *lingGraph);
          bool success;
          LinguisticGraphEdge e;
          if(inEdgeIt != inEdgeItEnd)
          {
              LinguisticGraphVertex previous = boost::source(*inEdgeIt, *lingGraph);
              boost::remove_edge(head, previous, *lingGraph);
              boost::tie(e, success) = boost::add_edge(previous, newVertex, *lingGraph);

              m_d->clearUnreachableVertices(anagraph.get(), previous);
              m_d->clearUnreachableVertices(anagraph.get(), head);
            }

            inEdgeIt++;
            //It is supposed that only one path in the graph exists before this module. Necessarily, only one edge in and out have to be updated
            if(inEdgeIt != inEdgeItEnd)
            {
              LimaException("Error: graph contains multiple paths");
            }

            LinguisticGraphOutEdgeIt outEdgeIt, outEdgeItEnd;
            boost::tie(outEdgeIt, outEdgeItEnd) = boost::out_edges(entityFound->getEnd(),
                                                                    *lingGraph);
            if(outEdgeIt != outEdgeItEnd)
            {
              auto next = boost::target(*outEdgeIt, *lingGraph);
              boost::remove_edge(entityFound->getEnd(), next, *lingGraph);
              boost::tie(e, success) = boost::add_edge(newVertex, next, *lingGraph);


              m_d->clearUnreachableVertices(anagraph.get(), entityFound->getEnd());
              m_d->clearUnreachableVertices(anagraph.get(), next);
            }

            outEdgeIt++;
            if(outEdgeIt != outEdgeItEnd)
            {
              LimaException("Error: graph contains multiple paths");
            }

            // Finalysing cleaning
            // TO DO : Check if it is useful
            auto entityFoundIt = entityFound->cbegin();
            auto entityFoundItEnd = entityFound->cend();
            for (; entityFoundIt != entityFoundItEnd; entityFoundIt++)
            {
              m_d->clearUnreachableVertices(anagraph.get(), (*entityFoundIt).getVertex());
            }
        }
      }
      if(entityTag == "O")
      {
        prev_tag = "O";
      }
      else
      {
        if(entityFound == nullptr)
        {
          entityFound = std::make_shared<Automaton::RecognizerMatch>(anagraph.get(), anaVertex, true);
          auto entityModex = m_d->m_typeMap[entityTag];
          entityModex.remove(QRegularExpression("^[BI]-"));
          EntityType seType;
          try
          {
            seType = Common::MediaticData::MediaticData::single().getEntityType(entityModex);
          } catch (LimaException& e)
          {
            PTLOGINIT;
            LIMA_EXCEPTION( "Lima exception while getting entity type "
                                    << entityModex << ": " << e.what());
          }
          entityFound->setType(seType);
        }
        else
        {
          entityFound->addFrontVertex(anaVertex);
        }
        prev_tag = QString::fromUtf8(m_d->m_tags[anaVerticesIndex].c_str());

      }


    }
    isTagged = false;
    anaVerticesIndex++;
  }
  LOG_MESSAGE(LDEBUG, "RnnNER NER done.");
  TimeUtils::logElapsedTime("RnnNER");
  return SUCCESS_ID;
}

void RnnNERPrivate::init(GroupConfigurationStructure& unitConfiguration)
{
  m_data = QString::fromStdString(getStringParameter(unitConfiguration, "data", 0, "SentenceBoundaries"));
  auto model_prefix = QString::fromStdString(getStringParameter(
    unitConfiguration, "model_prefix", ConfigurationHelper::REQUIRED | ConfigurationHelper::NOT_EMPTY));

  LOG_MESSAGE_WITH_PROLOG(LDEBUG, "RnnNERPrivate::init" << model_prefix);

  QString lang_str = MediaticData::single().media(m_language).c_str();
  QString resources_path = MediaticData::single().getResourcesPath().c_str();
  QString model_name = model_prefix;
  std::string udlang;
  MediaticData::single().getOptionValue("udlang", udlang);

  if (!fix_lang_codes(lang_str, udlang))
  {
      LIMA_EXCEPTION_SELECT_LOGINIT(PTLOGINIT,
                                    "RnnNERPrivate::init: Can't parse language id " << udlang.c_str(),
                                    Lima::InvalidConfiguration);
  }

  model_name.replace(QString("$udlang"), QString(udlang.c_str()));

  auto model_file_name = findFileInPaths(resources_path,
                                          QString::fromUtf8("/RnnNER/%1/%2.pt")
                                                  .arg(lang_str, model_name));

  if (model_file_name.isEmpty())
  {
    throw InvalidConfiguration("RnnNERPrivate::init: tagger model file not found.");
  }
  auto config_file_name = findFileInPaths(resources_path,
                                      QString::fromUtf8("/RnnNER/%1/ner.cfg")
                                              .arg(lang_str));
  if (config_file_name.isEmpty())
  {
    throw InvalidConfiguration("RnnNERPrivate::init: ner config file not found.");
  }

  m_typeMap = loadFileTags(config_file_name);

  m_load_fn = [this, model_file_name]()
  {
    if (m_loaded)
    {
      return;
    }
    m_tag = std::make_shared< TokenSequenceAnalyzer<> >(model_file_name.toStdString(), "", m_pResolver, 1024, 8);


    m_loaded = true;
  };

  if (!isInitLazy())
  {
    m_load_fn();
  }
  LOG_MESSAGE(LDEBUG, "classes name: " << m_tag->get_class_names());
  LOG_MESSAGE(LDEBUG, "classes: " << m_tag->get_classes());
  for (size_t i = 0; i < m_tag->get_classes().size(); ++i)
  {
    m_dumper.set_classes(i, m_tag->get_class_names()[i], m_tag->get_classes()[i]);
  }
  m_microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));

}

void RnnNERPrivate::tagger(std::vector<segmentation::token_pos>& buffer)
{
  m_tag->register_handler([this](std::shared_ptr< StringIndex > stridx,
                                          const token_buffer_t<>& tokens,
                                          const std::vector<StringIndex::idx_t>& lemmata,
                                          std::shared_ptr< StdMatrix<uint8_t> > classes,
                                          size_t begin,
                                          size_t end){
      TokenSequenceAnalyzer<>::TokenIterator ti(*stridx, tokens, lemmata, classes, begin, end);
      insertTags(ti);
  });
  LOG_MESSAGE_WITH_PROLOG(LDEBUG,buffer[0].m_pch);
  (*m_tag)(buffer, buffer.size());
  m_tag->finalize();
}

void RnnNERPrivate::insertTags(TokenSequenceAnalyzer<>::TokenIterator& ti)
{
  auto classes = m_dumper.getMClasses();
  while(!ti.end())
  {
    LOG_MESSAGE_WITH_PROLOG(LDEBUG, "index: " << ti.token_class(0));
    m_tags.push_back(classes[0][ti.token_class(0)]);
    ti.next();
  }
}

void RnnNERPrivate::addMicrosToMorphoSyntacticData(
        LinguisticAnalysisStructure::MorphoSyntacticData* newMorphData,
        const LinguisticAnalysisStructure::MorphoSyntacticData* oldMorphData,
        const std::set<LinguisticCode>& micros,
        LinguisticAnalysisStructure::LinguisticElement& elem) const
{
  // try to filter existing microcategories
  for (auto it = oldMorphData->cbegin(), it_end = oldMorphData->cend();
        it!=it_end; it++)
  {
      if (micros.find(m_microAccessor->readValue((*it).properties)) !=
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

void RnnNERPrivate::clearUnreachableVertices(
        AnalysisGraph* anagraph,
        LinguisticGraphVertex from) const
{
  auto& g = *(anagraph->getGraph());

  std::queue<LinguisticGraphVertex> verticesToCheck;
  verticesToCheck.push( from );
  while (! verticesToCheck.empty() )
  {

      LinguisticGraphVertex v = verticesToCheck.front();
      verticesToCheck.pop();
      bool toClear = false;
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
          boost::clear_vertex(v, g);
      }
  }
}

std::map<QString,QString> RnnNERPrivate::loadFileTags(const QString& filepath)
{
  QFile file(filepath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
      throw BadFileException("The file "+filepath.toStdString()+" doesn't exist.");
  }
  if(file.size()==0)
  {
      std::cout<<"The file is empty.";
      return {};
  }
  QTextStream in(&file);
  std::map<QString,QString> d;
  unsigned int i=0;
  while (!in.atEnd())
  {
      QString line = in.readLine();
      line=line.simplified();
      i=line.indexOf(":");
      QString type = line.left(i);
      QString modex = line.mid(i+1);
      d[type]=modex;
  }
  return d;
}

}
