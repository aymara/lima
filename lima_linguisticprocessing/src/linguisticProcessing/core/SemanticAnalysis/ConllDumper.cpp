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
};


ConllDumperPrivate::ConllDumperPrivate():
  m_language(0),
  m_graph("PosGraph"),
  m_conllLimaDepMapping()
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
  DUMPERLOGINIT;
  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);
  m_d->m_language=manager->getInitializationParameters().media;
  try
  {
    m_d->m_graph = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("graph").c_str());
  }
  catch (NoSuchParam& ) {} // keep default value

  try
  {
    std::string resourcePath = Common::MediaticData::MediaticData::single().getResourcesPath();
    QString mappingFile =  findFileInPaths(resourcePath.c_str(), unitConfiguration.getParamsValueAtKey("mappingFile").c_str());
    std::ifstream ifs(mappingFile.toUtf8().constData(), std::ifstream::binary);
    if (!ifs.good())
    {
      LERROR << "ERROR: cannot open" << mappingFile;
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
    LINFO << "no parameter 'mappingFile' in ConllDumper group" << " !";
//     throw InvalidConfiguration();
  }
}

LimaStatusCode ConllDumper::process(AnalysisContent& analysis) const
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LDEBUG << "ConllDumper::process";
#endif

  const auto& mediaData = static_cast<const LanguageData&>(
          Lima::Common::MediaticData::MediaticData::single().mediaData(m_d->m_language));
  const auto& propertyCodeManager = mediaData.getPropertyCodeManager();
  const auto& macroManager = propertyCodeManager.getPropertyManager("MACRO");
  const auto& microManager = propertyCodeManager.getPropertyManager("MICRO");
  const auto& managers = propertyCodeManager.getPropertyManagers();

  LinguisticMetaData* metadata = static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0)
  {
    DUMPERLOGINIT;
    LERROR << "ConllDumper::process no LinguisticMetaData ! abort";
    return MISSING_DATA;
  }

  AnnotationData* annotationData = static_cast<AnnotationData*>(analysis.getData("AnnotationData"));
  if (annotationData == 0)
  {
    DUMPERLOGINIT;
    LERROR << "ConllDumper::process no AnnotationData ! abort";
    return MISSING_DATA;
  }
  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData(m_d->m_graph.toUtf8().constData()));//est de type PosGraph et non pas AnalysisGraph
  if (tokenList==0)
  {
    DUMPERLOGINIT;
    LERROR << "ConllDumper::process graph" << m_d->m_graph << "has not been produced: check pipeline";
    return MISSING_DATA;
  }
  LinguisticGraph* graph=tokenList->getGraph();
  SegmentationData* sd=static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));
  if (sd==0)
  {
    DUMPERLOGINIT;
    LERROR << "ConllDumper::process no SentenceBoundaries! abort";
    return MISSING_DATA;
  }

  SyntacticData* syntacticData=static_cast<SyntacticData*>(analysis.getData("SyntacticData"));
  if (syntacticData==0)
  {
    syntacticData=new SyntacticData(tokenList,0);
    syntacticData->setupDependencyGraph();
    analysis.setData("SyntacticData",syntacticData);
  }
  const DependencyGraph* depGraph = syntacticData-> dependencyGraph();

  QScopedPointer<DumperStream> dstream(initialize(analysis));

  std::map< LinguisticGraphVertex, std::pair<LinguisticGraphVertex, std::string> > vertexDependencyInformations;

  uint64_t nbSentences((sd->getSegments()).size());
  if (nbSentences == 0)
  {
    DUMPERLOGINIT;
    LERROR << "ConllDumper::process 0 sentence to process";
    return SUCCESS_ID;
  }

  std::vector<Segment>::iterator sbItr=(sd->getSegments().begin());
#ifdef DEBUG_LP
  LDEBUG << "ConllDumper::process There are "<< nbSentences << " sentences";
#endif
  LinguisticGraphVertex sentenceBegin = sbItr->getFirstVertex();
  LinguisticGraphVertex sentenceEnd = sbItr->getLastVertex();


    const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_d->m_language);
