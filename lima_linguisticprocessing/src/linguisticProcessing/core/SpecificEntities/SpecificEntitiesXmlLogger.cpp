/*
    Copyright 2002-2013 CEA LIST

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
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
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
using namespace boost;

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


SpecificEntitiesXmlLogger::~SpecificEntitiesXmlLogger()
{}

void SpecificEntitiesXmlLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  SELOGINIT;
  LDEBUG << "SpecificEntitiesXmlLogger::init";
  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);

  m_language=manager->getInitializationParameters().media;

  try
  {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    SELOGINIT;
    LWARN << "No 'graph' parameter in unit configuration '"
        << unitConfiguration.getName() << "' ; using PosGraph";
    m_graph=string("PosGraph");
  }
  try
  {
    string val=unitConfiguration.getParamsValueAtKey("compactFormat");
    if (val=="yes" || val=="true" || val=="1") {
      m_compactFormat=true;
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // do nothing: optional

  try { 
    std::string str=unitConfiguration.getParamsValueAtKey("followGraph"); 
    if (str=="1" || str=="true" || str=="yes") {
      m_followGraph=true;
    }
    else {
      m_followGraph=false;
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // keep default value
}

LimaStatusCode SpecificEntitiesXmlLogger::process(
  AnalysisContent& analysis) const
{
  SELOGINIT;
  LDEBUG << "SpecificEntitiesXmlLogger::process";
  TimeUtils::updateCurrentTime();

  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData == 0) {
    SELOGINIT;
    LERROR << "no annotationData ! abort";
    return MISSING_DATA;
  }
  
  
  LinguisticAnalysisStructure::AnalysisGraph* graphp = static_cast<LinguisticAnalysisStructure::AnalysisGraph*>(analysis.getData(m_graph));
  if (graphp == 0) {
    SELOGINIT;
    LERROR << "no graph "<< m_graph <<" ! abort";
    return MISSING_DATA;
  }
  const LinguisticAnalysisStructure::AnalysisGraph& graph = *graphp;
  LinguisticGraph* lingGraph = const_cast<LinguisticGraph*>(graph.getGraph());
  VertexTokenPropertyMap tokenMap = get(vertex_token, *lingGraph);
  
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      SELOGINIT;
      LERROR << "no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }

  DumperStream* dstream=initialize(analysis);
  ostream& out=dstream->out();

  uint64_t offset(0);
  try {
    offset=atoi(metadata->getMetaData("StartOffset").c_str());
  }
  catch (LinguisticProcessingException& ) {
    // do nothing: not set in analyzeText (only in analyzeXmlDocuments)
  }

  uint64_t offsetIndexingNode(0);
  try {
    offsetIndexingNode=atoi(metadata->getMetaData("StartOffsetIndexingNode").c_str());
  }
  catch (LinguisticProcessingException& ) {
    // do nothing: not set in analyzeText (only in analyzeXmlDocuments)
  }

  std::string docId("");
  try {
    docId=metadata->getMetaData("DocId");
  }
  catch (LinguisticProcessingException& ) {
    // do nothing: not set in analyzeText (only in analyzeXmlDocuments)
  }

  if (m_compactFormat) {
    out << "<entities docid=\"" << docId
    << "\" offsetNode=\"" << offsetIndexingNode 
    << "\" offset=\"" << offset
    << "\">" << endl;
  }
  else {
    out << "<specific_entities>" << endl;
  }
//   SELOGINIT;

  if (m_followGraph) {
    // instead of looking to all annotations, follow the graph (in
    // morphological graph, some vertices are not related to main graph:
    // idiomatic expressions parts and named entity parts)
    // -> this will not include nested entities

    AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData(m_graph));
    if (tokenList==0) {
      LERROR << "graph " << m_graph << " has not been produced: check pipeline";
      return MISSING_DATA;
    }
    LinguisticGraph* graph=tokenList->getGraph();
    //const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);
    
    std::queue<LinguisticGraphVertex> toVisit;
    std::set<LinguisticGraphVertex> visited;
    toVisit.push(tokenList->firstVertex());
    
    LinguisticGraphOutEdgeIt outItr,outItrEnd;
    while (!toVisit.empty()) {
      LinguisticGraphVertex v=toVisit.front();
      toVisit.pop();
      if (v == tokenList->lastVertex()) {
        continue;
      }
      
      for (boost::tie(outItr,outItrEnd)=out_edges(v,*graph); outItr!=outItrEnd; outItr++) 
      {
        LinguisticGraphVertex next=target(*outItr,*graph);
        if (visited.find(next)==visited.end())
        {
          visited.insert(next);
          toVisit.push(next);
        }
      }
      const SpecificEntityAnnotation* annot=getSpecificEntityAnnotation(v,annotationData);
      if (annot != 0) {
        outputEntity(out,v,annot,tokenMap,offset);
      }
    }
  }
  else {
    // take all annotations
    AnnotationGraphVertexIt itv, itv_end;
    boost::tie(itv, itv_end) = vertices(annotationData->getGraph());
    for (; itv != itv_end; itv++)
    {
      //     LDEBUG << "SpecificEntitiesXmlLogger on annotation vertex " << *itv;
      if (annotationData->hasAnnotation(*itv,Common::Misc::utf8stdstring2limastring("SpecificEntity")))
      {
        //       LDEBUG << "    it has SpecificEntityAnnotation";
        const SpecificEntityAnnotation* annot = 0;
        try
        {
          annot = annotationData->annotation(*itv,Common::Misc::utf8stdstring2limastring("SpecificEntity"))
          .pointerValue<SpecificEntityAnnotation>();
        }
        catch (const boost::bad_any_cast& )
        {
          SELOGINIT;
          LERROR << "This annotation is not a SpecificEntity; SE not logged";
          continue;
        }
        
        // recuperer l'id du vertex morph cree
        LinguisticGraphVertex v;
        if (!annotationData->hasIntAnnotation(*itv,Common::Misc::utf8stdstring2limastring(m_graph)))
        {
          //         SELOGINIT;
          //         LDEBUG << *itv << " has no " << m_graph << " annotation. Skeeping it.";
          continue;
        }
        v = annotationData->intAnnotation(*itv,Common::Misc::utf8stdstring2limastring(m_graph));
        outputEntity(out,v,annot,tokenMap,offset);
      }
    }
  }   
  
  //   LDEBUG << "    all vertices done";
  if (m_compactFormat) {
    out << "</entities>" << endl;
  }
  else {
    out << "</specific_entities>" << endl;
  }
  delete dstream;
  TimeUtils::logElapsedTime("SpecificEntitiesXmlLogger");
  return SUCCESS_ID;
  
}

void SpecificEntitiesXmlLogger::
outputEntity(std::ostream& out, 
             LinguisticGraphVertex v,
             const SpecificEntityAnnotation* annot,
             const VertexTokenPropertyMap& tokenMap,
             uint64_t offset) const
{
  LinguisticAnalysisStructure::Token* vToken = tokenMap[v];
  //       LDEBUG << "SpecificEntitiesXmlLogger tokenMap[" << v << "] = " << vToken;
  if (vToken == 0)
  {
    SELOGINIT;
    LERROR << "Vertex " << v << " has no entry in the analysis graph token map. This should not happen !!";
  }
  else
  {
    if (m_compactFormat) {
      // same format as RecognizerResultLogger
      out 
      << "<entity>" 
      //<< "<pos>" << offset+annot->getPosition() << "</pos>" 
      << "<pos>" << offset+annot->getPosition() << "</pos>" 
      << "<len>" << annot->getLength() << "</len>" 
      //<< "<typeNum>" << (*m).getType() << "</typeNum>"
      << "<type>" 
      << Common::MediaticData::MediaticData::single().getEntityName(annot->getType()).toUtf8().data()
      << "</type>"
      << "<string>"<< Common::Misc::transcodeToXmlEntities(vToken->stringForm()).toUtf8().data() << "</string>"
      << "<norm>";
      const Automaton::EntityFeatures& features=annot->getFeatures();
      for (Automaton::EntityFeatures::const_iterator 
        featureItr=features.begin(),features_end=features.end();
      featureItr!=features_end; featureItr++)
      {
        out << "<" << featureItr->getName();
        out << " pos=\"" << featureItr->getPosition() << "\"";
        out << " len=\"" << featureItr->getLength() << "\"";
        out << ">";
        out << Common::Misc::limastring2utf8stdstring(Common::Misc::transcodeToXmlEntities(Common::Misc::utf8stdstring2limastring(featureItr->getValueString())))
        << "</" << featureItr->getName() << ">";
      }
      out << "</norm>"
      << "</entity>"
      << endl;
    }
    else {
      // recuperer le vertex morph en question dans le graphe morph
      // recuperer la chaine, la position et la longueur pour ce vertex morph
      out << "<specific_entity>" << endl;
      out << "  <string>" << Common::Misc::transcodeToXmlEntities(vToken->stringForm()).toUtf8().data() << "</string>" << endl;
      out << "  <position>" << vToken->position()<< "</position>" << endl;
      out << "  <length>" << vToken->length() << "</length>" << endl;
      out << "  <type>" 
      << Common::MediaticData::MediaticData::single().getEntityName(annot->getType())
      << "</type>" << endl;
      out << "</specific_entity>" << endl;
    }
  }
}

const SpecificEntityAnnotation* SpecificEntitiesXmlLogger::
getSpecificEntityAnnotation(LinguisticGraphVertex v,
                            const Common::AnnotationGraphs::AnnotationData* annotationData) const
{

  const SpecificEntityAnnotation* se=0;
  
  // check only entity found in current graph (not previous graph such as AnalysisGraph)
 
  std::set< AnnotationGraphVertex > matches = annotationData->matches(m_graph,v,"annot");
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
  it != matches.end(); it++)
  {
    AnnotationGraphVertex vx=*it;
    if (annotationData->hasAnnotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    {
      //BoWToken* se = createSpecificEntity(v,*it, annotationData, anagraph, posgraph, offsetBegin);
      se = annotationData->annotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")).
      pointerValue<SpecificEntityAnnotation>();
      if (se!=0) {
        return se;
      }
    }
  }
  return se;

}

} // SpecificEntities
} // LinguisticProcessing
} // Lima
