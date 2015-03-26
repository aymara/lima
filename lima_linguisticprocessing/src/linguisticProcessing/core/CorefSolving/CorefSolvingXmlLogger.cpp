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
/** @brief      xml logger for coreferences 
  *
  * @file       CorefSolvingXmlLogger.cpp
  * @author     Claire Mouton <claire.mouton@cea.fr>
  *             Copyright (c) 2007 by CEA
  */

#include "CorefSolvingXmlLogger.h"
#include "coreferentAnnotation.h"
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
#include <QtCore/QDateTime>

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
namespace Coreferences
{

SimpleFactory<MediaProcessUnit,CorefSolvingXmlLogger> fullTokenXmlLoggerFactory(COREFSOLVINGXMLLOGGER_CLASSID);

CorefSolvingXmlLogger::CorefSolvingXmlLogger():
AbstractLinguisticLogger("coref.xml")
{}

CorefSolvingXmlLogger::~CorefSolvingXmlLogger()
{}

void CorefSolvingXmlLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  AbstractLinguisticLogger::init(unitConfiguration,manager);
  try
  {
    // DTD
    m_outputSuffix=unitConfiguration.getParamsValueAtKey("outputSuffix")+".xml";
  }
  catch (NoSuchParam& )
  {
    m_outputSuffix=string(".coref")+".xml";
  }
  m_language=manager->getInitializationParameters().media;


  //m_propertyCodeManager= &(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager());
}


// Datas are extracted from coref annotations and written on the xml file according to the given dtd format
LimaStatusCode CorefSolvingXmlLogger::process(
  AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime(); 
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0)
  {
    COREFSOLVERLOGINIT;
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
        COREFSOLVERLOGINIT;
        LERROR << "no PosGraph ! abort";
        return MISSING_DATA;
    }
  

  dump(out, anagraph,/* static_cast<SyntacticData*>(analysis.getData("SyntacticData")),*/ static_cast<AnnotationData*>(analysis.getData("AnnotationData")));
  out.flush();
  out.close();  
  TimeUtils::logElapsedTime("CorefSolverXmlLogger");
  return SUCCESS_ID;
}

//**********************************************************************
// define a visitor to go through the graph and output the annotations


