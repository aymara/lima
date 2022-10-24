// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       SegmentFeatureExtractor.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Mon Feb  7 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * 
 ***********************************************************************/

#include "SegmentFeatureExtractor.h"
#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "common/Data/strwstrtools.h"
#include <sstream>

using namespace std;
using namespace boost;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;


namespace Lima {
namespace LinguisticProcessing {

  
AbstractSegmentFeatureExtractor::
AbstractSegmentFeatureExtractor(MediaId language, const std::string& /*complement*/)
{
  m_language=language;
}
  
//***********************************************************************
// factories for feature extractors
AbstractSegmentFeatureExtractorFactory::
AbstractSegmentFeatureExtractorFactory(const std::string& factoryId):
RegistrableFactory<AbstractSegmentFeatureExtractorFactory>(factoryId)
{
}

SegmentFeatureExtractorFactory<SegmentFeaturePosition> SegmentFeaturePositionFactory(SegmentFeaturePosition_ID);
SegmentFeatureExtractorFactory<SegmentFeatureLength> SegmentFeatureLengthFactory(SegmentFeatureLength_ID);
SegmentFeatureExtractorFactory<SegmentFeatureType> SegmentFeatureTypeFactory(SegmentFeatureType_ID);

//***********************************************************************
// Feature list
//***********************************************************************
SegmentFeatures::SegmentFeatures() : vector<AbstractSegmentFeatureExtractor*>(),
m_language(0)
{
}

SegmentFeatures::SegmentFeatures(MediaId language) : vector<AbstractSegmentFeatureExtractor*>(),
m_language(language)
{
}

SegmentFeatures::~SegmentFeatures()
{
  for (SegmentFeatures::iterator it=begin(),it_end=end(); it!=it_end; it++) {
    if (*it) {
      delete (*it);
      *it=0;
    }
  }
}

void SegmentFeatures::initialize(const deque<std::string>& featureNames)
{
  for (deque<std::string>::const_iterator it=featureNames.begin(),
         it_end=featureNames.end();it!=it_end;it++) 
  {
    string featureName=(*it);
    string complement;
    string::size_type i=featureName.find(":");
    if (i!=string::npos) {
      complement=string(featureName,i+1);
      featureName=string(featureName,0,i);
    }
    push_back(SegmentFeaturePositionFactory.getFactory(featureName)->create(m_language,complement));
  }
}

void SegmentFeatures::update(const AnalysisContent& analysis)
{
  for (iterator it=begin(),it_end=end(); it!=it_end; it++) {
    (*it)->update(analysis);
  }
}

//***********************************************************************
// Feature accessors
//***********************************************************************
SegmentFeaturePosition::SegmentFeaturePosition(MediaId language, const std::string& complement):
AbstractSegmentFeatureExtractor(language,complement),
m_offset(0)
{}

void SegmentFeaturePosition::
update(const AnalysisContent& analysis) {
  // update offset from metadata
  const LinguisticMetaData* metadata=static_cast<const LinguisticMetaData*>(analysis.getData("LinguisticMetaData").get());
  if (metadata == 0) {
    LOGINIT("LP::Segmentation");
    LWARN << "no LinguisticMetaData ! abort";
  }
  else {
    try {
      m_offset=atoi(metadata->getMetaData("StartOffset").c_str());
    }
    catch (LinguisticProcessingException& ) {
      // do nothing: not set in analyzeText (only in analyzeXmlDocuments)
    }
  }
}

std::string SegmentFeaturePosition::
getValue(const AnalysisGraph* /*graph*/,
         const Segment& seg) const
{
  ostringstream oss;
  oss << m_offset+seg.getPosBegin();
  return oss.str();
}

//***********************************************************************
SegmentFeatureLength::SegmentFeatureLength(MediaId language, const std::string& complement):
AbstractSegmentFeatureExtractor(language,complement)
{
}

void SegmentFeatureLength::
update(const AnalysisContent& /*analysis*/) {
}

std::string SegmentFeatureLength::
getValue(const AnalysisGraph* /*graph*/,
         const Segment& seg) const
{
  ostringstream oss;
  oss << seg.getLength();
  return oss.str();
}

//***********************************************************************
SegmentFeatureType::SegmentFeatureType(MediaId language, const std::string& complement):
AbstractSegmentFeatureExtractor(language,complement)
{
}

void SegmentFeatureType::
update(const AnalysisContent& /*analysis*/) {
}

std::string SegmentFeatureType::
getValue(const AnalysisGraph* /*graph*/,
         const Segment& seg) const
{
  return seg.getType();
}

} // end namespace
} // end namespace
