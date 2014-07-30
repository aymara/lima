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
/**
  *
  * @file       annotationGraphXmlDumper.cpp
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *             Copyright (C) 2004 by CEA LIST
  * @date       Mon Nov  8 2004
  * @version    $Id$
  * Project     
  * 
  * @brief      dump the content of the annotation graph in XML format
  * 
  * 
  */

#include "AnnotationGraphXmlDumper.h"

#include "common/time/timeUtilsController.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
// #include "linguisticProcessing/core/LinguisticProcessors/HandlerStreamBuf.h"
#include "common/MediaProcessors/HandlerStreamBuf.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/client/AnalysisHandlers/AbstractTextualAnalysisHandler.h"

#include <fstream>

using namespace boost;
using namespace boost::tuples;

typedef color_traits<default_color_type> Color;

namespace Lima {
    using namespace Common::XMLConfigurationFiles;
    using namespace Common::MediaticData;
    using namespace Common::AnnotationGraphs;
namespace LinguisticProcessing {
namespace AnalysisDumpers {


//***********************************************************************
// constructors
//***********************************************************************
SimpleFactory<MediaProcessUnit,AnnotationGraphXmlDumper> annotationGraphXmlDumperFactory(ANNOTATIONGRAPHXMLDUMPER_CLASSID);

AnnotationGraphXmlDumper::AnnotationGraphXmlDumper()
: MediaProcessUnit(),
m_handler()
{
}

AnnotationGraphXmlDumper::~AnnotationGraphXmlDumper()
{
}

void AnnotationGraphXmlDumper::init(
                                     Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
//  DUMPERLOGINIT;
  m_language=manager->getInitializationParameters().media;
/*  try
  {
    m_dumpFullTokens = (unitConfiguration.getParamsValueAtKey("dumpFullTokens") == "true");
  }
  catch (NoSuchParam& )
  {
    LWARN << "dumpFullTokens parameter not found, using default: " 
        << (m_dumpFullTokens?"true":"false");
  }*/
  try
  {
    m_handler=unitConfiguration.getParamsValueAtKey("handler");
  }
  catch (NoSuchParam& )
  {
    DUMPERLOGINIT;
    LERROR << "AnnotationGraphXmlDumper::init: Missing parameter handler in AnnotationGraphXmlDumper configuration";
    throw InvalidConfiguration();
  }
}

LimaStatusCode AnnotationGraphXmlDumper::process(
  AnalysisContent& analysis) const
{
  TimeUtilsController timer("AnnotationGraphXmlDumper");
  DUMPERLOGINIT;
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      LERROR << "AnnotationGraphXmlDumper::process: no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }
  LDEBUG << "handler will be: " << m_handler;
  AnalysisHandlerContainer* h = static_cast<AnalysisHandlerContainer*>(analysis.getData("AnalysisHandlerContainer"));
  AbstractTextualAnalysisHandler* handler = static_cast<AbstractTextualAnalysisHandler*>(h->getHandler(m_handler));
  if (handler==0)
  {
    LERROR << "AnnotationGraphXmlDumper::process: handler " << m_handler << " has not been given to the core client";
    return MISSING_DATA;
  }
  
  handler->startAnalysis();
  HandlerStreamBuf hsb(handler);
  std::ostream outputStream(&hsb);

  outputStream << "<?xml version='1.0' encoding='UTF-8'?>" << std::endl;
//  outputStream << "<!DOCTYPE lima_analysis_dump SYSTEM \"lima-xml-output.dtd\">" << std::endl;
  outputStream << "<lima_annotation_graph_dump>" << std::endl;
  
  dumpLimaData(outputStream, *(static_cast<AnnotationData*>(analysis.getData("AnnotationData"))));
  
  outputStream << "</lima_annotation_graph_dump>" << std::endl;
  handler->endAnalysis();

  return SUCCESS_ID;
}


//***********************************************************************
// main function for outputing the graph
//***********************************************************************
void AnnotationGraphXmlDumper::dumpLimaData(std::ostream& os,
                             const AnnotationData& annotData) const
{
  AnnotationGraph& graph = const_cast<AnnotationGraph&>(annotData.getGraph());
  
  // go through the graph, add BoWTokens that are not in complex terms
  DumpGraphVisitor vis(*this, annotData, os, m_language);

  os << "<annot-graph>" << std::endl;
  try 
  {
      AnnotationGraphVertexIt i, i_end;
      boost::tie(i, i_end) = vertices(graph);
      boost::depth_first_search(graph, boost::visitor(vis));
  }
  catch (DumpGraphVisitor::EndOfSearch)
  { //do nothing: normal ending
  }
  os << "</annot-graph>" << std::endl;
}

//***********************************************************************
// output functions
//***********************************************************************


void AnnotationGraphXmlDumper::outputVertex(const AnnotationGraphVertex v,
                                const AnnotationGraph& graph,
                                const AnnotationData& annotData,
                                std::ostream& xmlStream) const 
{
  xmlStream << "<vertex id=\"" << v << "\" >" << std::endl;  
  outputVertexIAnnotations(v, graph, annotData, xmlStream);
  outputVertexSAnnotations(v, graph, annotData, xmlStream);
  outputVertexGAnnotations(v, graph, annotData, xmlStream);
  xmlStream << "</vertex>" << std::endl;  
}
  
void AnnotationGraphXmlDumper::outputVertexIAnnotations(const AnnotationGraphVertex v,
                  const AnnotationGraph& graph,
                  const AnnotationData& annotData,
                  std::ostream& xmlStream) const
{
  const AGIannotProp& map = get(vertex_iannot, graph, v);
  AGIannotProp::const_iterator it, it_end;
  it = map.begin(); it_end = map.end();
  if (it != it_end)
  {
    xmlStream << "<iannots>" << std::endl;
    for (; it != it_end; it++)
    {
      xmlStream << "<iannot name=\"" << annotData.annotationName((*it).first) << "\" value=\"" << (*it).second << "\" />" << std::endl;    
    }
    xmlStream << "</iannots>" << std::endl;
  }
}

void AnnotationGraphXmlDumper::outputVertexSAnnotations(const AnnotationGraphVertex v,
                  const AnnotationGraph& graph,
                  const AnnotationData& annotData,
                  std::ostream& xmlStream) const
{
  const AGSannotProp& map = get(vertex_sannot, graph, v);
  AGSannotProp::const_iterator it, it_end;
  it = map.begin(); it_end = map.end();
  
  if (it != it_end)
  {
    xmlStream << "<sannots>" << std::endl;
    for (; it != it_end; it++)
    {
      xmlStream << "<sannot name=\"" << annotData.annotationName((*it).first) << "\" value=\"" << (*it).second << "\" />" << std::endl;    
    }
    xmlStream << "</sannots>" << std::endl;
  }
}

void AnnotationGraphXmlDumper::outputVertexGAnnotations(const AnnotationGraphVertex v,
                  const AnnotationGraph& graph,
                  const AnnotationData& annotData,
                  std::ostream& xmlStream) const
{
  const AGGannotProp& map = get(vertex_gannot, graph, v);
  AGGannotProp::iterator it, it_end;
  it = const_cast<AGGannotProp&>(map).begin(); it_end = const_cast<AGGannotProp&>(map).end();
  if (it != it_end)
  {
    xmlStream << "<annots>" << std::endl;
    for (; it != it_end; it++)
    {
      xmlStream << "<annot name=\"" << annotData.annotationName((*it).first) << "\" value=\"";
      const_cast<AnnotationData&>(annotData).dumpFunction((*it).first)->dump(xmlStream, ((*it).second));
      xmlStream << "\" />" << std::endl;    
    }
    xmlStream << "</annots>" << std::endl;
  }
}

void AnnotationGraphXmlDumper::outputEdge(const AnnotationGraphEdge e,
                              const AnnotationGraph& graph,
                              const AnnotationData& annotData,
                              std::ostream& xmlStream) const
{
  xmlStream << "<edge src=\"" << source(e, graph) 
          << "\" targ=\"" << target(e, graph) << "\" />" << std::endl;  
  outputEdgeIAnnotations(e, graph, annotData, xmlStream);
  outputEdgeSAnnotations(e, graph, annotData, xmlStream);
  outputEdgeGAnnotations(e, graph, annotData, xmlStream);
  xmlStream << "</edge>" << std::endl;  
}
  
void AnnotationGraphXmlDumper::outputEdgeIAnnotations(const AnnotationGraphEdge e,
                  const AnnotationGraph& graph,
                  const AnnotationData& annotData,
                  std::ostream& xmlStream) const
{
  const AGIannotProp& map = get(edge_iannot, graph, e);
  AGIannotProp::const_iterator it, it_end;
  it = map.begin(); it_end = map.end();
  if (it != it_end)
  {
    xmlStream << "<iannots>" << std::endl;
    for (; it != it_end; it++)
    {
      xmlStream << "<iannot name=\"" << annotData.annotationName((*it).first) << "\" value=\"" << (*it).second << "\" />" << std::endl;    
    }
    xmlStream << "</iannots>" << std::endl;
  }
}

void AnnotationGraphXmlDumper::outputEdgeSAnnotations(const AnnotationGraphEdge e,
                  const AnnotationGraph& graph,
                  const AnnotationData& annotData,
                  std::ostream& xmlStream) const
{
  const AGSannotProp& map = get(edge_sannot, graph, e);
  AGSannotProp::const_iterator it, it_end;
  it = map.begin(); it_end = map.end();
  if (it != it_end)
  {
    xmlStream << "<sannots>" << std::endl;
    for (; it != it_end; it++)
    {
      xmlStream << "<sannot name=\"" << annotData.annotationName((*it).first) << "\" value=\"" << (*it).second << "\" />" << std::endl;    
    }
    xmlStream << "</sannots>" << std::endl;
  }
}

void AnnotationGraphXmlDumper::outputEdgeGAnnotations(const AnnotationGraphEdge e,
                  const AnnotationGraph& graph,
                  const AnnotationData& annotData,
                  std::ostream& xmlStream) const
{
  const AGGannotProp& map = get(edge_gannot, graph, e);
  AGGannotProp::iterator it, it_end;
  it = const_cast<AGGannotProp&>(map).begin(); it_end = const_cast<AGGannotProp&>(map).end();
  if (it != it_end)
  {
    xmlStream << "<annots>" << std::endl;
    for (; it != it_end; it++)
    {
      xmlStream << "<annot name=\"" << annotData.annotationName((*it).first) << "\" value=\"";
      const_cast<AnnotationData&>(annotData).dumpFunction((*it).first)->dump(xmlStream, ((*it).second));
      xmlStream << "\" />" << std::endl;    
    }
    xmlStream << "</annots>" << std::endl;
  }
}

//***********************************************************************
// visitor definition
//***********************************************************************
// examine_vertex to test if we are at end of search
void AnnotationGraphXmlDumper::DumpGraphVisitor::examine_vertex(AnnotationGraphVertex v,
  const AnnotationGraph& g) 
{
  LIMA_UNUSED(g);
  DUMPERLOGINIT;
  LDEBUG << "DumpGraphVisitor: examine_vertex " << v;
}

void AnnotationGraphXmlDumper::DumpGraphVisitor::examine_edge(AnnotationGraphEdge e,
               const AnnotationGraph& g) 
{
    DUMPERLOGINIT;
    LDEBUG << "DumpGraphVisitor: discover_edge " << e;

    m_dumper.outputEdge(e, g, m_data, m_os);      
}

void AnnotationGraphXmlDumper::DumpGraphVisitor::discover_vertex(AnnotationGraphVertex v,
                const AnnotationGraph& graph)
{
    m_dumper.outputVertex(v, graph, m_data, m_os);
}

} // end namespace AnalysisDumpers
} // end namespace LinguisticProcessings
} // end namespace Lima
