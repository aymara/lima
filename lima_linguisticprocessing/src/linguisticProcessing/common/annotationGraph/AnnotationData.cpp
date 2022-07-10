// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** @brief       A structures that holds an annotation graph and gives a high
  *              level API to access it
  *
  * @file        AnnotationData.cpp
  * @author      Gael de Chalendar <Gael.de-Chalendar@cea.fr>

  *              Copyright (c) 2004-2005 by CEA
  * @date        Created on Nov, 8 2004
  * @version     $Id$
  *
  */

#ifndef WIN32
#include <cstdint> //uint32_t
#endif
#include "AnnotationData.h"
#include "common/Data/strwstrtools.h"

//using namespace boost;

namespace Lima
{
namespace Common
{
namespace AnnotationGraphs
{

/** @brief Definition of a dummy function suitable to be used as a dumper for
  * the annotations of an annotation graph
  *
  * @param @b os <I>std::ostream&amp;</I> the stream on which to dump
  * @param @b ga <I>GenericAnnotation&amp;</I> the annotation to be dumped.
  * @return <I>int</I> The dummy dumping is considered as succesful, so
  *         SUCCESS_ID is returned.
  */
class LIMA_ANNOTATIONGRAPH_EXPORT DummyDumpFunction :
    public AnnotationData::Dumper
{
public:
  virtual int dump(std::ostream&, GenericAnnotation&) const override;
};



class AnnotationDataPrivate
{
  friend class AnnotationData;

  /** @brief Default constructor */
  AnnotationDataPrivate();

  /** @brief Default destructor */
  ~AnnotationDataPrivate();

  /** @brief Holds the graph */
  AnnotationGraph m_graph;

  /** @brief Contains mapping between annotations names and annotations ids */
  Lima::StringsPool m_pool;

  /** @brief Associates a dumping function to each annotation id */
  std::map< uint64_t, const AnnotationData::Dumper* > m_dumpFunctions;

  /** @brief Stores matching between various graphs vertices and the annotation
   *  graph vertices.
   *
   *  each key is the name of a matching (e.g. index of 'morph2annot') and the value is the
   *  mapping itsel in which the keys are the source graph vertices and the
   *  values are the matching target graph vertices
   */
  std::map<StringsPoolIndex, std::multimap<AnnotationGraphVertex, AnnotationGraphVertex> > m_matchings;

