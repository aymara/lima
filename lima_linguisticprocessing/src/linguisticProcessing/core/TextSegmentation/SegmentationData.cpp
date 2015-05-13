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
 * @file       SegmentationData.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Tue Jan 18 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * 
 ***********************************************************************/

#include "SegmentationData.h"
#include <queue>
#include <set>
#include <algorithm>

using namespace std;
//using namespace boost;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {

//***********************************************************************
// Segment class
Segment::Segment(const std::string& type):
m_begin(0),
m_end(0),
m_posBegin(0),
m_length(0),
m_type(type)
{
}

Segment::Segment(const std::string& type, 
                 LinguisticGraphVertex begin, 
                 LinguisticGraphVertex end, 
                 LinguisticAnalysisStructure::AnalysisGraph* anagraph):
m_begin(begin),
m_end(end),
m_posBegin(0),
m_length(0),
m_type(type)
{
  // find position and length in graph
  LinguisticGraph* graph=anagraph->getGraph();

  // begin vertex is the vertex before first element of the segment :
  // use following vertices to find position of first element
  uint64_t position(0);
  bool foundPos(false);
  LinguisticGraphOutEdgeIt it, it_end;
  boost::tie(it, it_end)=boost::out_edges(begin, *graph);
  for (; it!=it_end; it++) {
    LinguisticGraphVertex v=target(*it,*graph);
    if (v==end) { // vertex following begin vertex is end vertex => empty segment
      LOGINIT("LP::Segmentation");
      LDEBUG << "Warning: empty segment";
      // keep default 0 values for (pos,len) to be informed that this is an empty segment
      return;
    }
    Token* t = get(vertex_token,*graph,v);
    if (t!=0) {
      if (foundPos && position!=t->position()) {
        LOGINIT("LP::Segmentation");
        LWARN << "Warning: conflicting position for alternative vertices";
      }
      else {
        position=t->position();
        foundPos=true;
      }
    }
    else {
      LOGINIT("LP::Segmentation");
      LDEBUG << "Warning: no token for vertex " << v;
    }
  }
  if (foundPos) {
    m_posBegin=position;
  }
  else {
    LOGINIT("LP::Segmentation");
    LERROR << "Error: cannot find position of begin vertex " << begin << " for segmentation data";
  }

  // last vertex is the vertex just after the last element of the
  // segment : use previous vertices (cannot use this one because it
  // can have no token, for instance, can be the last vertex of the
  // graph)
  uint64_t positionEnd(0);
  bool foundPosEnd(false);
  LinguisticGraphInEdgeIt pit, pit_end;
  boost::tie(pit, pit_end)=boost::in_edges(end, *graph);
  for (; pit!=pit_end; pit++) {
    LinguisticGraphVertex v=source(*pit,*graph);
    Token* t = get(vertex_token,*graph,v);
    if (t!=0) {
      if (foundPosEnd && positionEnd!=t->position()+t->length()) {
        LOGINIT("LP::Segmentation");
        LWARN << "Warning: conflicting position for alternative vertices";
      }
      else {
        positionEnd=t->position()+t->length();
        foundPosEnd=true;
      }
    }
    else {
      LOGINIT("LP::Segmentation");
      LWARN << "Warning: no token for vertex " << v;
    }
  }
  if (foundPosEnd) {
    m_length=positionEnd-m_posBegin+1;
  }
  else {
    LOGINIT("LP::Segmentation");
    LERROR << "Error: cannot determine length of segment for segmentation data ("
           << begin << "," << end << ")";
  }
}

// Segment::Segment(const std::string& type, 
//                  uint64_t posBegin, 
//                  uint64_t length, 
//                  LinguisticAnalysisStructure::AnalysisGraph* anagraph):
// m_begin(0),
// m_end(0),
// m_posBegin(posBegin),
// m_length(length),
// m_type(type)

void Segment::setVerticesFromPositions(uint64_t posBegin, 
               uint64_t length, 
               LinguisticAnalysisStructure::AnalysisGraph* anagraph)
{
  m_posBegin=posBegin;
  m_length=length;

  // find first and last vertex in graph : have to go through the graph
  LinguisticGraph* graph=anagraph->getGraph();

  uint64_t posEnd=posBegin+length;

  std::queue<std::pair<LinguisticGraphVertex,LinguisticGraphVertex> > toVisit;
  std::set<LinguisticGraphVertex> visited;
  
  LinguisticGraphOutEdgeIt outItr,outItrEnd;
 
  // output vertices between begin and end,
  // but do not include begin (beginning of text or previous end of sentence) and include end (end of sentence)
  toVisit.push(make_pair(anagraph->firstVertex(),0));

  bool first=true;
  while (!toVisit.empty()) {
    pair<LinguisticGraphVertex,LinguisticGraphVertex> v=toVisit.front();
    toVisit.pop();
    if (v.first == anagraph->lastVertex()) {
      break;
    }

    bool endIsNextVertex(false);
    if (first) {
      first=false;
    }
    else {
      Token* t=get(vertex_token,*graph,v.first);
      if(t!=0) {
        if (t->position() == posBegin) {
          m_begin=v.second;
        }
        if (t->position()+t->length()==posEnd) {
          // must take next vertex
          endIsNextVertex=true;
        }
      }
    }

    // add next vertices
    for (boost::tie(outItr,outItrEnd)=out_edges(v.first,*graph); outItr!=outItrEnd; outItr++) 
    {
      LinguisticGraphVertex next=target(*outItr,*graph);
      if (endIsNextVertex) {
        m_end=next;
        break; // no need to go further in the graph
      }
      if (visited.find(next)==visited.end())
      {
        visited.insert(next);
        toVisit.push(make_pair(next,v.first));
      }
    }
  }
}

Segment::~Segment()
{
}

bool Segment::operator<(const Segment& s) const
{
  return (m_posBegin<s.getPosBegin());
}

void Segment::addSegment(const Segment& s)
{
  LOGINIT("LP::Segmentation");
  LDEBUG << "add segment [" << s.getPosBegin() << "," << s.getLength() << "] to [" << getPosBegin() << "," << getLength() << "]";
  
  // do not check types, keep type of current segment
  // do not check adjacency, juste update end of segment
  m_end=s.getLastVertex();
  m_length=s.getPosEnd()-m_posBegin;
}
  
//***********************************************************************
// constructors and destructors
SegmentationData::SegmentationData(const std::string& graphId):
m_graphId(graphId)
{
}

SegmentationData::~SegmentationData() {
}

//***********************************************************************
void SegmentationData::add(const Segment& s)
{
  // segments are sorted in the vector: use binary search to insert new segment
  if (s.getLength()==0) {
    LOGINIT("LP::Segmentation");
    LDEBUG << "add(): trying to add empty segment: ignored";
  }
  else {
    
    // ??OME2 SegmentationData::iterator it=lower_bound( begin(),end(),s);
    std::vector<Lima::LinguisticProcessing::Segment>::iterator it=lower_bound( m_segments.begin(),m_segments.end(),s);
    // ??OME2 insert(it,s);
    m_segments.insert(it,s);
  }
}

} // end namespace
} // end namespace
