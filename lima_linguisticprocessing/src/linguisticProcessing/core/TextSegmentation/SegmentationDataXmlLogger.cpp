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
 * @file       SegmentationDataXmlLogger.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Jan 21 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * 
 ***********************************************************************/

#include "SegmentationDataXmlLogger.h"
#include "SegmentationData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/time/traceUtils.h"
#include <fstream>

using namespace std;

namespace Lima {
namespace LinguisticProcessing {

SimpleFactory<MediaProcessUnit,SegmentationDataXmlLogger> SegmentationDataXmlLoggerFactory(SEGMENTATIONDATAXMLLOGGER_CLASSID);

//***********************************************************************
// constructors and destructors
SegmentationDataXmlLogger::SegmentationDataXmlLogger():
AbstractLinguisticLogger(".seg.xml"),
m_data("segmentationData")
{
}

SegmentationDataXmlLogger::~SegmentationDataXmlLogger() {
}

//***********************************************************************
void SegmentationDataXmlLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  LOGINIT("LP::Segmentation");
  LDEBUG << "SegmentationDataXmlLogger::init" << LENDL;

  AbstractLinguisticLogger::init(unitConfiguration,manager);

  try {
    m_data=unitConfiguration.getParamsValueAtKey("data");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // do nothing, keep default
}

LimaStatusCode SegmentationDataXmlLogger::process(
  AnalysisContent& analysis) const
{
  LOGINIT("LP::Segmentation");
  LDEBUG << "SegmentationDataXmlLogger::process" << LENDL;
  TimeUtils::updateCurrentTime();

  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      LOGINIT("LP::Segmentation");
      LERROR << "no LinguisticMetaData ! abort" << LENDL;
      return MISSING_DATA;
  }

  // open output file
  ofstream out;
  if (!openLogFile(out,metadata->getMetaData("FileName"))) {
    LOGINIT("LP::Segmentation");
    LERROR << "Can't open log file '" << metadata->getMetaData("FileName") << "'" << LENDL;
    return UNKNOWN_ERROR;
  }

  // get metadata (useful for XML documents)
//   uint64_t offset(0);
//   uint64_t offsetIndexingNode(0);
  std::string docId("");
  try {
//     offset=atoi(metadata->getMetaData("StartOffset").c_str());
//     offsetIndexingNode=atoi(metadata->getMetaData("StartOffsetIndexingNode").c_str());
    docId=metadata->getMetaData("DocId");
  }
  catch (LinguisticProcessingException& ) {
    // do nothing: not set in analyzeText (only in analyzeXmlDocuments)
  }

  // log 
  out << "<segmentation>" << endl;
  const AnalysisData* data =analysis.getData(m_data);
  if (data!=0) {
    const SegmentationData* segData=static_cast<const SegmentationData*>(data);
    const vector<Segment> seg=segData->getSegments();
    for (vector<Segment>::const_iterator it=seg.begin(), it_end=seg.end(); it!=it_end; it++) {
      out 
        << "<segment>" 
        << "<pos>" << (*it).getPosBegin() << "</pos>" 
        << "<len>" << (*it).getLength() << "</len>"
        << "<type>" << (*it).getType() << "</type>"
        << "</segment>" 
        << endl;
    }
  }
  else {
    LOGINIT("LP::Segmentation");
    LDEBUG << "no SegmentationData of name " << m_data << LENDL;
  }
  
  out << "</segmentation>" << endl;
  out.close();

  TimeUtils::logElapsedTime("SegmentationDataXmlLogger");
  return SUCCESS_ID;
}


} // end namespace
} // end namespace
