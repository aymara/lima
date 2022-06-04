/*
    Copyright 2002-2020 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/

#include "ConllDumper.h"
#include "common/MediaProcessors/DumperStream.h"
#include "common/time/traceUtils.h"
#include "common/tools/FileUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/misc/AbstractAccessByString.h"
#include "common/misc/escaper.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationGraph.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/core/SemanticAnalysis/LimaConllTokenIdMapping.h"

#include <QQueue>
#include <QSet>
#include <QStringList>

#include <fstream>
#include <common/linguisticData/languageData.h>

using namespace Lima::Common;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima::Common::PropertyCode;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace Lima::LinguisticProcessing::SemanticAnalysis;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

using MedData = Lima::Common::MediaticData::MediaticData ;
using LangData = Lima::Common::MediaticData::LanguageData;

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDumpers
{

QStringList FORMATS = {"CoNLL-U", "CoNLL-03"};

SimpleFactory<MediaProcessUnit,ConllDumper> conllDumperFactory(CONLLDUMPER_CLASSID);

class ConllDumperPrivate
{
  friend class ConllDumper;
  ConllDumperPrivate();

  ~ConllDumperPrivate() = default;

  LimaStatusCode dumpPosGraphVertex(
    std::shared_ptr<DumperStream>& dstream,
    LinguisticGraphVertex v,
    int& tokenId,
    LinguisticGraphVertex vEndDone,
    std::map<LinguisticGraphVertex,int>& segmentationMapping,
    const QString& neType,
    bool first);

  /** Dumps an analysis graph vertex @ref v which is inside a specific entity
   * holded by pos graph vertex @ref posGraphVertex
   */
  LimaStatusCode dumpAnalysisGraphVertex(
    std::shared_ptr<DumperStream>& dstream,
    LinguisticGraphVertex v,
    LinguisticGraphVertex posGraphVertex,
    int& tokenId,
    LinguisticGraphVertex vEndDone,
    const QString& parentNeType,
    bool first,
    const Automaton::EntityFeatures& features);

  /** Gets the named entity type for the PosGraph vertex @ref posGraphVertex
   * if it is a specific entity. Return "_" otherwise
   */
  QString getNeType(LinguisticGraphVertex posGraphVertex);

  std::pair<QString, QString> getConllRelName(
    LinguisticGraphVertex v,
    std::map<LinguisticGraphVertex,int>& segmentationMapping);

  QStringList getPredicate(LinguisticGraphVertex v);

  bool hasSpaceAfter(LinguisticGraphVertex v, LinguisticGraph* graph);

  QString getMicro(MorphoSyntacticData* morphoData);

  QString getFeats(MorphoSyntacticData* morphoData);

  void getTransducedTokens(const LinguisticGraph& graph,
                           LinguisticGraphVertex begin,
                           LinguisticGraphVertex end,
                           std::map<int, size_t>& out);

  /** Dumps the the named entity of type @ref neType associated to the PosGraph
   * vertex @ref v
   */
  void dumpNamedEntity(std::shared_ptr<DumperStream>& dstream,
                       LinguisticGraphVertex v,
                       int& tokenId,
                       LinguisticGraphVertex vEndDone,
                       std::map<LinguisticGraphVertex,int>& segmentationMapping,
                       const QString& neType);

  /**
   * @brief Collect all annotation tokens corresponding to a predicate of the
   * sentence starting at @ref sentenceBegin and finishing at @ref sentenceEnd
   */
  void  collectPredicateTokens(
    Lima::AnalysisContent& analysis,
    LinguisticGraphVertex sentenceBegin,
    LinguisticGraphVertex sentenceEnd);

  void dumpToken(
    std::shared_ptr<DumperStream>& dstream,
    int tokenId, // ID
    const QString& inflectedToken, // FORM
    const QString& lemmatizedToken, // LEMMA
    const QString& micro, // UPOS
    const QString& xpos, // XPOS
    const QString& features,// FEATS
    const QString& targetConllIdString, // HEAD
    const QString& conllRelName, // DEPREL
    const QString& deps, // DEPS @TODO
    const QStringList& miscField,
    const QString& neType,
    const QString& previousNeType);

  const SpecificEntityAnnotation* getSpecificEntityAnnotation(
    LinguisticGraphVertex v) const;

  QString m_format = "CoNLL-U";

  MediaId m_language;
  bool m_withColsHeader;
  QString m_graph;
  QMap<QString, QString> m_conllLimaDepMapping;
  const Common::PropertyCode::PropertyAccessor* m_propertyAccessor;
  LinguisticGraph* posGraph;
  LinguisticGraph* anaGraph;
  DependencyGraph* depGraph;
  AnnotationData* annotationData;
  const LanguageData* languageData;
  const PropertyCodeManager* propertyCodeManager;
  const PropertyManager* microManager;
  const std::map< std::string, PropertyManager >* managers;
  const FsaStringsPool* sp;
  QMultiMap<LinguisticGraphVertex, AnnotationGraphVertex> predicates;
  QString previousNeType;
  std::map< LinguisticGraphVertex,
          std::pair<LinguisticGraphVertex,
                    std::string> > vertexDependencyInformations;
};

ConllDumperPrivate::ConllDumperPrivate():
  m_language(0),
  m_withColsHeader(false),
  m_graph("PosGraph"),
  m_conllLimaDepMapping(),
  posGraph(nullptr),
  anaGraph(nullptr),
  depGraph(nullptr),
  annotationData(nullptr),
  previousNeType("_")
{
}

ConllDumper::ConllDumper():
  AbstractTextualAnalysisDumper(),
  m_d(new ConllDumperPrivate())
{
}

ConllDumper::~ConllDumper()
{
  delete m_d;
}