  CVertexAGGannotPropertyMap m_mapCVertexAGGannotPropertyMap;
  CVertexAGSannotPropertyMap m_mapCVertexAGSannotPropertyMap;
  VertexAGIannotPropertyMap m_mapVertexAGIannotPropertyMap;
  VertexAGSannotPropertyMap m_mapVertexAGSannotPropertyMap;
  EdgeAGIannotPropertyMap m_mapEdgeAGIannotPropertyMap;
  CEdgeAGIannotPropertyMap m_mapCEdgeAGIannotPropertyMap;
  VertexAGGannotPropertyMap m_mapVertexAGGannotPropertyMap;
  EdgeAGSannotPropertyMap m_mapEdgeAGSannotPropertyMap;
  CEdgeAGSannotPropertyMap m_mapCEdgeAGSannotPropertyMap;
  EdgeAGGannotPropertyMap m_mapEdgeAGGannotPropertyMap;
  CEdgeAGGannotPropertyMap m_mapCEdgeAGGannotPropertyMap;
  CVertexAGIannotPropertyMap m_mapCVertexAGIannotPropertyMap;
};


AnnotationDataPrivate::AnnotationDataPrivate() :
  m_graph(),
  m_pool(),
  m_dumpFunctions(),
  m_matchings(),
  m_mapCVertexAGGannotPropertyMap( get(vertex_gannot, (const AnnotationGraph&)m_graph) ),
  m_mapCVertexAGSannotPropertyMap( get(vertex_sannot, (const AnnotationGraph&)m_graph) ),
  m_mapVertexAGIannotPropertyMap( get(vertex_iannot, m_graph) ),
  m_mapVertexAGSannotPropertyMap( get(vertex_sannot, m_graph) ),
  m_mapEdgeAGIannotPropertyMap( get(edge_iannot, m_graph) ),
  m_mapCEdgeAGIannotPropertyMap( get(edge_iannot, (const AnnotationGraph&)m_graph) ),
  m_mapVertexAGGannotPropertyMap( get(vertex_gannot, m_graph) ),
  m_mapEdgeAGSannotPropertyMap( get(edge_sannot, m_graph) ),
  m_mapCEdgeAGSannotPropertyMap( get(edge_sannot, (const AnnotationGraph&)m_graph) ),
  m_mapEdgeAGGannotPropertyMap( get(edge_gannot, m_graph) ),
  m_mapCEdgeAGGannotPropertyMap( get(edge_gannot, (const AnnotationGraph&)m_graph) ),
  m_mapCVertexAGIannotPropertyMap( get(vertex_iannot, (const AnnotationGraph&)m_graph) )
{
}

AnnotationDataPrivate::~AnnotationDataPrivate()
{
  for (std::map< uint64_t, const AnnotationData::Dumper* >::const_iterator it=m_dumpFunctions.begin();
       it!=m_dumpFunctions.end();
       it++)
  {
    delete it->second;
  }
}



AnnotationData::AnnotationData() :
  m_d(new AnnotationDataPrivate())
{
}

AnnotationData::~AnnotationData()
{
  delete m_d;
}

AnnotationGraph& AnnotationData::getGraph() { return m_d->m_graph; }
const AnnotationGraph& AnnotationData::getGraph() const { return m_d->m_graph; }
/** @} */

/** @brief Gives the name of an annotation id */
const LimaString& AnnotationData::annotationName(uint64_t annotationId) const
{
  return m_d->m_pool[static_cast<StringsPoolIndex>(annotationId)];
}

/** @brief Gives the id of an annotation name */
uint64_t AnnotationData::annotationId(const LimaString& s)
{
  return m_d->m_pool[s];
}


AnnotationGraphVertex AnnotationData::createAnnotationVertex()
{
  return add_vertex(m_d->m_graph);
}

AnnotationGraphEdge AnnotationData::createAnnotationEdge(AnnotationGraphVertex s, AnnotationGraphVertex t)
{
#ifdef DEBUG_LP
  AGLOGINIT;
  LDEBUG << "Creating annotation edge " << s << " -> " << t;
#endif
  return add_edge(s, t, m_d->m_graph).first;
}


void AnnotationData::annotate(AnnotationGraphVertex v, const LimaString& annot, uint64_t value)
{
  /*
    Critical function : comment logging messages
  */
  annotate(v, m_d->m_pool[annot], value);
}

void AnnotationData::annotate(AnnotationGraphVertex v, const LimaString& annot, const LimaString& value)
{
  /*
    Critical function : comment logging messages
  */
  annotate(v, m_d->m_pool[annot], value);
}

void AnnotationData::annotate(AnnotationGraphVertex v, const LimaString& annot, const GenericAnnotation& value)
{
  /*
    Critical function : comment logging messages
  */
  annotate(v, m_d->m_pool[annot], value);
}


void AnnotationData::annotate(AnnotationGraphVertex vs, AnnotationGraphVertex vt, const LimaString& annot, uint64_t value)
{
  bool ok; AnnotationGraphEdge e;
  boost::tie(e, ok) = edge(vs,vt,m_d->m_graph);
  if (!ok) e = createAnnotationEdge(vs, vt);
  annotate(e, m_d->m_pool[annot], value);
}

void AnnotationData::annotate(AnnotationGraphVertex vs, AnnotationGraphVertex vt, const LimaString& annot, const LimaString& value)
{
  bool ok; AnnotationGraphEdge e;
  boost::tie(e, ok) = edge(vs,vt,m_d->m_graph);
  if (!ok) e = createAnnotationEdge(vs, vt);
  annotate(e, m_d->m_pool[annot], value);
}

void AnnotationData::annotate(AnnotationGraphVertex vs, AnnotationGraphVertex vt, const LimaString& annot, const GenericAnnotation& value)
{
  bool ok; AnnotationGraphEdge e;
  boost::tie(e, ok) = edge(vs,vt,m_d->m_graph);
  if (!ok) e = createAnnotationEdge(vs, vt);
  annotate(e, m_d->m_pool[annot], value);
}


void AnnotationData::annotate(AnnotationGraphEdge e, const LimaString& annot, uint64_t value)
{
  /*
    Critical function : comment logging messages
  */
//  AGLOGINIT;
//  LDEBUG << "Annotating " << e << " ("<<annot<<") : " << value;
  annotate(e, m_d->m_pool[annot], value);
}

void AnnotationData::annotate(AnnotationGraphEdge e, const LimaString& annot, const LimaString& value)
{
  /*
    Critical function : comment logging messages
  */
//  AGLOGINIT;
//  LDEBUG << "Annotating " << e << " ("<<annot<<") : " << value;
  annotate(e, m_d->m_pool[annot], value);
}

void AnnotationData::annotate(AnnotationGraphEdge e, const LimaString& annot, const GenericAnnotation& value)
{
  /*
    Critical function : comment logging messages
  */
//  AGLOGINIT;
//  LDEBUG << "Annotating " << e << " ("<<annot<<") : GA";
  annotate(e, m_d->m_pool[annot], value);
}


uint64_t AnnotationData::intAnnotation(AnnotationGraphVertex v, const LimaString& annot) const
{
  const uint64_t idx = const_cast<AnnotationData*>(this)->m_d->m_pool[annot];
  return intAnnotation(v, idx);
}

const LimaString& AnnotationData::stringAnnotation(AnnotationGraphVertex v, const LimaString& annot) const
{
  return stringAnnotation(v, const_cast<AnnotationData*>(this)->m_d->m_pool[annot]);
}

const GenericAnnotation& AnnotationData::annotation(AnnotationGraphVertex v, const LimaString& annot) const
{
  return annotation(v, const_cast<AnnotationData*>(this)->m_d->m_pool[annot]);
}

GenericAnnotation& AnnotationData::annotation(AnnotationGraphVertex v, const LimaString& annot)
{
  return annotation(v, const_cast<AnnotationData*>(this)->m_d->m_pool[annot]);
}

uint64_t AnnotationData::intAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, const LimaString& annot) const
{
  const uint64_t idx = const_cast<AnnotationData*>(this)->m_d->m_pool[annot];
  return intAnnotation(v1, v2, idx);
}

const LimaString& AnnotationData::stringAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, const LimaString& annot) const
{
  return stringAnnotation(v1, v2, const_cast<AnnotationData*>(this)->m_d->m_pool[annot]);
}

const GenericAnnotation& AnnotationData::annotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, const LimaString& annot) const
{
  return annotation(v1, v2, const_cast<AnnotationData*>(this)->m_d->m_pool[annot]);
}

