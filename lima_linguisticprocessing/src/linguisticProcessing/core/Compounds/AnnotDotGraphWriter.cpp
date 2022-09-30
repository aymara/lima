// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "AnnotDotGraphWriter.h"

#include "linguisticProcessing/common/annotationGraph/AnnotationGraphWriter.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "common/time/traceUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"

#include <iostream>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::AnnotationGraphs;

namespace Lima
{
namespace LinguisticProcessing
{
namespace Compounds
{

SimpleFactory<MediaProcessUnit,AnnotDotGraphWriter> annotDotGraphWriterFactory(ANNOTDOTGRAPHWRITER_CLASSID);

AnnotDotGraphWriter::AnnotDotGraphWriter()
{}

AnnotDotGraphWriter::~AnnotDotGraphWriter()
{}

void AnnotDotGraphWriter::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  /** @addtogroup ProcessUnitConfiguration
   * - <b>&lt;group name="..." class="AnnotDotGraphWriter"&gt;</b>
   *    - \b outputSuffix : suffix for output file name. Default : '.graph.dot'
   */
  PTLOGINIT;
  m_language=manager->getInitializationParameters().media;
  try
  {
    m_outputSuffix=unitConfiguration.getParamsValueAtKey("outputSuffix");
  }
  catch (NoSuchParam& )
  {
    LWARN << "No param 'outputSuffix' in " << unitConfiguration.getName()
        << " group for language " << (int)m_language;
    LWARN << "use .ag.dot";
    m_outputSuffix=string(".ag.dot");
  }
  try
  {
    m_graphId=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (NoSuchParam& )
  {
    LWARN << "No param 'graph' in "<< unitConfiguration.getName()
        << " group for language " << (int)m_language;
    LWARN << "use PosGraph";
    m_graphId=string("PosGraph");
  }
}


// Each token of the specified path is
// searched into the specified dictionary.
LimaStatusCode AnnotDotGraphWriter::process(AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData(m_graphId));
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0)
  {
    COMPOUNDSLOGINIT;
    LERROR << "no LinguisticMetaData ! abort";
    return MISSING_DATA;
  }
  if (anagraph == 0)
  {
    COMPOUNDSLOGINIT;
    LERROR << "no AnalysisGraph named " << m_graphId << " ! ";
    return MISSING_DATA;
  }
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    COMPOUNDSLOGINIT;
    LERROR << "no annotation graph available !";
    return MISSING_DATA;
  }

  string outputFileName=metadata->getMetaData("FileName") + m_outputSuffix;
  AnnotationGraphWriter gw(
    *annotationData,
    m_language);
  gw.writeToDotFile(outputFileName);

  TimeUtils::logElapsedTime("AnnotDotGraphWriter");
  return SUCCESS_ID;
}

} // MorphologicAnalysis
} //namespace LinguisticProcessing
} // Lima


