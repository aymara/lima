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
/************************************************************************
 *
 * @file       SemanticRelationsXmlLogger.cpp
 * @author      (romaric.besancon@cea.fr)
 * @date       Mon Sep 17 2007
 * copyright   Copyright (C) 2007 by CEA LIST
 * 
 ***********************************************************************/

#include "SemanticRelationsXmlLogger.h"
#include "SemanticRelationAnnotation.h"
#include "SemanticAnnotation.h"

#include "common/MediaticData/mediaticData.h"
#include "common/Data/strwstrtools.h"
#include "common/time/traceUtils.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
//#include "common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"

#include <fstream>

#define SEMLOGINIT  LOGINIT("LP::SemanticAnalysis")

using namespace std;
using namespace boost;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::SpecificEntities;

namespace Lima {
namespace LinguisticProcessing {
namespace SemanticAnalysis {

SimpleFactory<MediaProcessUnit,SemanticRelationsXmlLogger> 
semanticRelationsXmlLoggerFactory(SEMANTICRELATIONSXMLLOGGER_CLASSID);

SemanticRelationsXmlLogger::SemanticRelationsXmlLogger() :
AbstractLinguisticLogger(".output.xml"),
m_language(0),
m_graph("PosGraph")
{}


SemanticRelationsXmlLogger::~SemanticRelationsXmlLogger()
{}

void SemanticRelationsXmlLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  AbstractLinguisticLogger::init(unitConfiguration,manager);

  m_language=manager->getInitializationParameters().media;

