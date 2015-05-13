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
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "SegmentFeaturesDumper.h"

#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"

#include <boost/algorithm/string/replace.hpp>

#include <fstream>
#include <queue>

using namespace std;
//using namespace boost;
using namespace boost::tuples;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

SimpleFactory<MediaProcessUnit,SegmentFeaturesDumper> SegmentFeaturesDumperFactory(SEGMENTFEATURESDUMPER_CLASSID);

SegmentFeaturesDumper::SegmentFeaturesDumper():
AbstractTextualAnalysisDumper(),
m_graph("PosGraph"),
m_data(""),
m_sep(" "),
m_sepReplace("_"),
m_features(0)
{}


SegmentFeaturesDumper::~SegmentFeaturesDumper()
{}

void SegmentFeaturesDumper::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                      Manager* manager)

{
  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);

  m_language=manager->getInitializationParameters().media;
  try
  {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (NoSuchParam& ) {} // keep default value
  
  try {
    m_data=unitConfiguration.getParamsValueAtKey("data");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // do nothing, keep default

  try { 
    m_sep=unitConfiguration.getParamsValueAtKey("sep"); 
  }
  catch (NoSuchParam& ) {} // keep default value
  
  try { 
    m_sepReplace=unitConfiguration.getParamsValueAtKey("sep_replace"); 
  }
  catch (NoSuchParam& ) {} // keep default value

  try { 
    std::deque<string> featureList=unitConfiguration.getListsValueAtKey("features"); 
    // initialize feature access
    m_features=new SegmentFeatures(m_language);
    m_features->initialize(featureList);
  }
  catch (NoSuchList& ) { // keep default value
    LOGINIT("LP::Dumper");
    LERROR << "Warning: no features selected in SegmentFeaturesDumper: output will be empty";
  }
  
}

LimaStatusCode SegmentFeaturesDumper::process(
  AnalysisContent& analysis) const
{
  LOGINIT("LP::Segmentation");
  LDEBUG << "SegmentFeaturesDumper::process";

  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      LERROR << "no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }

  if (m_features==0) {
    LERROR << "No features selected => no output";
    return SUCCESS_ID;
  }

  DumperStream* dstream=initialize(analysis);
  m_features->update(analysis);

  ostream& out=dstream->out();
  
  AnalysisData* data=analysis.getData(m_data);
  if (data==0) {
    LERROR << "Error: no data '"<< m_data << "'";
    return MISSING_DATA;
  }
  SegmentationData* segmData=static_cast<SegmentationData*>(data);
  AnalysisGraph* graph=static_cast<AnalysisGraph*>(analysis.getData(segmData->getGraphId()));

  // ??OME2 LDEBUG << "SegmentationData " << m_data << " has " << segmData->size() << " segments";
  LDEBUG << "SegmentationData " << m_data << " has " << segmData->getSegments().size() << " segments";
  
  uint64_t i(0);
  // ??OME2 for (vector<Segment>::const_iterator s=segmData->begin(),
  //     s_end=segmData->end();s!=s_end;s++) {
  for (vector<Segment>::const_iterator s=(segmData->getSegments()).begin(),
       s_end=(segmData->getSegments()).end();s!=s_end;s++) {
    LDEBUG << "looking at segment " << i << ":" << (*s).getPosBegin() << "," << (*s).getLength();
    bool first=true;
    for (SegmentFeatures::const_iterator f=m_features->begin(),f_end=m_features->end();f!=f_end;f++) {
      if (first) { first=false; }
      else {
        out << m_sep;
      }
      string str=(*f)->getValue(graph,*s);
      boost::replace_all(str,m_sep,m_sepReplace);
      out << str;
    }
    out << endl;
  }

  delete dstream;
  return SUCCESS_ID;
}

} // end namespace
} // end namespace
} // end namespace