void ConllDumper::init(GroupConfigurationStructure& unitConfiguration,
                       Manager* manager)
{
  DUMPERLOGINIT;
  AbstractTextualAnalysisDumper::init(unitConfiguration, manager);

  try
  {
    m_d->m_graph = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("graph").c_str());
  }
  catch (NoSuchParam& ) {} // keep default value

  m_d->m_language = manager->getInitializationParameters().media;
  m_d->languageData = &static_cast<const LanguageData&>(MedData::single().mediaData(m_d->m_language));
  m_d->propertyCodeManager = &m_d->languageData->getPropertyCodeManager();
  m_d->microManager = &m_d->propertyCodeManager->getPropertyManager("MICRO");
  m_d->managers = &m_d->propertyCodeManager->getPropertyManagers();
  m_d->m_propertyAccessor = &m_d->propertyCodeManager->getPropertyAccessor("MICRO");
  m_d->sp = &MedData::single().stringsPool(m_d->m_language);

  try
  {
    m_d->m_withColsHeader= QString(
      unitConfiguration.getParamsValueAtKey(
        "withColsHeader").c_str() ).toLower() == "true";
  }
  catch (NoSuchParam& ) {} // keep default value

  try
  {
    auto resourcePath = MedData::single().getResourcesPath();
    auto mappingFile = findFileInPaths(resourcePath.c_str(),
                                       unitConfiguration.getParamsValueAtKey(
                                         "mappingFile").c_str());
    std::ifstream ifs(mappingFile.toStdString(), std::ifstream::binary);
    if (!ifs.good())
    {
      LERROR << "ERROR: cannot open" << mappingFile;
      throw InvalidConfiguration();
    }
    while (ifs.good() && !ifs.eof())
    {
      auto line = readLine(ifs);
      QStringList strs = QString::fromUtf8(line.c_str()).split('\t');
      if (strs.size() == 2)
      {
        m_d->m_conllLimaDepMapping.insert(strs[0],strs[1]);
      }
    }

  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LINFO << "no parameter 'mappingFile' in ConllDumper group" << " !";
//     throw InvalidConfiguration();
  }

  try
  {
    m_d->m_format = QString::fromStdString(
      unitConfiguration.getParamsValueAtKey("format") );
    if (!FORMATS.contains(m_d->m_format))
    {
      QString errorMessage;
      QTextStream qts(&errorMessage);
      qts << "Invalid CoNLL dumper configuration. Known formats are"
          << FORMATS.join(",") << ". Got" << m_d->m_format;
      DUMPERLOGINIT;
      LERROR << errorMessage;
      throw InvalidConfiguration(errorMessage.toStdString());
    }
  }
  catch (NoSuchParam& ) {} // keep default value
}

LimaStatusCode ConllDumper::process(AnalysisContent& analysis) const
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LDEBUG << "ConllDumper::process";
#endif

  LinguisticMetaData* metadata = static_cast<LinguisticMetaData*>(
    analysis.getData("LinguisticMetaData"));
  if (metadata == 0)
  {
    DUMPERLOGINIT;
    LERROR << "ConllDumper::process no LinguisticMetaData ! abort";
    return MISSING_DATA;
  }

  AnnotationData* annotationData = static_cast<AnnotationData*>(analysis.getData("AnnotationData"));
  if (annotationData == nullptr)
  {
    DUMPERLOGINIT;
    LINFO << "ConllDumper::process no AnnotationData ! Will not contain NE nor predicates";
  }
  m_d->annotationData = annotationData;
  auto posGraphData=static_cast<AnalysisGraph*>(analysis.getData(m_d->m_graph.toStdString()));
  // posGraphData est de type PosGraph et non pas AnalysisGraph
  if (posGraphData==0)
  {
    DUMPERLOGINIT;
    LERROR << "ConllDumper::process graph" << m_d->m_graph << "has not been produced: check pipeline";
    return MISSING_DATA;
  }
  auto posGraph = posGraphData->getGraph();
  m_d->posGraph = posGraph;

  auto anaGraphData=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  if (anaGraphData==0)
  {
    DUMPERLOGINIT;
    LERROR << "ConllDumper::process graph AnalysisGraph has not been produced: check pipeline";
    return MISSING_DATA;
  }
  auto anaGraph = anaGraphData->getGraph();
  m_d->anaGraph = anaGraph;

  auto sd = static_cast<SegmentationData*>(
    analysis.getData("SentenceBoundaries"));
  if (sd == nullptr)
  {
    DUMPERLOGINIT;
    LERROR << "ConllDumper::process no SentenceBoundaries! abort";
    return MISSING_DATA;
  }

  auto syntacticData = static_cast<SyntacticData*>(
    analysis.getData("SyntacticData"));
  if (syntacticData == nullptr)
  {
    syntacticData = new SyntacticData(posGraphData,0);
    syntacticData->setupDependencyGraph();
    analysis.setData("SyntacticData", syntacticData);
  }
  auto depGraph = syntacticData-> dependencyGraph();
  m_d->depGraph = depGraph;

  auto dstream = initialize(analysis);

  uint64_t nbSentences((sd->getSegments()).size());
  if (nbSentences == 0)
  {
    DUMPERLOGINIT;
    LERROR << "ConllDumper::process 0 sentence to process";
    return SUCCESS_ID;
  }

  auto sbItr = sd->getSegments().begin();
#ifdef DEBUG_LP
  LDEBUG << "ConllDumper::process There are "<< nbSentences << " sentences";
