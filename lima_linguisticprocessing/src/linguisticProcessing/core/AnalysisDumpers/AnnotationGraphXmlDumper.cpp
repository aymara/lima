// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
: MediaProcessUnit(), m_handler()
{
}

AnnotationGraphXmlDumper::AnnotationGraphXmlDumper( const MediaId& language, const std::string& handler )
    : m_language(language), m_handler( handler)
    {}

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
  std::string prologue("<?xml version='1.0' encoding='UTF-8'?>");
  return dump( analysis, prologue );
}

LimaStatusCode AnnotationGraphXmlDumper::dump(
  AnalysisContent& analysis, const std::string& prologue ) const
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

  if( !prologue.empty() )
    outputStream << prologue << std::endl;
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
  catch (DumpGraphVisitor::EndOfSearch&)
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
  xmlStream << "  <vertex id=\"" << v << "\" >" << std::endl;
  outputVertexIAnnotations(v, graph, annotData, xmlStream);
  outputVertexSAnnotations(v, graph, annotData, xmlStream);
  outputVertexGAnnotations(v, graph, annotData, xmlStream);
  xmlStream << "  </vertex>" << std::endl;
}

void AnnotationGraphXmlDumper::outputVertexIAnnotations(const AnnotationGraphVertex v,
                  const AnnotationGraph& graph,
                  const AnnotationData& annotData,
                  std::ostream& xmlStream) const
{
  const auto& map = get(vertex_iannot, graph, v);
  auto it = map.cbegin(); auto it_end = map.cend();
  if (it != it_end)
  {
    xmlStream << "    <iannots>" << std::endl;
    for (; it != it_end; it++)
    {
      xmlStream << "      <iannot name=\""
                << annotData.annotationName((*it).first).toStdString()
                << "\" value=\"" << (*it).second << "\" />" << std::endl;
    }
    xmlStream << "    </iannots>" << std::endl;
  }
}

void AnnotationGraphXmlDumper::outputVertexSAnnotations(const AnnotationGraphVertex v,
                  const AnnotationGraph& graph,
                  const AnnotationData& annotData,
                  std::ostream& xmlStream) const
{
  const auto& map = get(vertex_sannot, graph, v);
  auto it = map.cbegin(); auto it_end = map.cend();

  if (it != it_end)
  {
    xmlStream << "    <sannots>" << std::endl;
    for (; it != it_end; it++)
    {
      xmlStream << "      <sannot name=\""
                << annotData.annotationName((*it).first).toStdString()
                << "\" value=\"" << (*it).second.toStdString() << "\" />" << std::endl;
    }
    xmlStream << "    </sannots>" << std::endl;
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
    xmlStream << "    <annots>" << std::endl;
    for (; it != it_end; it++)
    {
      xmlStream << "      <annot name=\""
                << annotData.annotationName((*it).first).toStdString()
                << "\" value=\"";
      const_cast<AnnotationData&>(annotData).dumpFunction((*it).first)->dump(
        xmlStream, ((*it).second));
      xmlStream << "\" />" << std::endl;
    }
    xmlStream << "    </annots>" << std::endl;
  }
}

void AnnotationGraphXmlDumper::outputEdge(const AnnotationGraphEdge e,
                              const AnnotationGraph& graph,
                              const AnnotationData& annotData,
                              std::ostream& xmlStream) const
{
  xmlStream << "  <edge src=\"" << source(e, graph)
          << "\" targ=\"" << target(e, graph) << "\">" << std::endl;
  outputEdgeIAnnotations(e, graph, annotData, xmlStream);
  outputEdgeSAnnotations(e, graph, annotData, xmlStream);
  outputEdgeGAnnotations(e, graph, annotData, xmlStream);
  xmlStream << "  </edge>" << std::endl;
}

void AnnotationGraphXmlDumper::outputEdgeIAnnotations(const AnnotationGraphEdge e,
                  const AnnotationGraph& graph,
                  const AnnotationData& annotData,
                  std::ostream& xmlStream) const
{
  const AGIannotProp& map = get(edge_iannot, graph, e);
  auto it = map.cbegin(); auto it_end = map.cend();
  if (it != it_end)
  {
    xmlStream << "    <iannots>" << std::endl;
    for (; it != it_end; it++)
    {
      xmlStream << "      <iannot name=\""
                << annotData.annotationName((*it).first).toStdString()
                << "\" value=\"" << (*it).second << "\" />" << std::endl;
    }
    xmlStream << "    </iannots>" << std::endl;
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
    xmlStream << "    <sannots>" << std::endl;
    for (; it != it_end; it++)
    {
      xmlStream << "      <sannot name=\""
                << annotData.annotationName((*it).first).toStdString()
                << "\" value=\"" << (*it).second.toStdString()
                << "\" />" << std::endl;
    }
    xmlStream << "    </sannots>" << std::endl;
  }
}

void AnnotationGraphXmlDumper::outputEdgeGAnnotations(const AnnotationGraphEdge e,
                  const AnnotationGraph& graph,
                  const AnnotationData& annotData,
                  std::ostream& xmlStream) const
{
  const AGGannotProp& map = get(edge_gannot, graph, e);
  auto it = const_cast<AGGannotProp&>(map).begin();
  auto it_end = const_cast<AGGannotProp&>(map).end();
  if (it != it_end)
  {
    xmlStream << "    <annots>" << std::endl;
    for (; it != it_end; it++)
    {
      xmlStream << "      <annot name=\""
                << annotData.annotationName((*it).first).toStdString()
                << "\" value=\"";
      const_cast<AnnotationData&>(annotData).dumpFunction((*it).first)->dump(
        xmlStream, ((*it).second));
      xmlStream << "\" />" << std::endl;
    }
    xmlStream << "    </annots>" << std::endl;
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
