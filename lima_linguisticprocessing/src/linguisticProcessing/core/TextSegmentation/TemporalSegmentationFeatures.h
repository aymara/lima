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
 * @file       TemporalSegmentationFeatures.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Thu Feb 10 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * Project     LIMA
 * 
 * @brief      Segment features for temporal segmentation
 * 
 * 
 ***********************************************************************/

#ifndef TEMPORALSEGMENTATIONFEATURES_H
#define TEMPORALSEGMENTATIONFEATURES_H

#include "TextSegmentationExport.h"
#include "SegmentFeatureExtractor.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/Automaton/recognizerData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"
#include "common/MediaticData/EntityType.h"

namespace Lima {
namespace LinguisticProcessing {

//----------------------------------------------------------------------
// segment features for dumper used for temporal segmentation

//----------------------------------------------------------------------
// check if an entity of a given type exists in the segment
#define SegmentFeatureEntity_ID "entity"
class LIMA_TEXTSEGMENTATION_EXPORT SegmentFeatureEntity : public AbstractSegmentFeatureExtractor {
public:
  SegmentFeatureEntity(MediaId language, const std::string& complement="");
  ~SegmentFeatureEntity() {}

  void update(const AnalysisContent& analysis);

  std::string
  getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
           const Segment& seg) const;
private:
  const std::string m_entityName;
  Common::MediaticData::EntityType m_entityType;
  const Common::AnnotationGraphs::AnnotationData* m_annotationData;
  
  bool isVertexInPosGraphAnEntityOfType(LinguisticGraphVertex v, 
                      const Common::AnnotationGraphs::AnnotationData* annotationData,
                      Common::MediaticData::EntityType type) const;

  bool isAnEntityOfType(LinguisticGraphVertex v, 
                        const Common::AnnotationGraphs::AnnotationData* annotationData,
                        Common::MediaticData::EntityType type,
                        const std::string& graphId) const;
                                       
};

//----------------------------------------------------------------------
#define SegmentFeatureEntityInData_ID "entityInData"
class LIMA_TEXTSEGMENTATION_EXPORT SegmentFeatureEntityInData : public AbstractSegmentFeatureExtractor {
public:
  SegmentFeatureEntityInData(MediaId language, const std::string& complement="");
  ~SegmentFeatureEntityInData() {}

  void update(const AnalysisContent& analysis);

  std::string
  getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
           const Segment& seg) const;
private:
  std::string m_entityName;
  std::string m_dataName;
  const ApplyRecognizer::RecognizerResultData* m_data;
  Common::MediaticData::EntityType m_entityType;
};

//----------------------------------------------------------------------
// give the rank of the segment
#define SegmentFeatureRank_ID "rank"
class LIMA_TEXTSEGMENTATION_EXPORT SegmentFeatureRank : public AbstractSegmentFeatureExtractor {
public:
  SegmentFeatureRank(MediaId language, const std::string& complement="");
  ~SegmentFeatureRank() {}

  void update(const AnalysisContent& analysis);

  std::string
  getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
             const Segment& seg) const;
private:
  std::string m_data;
  const SegmentationData* m_segmData;
};

//----------------------------------------------------------------------
// give the number of verbs with a given tense
#define SegmentFeatureCheckProperty_ID "checkProperty"
class LIMA_TEXTSEGMENTATION_EXPORT SegmentFeatureCheckProperty : public AbstractSegmentFeatureExtractor {
public:
  SegmentFeatureCheckProperty(MediaId language, const std::string& complement="");
  ~SegmentFeatureCheckProperty() {}

  void update(const AnalysisContent& analysis);

  std::string
  getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
             const Segment& seg) const;
private:
  std::string m_propertyName;
  std::string m_propertyValue;
  LinguisticCode m_value;
  const Common::PropertyCode::PropertyAccessor* m_propertyAccessor;

  bool isProperty(LinguisticGraphVertex v,
                  const LinguisticGraph* graph,
                  LinguisticCode propertyValue) const;
};

//----------------------------------------------------------------------
// give the number of verbs with a given tense
#define SegmentFeatureInSegment_ID "segmentType"
class LIMA_TEXTSEGMENTATION_EXPORT SegmentFeatureInSegment : public AbstractSegmentFeatureExtractor {
  public:
    SegmentFeatureInSegment(MediaId language, const std::string& complement="");
    ~SegmentFeatureInSegment() {}
    
    void update(const AnalysisContent& analysis);
    
    std::string
    getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
             const Segment& seg) const;
  private:
    std::string m_segmentData;
    const SegmentationData* m_data;
};

} // end namespace
} // end namespace

#endif
