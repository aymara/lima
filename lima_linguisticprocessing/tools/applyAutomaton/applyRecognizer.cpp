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
 * @file       applyRecognizer.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Wed Dec 17 2003
 * copyright   Copyright (C) 2003 by CEA LIST
 * 
 ***********************************************************************/

#include "applyRecognizer.h"
#include "tools/automatonCompiler/libautomatonCompiler/recognizerCompiler.h"
#include "tools/automatonCompiler/libautomatonCompiler/compilerExceptions.h"
#include "linguisticProcessing/core/Automaton/recognizer.h"
//#include "AutomatonText/recognizerText.h"
#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
// #include "common/linguisticData/linguisticData.h"
#include "common/MediaProcessors/MediaProcessors.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
//#include "linguisticProcessing/core/PosTagger/greedyPosTagger.h"
//#include "linguisticProcessing/core/TextAnalysis/textAnalysis.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <string.h>

// #include "applyRecognizer.h"
// #include "Configuration/linguisticData.h"
// #include "Graph/phoenixPathIterator.h"
// #include "Graph/phoenixGraph.h"
// #include "PosTagger/greedyPosTagger.h"

using namespace std;
using namespace Lima::LinguisticProcessing::Automaton;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common;
// using namespace Lima::Common::LinguisticData;