#endif
  auto sentenceBegin = sbItr->getFirstVertex();
  auto sentenceEnd = sbItr->getLastVertex();

  auto limaConllTokenIdMapping =
      static_cast<LimaConllTokenIdMapping*>(
        analysis.getData("LimaConllTokenIdMapping"));
  if (limaConllTokenIdMapping == nullptr)
  {
    limaConllTokenIdMapping = new LimaConllTokenIdMapping();
    analysis.setData("LimaConllTokenIdMapping", limaConllTokenIdMapping);
  }
  int sentenceNb = 0;
  LinguisticGraphVertex vEndDone = 0;

  const auto originalText = static_cast<LimaStringText*>(analysis.getData("Text"));

  if (m_d->m_format == "CoNLL-U")
  {
  }
  else if (m_d->m_format == "CoNLL-03")
  {
    dstream->out() << "-DOCSTART- -X- O O" << std::endl << std::endl;
  }
  else
  {
    DUMPERLOGINIT;
    QString errorMessage;
    QTextStream qts(&errorMessage);
    qts << "ConllDumper::process unknown format"  << m_d->m_format;
    LERROR << errorMessage;
    return UNKNOWN_FORMAT;
  }
  while (sbItr != sd->getSegments().end()) //for each sentence
  {
    sentenceNb++;
    // The cols list below is optionnal
    if (m_d->m_format == "CoNLL-U")
    {
      if( sentenceNb==1 && m_d->m_withColsHeader )
      {
        dstream->out()
          << "# global.columns = ID\tFORM\tLEMMA\tUPOS\tXPOS\tFEATS\tHEAD\tDEPREL\tDEPS\tMISC"
          << std::endl;
      }
      dstream->out() << "# sent_id = " << sentenceNb << std::endl;
    }
    else if (m_d->m_format == "CoNLL-03")
    {
    }
    else
    {
      DUMPERLOGINIT;
      QString errorMessage;
      QTextStream qts(&errorMessage);
      qts << "ConllDumper::process unknown format"  << m_d->m_format;
      LERROR << errorMessage;
      return UNKNOWN_FORMAT;
    }
    sentenceBegin=sbItr->getFirstVertex();
    sentenceEnd=sbItr->getLastVertex();
    std::map<LinguisticGraphVertex,int> segmentationMapping;//mapping the two types of segmentations (Lima and conll)
    std::map<int,LinguisticGraphVertex> segmentationMappingReverse;
    std::map<int,size_t> transducedTokens;
    m_d->getTransducedTokens(*anaGraph, sentenceBegin, sentenceEnd, transducedTokens);

#ifdef DEBUG_LP
    LDEBUG << "ConllDumper::process begin - end: " << sentenceBegin
            << " - " << sentenceEnd;
#endif

    LinguisticGraphOutEdgeIt outItr,outItrEnd;
    QQueue<LinguisticGraphVertex> toVisit;
    QSet<LinguisticGraphVertex> visited;
    toVisit.enqueue(sentenceBegin);
    int vertexId = 0;
    LinguisticGraphVertex v = 0;
    while (v != sentenceEnd && !toVisit.empty())
    {
      v = toVisit.dequeue();
#ifdef DEBUG_LP
      LDEBUG << "ConllDumper::process Vertex index : " << v;
#endif
      visited.insert(v);
      segmentationMapping.insert(std::make_pair(v,vertexId));
      segmentationMappingReverse.insert(std::make_pair(vertexId,v));
#ifdef DEBUG_LP
      LDEBUG << "ConllDumper::process conll id : " << vertexId
              << " Lima id : " << v;
#endif

      auto dcurrent = syntacticData->depVertexForTokenVertex(v);
      DependencyGraphOutEdgeIt dit, dit_end;
      boost::tie(dit,dit_end) = boost::out_edges(dcurrent,*depGraph);
      for (; dit != dit_end; dit++)
      {
#ifdef DEBUG_LP
        LDEBUG << "ConllDumper::process Dumping dependency edge "
                << (*dit).m_source << " -> " << (*dit).m_target;
#endif
        try
        {
          auto typeMap = get(edge_deprel_type, *depGraph);
          auto type = typeMap[*dit];
          auto syntRelName = m_d->languageData->getSyntacticRelationName(type);
#ifdef DEBUG_LP
          LDEBUG << "ConllDumper::process relation = " << syntRelName;
          LDEBUG << "ConllDumper::process Src  : Dep vertex= "
                  << boost::source(*dit, *depGraph);
          auto src = syntacticData->tokenVertexForDepVertex(
              boost::source(*dit, *depGraph));
          LDEBUG << "ConllDumper::process Src  : Morph vertex= " << src;
          LDEBUG << "ConllDumper::process Targ : Dep vertex= "
                  << boost::target(*dit, *depGraph);
#endif
          auto dest = syntacticData->tokenVertexForDepVertex(
            boost::target(*dit, *depGraph));
#ifdef DEBUG_LP
          LDEBUG << "ConllDumper::process Targ : Morph vertex= " << dest;
#endif
          if (syntRelName!="")
          {
#ifdef DEBUG_LP
            LDEBUG << "ConllDumper::process saving target for"
                    << v << ":" << dest << syntRelName;
#endif
            m_d->vertexDependencyInformations.insert(
                std::make_pair(v, std::make_pair(dest, syntRelName)));
          }
        }
        catch (const std::range_error& )
        {
        }
        catch (...)
        {
#ifdef DEBUG_LP
          LDEBUG << "ConllDumper::process: catch others.....";
#endif
          throw;
        }
      }
      if (v == sentenceEnd)
      {
        continue;
      }
      LinguisticGraphOutEdgeIt outItr,outItrEnd;
      for (boost::tie(outItr,outItrEnd)=boost::out_edges(v, *posGraph);
           outItr!=outItrEnd; outItr++)
      {
        LinguisticGraphVertex next=boost::target(*outItr,*posGraph);
        if (!visited.contains(next) && next != posGraphData->lastVertex())
        {
          toVisit.enqueue(next);
        }
      }
      ++vertexId;
    }

    // instead of looking to all vertices, follow the graph (in
    // morphological graph, some vertices are not related to main graph:
    // idiomatic expressions parts and named entity parts)

    toVisit.clear();
    visited.clear();

    sentenceBegin=sbItr->getFirstVertex();
    sentenceEnd=sbItr->getLastVertex();

    // get the list of predicates for the current sentence
    m_d->collectPredicateTokens( analysis,
                                                   sentenceBegin,
                                                   sentenceEnd );
#ifdef DEBUG_LP
    LDEBUG << "ConllDumper::process predicates for sentence between"
            << sentenceBegin << "and" << sentenceEnd << "are:" << m_d->predicates;
#endif
    auto keys = m_d->predicates.keys();

    v = sentenceBegin;
    bool firstTime = true;
    uint64_t pStart = 0;
    uint64_t pEnd = 0;
    while (v != sentenceEnd)
    {
      //as long as there are vertices in the sentence
      auto ft = get(vertex_token,*posGraph,v);
      if( ft != nullptr && v != sentenceBegin )
      {
        if(firstTime)
        {
            pStart = ft->position();
            firstTime = false;
        }
        pEnd = ft->position() + ft->length();
      }
      LinguisticGraphOutEdgeIt outIter,outIterEnd;
      for (boost::tie(outIter,outIterEnd) = boost::out_edges(v,*posGraph);
           outIter!=outIterEnd; outIter++)
      {
        v = boost::target(*outIter,*posGraph);
      }
    }
    auto curSentenceText = originalText->mid(pStart-1, pEnd-pStart+1);

    if (m_d->m_format == "CoNLL-U")
    {
      // The text below is mandatory for CONLL-U format
      dstream->out() << "# text = "
                      << curSentenceText.replace("\r\n"," ").replace("\n"," ").toStdString()
                      << std::endl;
    }
    else if (m_d->m_format == "CoNLL-03") {}
    else
    {
      DUMPERLOGINIT;
      QString errorMessage;
      QTextStream qts(&errorMessage);
      qts << "ConllDumper::process unknown format"  << m_d->m_format;
      LERROR << errorMessage;
      return UNKNOWN_FORMAT;
    }

    toVisit.enqueue(sentenceBegin);
    int tokenId = 1;
    v = 0;
    while (!toVisit.empty() && v!=sentenceEnd)
    { //as long as there are vertices in the sentence
      v = toVisit.dequeue();

      if (transducedTokens.find(tokenId) != transducedTokens.end())
      {
        int firstTokenId = tokenId;
        int lastTokenId = tokenId + transducedTokens[tokenId] - 1;
        LimaString tokenForm;
        auto ft = get(vertex_token,*anaGraph,v);
        if (ft == 0) {
          DUMPERLOGINIT;
          LWARN << "Empty token for vertex" << vertex_token << "in graph" << anaGraphData->getGraphId();
        }
        else if (ft->orthographicAlternatives().size() > 0)
        {
          StringsPoolIndex idx = *(ft->orthographicAlternatives().begin());
          tokenForm = (*m_d->sp)[idx];

          if (m_d->m_format == "CoNLL-U")
            dstream->out()  << firstTokenId << "-" << lastTokenId
                            << "\t" << tokenForm.toStdString() // FORM
                            << "\t" << "_" // LEMMA
                            << "\t" << "_" // UPOS
                            << "\t" << "_" // XPOS
                            << "\t" << "_" // FEATS
                            << "\t" << "_" // HEAD
                            << "\t" << "_" // DEPREL
                            << "\t" << "_" // DEPS
                            << "\t" << "_" // MISC
                            << std::endl;
        }
      }

      m_d->dumpPosGraphVertex(dstream,
                              v,
                              tokenId,
                              vEndDone,
                              segmentationMapping,
                              "",
                              false);


#ifdef DEBUG_LP
      LDEBUG << "ConllDumper::process look at out edges of" << v;
#endif
      LinguisticGraphOutEdgeIt outIter,outIterEnd;
      for (boost::tie(outIter,outIterEnd) = boost::out_edges(v,*posGraph);
           outIter != outIterEnd; outIter++)
      {
        LinguisticGraphVertex next = boost::target(*outIter,*posGraph);
#ifdef DEBUG_LP
        LDEBUG << "ConllDumper::process looking out vertex" << next;
#endif
        if (!visited.contains(next))
        {
#ifdef DEBUG_LP
          LDEBUG << "ConllDumper::process enqueuing" << next;
#endif
          visited.insert(next);
          toVisit.enqueue(next);
        }
      }
      if (v == sentenceEnd)
      {
        vEndDone = v;
        continue;
      }
    }
    limaConllTokenIdMapping->insert(std::make_pair(sentenceNb,
                                                   segmentationMappingReverse));

    sbItr++;
    if (sbItr != sd->getSegments().end())
    {
      dstream->out() << std::endl;
    }
  }

  return SUCCESS_ID;
}

