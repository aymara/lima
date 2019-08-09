/*
    Copyright 2002-2019 CEA LIST

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
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/core/AnalysisDumpers/EasyXmlDumper/ConstituantAndRelationExtractor.h"
#include "linguisticProcessing/core/AnalysisDumpers/EasyXmlDumper/relation.h"
#include "linguisticProcessing/core/SemanticAnalysis/LimaConllTokenIdMapping.h"

#include <QQueue>
#include <QSet>
#include <QStringList>

#include <fstream>

using namespace Lima::Common;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace Lima::LinguisticProcessing::SemanticAnalysis;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

using MedData = Lima::Common::MediaticData::MediaticData ;

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDumpers
{

SimpleFactory<MediaProcessUnit,ConllDumper> conllDumperFactory(CONLLDUMPER_CLASSID);

class ConllDumperPrivate
{
  friend class ConllDumper;
  ConllDumperPrivate();

  virtual ~ConllDumperPrivate();

  /**
   * @brief Collect all annotation tokens corresponding to a predicate of the
   * sentence starting at @ref sentenceBegin and finishing at @ref sentenceEnd
   */
  QMultiMap<LinguisticGraphVertex, AnnotationGraphVertex>  collectPredicateTokens(
                                  Lima::AnalysisContent& analysis, LinguisticGraphVertex sentenceBegin, LinguisticGraphVertex sentenceEnd);

  MediaId m_language;

  QString m_graph;
  QMap<QString, QString> m_conllLimaDepMapping;
  const Common::PropertyCode::PropertyAccessor* m_propertyAccessor;
  bool m_fakeDependencyGraph; //< set to true to generate a fake dependency graph compatible with conll eval tool
};


ConllDumperPrivate::ConllDumperPrivate():
  m_language(0),
  m_graph("PosGraph"),
  m_conllLimaDepMapping(),
  m_fakeDependencyGraph(false)
{
}

ConllDumperPrivate::~ConllDumperPrivate()
{}

ConllDumper::ConllDumper():
AbstractTextualAnalysisDumper(),
m_d(new ConllDumperPrivate())
{
}

ConllDumper::~ConllDumper()
{
  delete m_d;
}

void ConllDumper::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                      Manager* manager)
{
  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);
  m_d->m_language=manager->getInitializationParameters().media;
  try
  {
    m_d->m_graph = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("graph").c_str());
  }
  catch (NoSuchParam& ) {} // keep default value
  const Common::PropertyCode::PropertyCodeManager& codeManager=static_cast<const Common::MediaticData     ::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_d->m_language)).getPropertyCodeManager();
  m_d->m_propertyAccessor=&codeManager.getPropertyAccessor("MICRO");

  try
  {
    std::string resourcePath = MedData::single().getResourcesPath();
    QString mappingFile =  findFileInPaths(resourcePath.c_str(),
                                           unitConfiguration.getParamsValueAtKey(
                                             "mappingFile").c_str());
    std::ifstream ifs(mappingFile.toStdString(), std::ifstream::binary);
    if (!ifs.good())
    {
      DUMPERLOGINIT;
      LERROR << "ConllDumper::init ERROR: cannot open mapping file"
              << mappingFile;
      throw InvalidConfiguration();
    }
    while (ifs.good() && !ifs.eof())
    {
      std::string line = readLine(ifs);
      QStringList strs = QString::fromUtf8(line.c_str()).split('\t');
      if (strs.size() == 2)
      {
        m_d->m_conllLimaDepMapping.insert(strs[0],strs[1]);
      }
    }

  } catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    DUMPERLOGINIT;
    LINFO << "no parameter 'mappingFile' in ConllDumper group" << " !";
//     throw InvalidConfiguration();
  }


  try
  {
    m_d->m_fakeDependencyGraph = unitConfiguration.getBooleanParameter(
      "fakeDependencyGraph");
  } catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    DUMPERLOGINIT;
    LINFO << "no parameter 'fakeDependencyGraph' in ConllDumper group. Use default (false)" << " !";
  }

}