GenericAnnotation& AnnotationData::annotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, const LimaString& annot)
{
  return annotation(v1, v2, const_cast<AnnotationData*>(this)->m_d->m_pool[annot]);
}


uint64_t AnnotationData::intAnnotation(AnnotationGraphEdge e, const LimaString& annot)
{
  return intAnnotation(e, m_d->m_pool[annot]);
}

const LimaString& AnnotationData::stringAnnotation(AnnotationGraphEdge e, const LimaString& annot)
{
  return stringAnnotation(e, m_d->m_pool[annot]);
}

const GenericAnnotation& AnnotationData::annotation(AnnotationGraphEdge e, const LimaString& annot) const
{
  return annotation(e, m_d->m_pool[annot]);
}

GenericAnnotation& AnnotationData::annotation(AnnotationGraphEdge e, const LimaString& annot)
{
  return annotation(e, m_d->m_pool[annot]);
}


/** for annotation by feature id, the id have to already exist with a name */
void AnnotationData::annotate(AnnotationGraphVertex v, uint64_t annot, uint64_t value)
{
  if (m_d->m_dumpFunctions.find(annot) == m_d->m_dumpFunctions.end())
  {
    m_d->m_dumpFunctions[annot] = new DummyDumpFunction();
  }
  m_d->m_mapVertexAGIannotPropertyMap[v][annot] = value;
#ifdef DEBUG_LP
  AGLOGINIT;
  LDEBUG << "Annotating " << v << " ("<<m_d->m_pool[StringsPoolIndex(annot)]<<") : " << value;
#endif
}

