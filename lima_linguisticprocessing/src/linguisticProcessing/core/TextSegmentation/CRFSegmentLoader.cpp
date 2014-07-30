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
 * @file       CRFSegmentLoader.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Tue Jan 18 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * 
 ***********************************************************************/

#include "CRFSegmentLoader.h"

#include "common/AbstractFactoryPattern/SimpleFactory.h"

using namespace std;

using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
        
  SimpleFactory<MediaProcessUnit,CRFSegmentLoader> CRFSegmentLoaderFactory(CRFSEGMENTLOADER_CLASSID);

//***********************************************************************
// constructors and destructors
CRFSegmentLoader::CRFSegmentLoader():
AnalysisLoader(),
m_dataName(""),
m_fromDataName("")
{
}

CRFSegmentLoader::~CRFSegmentLoader() {
}

//***********************************************************************
void CRFSegmentLoader::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
          Manager* manager)
  
{
  AnalysisLoader::init(unitConfiguration,manager);

  try {
    m_dataName=unitConfiguration.getParamsValueAtKey("data");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // keep default value

  try {
    m_fromDataName=unitConfiguration.getParamsValueAtKey("fromData");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // keep default value

}

LimaStatusCode CRFSegmentLoader::process(AnalysisContent& analysis) const
{

  // get segmentation data on which the CRF annotation relied
  // fromData and CRF results must be aligned
  AnalysisData* data=analysis.getData(m_fromDataName);
  SegmentationData* fromData=0;
  if (data==0) {
    LOGINIT("LP::AnalysisLoader");
    LERROR << "no data '" << m_fromDataName << "'";
    return MISSING_DATA;
  }
  else {
    fromData = static_cast<SegmentationData*>(data);
    if (fromData==0) {
      LOGINIT("LP::AnalysisLoader");
      LERROR << "data "<< m_fromDataName <<" is not an object of class SegmentationData";
      return MISSING_DATA;
    }
  }  

  // get segmentation data or create new
  data=analysis.getData(m_dataName);
  SegmentationData* segmData=0;
  if (data==0) {
    segmData=new SegmentationData(fromData->getGraphId());
    analysis.setData(m_dataName,segmData);
  }
  else {
    segmData = static_cast<SegmentationData*>(data);
    if (segmData==0) {
      LOGINIT("LP::AnalysisLoader");
      LERROR << "data "<< m_dataName <<" is not an object of class SegmentationData";
      return MISSING_DATA;
    }
  }
  
  ifstream file(getInputFile(analysis).c_str(), std::ifstream::binary);
  if (! file.good()) {
    LOGINIT("LP::AnalysisLoader");
    LERROR << "Error: failed to open input file '" << getInputFile(analysis) << "'";
    return MISSING_DATA;
  }
  
  // segmentation in file must be aligned with fromData
  //SegmentationData::const_iterator seg=fromData->begin();
  std::vector<Segment>::const_iterator seg=(fromData->getSegments()).begin();
  string line;
  string currentType;
  Segment *currentSegment=0;
  while (file.good()) {
    getline(file,line);
    if (line.empty()) {
      break;
    }
    // ??OME2 if (seg==fromData->end()) {
    if (seg==(fromData->getSegments()).end()) {
      // data are not aligned: should not occur
      LOGINIT("LP::AnalysisLoader");
      LERROR << "CRFLoader: CRF output is not aligned with data " << m_fromDataName;
      break;
    }
    // CRF tag is the last element in line (space or tab separated)
    string::size_type k=line.find_last_of("\t");
    if (k!=string::npos) {
      string type(line,k+1);
      if (currentSegment==0) {
        // first segment
        currentSegment=new Segment(*seg);
        currentSegment->setType(type);
      }
      else if (type == currentSegment->getType()) {
        // contiguous segment with same type
        currentSegment->addSegment(*seg);
      }
      else {
        // add current segment, create new one
        segmData->add(*currentSegment);
        delete currentSegment;
        currentSegment=new Segment(*seg);
        currentSegment->setType(type);
      }
    }
    else {
      // ignore line
      continue;
    }
    seg++;
  }
  // add last segment
  if (currentSegment!=0) {
    segmData->add(*currentSegment);
  }
  return SUCCESS_ID;
}

} // end namespace
} // end namespace