LimaStatusCode ConllDumper::process(AnalysisContent& analysis) const
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LDEBUG << "ConllDumper::process";
#endif

  const auto& mediaData = static_cast<const LanguageData&>(
          MedData::single().mediaData(m_d->m_language));
  const auto& propertyCodeManager = mediaData.getPropertyCodeManager();
  const auto& managers = propertyCodeManager.getPropertyManagers();

  auto metadata = static_cast<LinguisticMetaData*>(
    analysis.getData("LinguisticMetaData"));
  if (metadata == nullptr)
  {
    DUMPERLOGINIT;
    LERROR << "ConllDumper::process no LinguisticMetaData ! abort";
    return MISSING_DATA;
  }

  auto annotationData = static_cast<AnnotationData*>(
    analysis.getData("AnnotationData"));
  if (annotationData == nullptr)
  {
    DUMPERLOGINIT;
    LINFO << "ConllDumper::process no AnnotationData ! Will not contain NE nor predicates";
  }
  auto tokenList = static_cast<AnalysisGraph*>(analysis.getData(
    m_d->m_graph.toStdString()));//est de type PosGraph et non pas AnalysisGraph
  if (tokenList == nullptr)
  {
    DUMPERLOGINIT;
    LERROR << "ConllDumper::process graph" << m_d->m_graph
            << "has not been produced: check pipeline";
    return MISSING_DATA;
  }
  auto graph = tokenList->getGraph();
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
    syntacticData = new SyntacticData(tokenList,0);
    syntacticData->setupDependencyGraph();
    analysis.setData("SyntacticData", syntacticData);
  }
  const auto depGraph = syntacticData-> dependencyGraph();

  QScopedPointer<DumperStream> dstream(initialize(analysis));

  // For each dependency relation accessible from the beginning of the graph,
  // associate the PoS graph vertex of its source to the PoS graph vertex of
  // its target and the type of the dependency
  std::map< LinguisticGraphVertex,
            std::pair<LinguisticGraphVertex,
                      std::string> > vertexDependencyInformations;

  auto nbSentences = ((sd->getSegments()).size());
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


    const auto& sp = MedData::single().stringsPool(
      m_d->m_language);