const SpecificEntityAnnotation* ConllDumperPrivate::getSpecificEntityAnnotation(
  LinguisticGraphVertex v) const
{
  // check only entity found in current graph (not previous graph such as AnalysisGraph)

  for (const auto& vx: annotationData->matches("PosGraph", v, "annot"))
  {
    if (annotationData->hasAnnotation(vx, QString::fromUtf8("SpecificEntity")))
    {
      //BoWToken* se = createSpecificEntity(v,*it, annotationData, anagraph, posgraph, offsetBegin);
      auto se = annotationData->annotation(
        vx,
        QString::fromUtf8("SpecificEntity")).pointerValue<SpecificEntityAnnotation>();
      if (se != nullptr)
      {
        return se;
      }
    }
  }
  return nullptr;

}

LimaStatusCode ConllDumperPrivate::dumpPosGraphVertex(
  std::shared_ptr<DumperStream>& dstream,
  LinguisticGraphVertex v,
  int& tokenId,
  LinguisticGraphVertex vEndDone,
  std::map<LinguisticGraphVertex,int>& segmentationMapping,
  const QString& parentNeType,
  bool first)
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LDEBUG << "ConllDumperPrivate::dumpPosGraphVertex IN" << v;
#endif
  if (anaGraph == nullptr || posGraph == nullptr || depGraph == nullptr
    || annotationData == nullptr)
  {
    DUMPERLOGINIT;
    LERROR << "ConllDumperPrivate::dumpPosGraphVertex missing data";
    return MISSING_DATA;
  }
  bool notDone(true);
  if( v == vEndDone )
    notDone = false;

  auto ft = get(vertex_token, *posGraph, v);
  auto morphoData = get(vertex_data, *posGraph, v);
  if( morphoData != 0 && ft != 0
    && ((!morphoData->empty()) || ft->length() > 0) && notDone )
  {
#ifdef DEBUG_LP
    LDEBUG << "ConllDumperPrivate::dumpPosGraphVertex PosGraph nb different LinguisticCode"
          << morphoData->size();
#endif

    auto micro = getMicro(morphoData);
#ifdef DEBUG_LP
    LDEBUG << "ConllDumperPrivate::dumpPosGraphVertex graphTag:" << micro;
#endif

    auto feats = getFeats(morphoData);
#ifdef DEBUG_LP
    LDEBUG << "ConllDumperPrivate::dumpPosGraphVertex feats:" << feats;
#endif

    auto inflectedToken = ft->stringForm().toStdString();
    if (inflectedToken.find_first_of("\r\n\t") != std::string::npos)
      boost::find_format_all(inflectedToken,
                              boost::token_finder(!boost::is_print()),
                              character_escaper());

    QString lemmatizedToken;
    if (morphoData != 0 && !morphoData->empty())
    {
      lemmatizedToken = (*sp)[(*morphoData)[0].lemma];
    }
    // @TODO Should follow instructions here to output all MWE:
    // https://universaldependencies.org/format.html#words-tokens-and-empty-nodes
    QString neType = getNeType(v);

    // Collect NE vertices and output them instead of a single line for
    // current v. NE vertices can not only be PosGraph
    // vertices (and thus can just call dumpPosGraphVertex
    // recursively) but also AnalysisGraph vertices. In the latter case, data
    // come partly from the AnalysisGraph and partly from the PosGraph
    // Furthermore, named entities can be recursive...
    if (neType != "_")
    {
      dumpNamedEntity(dstream, v, tokenId, vEndDone, segmentationMapping, neType);
    }
    else
    {
      if (!parentNeType.isEmpty())
      {
        neType = parentNeType;
      }

      QString conllRelName;
      QString targetConllIdString;
      std::tie(conllRelName,
              targetConllIdString) = getConllRelName(v, segmentationMapping);

      QStringList miscField;
      if (neType != "_")
      {
#ifdef DEBUG_LP
      LDEBUG << "ConllDumperPrivate::dumpPosGraphVertex specific entity type is"
              << neType;
#endif
        miscField << (QString("NE=") + (first?"B-":"I-") + neType);
        const auto annot = getSpecificEntityAnnotation(v);
#ifdef DEBUG_LP
        LDEBUG << "ConllDumperPrivate::dumpPosGraphVertex specific entity annotation is"
                << annot;
#endif
        if (annot != nullptr)
        {
          const auto& features = annot->getFeatures();
          for (const auto& feature: features)
          {
            QString featureString;
            QTextStream qts(&featureString);
            if(feature.getPosition() == UNDEFPOSITION
               && !feature.getName().empty()
               && !feature.getValueString().empty())
            {
              qts << "NE-" << QString::fromStdString(feature.getName()) << "=";
              qts << Common::Misc::transcodeToXmlEntities(
                QString::fromStdString(feature.getValueString()));
            }
            miscField << featureString;
          }
        }
      }

      miscField << (QString("Pos=") + QString::number(ft->position()) );
      miscField << (QString("Len=") +  QString::number(ft->length()) );

      if(!hasSpaceAfter(v, posGraph))
      {
        miscField << QString("SpaceAfter=No");
      }

      miscField << getPredicate(v);

      if (miscField.empty())
      {
        miscField << "_";
      }

      dumpToken(dstream,
                tokenId++, // ID
                QString::fromStdString(inflectedToken), // FORM
                lemmatizedToken, // LEMMA
                micro, // UPOS
                "_", // XPOS
                feats, // FEATS
                targetConllIdString, // HEAD
                conllRelName, // DEPREL
                "_", // DEPS @TODO
                miscField, // MISC
                neType,
                previousNeType
      );
      previousNeType = neType;
    }
  }
  return SUCCESS_ID;
}