void DumpXMLAnnotationVisitor::examine_edge(LinguisticGraphEdge e,
                                     const LinguisticGraph& g)
{
  COREFSOLVERLOGINIT;
  LDEBUG << "DumpXMLAnnotationVisitor::examine_edge";
  LinguisticGraphVertex v = target(e, g);
  // let process sentences like (...) have automatically tuned (...) where the graph has one token "have_tuned" with one branch "automatically" "tuned" and another one with the following of the sentence
  LinguisticGraphOutEdgeIt it, it_end;
  boost::tie(it, it_end) = boost::out_edges(v,g);  
   if (it == it_end) 
     return;
  // let process sentences where one tag has not been fully determined and there is still two (or more) tag options
  LinguisticGraphVertex v2 = target(m_lastEdge, g);
  if (v2==v)
    return;
  if (m_lastEdge!=LinguisticGraphEdge() && are_equivalent(e, v2, v, g))
    return;
  // begin
  // store this edge for the future tests
  if (get(vertex_token, g,v)!=0)
    m_lastEdge = e;
//   const FsaStringsPool& stringsPool= Common::MediaticData::MediaticData::single().stringsPool(m_language);
  Token* token = get(vertex_token, g, v);
  // processing of cases like "s'y introduire", tokenized as "y s'introduire"
  if (token != 0 && (token->stringForm() == "en" || token->stringForm() =="y"))
  {
    LinguisticGraphOutEdgeIt it, it_end;
    boost::tie(it, it_end) = boost::out_edges(v,g);  
    if (it != it_end)
    { 
      Token* t = get(vertex_token, g,target(*it, g));
      if (t!=0 && Common::Misc::limastring2utf8stdstring(t->stringForm()).substr(0,2)=="s'") 
      {
        m_ostream << "s'";
      }
    }
  }
  // process
  std::set< AnnotationGraphVertex > matches = m_ad->matches("PosGraph",v,"annot");
  if (matches.empty())
  {
    COREFSOLVERLOGINIT;
    LERROR << "DumpXMLAnnotationVisitor::examine_edge No annotation graph vertex matches PoS graph vertex " << v <<  ". This should not happen.";
    return;
  }
  AnnotationGraphVertex av = *matches.begin();
  
  
  
  if (m_ad->hasAnnotation(av, Common::Misc::utf8stdstring2limastring("Coreferent")))
  {
    GenericAnnotation ga = (m_ad->annotation(av,utf8stdstring2limastring("Coreferent")));
    Lima::LinguisticProcessing::Coreferences::CoreferentAnnotation ca;
    try
    {
      ca = ga.value<Lima::LinguisticProcessing::Coreferences::CoreferentAnnotation>();
      ca.outputXml(m_ostream,g,m_ad);
    }
    catch (const boost::bad_any_cast& )
    {
      COREFSOLVERLOGINIT;
      LERROR << "non coreferent annotation";
    }
  }
  else 
  {
    Token* token = get(vertex_token, g, v);
    if (token != 0)
    {
      std::string s = Common::Misc::limastring2utf8stdstring(token->stringForm());
      // processing of cases like "s'y introduire", tokenized as "y s'introduire"    
      if (s.substr(0,2) == "s'")
      {
        Token* t = get(vertex_token,g,source(e, g));
        if (t!=0 && (Common::Misc::limastring2utf8stdstring(t->stringForm()).substr(0,2)=="en" || Common::Misc::limastring2utf8stdstring(t->stringForm()).substr(0,2)=="y"))
        {
          s = s.substr(2,s.size());
        }
      }
      // processing of cases like "le Canada a-t-il envisagé...", où le mot entre "a" et "envisagé" se retrouverait rejeté après "a_envisagé". Nécessaire de traiter car problématique pour l'évaluation quand il s'agit d'un pronom clitique comme dans ce cas-ci.
      std::string formerMemo = m_memo;
      match_results<std::string::const_iterator> what; 
      string::const_iterator start = s.begin();
      string::const_iterator end = s.end();
      if (regex_search(s, what, regex("_")))
      {
        m_memo = std::string(what[0].second,end) + " ";
        s = std::string(start,what[0].first);
      }
      else m_memo = "";

      m_ostream << formerMemo << s;
      if (token->status().isAlphaPossessive())
      {
        m_ostream << "'s "; 
      }
    }
  }
  m_ostream << " ";
}


  
bool DumpXMLAnnotationVisitor::are_equivalent(
      LinguisticGraphEdge currentEdge,
      LinguisticGraphVertex vProcessed,
      LinguisticGraphVertex vNotProcessed,
      const LinguisticGraph& g)
{
  LinguisticGraphEdge refEdge = LinguisticGraphEdge();
  LinguisticGraphInEdgeIt it, it_end;
  boost::tie(it, it_end) = boost::in_edges(vProcessed,g);
  if (it != it_end)
  { 
    refEdge = *it;
  }
  if (refEdge!=LinguisticGraphEdge() 
      && (vProcessed != vNotProcessed)
      && get(vertex_token, g,vProcessed)!=0 
      && get(vertex_token, g,vNotProcessed)!=0 
      &&  Common::Misc::limastring2utf8stdstring(get(vertex_token, g,vProcessed)->stringForm())==Common::Misc::limastring2utf8stdstring(get(vertex_token, g,vNotProcessed)->stringForm()) 
      && source(refEdge,g)==source(currentEdge,g))
  {
    return true;
  }
  if (currentEdge!=DependencyGraphEdge() && source(currentEdge,g)!=vProcessed)
  {
    if (refEdge!=LinguisticGraphEdge())
    { 
      vProcessed = source(*it,g);
    }
    vNotProcessed = source(currentEdge,g);
    boost::tie(it, it_end) = boost::in_edges(vNotProcessed,g);
    if (it != it_end)
    { 
      currentEdge = *it;
    }
    return are_equivalent(currentEdge, vProcessed, vNotProcessed,g);
  }
   return false;
}
//**********************************************************************
// dumps memory structure on XML file



void CorefSolvingXmlLogger::dump(std::ostream& xmlStream, AnalysisGraph* anagraph,   /*SyntacticAnalysis::SyntacticData* sd,*/ AnnotationData* ad) const
{
  if (m_outputSuffix == ".wh.xml")
  {
    xmlStream << "<?xml version='1.0' standalone='no'?>" << std::endl;
    xmlStream << "<!--generated by Amose on ";
    xmlStream << QDateTime::currentDateTime().toString().toUtf8().data();
    xmlStream << "-->" << std::endl;
    xmlStream << "<!DOCTYPE COREF SYSTEM \"coref.dtd\">" << std::endl;
    xmlStream << std::endl;
    xmlStream << "<TEXT>" << std::endl;
    // dump the graph
    DumpXMLAnnotationVisitor vis(xmlStream, /*sd,*/ ad, m_language);
     breadth_first_search(*(anagraph->getGraph()), anagraph->firstVertex(),visitor(vis));
  

    xmlStream << std::endl << std::endl << "</TEXT>" << std::endl;
  }
}

} // Coreferences
} // LinguisticProcessing
} // Lima
