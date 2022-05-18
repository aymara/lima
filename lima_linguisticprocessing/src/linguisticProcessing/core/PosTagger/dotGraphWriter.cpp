// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "dotGraphWriter.h"

#include "posTaggingGraphWriter.h"

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

namespace Lima
{
namespace LinguisticProcessing
{

SimpleFactory<MediaProcessUnit,DotGraphWriter> dotGraphWriterFactory(DOTGRAPHWRITER_CLASSID);

DotGraphWriter::DotGraphWriter()
{}

DotGraphWriter::~DotGraphWriter()
{}

void DotGraphWriter::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  /** @addtogroup ProcessUnitConfiguration
   * - <b>&lt;group name="..." class="DotGraphWriter"&gt;</b>
   *    -  trigramMatrix : TrigramMatrix resource
   *    -  bigramMatrix : BigramMatrix resource
   *    -  outputSuffix : suffix for output file name. Default : '.graph.dot'
   */
  PTLOGINIT;
  m_language=manager->getInitializationParameters().media;
  try {
    string trigrams=unitConfiguration.getParamsValueAtKey("trigramMatrix");
    AbstractResource* res=LinguisticResources::single().getResource(m_language,trigrams);
    m_trigramMatrix=static_cast<PosTagger::TrigramMatrix*>(res);
  } catch (NoSuchParam& ) {
    LERROR << "No param 'trigramMatrix' in DotGraphWriter group for language " << (int)m_language;
    throw InvalidConfiguration();
  }
  try {
    string bigrams=unitConfiguration.getParamsValueAtKey("bigramMatrix");
    AbstractResource* res=LinguisticResources::single().getResource(m_language,bigrams);
    m_bigramMatrix=static_cast<PosTagger::BigramMatrix*>(res);
  } catch (NoSuchParam& ) {
    LWARN << "No param 'bigramMatrix' in DotGraphWriter group for language " << (int)m_language;
    throw InvalidConfiguration();
  }
  try
  {
    m_outputSuffix=unitConfiguration.getParamsValueAtKey("outputSuffix");
  }
  catch (NoSuchParam& )
  {
    LWARN << "No param 'outputSuffix' in DotGraphWriter group for language " << (int)m_language;
    LWARN << "use .graph.dot";
    m_outputSuffix=string(".graph.dot");
  }
  try
  {
    m_graphId=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (NoSuchParam& )
  {
    LWARN << "No param 'graph' in "<<unitConfiguration.getName() << " group for language " << (int)m_language;
    LWARN << "use PosGraph";
    m_graphId=string("PosGraph");
  }
  try
  {
    m_vertexDisplay=unitConfiguration.getListsValueAtKey("vertexDisplay");
  }
  catch (NoSuchList& )
  {
    // empty display
  }
  try 
  {
    m_graphDotOptions = unitConfiguration.getMapAtKey("graphDotOptions");
  }
  catch (NoSuchMap& ) {}
  
  try 
  {
    m_nodeDotOptions = unitConfiguration.getMapAtKey("nodeDotOptions");
  }
  catch (NoSuchMap& ) {}
  
  try 
  {
    m_edgeDotOptions = unitConfiguration.getMapAtKey("edgeDotOptions");
  }
  catch (NoSuchMap& ) {}
}


// Each token of the specified path is
// searched into the specified dictionary.
LimaStatusCode DotGraphWriter::process(AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData(m_graphId));
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      PTLOGINIT;
      LERROR << "no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }
  if (anagraph == 0) {
      PTLOGINIT;
      LERROR << "no AnalysisGraph named " << m_graphId << " ! ";
      return MISSING_DATA;
  }
  string outputFileName=metadata->getMetaData("FileName") + m_outputSuffix;
  PosTagger::PosTaggingGraphWriter gw(
    anagraph->getGraph(),
    m_language,
    m_trigramMatrix,
    m_bigramMatrix);
  gw.setOptions(m_graphDotOptions,m_nodeDotOptions,m_edgeDotOptions);
  gw.writeToDotFile(outputFileName,m_vertexDisplay);

  TimeUtils::logElapsedTime("DotGraphWriter");
  return SUCCESS_ID;
}

} // LinguisticProcessing
} // Lima
