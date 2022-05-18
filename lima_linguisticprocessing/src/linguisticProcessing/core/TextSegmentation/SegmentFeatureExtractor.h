// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       SegmentFeatureExtractor.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Mon Feb  7 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * Project     LIMA
 * 
 * @brief      accessors for various features of a word
 * 
 * 
 ***********************************************************************/

#ifndef SEGMENTFEATUREEXTRACTOR_H
#define SEGMENTFEATUREEXTRACTOR_H

#include "TextSegmentationExport.h"
#include "SegmentationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "common/AbstractFactoryPattern/RegistrableFactory.h"

namespace Lima {
namespace LinguisticProcessing {

//----------------------------------------------------------------------
// Abstract segment feature extractor class
class AbstractSegmentFeatureExtractor
{
public:
  AbstractSegmentFeatureExtractor(MediaId language, const std::string& complement="");
  virtual ~AbstractSegmentFeatureExtractor() {}

  virtual void update(const AnalysisContent& /*analysis*/) {}

  virtual std::string getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
                       const Segment& seg) const=0;
 protected:
  MediaId m_language;
};

//----------------------------------------------------------------------
// Asbtract factory for abstract feature extractor
class AbstractSegmentFeatureExtractorFactory : 
  public RegistrableFactory<AbstractSegmentFeatureExtractorFactory>
{
public:
  AbstractSegmentFeatureExtractorFactory(const std::string& factoryId); 
  virtual AbstractSegmentFeatureExtractor* create(MediaId language,const std::string& complement)=0;
private:
};

//----------------------------------------------------------------------
// template for factories for actual instances of feature extractors
template<typename SegmentFeatureExtractor> 
class LIMA_TEXTSEGMENTATION_EXPORT SegmentFeatureExtractorFactory : 
  public AbstractSegmentFeatureExtractorFactory
{
public:
  SegmentFeatureExtractorFactory(const std::string& factoryId):
  AbstractSegmentFeatureExtractorFactory(factoryId) {}
  AbstractSegmentFeatureExtractor* create(MediaId language, const std::string& complement) override {
    AbstractSegmentFeatureExtractor* newFeature=new SegmentFeatureExtractor(language, complement);
    return newFeature;
  }
private:
};

//----------------------------------------------------------------------
// a class for list of features
//----------------------------------------------------------------------
class SegmentFeatures : public std::vector<AbstractSegmentFeatureExtractor*> {
public:
    SegmentFeatures();
    SegmentFeatures(MediaId language);
    ~SegmentFeatures();
  
  void setLanguage(MediaId language) { m_language=language; }
  void initialize(const std::deque<std::string>& featureNames);
  void update(const AnalysisContent& analysis);

private:
  MediaId m_language;
};

//----------------------------------------------------------------------
// instances of feature extractors
//----------------------------------------------------------------------
// check if an entity of a given type exists in the segment
#define SegmentFeaturePosition_ID "position"
class LIMA_TEXTSEGMENTATION_EXPORT SegmentFeaturePosition : public AbstractSegmentFeatureExtractor {
public:
  SegmentFeaturePosition(MediaId language, const std::string& complement="");
  ~SegmentFeaturePosition() {}

  void update(const AnalysisContent& analysis) override;

  std::string
  getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
             const Segment& seg) const override;
private:
  uint64_t m_offset;
};

#define SegmentFeatureLength_ID "length"
class LIMA_TEXTSEGMENTATION_EXPORT SegmentFeatureLength : public AbstractSegmentFeatureExtractor {
public:
  SegmentFeatureLength(MediaId language, const std::string& complement="");
  ~SegmentFeatureLength() {}

  void update(const AnalysisContent& analysis) override;

  std::string
  getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
             const Segment& seg) const override;
private:
};

#define SegmentFeatureType_ID "type"
class LIMA_TEXTSEGMENTATION_EXPORT SegmentFeatureType : public AbstractSegmentFeatureExtractor {
public:
  SegmentFeatureType(MediaId language, const std::string& complement="");
  ~SegmentFeatureType() {}

  void update(const AnalysisContent& analysis) override;

  std::string
  getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
             const Segment& seg) const override;
private:
};


} // end namespace
} // end namespace

#endif
