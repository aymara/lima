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

// #include "common/linguisticData/linguisticData.h"
//#include "common/misc/strwstrtools.h"
//#include "common/misc/traceUtils.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/Data/LimaString.h"
#include "common/MediaProcessors/MediaProcessors.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"

#include <fstream>

#include "common/MediaProcessors/DumperStream.h"

//#define SEMANTICANALYSISLOGINIT  LOGINIT("LP::SemanticAnalysis")

using namespace std;
using namespace boost;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing::SpecificEntities;

namespace Lima {
namespace LinguisticProcessing {
namespace SemanticAnalysis {


SimpleFactory<MediaProcessUnit,SemanticRelationsXmlLogger> semanticRelationsXmlLoggerFactory(SEMANTICRELATIONSXMLLOGGER_CLASSID);

SemanticRelationsXmlLogger::SemanticRelationsXmlLogger() :
AbstractTextualAnalysisDumper()
{}


SemanticRelationsXmlLogger::~SemanticRelationsXmlLogger()
{}

void SemanticRelationsXmlLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);

  m_language=manager->getInitializationParameters().media;

  try
  {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    SEMANTICANALYSISLOGINIT;
    LWARN << "No 'graph' parameter in unit configuration '"
        << unitConfiguration.getName() << "' ; using PosGraph" << LENDL;
    m_graph=string("PosGraph");
  }
}

LimaStatusCode SemanticRelationsXmlLogger::
process(AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  
  SEMANTICANALYSISLOGINIT;
  LDEBUG << "SemanticRelationsXmlLogger" << LENDL;
    
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  
  const LinguisticAnalysisStructure::AnalysisGraph& graph = 
    *(static_cast<LinguisticAnalysisStructure::AnalysisGraph*>(analysis.getData(m_graph)));
  
  LinguisticGraph* lingGraph = const_cast<LinguisticGraph*>(graph.getGraph());
  VertexTokenPropertyMap tokenMap = get(vertex_token, *lingGraph);
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      SEMANTICANALYSISLOGINIT;
      LERROR << "no LinguisticMetaData ! abort" << LENDL;
      return MISSING_DATA;
  }

   DumperStream* dstream=initialize(analysis);
  std::ostream& outputStream=dstream->out();
  
  ofstream out;
  /*if (!openLogFile(out,metadata->getMetaData("FileName"))) {
    SEMANTICANALYSISLOGINIT;
    LERROR << "Can't open log file " << LENDL;
    return UNKNOWN_ERROR;
  }*/

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

  outputStream << "<?xml version='1.0' encoding='UTF-8'?>" << std::endl;
  outputStream << "<semantic_relation_dump>" << std::endl;
  
  outputStream << "<relations docid=\"" << docId
      << "\" offsetNode=\"" << offsetIndexingNode 
      << "\">" << endl;

//   LDEBUG << "SemanticRelationsXmlLogger on graph " << m_graph << LENDL;
  
  //look at all vertices for annotations
  AnnotationGraphVertexIt itv, itv_end;
  boost::tie(itv, itv_end) = vertices(annotationData->getGraph());
  for (; itv != itv_end; itv++)
  {
    LDEBUG << "SemanticRelationsXmlLogger on annotation vertex " << *itv << LENDL;
    if (annotationData->hasAnnotation(*itv,Common::Misc::utf8stdstring2limastring("SemanticAnnotation")))
    {
//       LDEBUG << "    it has SemanticRelationAnnotation" << LENDL;
      const SemanticAnnotation* annot = 0;
      try
      {
        annot = annotationData->annotation(*itv,Common::Misc::utf8stdstring2limastring("SemanticAnnotation"))
          .pointerValue<SemanticAnnotation>();
      }
      catch (const boost::bad_any_cast& e)
      {
        SEMANTICANALYSISLOGINIT;
        LERROR << "This annotation is not a SemanticRelation" << LENDL;
        continue;
      }

      // output
      outputStream << "<annotation type=\"" << annot->getType() << "\">" << endl
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
           << source(*it,annotGraph) << "->" << target(*it,annotationData->getGraph()) << LENDL;
    if (annotationData->hasAnnotation(*it,Common::Misc::utf8stdstring2limastring("SemanticRelation")))
    {
      SEMANTICANALYSISLOGINIT;
      LDEBUG << "found semantic relation" << LENDL;
      const SemanticRelationAnnotation* annot = 0;
      try
      {
        annot = annotationData->annotation(*it,Common::Misc::utf8stdstring2limastring("SemanticRelation"))
          .pointerValue<SemanticRelationAnnotation>();
      }
      catch (const boost::bad_any_cast& e)
      {
        SEMANTICANALYSISLOGINIT;
        LERROR << "This annotation is not a SemanticAnnotation" << LENDL;
        continue;
      }

      //output
      outputStream << "<relation type=\"" << annot->type() << "\">" << endl
          << vertexStringForSemanticAnnotation("source",source(*it,annotGraph),tokenMap,annotationData,offset)
          << vertexStringForSemanticAnnotation("target",target(*it,annotGraph),tokenMap,annotationData,offset)
          << "</relation>" << endl;
      
    }
  }

//   LDEBUG << "    all vertices done" << LENDL;
  outputStream << "</relations>" << endl;
  //out.close();
  outputStream << "</semantic_relation_dump>" << std::endl;
  delete dstream;
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
    // SEMANTICANALYSISLOGINIT;
    // LDEBUG << *itv << " has no " << m_graph << " annotation. Skeeping it." << LENDL;
    return "";
  }
  v = annotationData->intAnnotation(vertex,Common::Misc::utf8stdstring2limastring(m_graph));
  LinguisticAnalysisStructure::Token* vToken = tokenMap[v];
  //       LDEBUG << "SemanticRelationsXmlLogger tokenMap[" << v << "] = " << vToken << LENDL;
  if (vToken == 0)
  {
    SEMANTICANALYSISLOGINIT;
    LERROR << "Vertex " << v << " has no entry in the analysis graph token map. This should not happen !!" << LENDL;
    return "";
  }

  // get annotation : element in relation can be an entity => get entity type
  // otherwise, its type is "token"
  //EntityT type("token");

 //std::set< uint32_t > matches = annotationData->matches(m_graph,v,"annot");
 std::set< AnnotationGraphVertex > matches = annotationData->matches(m_graph,v,"annot");
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
       it != matches.end(); it++)
  {
    if (annotationData->hasAnnotation(*it,Common::Misc::utf8stdstring2limastring("SpecificEntity"))) {
      const SpecificEntityAnnotation* annot = 0;
      try {
        annot = annotationData->annotation(*it,Common::Misc::utf8stdstring2limastring("SpecificEntity"))
          .pointerValue<SpecificEntityAnnotation>();
      }
      catch (const boost::bad_any_cast& e) {
        SEMANTICANALYSISLOGINIT;
        LERROR << "This annotation is not a SemanticAnnotation" << LENDL;
        continue;
      }
        LimaString typeStr=Common::MediaticData::MediaticData::single().getEntityName(annot->getType());
  oss << "  <" << vertexRole 
      << " type=\"" << Common::Misc::limastring2utf8stdstring(typeStr) << "\"" 
      << " pos=\"" << offset+vToken->position() << "\"" 
      << " len=\"" << vToken->length() << "\"" 
      << " string=\"" << vToken->stringForm() << "\"" 
      << "/>" << endl;
       //EntityType type=Common::MediaticData::MediaticData::single().getEntityName(annot->getType());
      break;
    }
  }

  
  return oss.str();
}


} // SemanticAnalysis
} // LinguisticProcessing
} // Lima

