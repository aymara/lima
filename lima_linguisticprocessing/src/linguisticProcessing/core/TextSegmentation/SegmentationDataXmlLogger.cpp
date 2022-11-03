// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  LDEBUG << "SegmentationDataXmlLogger::init";

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
  LDEBUG << "SegmentationDataXmlLogger::process";
  TimeUtils::updateCurrentTime();

  auto metadata = std::dynamic_pointer_cast<LinguisticMetaData>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      LOGINIT("LP::Segmentation");
      LERROR << "no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }

  // open output file
  ofstream out;
  if (!openLogFile(out,metadata->getMetaData("FileName"))) {
    LOGINIT("LP::Segmentation");
    LERROR << "Can't open log file '" << metadata->getMetaData("FileName") << "'";
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
  auto data = analysis.getData(m_data);
  if (data!=0) {
    const SegmentationData* segData=static_cast<const SegmentationData*>(data.get());
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
    LDEBUG << "no SegmentationData of name " << m_data;
  }
  
  out << "</segmentation>" << endl;
  out.close();

  TimeUtils::logElapsedTime("SegmentationDataXmlLogger");
  return SUCCESS_ID;
}


} // end namespace
} // end namespace