void ConllDumperPrivate::collectPredicateTokens(Lima::AnalysisContent& analysis,
                                           LinguisticGraphVertex sentenceBegin,
                                           LinguisticGraphVertex sentenceEnd)
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
#endif
  QMultiMap<LinguisticGraphVertex, AnnotationGraphVertex> result;

  auto annotationData = static_cast<AnnotationData*>(
    analysis.getData("AnnotationData"));
  if (annotationData == nullptr)
  {
    DUMPERLOGINIT;
    LERROR << "No annotation data exists: check pipeline";
    predicates = result;
    return;
  }

  auto tokenList = static_cast<AnalysisGraph*>(
    analysis.getData(m_graph.toStdString()));
  if (tokenList == nullptr)
  {
    DUMPERLOGINIT;
    LERROR << "graph" << m_graph << "has not been produced: check pipeline";
    predicates = result;
    return;
  }
  auto graph = tokenList->getGraph();

  QQueue<LinguisticGraphVertex> toVisit;
  QSet<LinguisticGraphVertex> visited;
  toVisit.enqueue(sentenceBegin);
  LinguisticGraphVertex v = 0;
  while (v != sentenceEnd && !toVisit.empty())
  {
    v = toVisit.dequeue();
#ifdef DEBUG_LP
    LDEBUG << "ConllDumperPrivate::collectPredicateTokens vertex:" << v;
#endif
    visited.insert(v);

    auto vMatches = annotationData->matches(m_graph.toStdString(), v, "annot");
    for (const auto& vMatch : vMatches)
    {
      if (annotationData->hasStringAnnotation(vMatch, "Predicate"))
      {
#ifdef DEBUG_LP
        LDEBUG << "ConllDumperPrivate::collectPredicateTokens insert"
                << v << vMatch;
#endif
        result.insert(v, vMatch);
      }
    }
    LinguisticGraphOutEdgeIt outItr,outItrEnd;
    for (boost::tie(outItr,outItrEnd)=boost::out_edges(v,*graph);
         outItr!=outItrEnd; outItr++)
    {
      auto next = boost::target(*outItr, *graph);
      if (!visited.contains(next) && next != tokenList->lastVertex())
      {
        toVisit.enqueue(next);
      }
    }
  }
  predicates = result;
}

QString ConllDumperPrivate::getNeType(LinguisticGraphVertex posGraphVertex)
{
  auto neType = QString::fromUtf8("_") ;
  if (annotationData != nullptr)
  {
    // Check if the PosGraph vertex holds a specific entity
    auto matches = annotationData->matches(m_graph.toStdString(), posGraphVertex, "annot");
    for (const auto& vx: matches)
    {
      if (annotationData->hasAnnotation(
        vx, QString::fromUtf8("SpecificEntity")))
      {
        auto se = annotationData->annotation(vx, QString::fromUtf8("SpecificEntity")).
          pointerValue<SpecificEntityAnnotation>();
        neType = MedData::single().getEntityName(se->getType());
        break;
      }
    }
    if (neType == "_")
    {
      // The PosGraph vertex did not hold a specific entity,
      // check if the AnalysisGraph vertex does
      auto anaVertices = annotationData->matches(m_graph.toStdString(), posGraphVertex,
                                                  "AnalysisGraph");
      // note: anaVertices size should be 0 or 1
      for (const auto& anaVertex: anaVertices)
      {
        auto matches = annotationData->matches("AnalysisGraph",
                                              anaVertex,
                                              "annot");
        for (const auto& vx: matches)
        {
          if (annotationData->hasAnnotation(
            vx, QString::fromUtf8("SpecificEntity")))
          {
            auto se = annotationData->annotation(
              vx, QString::fromUtf8("SpecificEntity"))
                .pointerValue<SpecificEntityAnnotation>();
            neType = MedData::single().getEntityName(se->getType());
            break;
          }
        }
        if (neType != "_") break;
      }
    }
  }
  return neType;
}