void AnnotationData::annotate(AnnotationGraphVertex v, uint64_t annot, const LimaString& value)
{
  if (m_d->m_dumpFunctions.find(annot) == m_d->m_dumpFunctions.end())
  {
    m_d->m_dumpFunctions[annot] = new DummyDumpFunction();
  }
  m_d->m_mapVertexAGSannotPropertyMap[v][annot] = value;
#ifdef DEBUG_LP
  AGLOGINIT;
  LDEBUG << "Annotating " << v << " ("<<m_d->m_pool[StringsPoolIndex(annot)]<<") : " << value;
#endif
}

void AnnotationData::annotate(AnnotationGraphVertex v, uint64_t annot, const GenericAnnotation& value)
{
  if (m_d->m_dumpFunctions.find(annot) == m_d->m_dumpFunctions.end())
  {
    m_d->m_dumpFunctions[annot] = new DummyDumpFunction();
  }
  if (num_vertices(m_d->m_graph) <= v)
  {
    std::ostringstream oss;
    oss << "Trying to annotate an inexisting vertex: " << v
    << ". Graph size in vertices is: " << num_vertices(m_d->m_graph) << std::endl;
    throw std::range_error(oss.str());
  }
  std::map< uint64_t, GenericAnnotation > m = m_d->m_mapVertexAGGannotPropertyMap[v];
  m_d->m_mapVertexAGGannotPropertyMap[v][annot] = value;
#ifdef DEBUG_LP
  AGLOGINIT;
  LDEBUG << "Annotating " << v << " ("<< m_d->m_pool[StringsPoolIndex(annot)] <<") : GA";
#endif
}


void AnnotationData::annotate(AnnotationGraphVertex vs, AnnotationGraphVertex vt, uint64_t annot, uint64_t value)
{
  bool ok; AnnotationGraphEdge e;
  boost::tie(e, ok) = edge(vs,vt,m_d->m_graph);
  if (!ok) e = createAnnotationEdge(vs, vt);
  annotate(e, annot, value);
#ifdef DEBUG_LP
  AGLOGINIT;
  LDEBUG << "Annotating " << vs << " -> " << vt << " ("<<m_d->m_pool[StringsPoolIndex(annot)]<<") : " << value;
#endif
}

void AnnotationData::annotate(AnnotationGraphVertex vs, AnnotationGraphVertex vt, uint64_t annot, const LimaString& value)
{
  bool ok; AnnotationGraphEdge e;
  boost::tie(e, ok) = edge(vs,vt,m_d->m_graph);
  if (!ok) e = createAnnotationEdge(vs, vt);
  annotate(e, annot, value);
}

void AnnotationData::annotate(AnnotationGraphVertex vs, AnnotationGraphVertex vt, uint64_t annot, const GenericAnnotation& value)
{
  bool ok; AnnotationGraphEdge e;
  boost::tie(e, ok) = edge(vs,vt,m_d->m_graph);
  if (!ok) e = createAnnotationEdge(vs, vt);
  annotate(e, annot, value);
}


void AnnotationData::annotate(AnnotationGraphEdge e, uint64_t annot, uint64_t value)
{
  if (m_d->m_dumpFunctions.find(annot) == m_d->m_dumpFunctions.end())
  {
    m_d->m_dumpFunctions[annot] = new DummyDumpFunction();
  }
  m_d->m_mapEdgeAGIannotPropertyMap[e][annot] = value;
}

void AnnotationData::annotate(AnnotationGraphEdge e, uint64_t annot, const LimaString& value)
{
  if (m_d->m_dumpFunctions.find(annot) == m_d->m_dumpFunctions.end())
  {
    m_d->m_dumpFunctions[annot] = new DummyDumpFunction();
  }
  m_d->m_mapEdgeAGSannotPropertyMap[e][annot] = value;
}

void AnnotationData::annotate(AnnotationGraphEdge e, uint64_t annot, const GenericAnnotation& value)
{
  if (m_d->m_dumpFunctions.find(annot) == m_d->m_dumpFunctions.end())
  {
    m_d->m_dumpFunctions[annot] = new DummyDumpFunction();
  }
  m_d->m_mapEdgeAGGannotPropertyMap[e][annot] = value;
}


uint64_t AnnotationData::intAnnotation(AnnotationGraphVertex v, uint64_t annot) const
{
  return m_d->m_mapVertexAGIannotPropertyMap[v][annot];
}