//   for (auto im=m_d->m_conllLimaDepMapping.begin();im!=m_d->m_conllLimaDepMapping.end();im++)
//   {
//     LDEBUG << "("<< (*im).first<< "," << (*im).second << ")" << endl;
//   }

  auto limaConllTokenIdMapping = static_cast<LimaConllTokenIdMapping*>(
    analysis.getData("LimaConllTokenIdMapping"));
  if (limaConllTokenIdMapping == nullptr)
  {
    limaConllTokenIdMapping = new LimaConllTokenIdMapping();
    analysis.setData("LimaConllTokenIdMapping", limaConllTokenIdMapping);
  }
  size_t sentenceNb = 0;
  LinguisticGraphVertex vEndDone = 0;

  while (sbItr != sd->getSegments().end() ) //for each sentence
  {
    sentenceNb++;
    dstream->out() << "# sent_id = " << sentenceNb << std::endl;
    // @TODO Fill the text below. It is mandatory for CONLL-U format
    dstream->out() << "# text = " << "" << std::endl;
    sentenceBegin = sbItr->getFirstVertex();
    sentenceEnd = sbItr->getLastVertex();

    // mapping the two types of tokens (Lima and conll):
    // PoS graph tokens to an id unique for this sentence for each.
    // Filled during the first traversal of the sentence.
    // Note that if there is cross-sentence dependencies, both vertices will
    // not be present in this mapping at the same time.
    std::map<LinguisticGraphVertex, int> segmentationMapping;
    // and reverse
    std::map<int,LinguisticGraphVertex> segmentationMappingReverse;

#ifdef DEBUG_LP
    LDEBUG << "ConllDumper::process begin - end: " << sentenceBegin
            << " - " << sentenceEnd;
#endif
    //LinguisticGraphOutEdgeIt outItr,outItrEnd;
    QQueue<LinguisticGraphVertex> toVisit;
    QSet<LinguisticGraphVertex> visited;
    toVisit.enqueue(sentenceBegin);
    int tokenId = 0;
    LinguisticGraphVertex v = 0;
    uint64_t previous = std::numeric_limits<uint64_t>::max();

    // First traversing of the sentence to fill the
    // vertexDependencyInformations and segmentationMapping data structures
    while (v != sentenceEnd && !toVisit.empty())
    {
      v = toVisit.dequeue();
#ifdef DEBUG_LP
      LDEBUG << "ConllDumper::process Vertex index : " << v;
#endif
      visited.insert(v);
      segmentationMapping.insert(std::make_pair(v,tokenId));
      segmentationMappingReverse.insert(std::make_pair(tokenId,v));
#ifdef DEBUG_LP
      LDEBUG << "ConllDumper::process conll id : " << tokenId
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
        auto typeMap = get(edge_deprel_type, *depGraph);
        auto type = typeMap[*dit];
        auto syntRelName = static_cast<const LanguageData&>(
          mediaData).getSyntacticRelationName(type);
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
        auto dest =
            syntacticData->tokenVertexForDepVertex(boost::target(*dit,
                                                                  *depGraph));
#ifdef DEBUG_LP
        LDEBUG << "ConllDumper::process Targ : Morph vertex= " << dest;
#endif
        if (syntRelName != "")
        {
#ifdef DEBUG_LP
          LDEBUG << "ConllDumper::process saving target for"
                  << v << ":" << dest << syntRelName;
#endif
          vertexDependencyInformations.insert(
              std::make_pair(v, std::make_pair(dest,syntRelName)));
        }
      }
      if (v == sentenceEnd)
      {
        continue;
      }
      LinguisticGraphOutEdgeIt outItr,outItrEnd;
      for (boost::tie(outItr,outItrEnd) = boost::out_edges(v, *graph);
           outItr != outItrEnd; outItr++)
      {
        auto next = boost::target(*outItr,*graph);
        if (!visited.contains(next) && next != tokenList->lastVertex())
        {
          toVisit.enqueue(next);
        }
      }
      ++tokenId;
    }

  // instead of looking to all vertices, follow the graph (in
  // morphological graph, some vertices are not related to main graph:
  // idiomatic expressions parts and named entity parts)

    toVisit.clear();
    visited.clear();

    sentenceBegin=sbItr->getFirstVertex();
    sentenceEnd=sbItr->getLastVertex();

    // get the list of predicates for the current sentence
    auto predicates = m_d->collectPredicateTokens( analysis,
                                                   sentenceBegin,
                                                   sentenceEnd );
#ifdef DEBUG_LP
    LDEBUG << "ConllDumper::process predicates for sentence between"
            << sentenceBegin << "and" << sentenceEnd << "are:" << predicates;