namespace Lima {
namespace LinguisticProcessing {

//**********************************************************************
// initialization of static members 
//**********************************************************************
// hard-coded info about the output of named entities
const std::vector<std::string>& RecognizerToApply::knownTypes() {
  static std::vector<std::string> types(0);
  if (types.empty()) {
    types.push_back("PERSON");
    types.push_back("LOCATION");
    types.push_back("ORGANIZATION");
    types.push_back("EVENT");
    types.push_back("PRODUCT");
    types.push_back("TIMEX");
    types.push_back("NUMEX");
  }
  return types;
}
const std::vector<std::string>& RecognizerToApply::openingTags() {
  static std::vector<std::string> tags(0);
  if (tags.empty()) {
    tags.push_back("ENAMEX TYPE=\"PERSON\"");
    tags.push_back("ENAMEX TYPE=\"LOCATION\"");
    tags.push_back("ENAMEX TYPE=\"ORGANIZATION\"");
    tags.push_back("ENAMEX TYPE=\"EVENT\"");
    tags.push_back("ENAMEX TYPE=\"PRODUCT\"");
    tags.push_back("TIMEX");
    tags.push_back("NUMEX");
  }
  return tags;
}

const std::vector<std::string>& RecognizerToApply::closingTags() {
  static std::vector<std::string> tags(0);
  if (tags.empty()) {
    tags.push_back("/ENAMEX");
    tags.push_back("/ENAMEX");
    tags.push_back("/ENAMEX");
    tags.push_back("/ENAMEX");
    tags.push_back("/ENAMEX");
    tags.push_back("/TIMEX");
    tags.push_back("/NUMEX");
  }
  return tags;
}

bool RecognizerToApply::findType(const std::string& type,
                                 uint64_t& i) {
  for (i=0; i<knownTypes().size(); i++) {
    if (knownTypes()[i] == type) { 
      return true;
    }
  }
  return false;
}

const std::string& RecognizerToApply::getOpeningTag(const uint64_t& i) {
  return openingTags()[i];
}
const std::string& RecognizerToApply::getClosingTag(const uint64_t& i) {
  return closingTags()[i];
}

//**********************************************************************
// generic functions 
//**********************************************************************
void RecognizerToApply::setParameters(const bool listEntities, 
                                      const std::string& encoding,
                                      const StyleOutput& outputStyle,
                                      const OutputNormalizationType& 
                                      normalization,
                                      const bool doPosTagging,
                                      const bool testOnFullToken) {
  m_listEntities=listEntities;
  m_encoding=encoding;
  m_outputStyle=outputStyle;
  m_normalizationStyle=normalization;
  m_doPosTagging=doPosTagging;
  m_testOnFullToken=testOnFullToken;
}

// the template function can be defined in the .cpp because 
// it is only instanciated in this file
template<typename ResultType>
void RecognizerToApply::printResults(const ResultType& result, 
                                     const LimaString& text,
                                     ostream& output) {
  
  Common::MediaticData::LanguageData::EntityNames
    entityNames = Common::LinguisticData::LinguisticData::single().languageData(m_language).getEntityNames("SpecificEntities");
  
  uint64_t offsetLastSuccess(0);
  string tmpString;
  if (m_listEntities) {
    for (uint64_t i(0); i<result.size(); i++) {
      LanguageData::EntityNames::const_iterator
        type=entityNames.find(result[i].getType());
      if (type==entityNames.end()) {
        AULOGINIT;
        LERROR << "undefined type " << result[i].getType();
      }
      else {
        output << result[i] << " "
               << (*type).second << " "
               << result[i].getNormalizedForm() << endl;
      }
    }
  }
  else {
    for (uint64_t i(0); i<result.size(); i++) {
      //std::cerr << "entity " << i << "=[" << result[i] << "]" << endl;
      switch (m_outputStyle) {
      case INLINE:
      {
//     if (i>0 && result[i].positionBegin() < result[i-1].positionEnd()) {
//       std::cerr << "Warning : overlap in named entities : "
//         << result[i-1].concatString() << "/"
//         << result[i].concatString()
//         << "... second ignored" << endl;
//       // print rest of string
//       //output << result[i].concatString();
//       output << LimaString(text,offsetLastSuccess,
//                 result[i].positionEnd()-
//                 offsetLastSuccess-1);
//       offsetLastSuccess = result[i].positionEnd()-1;

        if (i<result.size()-1 &&
            result[i+1].positionBegin() < result[i].positionEnd()) {
          std::cerr << "Warning : overlap in named entities : "
                    << result[i].concatString() << "/"
                    << result[i+1].concatString()
                    << "... first ignored" << endl;
          // print rest of string
          //output << result[i].concatString();
          if (m_encoding=="latin1") {
            output << LimaString(text,offsetLastSuccess,
                                  result[i+1].positionBegin()-
                                  offsetLastSuccess-1);
          }
          else { 
            tmpString = Misc::limastring2utf8stdstring(
                                     LimaString(text,offsetLastSuccess,
                                                 result[i+1].positionBegin()-
                                                 offsetLastSuccess-1));
            output << tmpString;
          }
          offsetLastSuccess = result[i+1].positionBegin()-1;
        }
        else {
          if (m_encoding=="latin1") {
            output << outputString(result[i], text,
                                   offsetLastSuccess,
                                   entityNames,
                                   m_normalizationStyle);
          }
          else {
            tmpString = Misc::limastring2utf8stdstring(
                                     outputString(result[i],text,
                                                  offsetLastSuccess,
                                                  entityNames,
                                                  m_normalizationStyle));
            output << tmpString;
          }
          offsetLastSuccess = result[i].positionEnd()-1;
        }
        break;
      }
      case OFFLINE:
        {
          std::cerr << "OFFLINE mode not available for output" << std::endl;
//          tmpString = = Common::Misc::limastring2utf8stdstring(result[i].xmlOutput(result[i].concatString()));
//           output << tmpString;
//           offsetLastSuccess = result[i].positionEnd()-1;
          break;
        }
      }
    }
    if (offsetLastSuccess < text.length()) {
      if (m_encoding=="latin1") {
        output << LimaString(text,offsetLastSuccess);
      }
      else {
        tmpString = Misc::limastring2utf8stdstring(LimaString(text,offsetLastSuccess));
        output << tmpString;
      }
    }
  }
}

//**********************************************************************
// recognizer on simple text
//**********************************************************************
// temporarily deactivated
/*
RecognizerOnSimpleText::RecognizerOnSimpleText(Recognizer* reco):
RecognizerToApply(reco),
m_recognizerText(0)
{
  m_recognizerText=dynamic_cast<RecognizerText*>(m_recognizer);
  if (m_recognizerText == 0) {
    cerr << "Cannot use a standard recognizer as text recognizer" << endl;
    exit(1);
  }
}

uint64_t RecognizerOnSimpleText::applyToText(LimaString contentText, 
                                                 ostream& output)
{
  uint64_t nbEntitiesFound(0);

  vector<PosMultipleMatch> result;
  
  m_recognizerText->test(contentText,result);

  // print the results
  printResults(result,contentText,output);
  nbEntitiesFound=result.size();

  return nbEntitiesFound;
}
*/

//**********************************************************************
// recognizer on tokenized text
//********************************************************************** 
/*
bool findTag(const LimaString& line, 
             const LimaString& tag,
             std::string& value) {

  LimaString openingTag=LimaString("<")+tag+LimaString(">");
  LimaString closingTag=LimaString("</")+tag+LimaString(">");

  uint64_t tagOpen=line.find(openingTag,0);
  if (tagOpen != string::npos) {
    uint64_t beginValue=tagOpen+openingTag.length();
    uint64_t tagEnd=line.find(closingTag,0);
    if (tagEnd != string::npos) {
      Misc::wstring2string(value,
                           LimaString(line,beginValue,tagEnd-beginValue));
      return true;
    }
  }
  return false;
}

RecognizerOnTokenizedText::
RecognizerOnTokenizedText(Recognizer* reco,
                          MediaId language,
                          const std::string& resourcesPath):
  RecognizerToApply(reco),
  m_tokenizer(0),
  m_language(language) {
    
  try  {
    m_tokenizer=LinguisticProcessors::single().getTokenizer(Common::LinguisticData::LinguisticData::single().language(language));
  }
  catch (exception& e) {
    throw runtime_error("can't find tokenizer for language " +language);
  }
}

uint64_t RecognizerOnTokenizedText::applyToText(LimaString contentText, 
                                                    ostream& output)
{
  uint64_t nbEntitiesFound(0);
  
  PhoenixPathIterator analysis;
  try {
    analysis=m_tokenizer->tokenize(contentText);
  }
  catch (exception& e) {
    cerr << "exception caught in tokenizer: " << e.what() << endl;
    return 0;
  }
  
  vector<RecognizerMatch> result;
  
  const LinguisticGraph* graph=analysis.getGraph();
  LinguisticGraphVertex beginning=firstVertex(*graph);

  m_recognizer->test(analysis,beginning,result);

  // print the results
  printResults(result,contentText,output);
  nbEntitiesFound=result.size();
  
  return nbEntitiesFound;
}
*/

//**********************************************************************
// recognizer on analyzed text
//**********************************************************************
RecognizerOnAnalyzedText::
RecognizerOnAnalyzedText(Recognizer* reco, 
                         MediaId language,
                         const std::string& resourcesPath,
                         const bool dumpXML,
                         const bool reorganizeRules):
  RecognizerToApply(reco,language) {

  // reorganize the rules if they are not in binary format
  if (reorganizeRules) {
    cerr << "Rules reorganization not available: ignored" << endl;
  /*
    //reorganize rules according to the dictionary
      Lima::Dictionary::Dictionary* dico=LinguisticData::single().getDictionary(language);
      if (dico==0)
      {
    throw runtime_error("no dictionary available for language " + language);
      }
      m_recognizer->reorganizeRules(*dico);
  */
  }
}

uint64_t RecognizerOnAnalyzedText::applyToText(LimaString contentText, 
                                                   ostream& output)
{
/*  cerr << "analyzing text ("<< m_language << ") [" 
        << contentText << "]" << endl;*/
  AULOGINIT;
  //PhoenixPathIterator analysis;
//   AnalysisContent analysis;
  
  ostringstream error;
  try {
    // create analysis content
    LinguisticMetaData* metadataholder=new LinguisticMetaData(); // will be destroyed in AnalysisContent destructor
    m_analysis.setData("LinguisticMetaData",metadataholder);
    std::map<std::string,std::string> metaData;
    metaData["FileName"]="applyrules-output";
    metadataholder->setMetaData(metaData);
    LimaStringText* lstexte=new LimaStringText(contentText);
    m_analysis.setData("Text",lstexte);

    // get pipeline
    std::string pipelineId("main"); // hard-coded pipeline
    const LinguisticProcessUnitPipeline* pipeline=LinguisticProcessors::single().getPipelineForId(m_language,pipelineId);
    if (pipeline==0) {
      LWARN << "can't get pipeline '" << pipelineId << "'";
      throw LinguisticProcessingException();
    }

    // process pipeline
    LimaStatusCode status=pipeline->process(m_analysis);
    LDEBUG << "pipeline process returned status " << (int)status ;
    if (status!=SUCCESS_ID) {
      LWARN << "analysis failed : receive status " << (int)status << " from pipeline. exit";
      throw LinguisticProcessingException();    
    }
/*      LinguisticProcessing::TextAnalysis textanalysis(
    string("applytext"),
    contentText,
    m_language,
    true);
      textanalysis.prepare();
      Recognizer::setTestOnDicoWord();
      analysis=textanalysis.getAnalysis();
*/
  }
  catch (std::exception& e) {
    cerr << "caught exception" << endl;
    cerr << e.what();
    throw;
  }
  catch (...) {
    cerr << "unknown exception";
    throw;
  }

  uint64_t nbEntitiesFound(0);

//   cerr << "applying recognizer on graph" << endl;

  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(m_analysis.getData("AnalysisGraph"));

  //LinguisticGraphVertex tokenList=analysis.getCurrent();
//  LinguisticGraph* graph=tokenList->getGraph();
  vector<RecognizerMatch> result;
  //reco.test(*graph,tokenList,result);
  LinguisticGraphVertex beginning=tokenList->firstVertex();
  LinguisticGraphVertex end=tokenList->lastVertex();
  bool testOnDicoWord=Recognizer::testOnDicoWord();
  if (m_testOnFullToken) {
    Recognizer::setTestOnFullToken();
  }
  else {
    Recognizer::setTestOnDicoWord();
  }
  m_recognizer->apply(*tokenList,beginning,end,m_analysis,result);
  m_recognizer->resolveOverlappingEntities(result,Automaton::IGNORE_SMALLEST);
//     uint64_t n(0);
//     for (vector<RecognizerMatch>::const_iterator i=result.begin();
//          i != result.end(); i++) {
//       std::cerr<<LimaString("NamedEntities:results("<<n<<")=")
//            <<*i<< endl;
//       n++;
//     }

  //reset old value (useful?)
  if (testOnDicoWord) {
    Recognizer::testOnDicoWord();
  }
  else {
    Recognizer::setTestOnFullToken();
  }

  printResults(result,contentText,output);
  nbEntitiesFound=result.size();

  // print the results
  return nbEntitiesFound;
}

} // end namespace
} // end namespace