const LimaString& AnnotationData::stringAnnotation(AnnotationGraphVertex v, uint64_t annot) const
{
  return m_d->m_mapVertexAGSannotPropertyMap[v][annot];
}

const GenericAnnotation& AnnotationData::annotation(AnnotationGraphVertex v, uint64_t annot) const
{
  return m_d->m_mapVertexAGGannotPropertyMap[v][annot];
}

GenericAnnotation& AnnotationData::annotation(AnnotationGraphVertex v, uint64_t annot)
{
  return m_d->m_mapVertexAGGannotPropertyMap[v][annot];
}


uint64_t AnnotationData::intAnnotation(AnnotationGraphEdge e, uint64_t annot)
{
  return m_d->m_mapEdgeAGIannotPropertyMap[e][annot];
}

LimaString& AnnotationData::stringAnnotation(AnnotationGraphEdge e, uint64_t annot)
{
  return m_d->m_mapEdgeAGSannotPropertyMap[e][annot];
}

const GenericAnnotation& AnnotationData::annotation(AnnotationGraphEdge e, uint64_t annot) const
{
  return (*(m_d->m_mapCEdgeAGGannotPropertyMap[e].find(annot))).second;
}

GenericAnnotation& AnnotationData::annotation(AnnotationGraphEdge e, uint64_t annot)
{
  return m_d->m_mapEdgeAGGannotPropertyMap[e][annot];
}


//////////////////////////////////////
uint64_t AnnotationData::intAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, uint64_t annot) const
{
  bool ok; AnnotationGraphEdge e;
  boost::tie(e, ok) = edge(v1,v2,m_d->m_graph);
  if (!ok)
  {
    std::ostringstream oss;
    oss << "Error trying to access to inexistant edge " << v1 << " -> " << v2;
    throw std::runtime_error(oss.str().c_str());
  }
  return (*(m_d->m_mapCEdgeAGIannotPropertyMap[e].find(annot))).second;
}

const LimaString& AnnotationData::stringAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, uint64_t annot) const
{
  bool ok; AnnotationGraphEdge e;
  boost::tie(e, ok) = edge(v1,v2,m_d->m_graph);
  if (!ok)
  {
    std::ostringstream oss;
    oss << "Error trying to access to inexistant edge " << v1 << " -> " << v2;
    throw std::runtime_error(oss.str().c_str());
  }
  return (*(m_d->m_mapCEdgeAGSannotPropertyMap[e].find(annot))).second;
}

const GenericAnnotation& AnnotationData::annotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, uint64_t annot) const
{
  bool ok; AnnotationGraphEdge e;
  boost::tie(e, ok) = edge(v1,v2,m_d->m_graph);
  if (!ok)
  {
    std::ostringstream oss;
    oss << "Error trying to access to inexistant edge " << v1 << " -> " << v2;
    throw std::runtime_error(oss.str().c_str());
  }
  if (m_d->m_mapCEdgeAGGannotPropertyMap[e].find(annot) == m_d->m_mapCEdgeAGGannotPropertyMap[e].end())
  {
    std::ostringstream oss;
    oss << "Error trying to access to inexistant edge annotation" << v1 << " -> " << v2 << ", " << annot;
    throw std::runtime_error(oss.str().c_str());
  }
  return (*(m_d->m_mapCEdgeAGGannotPropertyMap[e].find(annot))).second;
}

GenericAnnotation& AnnotationData::annotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, uint64_t annot)
{
  bool ok; AnnotationGraphEdge e;
  boost::tie(e, ok) = edge(v1,v2,m_d->m_graph);
  if (!ok)
  {
    std::ostringstream oss;
    oss << "Error trying to access to inexistant edge " << v1 << " -> " << v2;
    throw std::runtime_error(oss.str().c_str());
  }
  if (m_d->m_mapEdgeAGGannotPropertyMap[e].find(annot) == m_d->m_mapEdgeAGGannotPropertyMap[e].end())
  {
    std::ostringstream oss;
    oss << "Error trying to access to inexistant edge annotation" << v1 << " -> " << v2 << ", " << annot;
    throw std::runtime_error(oss.str().c_str());
  }
  return (*(m_d->m_mapEdgeAGGannotPropertyMap[e].find(annot))).second;
}
//////////////////////////////////////



