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
/**
 *
 * @file       SentenceBoundariesXmlLogger.cpp
 * @author     Romaric Besan�n <romaric.besancon@cea.fr>
 *             Copyright (c) 2006 by CEA
 */

#include "SentenceBoundariesXmlLogger.h"
#include "SegmentationData.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"

#include <iostream>
#include <fstream>

using namespace std;
//using namespace boost;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;

namespace Lima
{
namespace LinguisticProcessing
{
namespace LinguisticAnalysisStructure
{

SimpleFactory<MediaProcessUnit,SentenceBoundariesXmlLogger>
sentenceBoundariesXmlLoggerFactory(SENTENCEBOUNDARIESXMLLOGGER_CLASSID);

SentenceBoundariesXmlLogger::SentenceBoundariesXmlLogger():
AbstractTextualAnalysisDumper(),
m_graphId("PosGraph"),
m_boundaries()
{}

SentenceBoundariesXmlLogger::~SentenceBoundariesXmlLogger()
{}

void SentenceBoundariesXmlLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);

  try
  {
    m_graphId=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (NoSuchParam& ) {} // use default value
  try
  {
    m_boundaries=unitConfiguration.getParamsValueAtKey("boundaries");
  }
  catch (NoSuchParam& ) {m_boundaries="SentenceBoundaries";} // use default value

}

// Each token of the specified path is
// searched into the specified dictionary.
LimaStatusCode SentenceBoundariesXmlLogger::process(
  AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0)
  {
    SEGMENTATIONLOGINIT;
    LERROR << "no LinguisticMetaData ! abort";
    return MISSING_DATA;
  }
  AnalysisGraph* graph=static_cast<AnalysisGraph*>(analysis.getData(m_graphId));

  AnalysisData* data=analysis.getData(m_boundaries);
  if (data == 0) {
    SEGMENTATIONLOGINIT;
    LERROR << "no SentenceBoundaries data";
    return MISSING_DATA;
  }
  SegmentationData* sb=static_cast<SegmentationData*>(data);

  std::string docId("");
  try {
    docId=metadata->getMetaData("DocId");
  }
  catch (LinguisticProcessingException& ) {
    // do nothing: not set in analyzeText (only in analyzeXmlDocuments)
  }

  uint64_t offsetIndexingNode(0);
  try {
    offsetIndexingNode=atoi(metadata->getMetaData("StartOffsetIndexingNode").c_str());
  }
  catch (LinguisticProcessingException& ) {
    // do nothing: not set in analyzeText (only in analyzeXmlDocuments)
  }

  auto dstream=AbstractTextualAnalysisDumper::initialize(analysis);
  ostream& out=dstream->out();

  if (!docId.empty()) {
    out << "<sentence_boundaries id=\""<< docId
          << "\" offsetNode=\"" << offsetIndexingNode
          << "\">" << endl;
  }
  else {
    out << "<sentence_boundaries>" << endl;
  }

  for (vector<Segment>::const_iterator it=sb->getSegments().begin(),it_end=sb->getSegments().end();
       it!=it_end; it++) {

    // use end vertex of segment as sentence boundary
    LinguisticGraphVertex end=(*it).getLastVertex();

    if (end == graph->lastVertex()) {
      //out << "<sentence_boundary special_value=\"END\"/>" << endl;
      continue;
    }

    Token* t = get(vertex_token,*(graph->getGraph()), end);
    out << "<sentence_boundary pos=\"" << t->position()
         << "\" length=\"" << t->length()
         << "\" value=\"" << Common::Misc::limastring2utf8stdstring(t->stringForm())
         << "\"/>" << endl;
  }
  out << "</sentence_boundaries>" << endl;

  TimeUtils::logElapsedTime("SentenceBoundariesXmlLogger");
  return SUCCESS_ID;
}

} // LinguisticAnalysisStructure
} // LinguisticProcessing
} // Lima
