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

#include "EntityTrackerXmlLogger.h"

#include "common/MediaticData/mediaticData.h"
#include "common/Data/strwstrtools.h"
#include "common/time/traceUtils.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/common/misc/AbstractLinguisticLogger.h"
#include "CoreferenceData.h"

#include <fstream>

using namespace std;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::Common::Misc;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{
namespace LinguisticProcessing
{
namespace EntityTracking
{

SimpleFactory<MediaProcessUnit,EntityTrackerXmlLogger> EntityTrackerXmlLoggerFactory(ENTITYTRACKERXMLLOGGER_CLASSID);

EntityTrackerXmlLogger::EntityTrackerXmlLogger() :
AbstractLinguisticLogger(".output.xml"),
m_language(0)
{
}


EntityTrackerXmlLogger::~EntityTrackerXmlLogger()
{}

void EntityTrackerXmlLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  SELOGINIT;
  LDEBUG << "EntityTrackerXmlLogger::init";
  AbstractLinguisticLogger::init(unitConfiguration,manager);

  m_language=manager->getInitializationParameters().media;

  try
  {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    SELOGINIT;
    LWARN << "No 'graph' parameter in unit configuration '"
        << unitConfiguration.getName() << "' ; using AnalysisGraph";
    m_graph=string("AnalysisGraph");
  }
  try
  {
    string val=unitConfiguration.getParamsValueAtKey("compactFormat");
    if (val=="yes" || val=="true" || val=="1") {
      m_compactFormat=true;
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // do nothing: optional

}


LimaStatusCode EntityTrackerXmlLogger::process(
  AnalysisContent& analysis) const
{
  SELOGINIT;
  LDEBUG << "EntityTrackerXmlLogger::process";
  TimeUtils::updateCurrentTime();
  /* permet de récupérer les annotations */
  //AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));

  /* recupérer le graph après l'analyse */
  //const LinguisticAnalysisStructure::AnalysisGraph& graph = *(static_cast<LinguisticAnalysisStructure::AnalysisGraph*>(analysis.getData(m_graph)));

  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      SELOGINIT;
      LERROR << "no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }

  CoreferenceData* corefData=static_cast<CoreferenceData*>(analysis.getData("CoreferenceData"));
  if (corefData == 0) {
      SELOGINIT;
      LERROR << "no CoreferenceData ! abort";
      return MISSING_DATA;
  }

  ofstream out;
  if (!openLogFile(out,metadata->getMetaData("FileName"))) {
    SELOGINIT;
    LERROR << "Can't open log file '" << metadata->getMetaData("FileName") << "'";
    return UNKNOWN_ERROR;
  }

  out << "<coreference>" << endl;
  for (CoreferenceData::const_iterator it=corefData->begin(), 
         it_end=corefData->end(); it != it_end; it++)
  {
    out << "<entity mentions=\"" << (*it).size() << "\">" << endl;
    for (vector<Token>::const_iterator it2=(*it).begin(), it2_end=(*it).end();
         it2 != it2_end; it2++)
    {
      out << "  <entity_mention>" 
          << limastring2utf8stdstring((*it2).stringForm())
          <<"</entity_mention>";
    }
    out << "<entity>" <<endl;
  }
  out.close();

  return SUCCESS_ID;
}


} // EntityTracking
} // LinguisticProcessing
} // Lima