bool AnnotationData::hasIntAnnotation(AnnotationGraphVertex v, const LimaString& annot) const
{
  return hasIntAnnotation(v, const_cast<AnnotationData*>(this)->m_d->m_pool[annot]);
}

bool AnnotationData::hasStringAnnotation(AnnotationGraphVertex v, const LimaString& annot) const
{
  return hasStringAnnotation(v, const_cast<AnnotationData*>(this)->m_d->m_pool[annot]);
}

bool AnnotationData::hasAnnotation(AnnotationGraphVertex v, const LimaString& annot) const
{
  return hasAnnotation(v, const_cast<AnnotationData*>(this)->m_d->m_pool[annot]);
}

bool AnnotationData::hasIntAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, const LimaString& annot) const
{
  return hasIntAnnotation(v1, v2, const_cast<AnnotationData*>(this)->m_d->m_pool[annot]);
}

bool AnnotationData::hasStringAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, const LimaString& annot) const
{
  return hasStringAnnotation(v1, v2, const_cast<AnnotationData*>(this)->m_d->m_pool[annot]);
}

bool AnnotationData::hasAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, const LimaString& annot) const
{
  return hasAnnotation(v1, v2, const_cast<AnnotationData*>(this)->m_d->m_pool[annot]);
}

bool AnnotationData::hasIntAnnotation(AnnotationGraphEdge e, const LimaString& annot) const
{
  return hasIntAnnotation(e, const_cast<AnnotationData*>(this)->m_d->m_pool[annot]);
}

bool AnnotationData::hasStringAnnotation(AnnotationGraphEdge e, const LimaString& annot) const
{
  return hasStringAnnotation(e, const_cast<AnnotationData*>(this)->m_d->m_pool[annot]);
}

bool AnnotationData::hasAnnotation(AnnotationGraphEdge e, const LimaString& annot) const
{
  return hasAnnotation(e, const_cast<AnnotationData*>(this)->m_d->m_pool[annot]);
}

bool AnnotationData::hasIntAnnotation(AnnotationGraphVertex v, uint64_t annot) const
{
  return (m_d->m_mapCVertexAGIannotPropertyMap[v].find(annot) != m_d->m_mapCVertexAGIannotPropertyMap[v].end());
}

bool AnnotationData::hasStringAnnotation(AnnotationGraphVertex v, uint64_t annot) const
{
  return (m_d->m_mapCVertexAGSannotPropertyMap[v].find(annot) != m_d->m_mapCVertexAGSannotPropertyMap[v].end());
}

bool AnnotationData::hasAnnotation(AnnotationGraphVertex v, uint64_t annot) const
{
  return (m_d->m_mapCVertexAGGannotPropertyMap[v].find(annot) != m_d->m_mapCVertexAGGannotPropertyMap[v].end());
}

bool AnnotationData::hasIntAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, uint64_t annot) const
{
  bool ok; AnnotationGraphEdge e;
  boost::tie(e, ok) = edge(v1,v2,m_d->m_graph);
  if (!ok) return false;
  return hasIntAnnotation(e, annot);
}

bool AnnotationData::hasStringAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, uint64_t annot) const
{
  bool ok; AnnotationGraphEdge e;
  boost::tie(e, ok) = edge(v1,v2,m_d->m_graph);
  if (!ok) return false;
  return hasStringAnnotation(e, annot);
}

bool AnnotationData::hasAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, uint64_t annot) const
{
  bool ok; AnnotationGraphEdge e;
  boost::tie(e, ok) = edge(v1,v2,m_d->m_graph);
  if (!ok) return false;
  return hasAnnotation(e, annot);
}

bool AnnotationData::hasIntAnnotation(AnnotationGraphEdge e, uint64_t annot) const
{
  return (m_d->m_mapCEdgeAGIannotPropertyMap[e].find(annot) != m_d->m_mapCEdgeAGIannotPropertyMap[e].end());
}

bool AnnotationData::hasStringAnnotation(AnnotationGraphEdge e, uint64_t annot) const
{
  return (m_d->m_mapCEdgeAGSannotPropertyMap[e].find(annot) != m_d->m_mapCEdgeAGSannotPropertyMap[e].end());
}

