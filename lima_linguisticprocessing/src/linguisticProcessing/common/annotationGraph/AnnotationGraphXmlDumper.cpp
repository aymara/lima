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
  * @file       AnnotationGraphXmlDumper.cpp
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *             Copyright (C) 2005-2012 by CEA LIST
  * @date       Mon Nov 23 2005
  *
  * @brief      dump the full content of the annotation graph in XML format
  *
  *
  */

#include "AnnotationGraphXmlDumper.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"

using namespace std;
using namespace boost;



typedef color_traits<default_color_type> Color;

namespace Lima {
namespace Common {
namespace AnnotationGraphs {


AnnotationGraphXmlDumper::AnnotationGraphXmlDumper()
{
}

AnnotationGraphXmlDumper::~AnnotationGraphXmlDumper()
{
}

//***********************************************************************
// main function for outputing the graph
//***********************************************************************
void AnnotationGraphXmlDumper::dump(
    std::ostream& os,
    const AnnotationGraph* graph,
    const AnnotationData& annotationData) const
{
  DumpGraphVisitor vis(os, annotationData);

  os << "<annot-graph>" << std::endl;
//   Color color;
  boost::depth_first_search(*graph, boost::visitor(vis));
  os << "  <matchings>" << std::endl;
  std::map<StringsPoolIndex, std::multimap<AnnotationGraphVertex, AnnotationGraphVertex> >::const_iterator it, it_end;
  it = annotationData.matchings().begin();
  it_end = annotationData.matchings().end();
  for (; it != it_end; it++)
  {
    const std::multimap<AnnotationGraphVertex, AnnotationGraphVertex>& matching = (*it).second;
    os << "    <matching id=\"" << Misc::limastring2utf8stdstring(annotationData.annotationName((*it).first)) << "\">" << std::endl;
    std::multimap<AnnotationGraphVertex, AnnotationGraphVertex>::const_iterator sit, sit_end;
    sit = matching.begin(); sit_end = matching.end();
    for (; sit != sit_end; sit++)
    {
      os << "      <pair k=\""<<(*sit).first<<"\" v=\""<<(*sit).second<<"\"/>" << std::endl;
    }
    os << "    </matching>" << std::endl;
  }
  os << "  </matchings>" << std::endl;
  os << "</annot-graph>" << std::endl;
}

//***********************************************************************
// visitor definition
//***********************************************************************

void AnnotationGraphXmlDumper::DumpGraphVisitor::discover_vertex(
    AnnotationGraphVertex v,
    const AnnotationGraph& g)
{
  AGLOGINIT;
  LDEBUG << "DumpGraphVisitor: discover_vertex " << v << LENDL;

  m_os << "<vertex v=\"" << v << "\">" << std::endl;
  /// @todo parcours des annotations, dump de chaque
  CVertexAGIannotPropertyMap imap = get(vertex_iannot, g);
  if (!imap[v].empty())
  {
    m_os << "<iannots>" << std::endl;
    AGIannotProp::const_iterator it, it_end;
    it = imap[v].begin(); it_end = imap[v].end();
    for (; it != it_end; it++)
    {
      m_os << "<iannot name=\""
          << Misc::limastring2utf8stdstring(m_annotData.annotationName((*it).first))
          << "\" value=\"" << (*it).second << "\"/>" << std::endl;
    }
    m_os << "</iannots>" << std::endl;
  }
  CVertexAGSannotPropertyMap smap = get(vertex_sannot, g);
  if (!smap[v].empty())
  {
    m_os << "<sannots>" << std::endl;
    AGSannotProp::const_iterator it, it_end;
    it = smap[v].begin(); it_end = smap[v].end();
    for (; it != it_end; it++)
    {
      m_os << "<sannot name=\""
          << Misc::limastring2utf8stdstring(m_annotData.annotationName((*it).first))
          << "\" value=\""
        << Misc::limastring2utf8stdstring((*it).second)
          << "\"/>" << std::endl;
    }
    m_os << "</sannots>" << std::endl;
  }
  CVertexAGGannotPropertyMap gmap = get(vertex_gannot, g);
  if (!gmap[v].empty())
  {
    m_os << "<gannots>" << std::endl;
    AGGannotProp::const_iterator it, it_end;
    it = gmap[v].begin(); it_end = gmap[v].end();
    for (; it != it_end; it++)
    {
      m_os << "<gannot name=\""
          << Misc::limastring2utf8stdstring(m_annotData.annotationName((*it).first))
          << "\">" << std::endl;
      const_cast<AnnotationData*>(&m_annotData)->dumpFunction((*it).first)->dump(
          m_os,
      const_cast<GenericAnnotation&>(((*it).second)));
      m_os << "</gannot>" << std::endl;
    }
    m_os << "</gannots>" << std::endl;
  }
  m_os << "</vertex>" << std::endl;
}

void AnnotationGraphXmlDumper::DumpGraphVisitor::examine_edge(
    AnnotationGraphEdge e,
    const AnnotationGraph& g)
{
  AGLOGINIT;
  LDEBUG << "DumpGraphVisitor: examine_edge " << e << LENDL;

  m_os << "<edge src=\""<<source(e,g)<<"\" tgt=\""<<target(e,g)<<"\">" << std::endl;
  CEdgeAGIannotPropertyMap imap = get(edge_iannot, g);
  if (!imap[e].empty())
  {
    m_os << "<iannots>" << std::endl;
    AGIannotProp::const_iterator it, it_end;
    it = imap[e].begin(); it_end = imap[e].end();
    for (; it != it_end; it++)
    {
      m_os << "<iannot name=\""
          << Misc::limastring2utf8stdstring(m_annotData.annotationName((*it).first))
          << "\" value=\"" << (*it).second << "\"/>" << std::endl;
    }
    m_os << "</iannots>" << std::endl;
  }
  CEdgeAGSannotPropertyMap smap = get(edge_sannot, g);
  if (!smap[e].empty())
  {
    m_os << "<sannots>" << std::endl;
    AGSannotProp::const_iterator it, it_end;
    it = smap[e].begin(); it_end = smap[e].end();
    for (; it != it_end; it++)
    {
      m_os << "<sannot name=\""
          << Misc::limastring2utf8stdstring(m_annotData.annotationName((*it).first))
          << "\" value=\""
        << Misc::limastring2utf8stdstring((*it).second)
          << "\"/>" << std::endl;
    }
    m_os << "</sannots>" << std::endl;
  }
  CEdgeAGGannotPropertyMap gmap = get(edge_gannot, g);
  if (!gmap[e].empty())
  {
    m_os << "<gannots>" << std::endl;
    AGGannotProp::const_iterator it, it_end;
    it = gmap[e].begin(); it_end = gmap[e].end();
    for (; it != it_end; it++)
    {
      m_os << "<gannot name=\""
          << Misc::limastring2utf8stdstring(m_annotData.annotationName((*it).first))
          << "\">" << std::endl;
      const_cast<AnnotationData*>(&m_annotData)->dumpFunction((*it).first)->dump(
          m_os,
          const_cast<GenericAnnotation&>(((*it).second)));
      m_os << "</gannot>" << std::endl;
    }
    m_os << "</gannots>" << std::endl;
  }
  m_os << "</edge>" << std::endl;
}

} // end namespace AnalysisDumpers
} // end namespace LinguisticProcessings
} // end namespace Lima