  try
  {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    SEMLOGINIT;
    LWARN << "No 'graph' parameter in unit configuration '"
        << unitConfiguration.getName() << "' ; using PosGraph";
    m_graph=string("PosGraph");
  }
}

LimaStatusCode SemanticRelationsXmlLogger::
process(AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  
  SEMLOGINIT;
  LERROR << "SemanticRelationsXmlLogger";
    
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  
  const LinguisticAnalysisStructure::AnalysisGraph& graph = 
    *(static_cast<LinguisticAnalysisStructure::AnalysisGraph*>(analysis.getData(m_graph)));
  
  LinguisticGraph* lingGraph = const_cast<LinguisticGraph*>(graph.getGraph());
  VertexTokenPropertyMap tokenMap = get(vertex_token, *lingGraph);
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      SEMLOGINIT;
      LERROR << "no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }

  ofstream out;
  if (!openLogFile(out,metadata->getMetaData("FileName"))) {
    SEMLOGINIT;
    LERROR << "Can't open log file ";
    return UNKNOWN_ERROR;
  }

  uint64_t offset(0);
  try {
    offset=atoi(metadata->getMetaData("StartOffset").c_str());
  }
  catch (LinguisticProcessingException& e) {
    // do nothing: not set in analyzeText (only in analyzeXmlDocuments)
  }

  uint64_t offsetIndexingNode(0);
  try {
    offsetIndexingNode=atoi(metadata->getMetaData("StartOffsetIndexingNode").c_str());
  }
  catch (LinguisticProcessingException& e) {
    // do nothing: not set in analyzeText (only in analyzeXmlDocuments)
  }

  std::string docId("");
  try {
    docId=metadata->getMetaData("DocId");
  }
  catch (LinguisticProcessingException& e) {
    // do nothing: not set in analyzeText (only in analyzeXmlDocuments)
  }

  out << "<relations docid=\"" << docId
      << "\" offsetNode=\"" << offsetIndexingNode 
      << "\">" << endl;

//   LDEBUG << "SemanticRelationsXmlLogger on graph " << m_graph;
  
  //look at all vertices for annotations
  AnnotationGraphVertexIt itv, itv_end;
  boost::tie(itv, itv_end) = vertices(annotationData->getGraph());
  for (; itv != itv_end; itv++)
  {
    LDEBUG << "SemanticRelationsXmlLogger on annotation vertex " << *itv;
    if (annotationData->hasAnnotation(*itv,("SemanticAnnotation")))
    {
//       LDEBUG << "    it has SemanticRelationAnnotation";
      const SemanticAnnotation* annot = 0;
      try
      {
        annot = annotationData->annotation(*itv,("SemanticAnnotation"))
          .pointerValue<SemanticAnnotation>();
      }
      catch (const boost::bad_any_cast& e)
      {
        SEMLOGINIT;
        LERROR << "This annotation is not a SemanticRelation";
        continue;
      }

      // output
      out << "<annotation type=\"" << annot->getType() << "\">" << endl
          << vertexStringForSemanticAnnotation("vertex",*itv,tokenMap,annotationData,offset)
          << "</annotation>" << endl;
    }
  }

  // look at all edges for relations
  AnnotationGraphEdgeIt it,it_end;
  const AnnotationGraph& annotGraph=annotationData->getGraph();
  boost::tie(it, it_end) = edges(annotGraph);
  for (; it != it_end; it++) {
    LDEBUG << "SemanticRelationsXmlLogger on annotation edge " 
           << source(*it,annotGraph) << "->" << target(*it,annotationData->getGraph());
    if (annotationData->hasAnnotation(*it,("SemanticRelation")))
    {
      SEMLOGINIT;
      LDEBUG << "found semantic relation";
      const SemanticRelationAnnotation* annot = 0;
      try
      {
        annot = annotationData->annotation(*it,("SemanticRelation"))
          .pointerValue<SemanticRelationAnnotation>();
      }
      catch (const boost::bad_any_cast& e)
      {
        SEMLOGINIT;
        LERROR << "This annotation is not a SemanticAnnotation";
        continue;
      }

      //output
      out << "<relation type=\"" << annot->type() << "\">" << endl
          << vertexStringForSemanticAnnotation("source",source(*it,annotGraph),tokenMap,annotationData,offset)
          << vertexStringForSemanticAnnotation("target",target(*it,annotGraph),tokenMap,annotationData,offset)
          << "</relation>" << endl;
      
    }
  }

//   LDEBUG << "    all vertices done";
  out << "</relations>" << endl;
  out.close();

  TimeUtils::logElapsedTime("SemanticRelationsXmlLogger");
  return SUCCESS_ID;
}

std::string SemanticRelationsXmlLogger::
vertexStringForSemanticAnnotation(const std::string& vertexRole, 
                                  const AnnotationGraphVertex& vertex,
                                  const VertexTokenPropertyMap& tokenMap,
                                  AnnotationData* annotationData,
                                  uint64_t offset) const
{
  ostringstream oss;

  // get id of the corresponding vertex in analysis graph
  LinguisticGraphVertex v;
  if (!annotationData->hasIntAnnotation(vertex,Common::Misc::utf8stdstring2limastring(m_graph)))
  {
    // SEMLOGINIT;
    // LDEBUG << *itv << " has no " << m_graph << " annotation. Skeeping it.";
    return "";
  }
  v = annotationData->intAnnotation(vertex,Common::Misc::utf8stdstring2limastring(m_graph));
  LinguisticAnalysisStructure::Token* vToken = tokenMap[v];
  //       LDEBUG << "SemanticRelationsXmlLogger tokenMap[" << v << "] = " << vToken;
  if (vToken == 0)
  {
    SEMLOGINIT;
    LERROR << "Vertex " << v << " has no entry in the analysis graph token map. This should not happen !!";
    return "";
  }

  // get annotation : element in relation can be an entity => get entity type
  // otherwise, its type is "token"
  std::string type("token");

  auto matches = annotationData->matches(m_graph,v,"annot");
  for (auto it = matches.begin(); it != matches.end(); it++)
  {
    if (annotationData->hasAnnotation(*it,Common::Misc::utf8stdstring2limastring("SpecificEntity"))) {
      const SpecificEntityAnnotation* annot = 0;
      try {
        annot = annotationData->annotation(*it,Common::Misc::utf8stdstring2limastring("SpecificEntity"))
          .pointerValue<SpecificEntityAnnotation>();
      }
      catch (const boost::bad_any_cast& e) {
        SEMLOGINIT;
        LERROR << "This annotation is not a SemanticAnnotation";
        continue;
      }
      type=Common::Misc::limastring2utf8stdstring(Common::MediaticData::MediaticData::single().getEntityName(annot->getType()));
      break;
    }
  }

  oss << "  <" << vertexRole 
      << " type=\"" << type << "\"" 
      << " pos=\"" << offset+vToken->position() << "\"" 
      << " len=\"" << vToken->length() << "\"" 
      << " string=\"" << vToken->stringForm() << "\"" 
      << "/>" << endl;
  return oss.str();
}


} // SemanticAnalysis
} // LinguisticProcessing
} // Lima