bool AnnotationData::hasAnnotation(AnnotationGraphEdge e, uint64_t annot) const
{
  return (m_d->m_mapCEdgeAGGannotPropertyMap[e].find(annot) != m_d->m_mapCEdgeAGGannotPropertyMap[e].end());
}






void AnnotationData::dumpFunction(const std::string& annot, const AnnotationData::Dumper* f)
{
  LimaString wannot = QString::fromStdString(annot);
  uint64_t annotId = annotationId(wannot);
  if (m_d->m_dumpFunctions.find(annotId) != m_d->m_dumpFunctions.end())
  {
    AGLOGINIT;
    LWARN << "Replacing dump function for " << annot << " annotation. Deleting old one";
    delete m_d->m_dumpFunctions[annotId];
  }
  m_d->m_dumpFunctions[annotId] = f;
}

const AnnotationData::Dumper* AnnotationData::dumpFunction(const std::string& annot)
{
  LimaString wannot = QString::fromStdString(annot);
  uint64_t annotId = annotationId(wannot);
  if (m_d->m_dumpFunctions.find(annotId) == m_d->m_dumpFunctions.end())
  {
    return 0;
  }
  else
  {
    return m_d->m_dumpFunctions[annotId];
  }
}

const AnnotationData::Dumper* AnnotationData::dumpFunction(uint64_t annot)
{
  return m_d->m_dumpFunctions[annot];
}

/** @brief Definition of a dummy function suitable to be used as a dumper for
  * the annotations of an annotation graph
  * @param @b os <I>std::ostream&amp;</I> the stream on which to dump
  * @param @b ga <I>GenericAnnotation&amp;</I> the annotation to be dumped.
  * @return <I>int</I> The dummy dumping is considered as successful, so
  *         SUCCESS_ID is returned.
  */
int DummyDumpFunction::dump(std::ostream& os, GenericAnnotation&) const
{
  os << "no_dump_function_defined";
  return SUCCESS_ID;
}

void AnnotationData::addMatching(const StringsPoolIndex& direction, AnnotationGraphVertex firstVx,
                                 AnnotationGraphVertex secondVx)
{
  m_d->m_matchings[direction].insert(std::make_pair(firstVx,secondVx));
}

/** @brief Access to the underlying raw matching structure */
const std::map<StringsPoolIndex, std::multimap<AnnotationGraphVertex, AnnotationGraphVertex> >& AnnotationData::matchings() const {return m_d->m_matchings;}


void AnnotationData::addMatching(const std::string& first,
                                 AnnotationGraphVertex firstVx,
                                 const std::string& second,
                                 AnnotationGraphVertex secondVx)
{
#ifdef DEBUG_LP
  AGLOGINIT;
  LDEBUG << "AnnotationData::addMatching " << first << firstVx
          << second << secondVx;
#endif

  addMatching( m_d->m_pool[ QString::fromStdString(first + second) ],
               firstVx, secondVx);
  addMatching( m_d->m_pool[ QString::fromStdString(second + first) ],
               secondVx, firstVx);
}

/** @brief Gets the set of vertices matched in the second graph by the given
 * vertex of the first graph
 */
std::set< AnnotationGraphVertex > AnnotationData::matches(const std::string& first, AnnotationGraphVertex firstVx,
      const std::string& second) const
  {
    return matches( m_d->m_pool[ QString::fromStdString(first + second) ], firstVx);
  }

/** @brief Tests if the two given vertices are matching in the two given graphs
 */
bool AnnotationData::isMatching(const std::string& first, AnnotationGraphVertex firstVx,
                                const std::string& second, AnnotationGraphVertex secondVx) const
{
  return isMatching( m_d->m_pool[ QString::fromStdString(first + second) ], firstVx, secondVx);
}

void AnnotationData::addMatching(const StringsPoolIndex& first, AnnotationGraphVertex firstVx,
                                 const StringsPoolIndex& second, AnnotationGraphVertex secondVx)
{
  addMatching( m_d->m_pool[ m_d->m_pool[first] + m_d->m_pool[second] ], firstVx, secondVx);
  addMatching( m_d->m_pool[ m_d->m_pool[second] + m_d->m_pool[first] ], secondVx, firstVx);
}