//   for (auto im=m_d->m_conllLimaDepMapping.begin();im!=m_d->m_conllLimaDepMapping.end();im++)
//   {
//     LDEBUG << "("<< (*im).first<< "," << (*im).second << ")" << endl;
//   }

  LimaConllTokenIdMapping* limaConllTokenIdMapping =
      static_cast<LimaConllTokenIdMapping*>(analysis.getData("LimaConllTokenIdMapping"));
  if (limaConllTokenIdMapping == 0)
  {
    limaConllTokenIdMapping = new LimaConllTokenIdMapping();
    analysis.setData("LimaConllTokenIdMapping", limaConllTokenIdMapping);
  }
  int sentenceNb=0;
  LinguisticGraphVertex vEndDone = 0;

  while (sbItr != sd->getSegments().end() ) //for each sentence
  {
    sentenceNb++;
    dstream->out() << "# sent_id = " << sentenceNb << std::endl;
    // @TODO Fill the text below. It is mandatory for CONLL-U format
    dstream->out() << "# text = " << "" << std::endl;
    sentenceBegin=sbItr->getFirstVertex();
    sentenceEnd=sbItr->getLastVertex();
    std::map<LinguisticGraphVertex,int>segmentationMapping;//mapping the two types of segmentations (Lima and conll)
    std::map<int,LinguisticGraphVertex>segmentationMappingReverse;

#ifdef DEBUG_LP
    LDEBUG << "ConllDumper::process begin - end: " << sentenceBegin << " - " << sentenceEnd;
#endif
    //LinguisticGraphOutEdgeIt outItr,outItrEnd;
    QQueue<LinguisticGraphVertex> toVisit;
    QSet<LinguisticGraphVertex> visited;
    toVisit.enqueue(sentenceBegin);
    int tokenId = 0;
    LinguisticGraphVertex v = 0;
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
      LDEBUG << "ConllDumper::process conll id : " << tokenId << " Lima id : " << v;
#endif
      DependencyGraphVertex dcurrent = syntacticData->depVertexForTokenVertex(v);
      DependencyGraphOutEdgeIt dit, dit_end;
      boost::tie(dit,dit_end) = boost::out_edges(dcurrent,*depGraph);
      for (; dit != dit_end; dit++)
      {
#ifdef DEBUG_LP
        LDEBUG << "ConllDumper::process Dumping dependency edge " << (*dit).m_source << " -> " << (*dit).m_target;
#endif
        try
        {
          CEdgeDepRelTypePropertyMap typeMap = get(edge_deprel_type, *depGraph);
          SyntacticRelationId type = typeMap[*dit];
          std::string syntRelName=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_d->m_language)).getSyntacticRelationName(type);
#ifdef DEBUG_LP
          LDEBUG << "ConllDumper::process relation = " << syntRelName;
          LDEBUG << "ConllDumper::process Src  : Dep vertex= "
                  << boost::source(*dit, *depGraph);
          LinguisticGraphVertex src = syntacticData->tokenVertexForDepVertex(
              boost::source(*dit, *depGraph));
          LDEBUG << "ConllDumper::process Src  : Morph vertex= " << src;
          LDEBUG << "ConllDumper::process Targ : Dep vertex= "
                  << boost::target(*dit, *depGraph);
#endif
          LinguisticGraphVertex dest =
              syntacticData->tokenVertexForDepVertex(boost::target(*dit,
                                                                   *depGraph));
#ifdef DEBUG_LP
          LDEBUG << "ConllDumper::process Targ : Morph vertex= " << dest;
#endif
          if (syntRelName!="")
          {
#ifdef DEBUG_LP
            LDEBUG << "ConllDumper::process saving target for"
                    << v << ":" << dest << syntRelName;
#endif
            vertexDependencyInformations.insert(
                std::make_pair(v, std::make_pair(dest,syntRelName)));
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
      for (boost::tie(outItr,outItrEnd)=boost::out_edges(v,*graph);
           outItr!=outItrEnd; outItr++)
      {
        LinguisticGraphVertex next=boost::target(*outItr,*graph);
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
    QList< LinguisticGraphVertex > keys = predicates.keys();

    toVisit.enqueue(sentenceBegin);
    tokenId=0;
    v=0;
    while (!toVisit.empty() && v!=sentenceEnd)
    { //as long as there are vertices in the sentence
      v = toVisit.dequeue();
      bool notDone(true);
      if( v == vEndDone )
        notDone = false;

      Token* ft=get(vertex_token,*graph,v);
      MorphoSyntacticData* morphoData = get(vertex_data,*graph, v);
#ifdef DEBUG_LP
      LDEBUG << "ConllDumper::process PosGraph token" << v;
#endif
      if( morphoData != 0 && ft != 0 && ((!morphoData->empty()) || ft->length() > 0) && notDone )
      {
#ifdef DEBUG_LP
        LDEBUG << "ConllDumper::process PosGraph nb different LinguisticCode"
              << morphoData->size();
#endif
        LinguisticCode code = (*morphoData->begin()).properties;
//         const QString macro =
//             QString::fromUtf8(macroManager.getPropertySymbolicValue(code).c_str());
        const QString micro =
            QString::fromUtf8(microManager.getPropertySymbolicValue(code).c_str());
#ifdef DEBUG_LP
        LDEBUG << "ConllDumper::process graphTag:" << micro;
#endif
        QStringList featuresList;
        for (auto propItr = managers.cbegin();
             propItr != managers.cend(); propItr++)
        {
          QString key = QString::fromUtf8(propItr->first.c_str());
          if (key != "MACRO" && key != "MICRO")
          {
            QString value = QString::fromUtf8(propItr->second.getPropertySymbolicValue(code).c_str());
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

        std::string inflectedToken=ft->stringForm().toUtf8().constData();
        std::string lemmatizedToken;
        if (morphoData != 0 && !morphoData->empty())
        {
          lemmatizedToken=sp[(*morphoData)[0].lemma].toUtf8().constData();
        }
        // @TODO Should follow instructions here to output all MWE:
        // https://universaldependencies.org/format.html#words-tokens-and-empty-nodes
        QString neType = QString::fromUtf8("_") ;
        std::set< AnnotationGraphVertex > anaVertices =
            annotationData->matches("PosGraph", v, "AnalysisGraph");
        // note: anaVertices size should be 0 or 1
        for (auto anaVerticesIt = anaVertices.cbegin();
            anaVerticesIt != anaVertices.cend(); anaVerticesIt++)
        {
          auto matches = annotationData->matches("AnalysisGraph",
                                                 *anaVerticesIt,"annot");
          for (auto it = matches.cbegin(); it != matches.cend(); it++)
          {
            AnnotationGraphVertex vx=*it;
            if (annotationData->hasAnnotation(
                vx, utf8stdstring2limastring("SpecificEntity")))
            {
              const SpecificEntityAnnotation* se =
                annotationData->annotation(vx, utf8stdstring2limastring("SpecificEntity")).
                pointerValue<SpecificEntityAnnotation>();
              neType = Lima::Common::MediaticData::MediaticData::single().getEntityName(se->getType());
              break;
            }
          }
          if (neType != "_") break;
        }
        QString conllRelName = "_";
        int targetConllId = 0;
        if (vertexDependencyInformations.count(v)!=0)
        {
          LinguisticGraphVertex target = vertexDependencyInformations.find(v)->second.first;
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
                  << tokenId << " is " << targetConllId;
#endif
          QString relName = QString::fromUtf8(vertexDependencyInformations.find(v)->second.second.c_str());
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
            conllRelName= relName;
//             LERROR << "ConllDumper::process" << relName << "not found in mapping";
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

        QString targetConllIdString = targetConllId > 0 ? QString(QLatin1String("%1")).arg(targetConllId) : "_";
        dstream->out()  << tokenId // ID
                        << "\t" << inflectedToken // FORM
                        << "\t" << lemmatizedToken // LEMMA
                        << "\t" << micro.toUtf8().constData() // UPOS
                        << "\t" << "_" // XPOS
                        << "\t" << "_" // FEATS
                        << "\t" << targetConllIdString.toUtf8().constData() // HEAD
                        << "\t" << conllRelName.toUtf8().constData() // DEPREL
                        << "\t" << "_"; // DEPS
        QStringList miscField;
        if (neType != "_")
        {
          miscField << (QString("NE=") + neType.toUtf8().constData());
        }
//           LDEBUG << "ConllDumper::process output the predicate if any";
        if (predicates.contains(v))
        {
          QString predicate = annotationData->stringAnnotation(predicates.value(v),
                                                              "Predicate");

          // Now output the roles supported by the current PoS graph token
#ifdef DEBUG_LP
          LDEBUG << "ConllDumper::process output the roles for the"
                  << keys.size() << "predicates";
#endif
          for (int i = 0; i < keys.size(); i++)
          {
            AnnotationGraphVertex predicateVertex =
                predicates.value(keys[keys.size()-1-i]);

            auto vMatches = annotationData->matches("PosGraph", v, "annot");
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
                AnnotationGraphVertex vMatch = *it;
                AnnotationGraphInEdgeIt vMatchInEdgesIt, vMatchInEdgesIt_end;
                boost::tie(vMatchInEdgesIt, vMatchInEdgesIt_end) =
                    boost::in_edges(vMatch,annotationData->getGraph());
                for (; vMatchInEdgesIt != vMatchInEdgesIt_end; vMatchInEdgesIt++)
                {
                  AnnotationGraphVertex inVertex =
                      boost::source(*vMatchInEdgesIt,
                                    annotationData->getGraph());
                  auto inVertexAnnotPosGraphMatches =
                      annotationData->matches("annot",inVertex,"PosGraph");
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
        dstream->out() << "\t" << miscField.join('|').toUtf8().constData(); // MISC
        dstream->out() << std::endl;
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
        LinguisticGraphVertex next = boost::target(*outIter,*graph);
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

  AnnotationData* annotationData =
      static_cast<AnnotationData*>(analysis.getData("AnnotationData"));

  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData(m_graph.toUtf8().constData()));
  if (tokenList==0) {
    DUMPERLOGINIT;
    LERROR << "graph " << m_graph << " has not been produced: check pipeline";
    return result;
  }
  LinguisticGraph* graph=tokenList->getGraph();


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

    std::set< AnnotationGraphVertex > vMatches =
        annotationData->matches("PosGraph", v, "annot");
    for (auto it = vMatches.begin(); it != vMatches.end(); it++)
    {
      AnnotationGraphVertex vMatch = *it;
      if (annotationData->hasStringAnnotation(vMatch,"Predicate"))
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
      LinguisticGraphVertex next=boost::target(*outItr,*graph);
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