std::pair<QString, QString> ConllDumperPrivate::getConllRelName(
  LinguisticGraphVertex v,
  std::map<LinguisticGraphVertex,int>& segmentationMapping)
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LDEBUG << "ConllDumperPrivate::getConllRelName" << v;
#endif
  QString conllRelName = "_";
  int targetConllId = 0;
  if (vertexDependencyInformations.count(v) != 0)
  {
    auto target = vertexDependencyInformations.find(v)->second.first;
#ifdef DEBUG_LP
    LDEBUG << "ConllDumper::process target saved for"
            << v << "is" << target;
#endif
    if (segmentationMapping.find(target) != segmentationMapping.end())
    {
      targetConllId =  segmentationMapping.find(target)->second;
    }
    else
    {
      DUMPERLOGINIT;
      LERROR << "ConllDumper::process target" << target
              << "not found in segmentation mapping";
    }
#ifdef DEBUG_LP
    LDEBUG << "ConllDumper::process conll target saved for "
            << v << " is " << targetConllId;
#endif
    auto relName = QString::fromUtf8(
      vertexDependencyInformations.find(v)->second.second.c_str());
#ifdef DEBUG_LP
    LDEBUG << "ConllDumper::process the lima dependency tag for "
            << v << " is " << relName;
#endif
    if (m_conllLimaDepMapping.contains(relName))
    {
      conllRelName = m_conllLimaDepMapping[relName];
    }
    else
    {
      conllRelName = relName;
//             LERROR << "ConllDumper::process" << relName << "not found in mapping";
    }

    // There is no way for vertex to have 0 as head.
    if (conllRelName == "root")
    {
      targetConllId = 0;
    }
  }
  QString targetConllIdString = QString(QLatin1String("%1")).arg(targetConllId);

  return { conllRelName, targetConllIdString };
}

QStringList ConllDumperPrivate::getPredicate(LinguisticGraphVertex v)
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LDEBUG << "ConllDumperPrivate::getPredicate" << v;
#endif
  QStringList miscField;
  if (annotationData != nullptr && predicates.contains(v))
  {
    auto keys = predicates.keys();
    auto predicate = annotationData->stringAnnotation(predicates.value(v),
                                                      "Predicate");

    // Now output the roles supported by the current PoS graph token
#ifdef DEBUG_LP
    LDEBUG << "ConllDumper::process output the roles for the"
            << keys.size() << "predicates";
#endif
    for (int i = 0; i < keys.size(); i++)
    {
      auto predicateVertex = predicates.value(keys[keys.size()-1-i]);

      auto vMatches = annotationData->matches(m_graph.toStdString(), v, "annot");
      if (!vMatches.empty())
      {
#ifdef DEBUG_LP
        LDEBUG << "ConllDumper::process there is" << vMatches.size()
                << "nodes matching PoS graph vertex" << v
                << "in the annotation graph.";
#endif
        QString roleAnnotation;
        for (auto it = vMatches.begin(); it != vMatches.end(); it++)
        {
          auto vMatch = *it;
          AnnotationGraphInEdgeIt vMatchInEdgesIt, vMatchInEdgesIt_end;
          boost::tie(vMatchInEdgesIt, vMatchInEdgesIt_end) =
              boost::in_edges(vMatch,annotationData->getGraph());
          for (; vMatchInEdgesIt != vMatchInEdgesIt_end; vMatchInEdgesIt++)
          {
            auto inVertex = boost::source(*vMatchInEdgesIt,
                                          annotationData->getGraph());
            auto inVertexAnnotPosGraphMatches = annotationData->matches(
              "annot",inVertex,m_graph.toStdString());
            if (inVertex == predicateVertex
                && !inVertexAnnotPosGraphMatches.empty())
            {
              // Current edge is holding a role of the current predicate
              roleAnnotation =
                  annotationData->stringAnnotation(*vMatchInEdgesIt,
                                                    "SemanticRole");
              break;
            }
          }
        }
        if (!roleAnnotation.isEmpty() )
          predicate = roleAnnotation + ":" + predicate;
      }
    }
    if (!predicate.isEmpty())
    {
      miscField << predicate;
    }
  }
  return miscField;
}

bool ConllDumperPrivate::hasSpaceAfter(LinguisticGraphVertex v,
                                       LinguisticGraph* graph)
{
  auto ft = get(vertex_token, *graph, v);
  bool SpaceAfter = true;
  LinguisticGraphOutEdgeIt outIter, outIterEnd;
  for (boost::tie(outIter,outIterEnd) = boost::out_edges(v, *graph);
        outIter!=outIterEnd; outIter++)
  {
      auto next = boost::target(*outIter, *graph);
      auto nt = get(vertex_token, *graph, next);
      if( nt != nullptr
        && (nt->position() == ft->position()+ft->length()) )
      {
          SpaceAfter = false;
          break;
      }
  }
  return SpaceAfter;
}

QString ConllDumperPrivate::getMicro(MorphoSyntacticData* morphoData)
{
  return QString::fromUtf8(static_cast<const LangData&>(
      MedData::single().mediaData(m_language)).getPropertyCodeManager()
        .getPropertyManager("MICRO")
        .getPropertySymbolicValue(morphoData->firstValue(
          *m_propertyAccessor)).c_str());
}

QString ConllDumperPrivate::getFeats(MorphoSyntacticData* morphoData)
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
#endif

  QStringList featuresList;
  for (auto i = managers->cbegin(); i != managers->cend(); i++)
  {
    auto key = QString::fromUtf8(i->first.c_str());
    if (key != "MACRO" && key != "MICRO")
    {
      const auto& pa = propertyCodeManager->getPropertyAccessor(key.toStdString());
      LinguisticCode lc = morphoData->firstValue(pa);
      auto value = QString::fromUtf8(i->second.getPropertySymbolicValue(lc).c_str());
      if (value != "NONE")
      {
        featuresList << QString("%1=%2").arg(key).arg(value);
      }
    }
  }

  featuresList.sort();
  QString features;
  QTextStream featuresStream(&features);
  if (featuresList.isEmpty())
  {
    features = "_";
  }
  else
  {
    for (auto featuresListIt = featuresList.cbegin(); featuresListIt != featuresList.cend(); featuresListIt++)
    {
      if (featuresListIt != featuresList.cbegin())
      {
        featuresStream << "|";
      }
      featuresStream << *featuresListIt;
    }
  }
#ifdef DEBUG_LP
  LDEBUG << "ConllDumper::process features:" << features;
#endif

  return features;
}

void ConllDumperPrivate::getTransducedTokens(const LinguisticGraph& graph,
                                             LinguisticGraphVertex begin,
                                             LinguisticGraphVertex end,
                                             std::map<int, size_t>& out)
{
  QQueue<LinguisticGraphVertex> toVisit;
  QSet<LinguisticGraphVertex> visited;

  toVisit.enqueue(begin);
  int tokenId = 0;
  LinguisticGraphVertex v = 0;
  int firstTransducedTokenId = -1;
  uint64_t prev_pos = 0;

  while (!toVisit.empty() && v != end)
  {
    //as long as there are vertices in the sentence
    v = toVisit.dequeue();

    const Token *ft = get(vertex_token, graph, v);

    if (ft != nullptr && prev_pos > 0 && prev_pos == ft->position())
    {
      if (firstTransducedTokenId >= 0)
      {
        out[firstTransducedTokenId] += 1;
      }
      else
      {
        if (tokenId == 0)
          throw;
        out[tokenId - 1] = 2;
        firstTransducedTokenId = tokenId - 1;
      }
    }
    else
    {
      firstTransducedTokenId = -1;
    }

    if (ft != nullptr)
      prev_pos = ft->position();

    if (v == end)
    {
      continue;
    }

    LinguisticGraphOutEdgeIt outIter,outIterEnd;
    for (boost::tie(outIter,outIterEnd) = boost::out_edges(v,graph);
         outIter != outIterEnd; outIter++)
    {
      auto next = boost::target(*outIter, graph);
      if (!visited.contains(next))
      {
        visited.insert(next);
        toVisit.enqueue(next);
      }
    }
    tokenId++;
  }
}