#endif
    auto keys = predicates.keys();

    toVisit.enqueue(sentenceBegin);
    tokenId = 0;
    v = 0;

    // Second traversing of the sentence
    while (!toVisit.empty() && v!=sentenceEnd)
    { //as long as there are vertices in the sentence
      v = toVisit.dequeue();
      auto notDone = true;
      if( v == vEndDone )
        notDone = false;

      auto ft = get(vertex_token,*graph,v);
      auto morphoData = get(vertex_data,*graph, v);
#ifdef DEBUG_LP
      LDEBUG << "ConllDumper::process PosGraph token" << v;
#endif
      if( morphoData != nullptr && ft != nullptr
        && ((!morphoData->empty()) || ft->length() > 0) && notDone )
      {
#ifdef DEBUG_LP
        LDEBUG << "ConllDumper::process PosGraph nb different LinguisticCode"
                << morphoData->size();
#endif
//         const QString macro=QString::fromUtf8(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_d->m_language)).getPropertyCodeManager().getPropertyManager("MACRO").getPropertySymbolicValue(morphoData->firstValue(*m_d->m_propertyAccessor)).c_str());
        const auto micro = QString::fromStdString(
          static_cast<const Common::MediaticData::LanguageData&>(
            Common::MediaticData::MediaticData::single().mediaData(
              m_d->m_language)).getPropertyCodeManager().getPropertyManager("MICRO").getPropertySymbolicValue(
                morphoData->firstValue(*m_d->m_propertyAccessor)));
#ifdef DEBUG_LP
        LDEBUG << "ConllDumper::process graphTag:" << micro;
#endif
        QStringList featuresList;
        for (auto propItr = managers.cbegin();
             propItr != managers.cend(); propItr++)
        {
          auto key = QString::fromStdString(propItr->first);
          if (key != "MACRO" && key != "MICRO")
          {
            auto value = QString::fromStdString(
              propItr->second.getPropertySymbolicValue(
                morphoData->firstValue(*m_d->m_propertyAccessor)));
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
          for (auto featuresListIt = featuresList.cbegin();
               featuresListIt != featuresList.cend(); featuresListIt++)
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

        auto inflectedToken = ft->stringForm().toStdString();
        if (inflectedToken.find_first_of("\r\n\t") != std::string::npos)
          boost::find_format_all(inflectedToken,
                                 boost::token_finder(!boost::is_print()),
                                 character_escaper());

        std::string lemmatizedToken;
        if (morphoData != nullptr && !morphoData->empty())
        {
          lemmatizedToken = sp[(*morphoData)[0].lemma].toStdString();
          if (lemmatizedToken.find_first_of("\r\n\t") != std::string::npos)
            boost::find_format_all(lemmatizedToken,
                                   boost::token_finder(!boost::is_print()),
                                   character_escaper());
        }

        if (lemmatizedToken.empty())
          lemmatizedToken = inflectedToken;

        // @TODO Should follow instructions here to output all MWE:
        // https://universaldependencies.org/format.html#words-tokens-and-empty-nodes
        auto neType = QString::fromUtf8("_") ;
        if (annotationData != nullptr)
        {
          auto anaVertices = annotationData->matches("PosGraph", v,
                                                     "AnalysisGraph");
          // note: anaVertices size should be 0 or 1
          for (const auto& anaVertex: anaVertices)
          {
            auto matches = annotationData->matches("AnalysisGraph",
                                                   anaVertex,
                                                   "annot");
            for (const auto& vx: matches)
            {
              if (annotationData->hasAnnotation(vx,
                QString::fromUtf8("SpecificEntity")))
              {
                auto se = annotationData->annotation(vx, QString::fromUtf8("SpecificEntity")).
                  pointerValue<SpecificEntityAnnotation>();
                neType = MedData::single().getEntityName(se->getType());
                break;
              }
            }
            if (neType != "_") break;
          }
        }
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
                    << "not found in token to CoNLL id mapping for" << v << "-"
                    << vertexDependencyInformations.find(v)->second.second
                    << "->" << target ;
            LERROR << "    This relation is a cross-sentence relation. This is a parsing error.";
          }
#ifdef DEBUG_LP
          LDEBUG << "ConllDumper::process conll target saved for "
                  << tokenId << " is " << targetConllId;
#endif
          QString relName = QString::fromUtf8(
            vertexDependencyInformations.find(v)->second.second.c_str());
#ifdef DEBUG_LP
          LDEBUG << "ConllDumper::process the lima dependency tag for "
                 << ft->stringForm()<< " is " << relName;
#endif
          if (m_d->m_conllLimaDepMapping.contains(relName))
          {
            conllRelName=m_d->m_conllLimaDepMapping[relName];
          }
          else
          {
            conllRelName= "nsubj";
            DUMPERLOGINIT;
            LWARN << "ConllDumper::process" << relName
                    << "not found in LIMA to CoNLL dependencies mapping. Using 'nsubj'";
          }
        }

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

        auto targetConllIdString = targetConllId > 0
                                    ? QString("%1").arg(targetConllId)
                                    : QString("0");
        auto head = targetConllIdString;
        auto deprel = conllRelName;
        if (m_d->m_fakeDependencyGraph)
        {
          if (tokenId == 1)
          {
            head = "0";
            deprel = "root";
          }
          else
          {
            head = "1";
            deprel = "nsubj";
          }
        }
        if (0 == tokenId)
        {
            DUMPERLOGINIT;
            LERROR << "ConllDumper::process: tokenId == 0.";
            throw LimaException();
        }
        auto position = ft->position();
#ifdef DEBUG_LP
        LDEBUG << "ConllDumper::process previous:" << previous
                << "; position:" << position;
#endif
        if (position != previous)
        {
          dstream->out()  << tokenId // ID
                          << "\t" << inflectedToken // FORM
                          << "\t" << lemmatizedToken // LEMMA
                          << "\t" << micro.toStdString() // UPOS
                          << "\t" << "_" // XPOS
                          << "\t" << "_" // FEATS
                          << "\t" << head.toStdString() // HEAD
                          << "\t" << deprel.toStdString() // DEPREL
                          << "\t" << "_"; // DEPS
          QStringList miscField;
          if (neType != "_")
          {
            miscField << (QString("NE=") + neType);
          }
  //           LDEBUG << "ConllDumper::process output the predicate if any";
          if (annotationData != nullptr && predicates.contains(v))
          {
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

              auto vMatches = annotationData->matches("PosGraph", v, "annot");
              if (!vMatches.empty())
              {
  #ifdef DEBUG_LP
                LDEBUG << "ConllDumper::process there is" << vMatches.size()
                        << "nodes matching PoS graph vertex" << v
                        << "in the annotation graph.";
  #endif
                QString roleAnnotation;
                for (auto vMatch: vMatches)
                {
                  AnnotationGraphInEdgeIt vMatchInEdgesIt, vMatchInEdgesIt_end;
                  boost::tie(vMatchInEdgesIt, vMatchInEdgesIt_end) =
                      boost::in_edges(vMatch,annotationData->getGraph());
                  for (; vMatchInEdgesIt != vMatchInEdgesIt_end; vMatchInEdgesIt++)
                  {
                    auto inVertex = boost::source(*vMatchInEdgesIt,
                                                  annotationData->getGraph());
                    auto inVertexAnnotPosGraphMatches =
                        annotationData->matches("annot", inVertex, "PosGraph");
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
          if (miscField.empty())
          {
            miscField << "_";
          }
          dstream->out() << "\t" << miscField.join('|').toStdString(); // MISC
          dstream->out() << std::endl;
        }
        previous = position;
      }

      if (v == sentenceEnd)
      {
        vEndDone = v;
        continue;
      }
#ifdef DEBUG_LP
      LDEBUG << "ConllDumper::process look at out edges of" << v;
#endif
      LinguisticGraphOutEdgeIt outIter,outIterEnd;
      for (boost::tie(outIter,outIterEnd) = boost::out_edges(v,*graph);
           outIter != outIterEnd; outIter++)
      {
        auto next = boost::target(*outIter, *graph);
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
      tokenId++;
    }
    dstream->out() << std::endl;
    limaConllTokenIdMapping->insert(std::make_pair(sentenceNb,
                                                   segmentationMappingReverse));
    sbItr++;
  }

  return SUCCESS_ID;
}


QMultiMap<LinguisticGraphVertex, AnnotationGraphVertex>
ConllDumperPrivate::collectPredicateTokens(Lima::AnalysisContent& analysis,
                                           LinguisticGraphVertex sentenceBegin,
                                           LinguisticGraphVertex sentenceEnd)
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
#endif
  QMap<LinguisticGraphVertex, AnnotationGraphVertex> result;

  auto annotationData = static_cast<AnnotationData*>(
    analysis.getData("AnnotationData"));
  if (annotationData == nullptr)
    return result;
  auto tokenList = static_cast<AnalysisGraph*>(
    analysis.getData(m_graph.toStdString()));
  if (tokenList == nullptr)
  {
    DUMPERLOGINIT;
    LERROR << "graph " << m_graph << " has not been produced: check pipeline";
    return result;
  }
  auto graph = tokenList->getGraph();


  QQueue<LinguisticGraphVertex> toVisit;
  QSet<LinguisticGraphVertex> visited;
  toVisit.enqueue(sentenceBegin);
  LinguisticGraphVertex v = 0;
  while (v!=sentenceEnd && !toVisit.empty())
  {
    v = toVisit.dequeue();
#ifdef DEBUG_LP
    LDEBUG << "ConllDumperPrivate::collectPredicateTokens vertex:" << v;
#endif
    visited.insert(v);

    auto vMatches = annotationData->matches("PosGraph", v, "annot");
    for (auto vMatch: vMatches)
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
  return result;
}

} // end namespace
} // end namespace
} // end namespace
