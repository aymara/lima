/*
    Copyright 2004-2020 CEA LIST

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
#include "SpecificEntitiesXmlLogger.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"

#include "common/MediaticData/mediaticData.h"
#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include "common/time/traceUtils.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"

#include <fstream>
#include <queue>

using namespace std;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
//using namespace boost;

namespace Lima
{
namespace LinguisticProcessing
{
namespace SpecificEntities
{

SimpleFactory<MediaProcessUnit,SpecificEntitiesXmlLogger> specificEntitiesXmlLoggerFactory(SPECIFICENTITIESXMLLOGGER_CLASSID);

SpecificEntitiesXmlLogger::SpecificEntitiesXmlLogger() :
    AbstractTextualAnalysisDumper(),
    m_compactFormat(false),
    m_language(0),
    m_graph("PosGraph"),
    m_followGraph(false)
{
  SELOGINIT;
  LDEBUG << "SpecificEntitiesXmlLogger::SpecificEntitiesXmlLogger()";
}


void SpecificEntitiesXmlLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  SELOGINIT;
  LDEBUG << "SpecificEntitiesXmlLogger::init";
  AbstractTextualAnalysisDumper::init(unitConfiguration, manager);

  m_language = manager->getInitializationParameters().media;

  try
  {
    m_graph = unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    SELOGINIT;
    LWARN << "No 'graph' parameter in unit configuration '"
          << unitConfiguration.getName() << "' ; using PosGraph";
    m_graph = string("PosGraph");
  }
  try
  {
    std::string val = unitConfiguration.getParamsValueAtKey("compactFormat");
    if (val == "yes" || val == "true" || val == "1")
    {
      m_compactFormat = true;
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // do nothing: optional

  try
  {
    auto str = unitConfiguration.getParamsValueAtKey("followGraph");
    m_followGraph = (str == "1" || str == "true" || str == "yes");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // keep default value
}

LimaStatusCode SpecificEntitiesXmlLogger::process(
  AnalysisContent& analysis) const
{
  SELOGINIT;
  LDEBUG << "SpecificEntitiesXmlLogger::process";
  TimeUtils::updateCurrentTime();

  auto annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData == nullptr)
  {
    SELOGINIT;
    LERROR << "no annotationData ! abort";
    return MISSING_DATA;
  }

  auto graphp = static_cast<LinguisticAnalysisStructure::AnalysisGraph*>(
    analysis.getData(m_graph));
  if (graphp == nullptr)
  {
    SELOGINIT;
    LERROR << "no graph "<< m_graph <<" ! abort";
    return MISSING_DATA;
  }
  const auto& graph = *graphp;
  auto lingGraph = const_cast<LinguisticGraph*>(graph.getGraph());
  auto tokenMap = get(vertex_token, *lingGraph);

  auto metadata = static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == nullptr)
  {
      SELOGINIT;
      LERROR << "no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }

  auto dstream = initialize(analysis);
  auto& out = dstream->out();

  uint64_t offset = 0;
  try
  {
    offset = QString::fromStdString(metadata->getMetaData("StartOffset")).toUInt();
  }
  catch (LinguisticProcessingException& )
  {
    // do nothing: not set in analyzeText (only in analyzeXmlDocuments)
  }

  uint64_t offsetIndexingNode = 0;
  try
  {
    offsetIndexingNode = QString::fromStdString(
      metadata->getMetaData("StartOffsetIndexingNode")).toUInt();
  }
  catch (LinguisticProcessingException& )
  {
    // do nothing: not set in analyzeText (only in analyzeXmlDocuments)
  }

  std::string docId;
  try
  {
    docId = metadata->getMetaData("DocId");
  }
  catch (LinguisticProcessingException& )
  {
    // do nothing: not set in analyzeText (only in analyzeXmlDocuments)
  }

  if (m_compactFormat)
  {
    out << "<entities docid=\"" << docId
    << "\" offsetNode=\"" << offsetIndexingNode
    << "\" offset=\"" << offset
    << "\">" << std::endl;
  }
  else
  {
    out << "<specific_entities>" << std::endl;
  }
//   SELOGINIT;

  if (m_followGraph)
  {
    // instead of looking to all annotations, follow the graph (in
    // morphological graph, some vertices are not related to main graph:
    // idiomatic expressions parts and named entity parts)
    // -> this will not include nested entities

    auto tokenList = static_cast<AnalysisGraph*>(analysis.getData(m_graph));
    if (tokenList == nullptr)
    {
      LERROR << "graph " << m_graph << " has not been produced: check pipeline";
      return MISSING_DATA;
    }
    auto graph = tokenList->getGraph();
    //const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);

    std::queue<LinguisticGraphVertex> toVisit;
    std::set<LinguisticGraphVertex> visited;
    toVisit.push(tokenList->firstVertex());

    LinguisticGraphOutEdgeIt outItr, outItrEnd;
    while (!toVisit.empty())
    {
      auto v = toVisit.front();
      toVisit.pop();
      if (v == tokenList->lastVertex())
      {
        continue;
      }

      for (boost::tie(outItr, outItrEnd) = out_edges(v, *graph);
           outItr != outItrEnd; outItr++)
      {
        auto next = target(*outItr, *graph);
        if (visited.find(next) == visited.end())
        {
          visited.insert(next);
          toVisit.push(next);
        }
      }
      const auto annot = getSpecificEntityAnnotation(v, annotationData);
      if (annot != 0)
      {
        outputEntity(annotationData, out, v, annot, tokenMap, offset);
      }
    }
  }
  else
  {
    // take all annotations
    AnnotationGraphVertexIt itv, itv_end;
    boost::tie(itv, itv_end) = vertices(annotationData->getGraph());
    for (; itv != itv_end; itv++)
    {
      //     LDEBUG << "SpecificEntitiesXmlLogger on annotation vertex " << *itv;
      if (annotationData->hasAnnotation(*itv, QString::fromUtf8("SpecificEntity")))
      {
        //       LDEBUG << "    it has SpecificEntityAnnotation";
        const SpecificEntityAnnotation* annot = nullptr;
        try
        {
          annot = annotationData->annotation(
            *itv, QString::fromUtf8("SpecificEntity")).pointerValue<SpecificEntityAnnotation>();
        }
        catch (const boost::bad_any_cast& )
        {
          SELOGINIT;
          LERROR << "This annotation is not a SpecificEntity; SE not logged";
          continue;
        }

        // recuperer l'id du vertex morph cree
        LinguisticGraphVertex v;
        if (!annotationData->hasIntAnnotation(*itv,
                                              QString::fromStdString(m_graph)))
        {
          //         SELOGINIT;
          //         LDEBUG << *itv << " has no " << m_graph << " annotation. Skeeping it.";
          continue;
        }
        v = annotationData->intAnnotation(*itv, QString::fromStdString(m_graph));
        outputEntity(annotationData, out, v, annot, tokenMap, offset);
      }
    }
  }

  //   LDEBUG << "    all vertices done";
  if (m_compactFormat)
  {
    out << "</entities>" << std::endl;
  }
  else
  {
    out << "</specific_entities>" << std::endl;
  }

  TimeUtils::logElapsedTime("SpecificEntitiesXmlLogger");
  return SUCCESS_ID;
}

void SpecificEntitiesXmlLogger::outputEntity(AnnotationData* annotationData,
                                             std::ostream& out,
                                             LinguisticGraphVertex v,
                                             const SpecificEntityAnnotation* annot,
                                             const VertexTokenPropertyMap& tokenMap,
                                             uint64_t offset) const
{
  auto vToken = tokenMap[v];
  //       LDEBUG << "SpecificEntitiesXmlLogger tokenMap[" << v << "] = " << vToken;
  if (vToken == nullptr)
  {
    SELOGINIT;
    LERROR << "SpecificEntitiesXmlLogger::outputEntity: Vertex " << v
           << " has no entry in the analysis graph token map. This should not happen !!";
  }
  else
  {
    if (m_compactFormat)
    {
      // same format as RecognizerResultLogger
      // Check compatibility with RecognizerResultLogger
      // display list of components and normalization
      // components are the list of features whose value are strings in the texte with a (position,length) info
      // normalisations  are the list of features whose value are computed and does not have (position,length) info
      const auto& features = annot->getFeatures();
      out << "<entity>"
          //<< "<pos>" << offset+annot->getPosition() << "</pos>"
          << "<pos>" << offset+annot->getPosition() << "</pos>"
          << "<len>" << annot->getLength() << "</len>"
          //<< "<typeNum>" << (*m).getType() << "</typeNum>"
          << "<type>"
          << Common::MediaticData::MediaticData::single().getEntityName(annot->getType()).toStdString()
          << "</type>"
          << "<string>"<< Common::Misc::transcodeToXmlEntities(vToken->stringForm()).toStdString() << "</string>"
          << "<components>";
      for (auto featureItr = features.cbegin(), features_end = features.cend();
           featureItr != features_end; featureItr++)
      {
        if( featureItr->getPosition() != UNDEFPOSITION )
        {
          out << "<" << featureItr->getName();
          out << " pos=\"" << featureItr->getPosition() << "\"";
          out << " len=\"" << featureItr->getLength() << "\"";
          out << ">";
          out << Common::Misc::transcodeToXmlEntities(
                  QString::fromStdString(
                    featureItr->getValueString())
                                                     ).toStdString()
              << "</" << featureItr->getName() << ">";
        }
      }

      // TODO: Follow "belongstose" links to outputs embeded entities as components
      // Get the current annotationVertex (is there any more simple solution???)
      auto matches = annotationData->matches(m_graph,v,"annot");
      AnnotationGraphVertex va1;

      auto it = matches.begin();
      for(; it != matches.end(); it++)
      {
        va1 = *it;
        SELOGINIT;
        LDEBUG << "SpecificEntitiesXmlLogger::outputEntity: get agv = " << va1;
        if (annotationData->hasAnnotation(
              va1, QString::fromStdString("SpecificEntity")))
        break;
      }
      if( it == matches.end() )
      {
        SELOGINIT;
        LERROR << "SpecificEntitiesXmlLogger::outputEntity: could not find annotation of node "
                << v << "in LinguisticGraph";
      }
      else
      {
        SELOGINIT;
        LDEBUG << "SpecificEntitiesXmlLogger::outputEntity: agv " << va1
                << " is a SpecificEntity Annotation";
        // Follow "belongstose" out_edges to get annotationVertex of embededed NE
        AnnotationGraphOutEdgeIt it1, it1_end;
        boost::tie(it1, it1_end) = boost::out_edges(va1, annotationData->getGraph());
        for (; it1 != it1_end; it1++)
        {
          if ( annotationData->intAnnotation((*it1), QString::fromUtf8("holds"))==1)
          {
            AnnotationGraphVertex va2 = target(*it1, annotationData->getGraph());
            LDEBUG << "SpecificEntitiesXmlLogger::outputEntity: embeded agv = "
                    << va2;
            // récupérer le noeud du graphe linguistique
            auto v2 = annotationData->intAnnotation(
                        va2, QString::fromStdString(m_graph));
            LDEBUG << "SpecificEntitiesXmlLogger::outputEntity: vertex in "
                    << m_graph << " is " << v2;
            // récupérer l'annotation SpecifiEntity
            if (annotationData->hasAnnotation(
                  va2, QString::fromUtf8("SpecificEntity")))
            {
              auto annot2 = annotationData->annotation(
                  va2, QString::fromUtf8("SpecificEntity")
                ).pointerValue<SpecificEntityAnnotation>();
              LDEBUG << "SpecificEntitiesXmlLogger::outputEntity: annot2 = " << annot2;
              outputEntity(annotationData,out, v2, annot2, tokenMap, offset);
              break;
            }
          }
        }
      }
      out << "</components>"
      << "<normalization>";
      SELOGINIT;
      LDEBUG << "SpecificEntitiesXmlLogger::outputEntity: features:" << features;
      for (auto featureItr = features.begin(), features_end=features.end();
           featureItr != features_end; featureItr++)
      {
        if( featureItr->getPosition() == UNDEFPOSITION )
        {
          out << "<" << featureItr->getName() << ">";
          out << Common::Misc::transcodeToXmlEntities(
            QString::fromStdString(featureItr->getValueString())).toStdString();
          out << "</" << featureItr->getName() << ">";
        }
      }
      out << "</normalization>"
          << "</entity>" << std::endl;
    }
    else
    {
      // recuperer le vertex morph en question dans le graphe morph
      // recuperer la chaine, la position et la longueur pour ce vertex morph
      out << "<specific_entity>" << std::endl;
      out << "  <string>"
          << Common::Misc::transcodeToXmlEntities(vToken->stringForm()).toStdString()
          << "</string>" << std::endl;
      out << "  <position>" << vToken->position()<< "</position>" << endl;
      out << "  <length>" << vToken->length() << "</length>" << endl;
      out << "  <type>"
          << Common::MediaticData::MediaticData::single().getEntityName(annot->getType()).toStdString()
          << "</type>" << std::endl;
      out << "  <normalization>" << std::endl;
      const auto& features = annot->getFeatures();
      SELOGINIT;
      LDEBUG << "SpecificEntitiesXmlLogger::outputEntity: features:" << features;
      for (auto featureItr = features.begin(), features_end = features.end();
           featureItr != features_end; featureItr++)
      {
        if( featureItr->getPosition() == UNDEFPOSITION )
        {
          out << "    <" << featureItr->getName() << ">";
          out << Common::Misc::transcodeToXmlEntities(
            QString::fromStdString(featureItr->getValueString())).toStdString();
          out << "</" << featureItr->getName() << ">" << std::endl;
        }
      }
      out << "  </normalization>" << std::endl;
      out << "</specific_entity>" << std::endl;
    }
  }
}

const SpecificEntityAnnotation* SpecificEntitiesXmlLogger::getSpecificEntityAnnotation(
  LinguisticGraphVertex v,
  const Common::AnnotationGraphs::AnnotationData* annotationData) const
{
  const SpecificEntityAnnotation* se = nullptr;

  // check only entity found in current graph (not previous graph such as AnalysisGraph)

  auto matches = annotationData->matches(m_graph,v,"annot");
  for (auto it = matches.cbegin(); it != matches.cend(); it++)
  {
    auto vx = *it;
    if (annotationData->hasAnnotation(vx, QString::fromUtf8("SpecificEntity")))
    {
      //BoWToken* se = createSpecificEntity(v,*it, annotationData, anagraph, posgraph, offsetBegin);
      se = annotationData->annotation(
        vx,
        QString::fromUtf8("SpecificEntity")).pointerValue<SpecificEntityAnnotation>();
      if (se != 0)
      {
        return se;
      }
    }
  }
  return se;

}

} // SpecificEntities
} // LinguisticProcessing
} // Lima