QString matchesS(const std::set<AnnotationGraphVertex>& s)
{
  QString result;
  QTextStream qts(&result);
  for (auto i: s) {qts << i << ",";}
  return result;
}

void ConllDumperPrivate::dumpNamedEntity(std::shared_ptr<DumperStream>& dstream,
                                         LinguisticGraphVertex v,
                                         int& tokenId,
                                         LinguisticGraphVertex vEndDone,
                                         std::map<LinguisticGraphVertex,int>& segmentationMapping,
                                         const QString& neType)
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LDEBUG << "ConllDumperPrivate::dumpNamedEntity" << v << tokenId << vEndDone
          << neType;
#endif
  // Check if the named entity is on AnalysisGraph.
  // If so, then we have to recursively get all analysis graph tokens and
  // collect the information about them, chosing randomly the "right" category
  // Otherwise, will retrieve the pos graph tokens and recursively do the same.
  // For final tokens that are on pos graph, the category will be unique.

  if (annotationData != nullptr)
  {
    // Check if the PosGraph vertex holds a specific entity
    auto matches = annotationData->matches(m_graph.toStdString(), v, "annot");
#ifdef DEBUG_LP
    LDEBUG << "ConllDumperPrivate::dumpNamedEntity matches PosGraph" << v
            << "annot:" << matchesS(matches);
#endif
    for (const auto& vx: matches)
    {
      if (annotationData->hasAnnotation(
        vx, QString::fromUtf8("SpecificEntity")))
      {
        auto se = annotationData->annotation(vx,
                                             QString::fromUtf8("SpecificEntity"))
          .pointerValue<SpecificEntityAnnotation>();
        previousNeType = "O";
        bool first = true;
        for (const auto& vse : se->vertices())
        {
          dumpPosGraphVertex(dstream, vse, tokenId, vEndDone,
                             segmentationMapping, neType, first);
          first = false;
        }
#ifdef DEBUG_LP
        LDEBUG << "ConllDumperPrivate::dumpNamedEntity return after SpecificEntity annotation on PosGraph";
#endif
        return;
      }
    }
    auto anaVertices = annotationData->matches(m_graph.toStdString(), v, "AnalysisGraph");
#ifdef DEBUG_LP
    LDEBUG << "ConllDumperPrivate::dumpNamedEntity anaVertices for" << v
           << ":" << matchesS(anaVertices);
#endif

    assert(anaVertices.size() == 1);
    auto anaVertex = *anaVertices.begin();
#ifdef DEBUG_LP
    LDEBUG << "ConllDumperPrivate::dumpNamedEntity anaVertex is" << anaVertex;
#endif
    if (annotationData->hasAnnotation(
      anaVertex, QString::fromUtf8("SpecificEntity")))
    {
      auto se = annotationData->annotation(anaVertex,
                                            QString::fromUtf8("SpecificEntity"))
        .pointerValue<SpecificEntityAnnotation>();
#ifdef DEBUG_LP
      LDEBUG << "ConllDumperPrivate::dumpNamedEntity anaVertex se ("
              << (*sp)[se->getString()]
              << ") annotation vertices are" << se->vertices()
              << "and normalized form:" << (*sp)[se->getNormalizedForm()]
               << "and features:" << se->getFeatures();
#endif
      // All retrieved lines/tokens have the same netype. Depending on the
      // output style (CoNLL 2003, CoNLL-U, …), the generated line is different
      // and the ne-Type includes or not BIO information using in this case the
      // previousNeType member.
      previousNeType = "O";
      bool first = true;
      for (const auto& vse : se->vertices())
      {
        dumpAnalysisGraphVertex(dstream, vse, v, tokenId, vEndDone, neType,
                                first, se->getFeatures());
        first = false;
      }
      previousNeType = neType;
    }
  }
}

