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
 * @file       SegmentationData.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Tue Jan 18 2011
 * copyright   Copyright (C) 2011 by CEA LIST (LVIC)
 * Project     MM
 *
 * @brief      this class contains information about text segmentation
 *
 *
 ***********************************************************************/

#ifndef SEGMENTATIONDATA_H
#define SEGMENTATIONDATA_H

#include "TextSegmentationExport.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"

namespace Lima {
namespace LinguisticProcessing {

class LIMA_TEXTSEGMENTATION_EXPORT Segment
{
public:
  /* Segment(); */
  explicit Segment(const std::string& type);

  /* smart constructors: give them part of the information, and they complete it using the graph */
  Segment(const std::string& type, 
          LinguisticGraphVertex begin, 
          LinguisticGraphVertex end, 
          LinguisticAnalysisStructure::AnalysisGraph* graph);
  /* Segment(const std::string& type,  */
  /*         uint64_t posBegin,  */
  /*         uint64_t length,  */
  /*         LinguisticAnalysisStructure::AnalysisGraph* graph); */
  ~Segment();
  bool operator<(const Segment& s) const;

  void addSegment(const Segment& s);

  void setVerticesFromPositions(uint64_t posBegin,
                                uint64_t length,
                                LinguisticAnalysisStructure::AnalysisGraph* anagraph);

  /* no setters : use constructors */
  /* void setFirstVertex(LinguisticGraphVertex v) { m_begin=v; } */
  /* void setLastVertex(LinguisticGraphVertex v) { m_end=v; } */
  /* void setPosBegin(uint64_t p) { m_posBegin=p; } */
  /* void setLength(uint64_t l) { m_length=l; } */
  void setType(const std::string& type) { m_type=type; }

  LinguisticGraphVertex getFirstVertex() const { return m_begin; }
  LinguisticGraphVertex getLastVertex() const { return m_end; }
  uint64_t getPosBegin() const { return m_posBegin; }
  uint64_t getPosEnd() const { return m_posBegin+m_length; }
  uint64_t getLength() const { return m_length; }
  const std::string& getType() const { return m_type; }
  
private:
  LinguisticGraphVertex m_begin; /*< the vertex before the first vertex of the segment */
  LinguisticGraphVertex m_end;   /*< the vertex after the last vertex of the segment */ 
  uint64_t m_posBegin;
  uint64_t m_length;
  std::string m_type;
};

// class to store segmentation data: a list of segments, defined by their position and length and a type
class LIMA_TEXTSEGMENTATION_EXPORT SegmentationData : public AnalysisData
// ??OME2 class SegmentationData : public AnalysisData, public std::vector<Segment>
{
 public:
  explicit SegmentationData(const std::string& sourceGraph="");
  ~SegmentationData();

  void add(const Segment& s);
// ??OME2 const std::vector<Segment>& getSegments() const { return *this; }
  const std::vector<Segment>& getSegments() const { return m_segments; }
  std::vector<Segment>& getSegments() { return m_segments; }
  
  inline const std::string& getGraphId() const { return m_graphId; }
  std::vector<Segment> m_segments;
 private:
  std::string m_graphId;
};


} // end namespace
} // end namespace

#endif
