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
/** @brief      xml logger for Word Senses 
  *
  * @file       WordSenseXmlLogger.cpp
  * @author     Claire Mouton <claire.mouton@cea.fr>
  *             Copyright (c) 2010 by CEA
  */
#include "WordSenseXmlLogger.h"
#include "WordSenseAnnotation.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AgglutinatedToken.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"

#include <iostream>
#include <fstream>
#include <boost/regex.hpp>

using namespace std;
using namespace boost;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::Common::Misc;


namespace Lima
{
namespace LinguisticProcessing
{
namespace WordSenseDisambiguation
{

SimpleFactory<MediaProcessUnit,WordSenseXmlLogger> fullTokenXmlLoggerFactory(WORDSENSEXMLLOGGER_CLASSID);

WordSenseXmlLogger::WordSenseXmlLogger():
AbstractLinguisticLogger(".senses.xml")
{}

WordSenseXmlLogger::~WordSenseXmlLogger()
{}

void WordSenseXmlLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  AbstractLinguisticLogger::init(unitConfiguration,manager);
  try
  {
    // DTD
    m_outputSuffix=unitConfiguration.getParamsValueAtKey("outputSuffix");
  }
  catch (NoSuchParam& )
  {
    m_outputSuffix=string(".senses")+".xml";
  }
  m_language=manager->getInitializationParameters().media;


}


// Datas are extracted from word sense annotations and written on the xml file according to the given dtd format
LimaStatusCode WordSenseXmlLogger::process(
  AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime(); 
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0)
  {
    LOGINIT("WordSenseDisambiguator");
    LERROR << "no LinguisticMetaData ! abort";
    return MISSING_DATA;
  }

  string textFileName = metadata->getMetaData("FileName");
  string outputFile = textFileName + m_outputSuffix;
  ofstream out(outputFile.c_str(), std::ofstream::binary);
  if (!out.good()) {
      throw runtime_error("can't open file " + outputFile);
  }
   AnalysisGraph* /*anagraph=static_cast<AnalysisGraph*>(analysis.getData("SimpleGraph"));
   if (anagraph==0)*/
  
    anagraph=static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
    if (anagraph==0)
    {
        LOGINIT("WordSenseDisambiguator");
        LERROR << "no AnalysisGraph ! abort";
        return MISSING_DATA;
    }
  

  dump(out, anagraph,/* static_cast<SyntacticData*>(analysis.getData("SyntacticData")),*/ static_cast<AnnotationData*>(analysis.getData("AnnotationData")));
  out.flush();
  out.close();  
  TimeUtils::logElapsedTime("WordSenseDisambiguatorXmlLogger");
  return SUCCESS_ID;
}

//**********************************************************************
// define a visitor to go through the graph and output the annotations


void DumpXMLAnnotationVisitor::examine_edge(LinguisticGraphEdge e,
                                     const LinguisticGraph& g)
{
  LinguisticGraphVertex v = target(e, g);
  // process
  if (m_ad->hasAnnotation(v, Common::Misc::utf8stdstring2limastring("WordSense")))
  {
    GenericAnnotation ga = (m_ad->annotation(v,utf8stdstring2limastring("WordSense")));
    Lima::LinguisticProcessing::WordSenseDisambiguation::WordSenseAnnotation wsa;
    try
    {
      wsa = ga.value<Lima::LinguisticProcessing::WordSenseDisambiguation::WordSenseAnnotation>();
      wsa.outputXml(m_ostream,g);
    }
    catch (const boost::bad_any_cast& e)
    {
      LOGINIT("WordSenseDisambiguator");
      LERROR << "non word sense annotation"<< LENDL;
    }
  }
  else 
  {
    Token* token = get(vertex_token, g, v);
    if (token != 0)
    {
      std::string s = Common::Misc::limastring2utf8stdstring(token->stringForm());
      m_ostream << s;      
    }
  }
  m_ostream << " ";
}


  
//**********************************************************************
// dumps memory structure on XML file



void WordSenseXmlLogger::dump(std::ostream& xmlStream, AnalysisGraph* anagraph,   /*SyntacticAnalysis::SyntacticData* sd,*/ AnnotationData* ad) const
{  
  if (m_outputSuffix == ".senses.xml")
  {
    xmlStream << "<?xml version='1.0' standalone='no'?>" << std::endl;
    xmlStream << "<!--generated by MM project on ";
    time_t aclock;
    time(&aclock);                   /* Get time in seconds */
    std::string str(ctime(&aclock));
    str = str.substr(0,str.size()-1);
    xmlStream << str;
    xmlStream << "-->" << std::endl;
    xmlStream << "<!DOCTYPE WORDSENSE SYSTEM \"wordsense.dtd\">" << std::endl;
    xmlStream << std::endl;
    xmlStream << "<TEXT>" << std::endl;
    // dump the graph
    DumpXMLAnnotationVisitor vis(xmlStream, /*sd,*/ ad, m_language);
     breadth_first_search(*(anagraph->getGraph()), anagraph->firstVertex(),visitor(vis));
  

    xmlStream << std::endl << std::endl << "</TEXT>" << std::endl;
  }
}

} // WordSenseDisambiguation
} // LinguisticProcessing
} // Lima
