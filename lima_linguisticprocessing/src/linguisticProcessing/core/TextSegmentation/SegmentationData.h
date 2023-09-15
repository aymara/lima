// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Tue Jan 18 2011
 * @brief      this class contains information about text segmentation
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
  ~Segment() { }
  bool operator<(const Segment& s) const;

  void addSegment(const Segment& s);

  void setVerticesFromPositions(uint64_t posBegin,
                                uint64_t length,
                                LinguisticAnalysisStructure::AnalysisGraph* anagraph);

  /* no setters : use constructors */
  void setFirstVertex(LinguisticGraphVertex v) { m_begin=v; }
  void setLastVertex(LinguisticGraphVertex v) { m_end=v; }
  /* void setPosBegin(uint64_t p) { m_posBegin=p; } */
  void setLength(uint64_t l) { m_length=l; }
  void setType(const std::string& type) { m_type=type; }

  LinguisticGraphVertex getFirstVertex() const { return m_begin; }
  LinguisticGraphVertex getLastVertex() const { return m_end; }
  uint64_t getPosBegin() const { return m_posBegin; }
  uint64_t getPosEnd() const { return m_posBegin+m_length; }
  uint64_t getLength() const { return m_length; }
  const std::string& getType() const { return m_type; }

  LIMA_TEXTSEGMENTATION_EXPORT  friend std::ostream& operator<<(std::ostream& os, const Segment& seg);
  LIMA_TEXTSEGMENTATION_EXPORT  friend QDebug& operator<<(QDebug& os, const Segment& seg);

private:
  LinguisticGraphVertex m_begin; /*< the vertex before the first vertex of the segment */
  LinguisticGraphVertex m_end;   /*< the vertex after the last vertex of the segment */
  uint64_t m_posBegin;
  uint64_t m_length;
  std::string m_type;
};

// class to store segmentation data: a list of segments, defined by their position and length and a type
class LIMA_TEXTSEGMENTATION_EXPORT SegmentationData : public AnalysisData
{
 public:
  explicit SegmentationData(const std::string& sourceGraph="");
  ~SegmentationData();

  void add(const Segment& s);
  const std::vector<Segment>& getSegments() const { return m_segments; }
  std::vector<Segment>& getSegments() { return m_segments; }

  inline const std::string& getGraphId() const { return m_graphId; }

 private:
  std::vector<Segment> m_segments;
  std::string m_graphId;
};


} // end namespace
} // end namespace

#endif