// TODO Split idiomatic alternative tokens and compound tokens
LimaStatusCode ConllDumperPrivate::dumpAnalysisGraphVertex(
  std::shared_ptr<DumperStream>& dstream,
  LinguisticGraphVertex v,
  LinguisticGraphVertex posGraphVertex,
  int& tokenId,
  LinguisticGraphVertex vEndDone,
  const QString& neType,
  bool first,
  const Automaton::EntityFeatures& features)
{
  LIMA_UNUSED(posGraphVertex);
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LDEBUG << "ConllDumperPrivate::dumpAnalysisGraphVertex" << v << posGraphVertex
          << neType;
#endif
  if (anaGraph == nullptr || posGraph == nullptr || depGraph == nullptr
    || annotationData == nullptr)
  {
    DUMPERLOGINIT;
    LERROR << "ConllDumperPrivate::dumpAnalysisGraphVertex missing data";
    return MISSING_DATA;
  }
  bool notDone(true);
  if( v == vEndDone )
    notDone = false;

  auto ft = get(vertex_token, *anaGraph, v);
  auto morphoData = get(vertex_data, *anaGraph, v);
#ifdef DEBUG_LP
  LDEBUG << "ConllDumperPrivate::dumpAnalysisGraphVertex PosGraph token" << v;
#endif
  if( morphoData != 0 && ft != 0
    && ((!morphoData->empty()) || ft->length() > 0) && notDone )
  {
#ifdef DEBUG_LP
    LDEBUG << "ConllDumperPrivate::dumpAnalysisGraphVertex PosGraph nb different LinguisticCode"
          << morphoData->size();
#endif

    auto micro = getMicro(morphoData);
#ifdef DEBUG_LP
    LDEBUG << "ConllDumperPrivate::dumpAnalysisGraphVertex micro:" << micro;
#endif

    auto feats = getFeats(morphoData);
#ifdef DEBUG_LP
    LDEBUG << "ConllDumperPrivate::dumpAnalysisGraphVertex feats:" << feats;
#endif

    auto inflectedToken = ft->stringForm().toStdString();
    if (inflectedToken.find_first_of("\r\n\t") != std::string::npos)
      boost::find_format_all(inflectedToken,
                              boost::token_finder(!boost::is_print()),
                              character_escaper());

    QString lemmatizedToken;
    if (morphoData != 0 && !morphoData->empty())
    {
      lemmatizedToken = (*sp)[(*morphoData)[0].lemma];
    }
    // @TODO Should follow instructions here to output all MWE:
    // https://universaldependencies.org/format.html#words-tokens-and-empty-nodes

    // TODO Get correct UD dep relation for relations inside the named entity
    // and for the token that must be linked to the outside. For this one, the
    // relation is the one which links to posGraphVertex to the rest of the pos
    // graph.
    QString conllRelName = "_";
    QString targetConllIdString = "_";

    QStringList miscField;
    if (neType != "_")
    {
#ifdef DEBUG_LP
      LDEBUG << "ConllDumperPrivate::dumpAnalysisGraphVertex specific entity type is" << neType;
      LDEBUG << "ConllDumperPrivate::dumpAnalysisGraphVertex posGraphVertex is" << posGraphVertex;
#endif
      miscField << (QString("NE=") + (first?"B-":"I-") + neType);
      for (const auto& feature: features)
      {
        if(!feature.getName().empty()
           && !feature.getValueString().empty())
        {
          QString featureString;
          QTextStream qts(&featureString);
          qts << "NE-" << QString::fromStdString(feature.getName()) << "=";
          qts << Common::Misc::transcodeToXmlEntities(
            QString::fromStdString(feature.getValueString()));
          miscField << featureString;
        }
      }
    }

    miscField << (QString("Pos=") + QString::number(ft->position()) );
    miscField << (QString("Len=") +  QString::number(ft->length()) );

    if(!hasSpaceAfter(v, anaGraph))
    {
      miscField << QString("SpaceAfter=No");
    }

    miscField << getPredicate(v);

    if (miscField.empty())
    {
      miscField << "_";
    }
    dumpToken(dstream,
              tokenId++, // ID
              QString::fromStdString(inflectedToken), // FORM
              lemmatizedToken, // LEMMA
              micro, // UPOS
              "_", // XPOS
              feats, // FEATS
              targetConllIdString, // HEAD
              conllRelName, // DEPREL
              "_", // DEPS @TODO
              miscField, // MISC
              neType,
              previousNeType
    );
  }
  return SUCCESS_ID;
}

void ConllDumperPrivate::dumpToken(
  std::shared_ptr<DumperStream>& dstream,
  int tokenId, // ID
  const QString& inflectedToken, // FORM
  const QString& lemmatizedToken, // LEMMA
  const QString& micro, // UPOS
  const QString& xpos, // XPOS
  const QString& features,// FEATS @TODO
  const QString& targetConllIdString, // HEAD
  const QString& conllRelName, // DEPREL
  const QString& deps, // DEPS @TODO
  const QStringList& miscField,
  const QString& neType,
  const QString& previousNeType)
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LDEBUG << "ConllDumperPrivate::dumpToken" << tokenId;
#endif
  if (m_format == "CoNLL-U")
  {
    // CONLL-U format
    // https://universaldependencies.org/format.html
    //
    // ID: Word index, integer starting at 1 for each new sentence; may be a
    //      range for multiword tokens; may be a decimal number for empty
    //      nodes (decimal numbers can be lower than 1 but must be greater
    //      than 0).
    // FORM: Word form or punctuation symbol.
    // LEMMA: Lemma or stem of word form.
    // UPOS: Universal part-of-speech tag.
    // XPOS: Language-specific part-of-speech tag; underscore if not
    //        available.
    // FEATS: List of morphological features from the universal feature
    //        inventory or from a defined language-specific extension;
    //        underscore if not available (this is the case currently in
    //        LIMA).
    // HEAD: Head of the current word, which is either a value of ID or
    //        zero (0).
    // DEPREL: Universal dependency relation to the HEAD (root iff HEAD = 0)
    //          or a defined language-specific subtype of one.
    // DEPS: Enhanced dependency graph in the form of a list of head-deprel
    //        pairs. Currently unavailable in LIMA, thus underscore.
    // MISC: Any other annotation. In LIMA, named entities and SRL
    //        information.

    dstream->out()  << tokenId++ // ID
                    << "\t" << inflectedToken.toStdString() // FORM
                    << "\t" << lemmatizedToken.toStdString() // LEMMA
                    << "\t" << micro.toStdString() // UPOS
                    << "\t" << xpos.toStdString() // XPOS
                    << "\t" << features.toStdString() // FEATS @TODO
                    << "\t" << targetConllIdString.toStdString() // HEAD
                    << "\t" << conllRelName.toStdString() // DEPREL
                    << "\t" << deps.toStdString() // DEPS @TODO
                    << "\t" << miscField.join('|').toStdString(); // MISC
    dstream->out() << std::endl;
  }
  else if (m_format == "CoNLL-03")
  {
    // CONLL 2003 format
    //
    // -DOCSTART- -X- O O
    //
    // CRICKET NNP I-NP O
    // - : O O
    // LEICESTERSHIRE NNP I-NP I-ORG
    // TAKE NNP I-NP O


#ifdef DEBUG_LP
    LDEBUG << "ConllDumperPrivate::dumpToken" << tokenId
            << inflectedToken.toStdString() << neType << previousNeType;
#endif
    QString inflectedTokenEscaped = inflectedToken;
    inflectedTokenEscaped.replace(" ", "_");
    dstream->out()  << inflectedTokenEscaped.toStdString() // FORM
                    << " " << micro.toStdString() // UPOS
                    << " " << "I-NP";
    if (neType.isEmpty() || neType == "_")
    {
      dstream->out() << " " << "O";
    }
    else
    {
      if (neType == previousNeType)
      {
        dstream->out() << " " << "I-";
      }
      else
      {
        dstream->out() << " " << "B-";
      }
      dstream->out() << neType.toStdString();
    }
    dstream->out() << std::endl;
  }
  else
  {
    DUMPERLOGINIT;
    QString errorMessage;
    QTextStream qts(&errorMessage);
    qts << "ConllDumper::dumpToken unknown format"  << m_format;
    LERROR << errorMessage;
    throw std::runtime_error(errorMessage.toStdString());
  }
}

} // end namespace
} // end namespace
} // end namespace