std::set< AnnotationGraphVertex > AnnotationData::matches(const StringsPoolIndex& first, AnnotationGraphVertex firstVx,
      const StringsPoolIndex& second) const
  {
    return matches( m_d->m_pool[ m_d->m_pool[first] + m_d->m_pool[second] ], firstVx);
  }

bool AnnotationData::isMatching(const StringsPoolIndex& first, AnnotationGraphVertex firstVx,
                                const StringsPoolIndex& second, AnnotationGraphVertex secondVx) const
{
  return isMatching( m_d->m_pool[ m_d->m_pool[first] + m_d->m_pool[second] ], firstVx, secondVx);
}

std::set< AnnotationGraphVertex > AnnotationData::matches(
    const StringsPoolIndex& direction,
    AnnotationGraphVertex firstVx) const
{
  if (m_d->m_matchings.find(direction) == m_d->m_matchings.end())
  {
    return std::set< AnnotationGraphVertex >();
  }
  else
  {
    std::set< AnnotationGraphVertex > result;
    auto range = m_d->m_matchings[direction].equal_range(firstVx);
    for (; range.first != range.second; range.first++)
    {
      result.insert( (*(range.first)).second);
    }
    return result;
  }
}

bool AnnotationData::isMatching(const StringsPoolIndex& direction,
                                AnnotationGraphVertex firstVx,
                                AnnotationGraphVertex secondVx) const
{
  if (m_d->m_matchings.find(direction) == m_d->m_matchings.end())
  {
    return false;
  }
  else
  {
    std::pair< std::multimap<AnnotationGraphVertex, AnnotationGraphVertex>::iterator, std::multimap<AnnotationGraphVertex, AnnotationGraphVertex>::iterator > range;
    range = const_cast< std::map< StringsPoolIndex, std::multimap<AnnotationGraphVertex, AnnotationGraphVertex> >& >(m_d->m_matchings)[direction].equal_range(firstVx);
    for (; range.first != range.second; range.first++)
    {
      if ( (*(range.first)).second == secondVx )
      {
        return true;
      }
    }
    return false;
  }

}

void AnnotationData::cloneAnnotations(AnnotationGraphVertex src,
                                      AnnotationGraphVertex tgt,
                                      std::set< std::string >& excepted)
{
  std::set< uint64_t > iexcepted;
  std::set< std::string >::const_iterator eit, eit_end;
  eit=excepted.begin(); eit_end = excepted.end();
  for (; eit!=eit_end; eit++)
  {
    iexcepted.insert(m_d->m_pool[QString::fromStdString(*eit)]);
  }

  AGIannotProp& srcimap = m_d->m_mapVertexAGIannotPropertyMap[src];
  AGIannotProp& tgtimap = m_d->m_mapVertexAGIannotPropertyMap[tgt];
  for (AGIannotProp::const_iterator iamit=srcimap.begin();
       iamit != srcimap.end(); iamit++)
  {
    if (iexcepted.find((*iamit).first) == iexcepted.end())
    {
      tgtimap.insert(*iamit);
    }
  }

  std::map< uint64_t, LimaString >& srcsmap = m_d->m_mapVertexAGSannotPropertyMap[src];
  std::map< uint64_t, LimaString >& tgtsmap = m_d->m_mapVertexAGSannotPropertyMap[tgt];
  for (std::map< uint64_t, LimaString >::const_iterator samit=srcsmap.begin();
       samit != srcsmap.end(); samit++)
  {
    if (iexcepted.find((*samit).first) == iexcepted.end())
    {
      tgtsmap.insert(*samit);
    }
  }

  std::map< uint64_t, GenericAnnotation >& srcmap = m_d->m_mapVertexAGGannotPropertyMap[src];
  std::map< uint64_t, GenericAnnotation >& tgtmap = m_d->m_mapVertexAGGannotPropertyMap[tgt];
  for (std::map< uint64_t, GenericAnnotation >::const_iterator amit=srcmap.begin();
       amit != srcmap.end(); amit++)
  {
    if (iexcepted.find((*amit).first) == iexcepted.end())
    {
      tgtmap.insert(*amit);
    }
  }

}

} // closing namespace AnnotationGraphs
} // closing namespace Common
} // closing namespace Lima
