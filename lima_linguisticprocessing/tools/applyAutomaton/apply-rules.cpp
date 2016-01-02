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
/***********************************************************************
*
* File       : apply-rules.cpp
* Author     : Romaric Besan�n (besanconr@zoe.cea.fr)
* Created on : Mon Sept 29 2003
* Copyright  : (c) 2003 by CEA
*
************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "applyRecognizer.h"
#include "tools/automatonCompiler/recognizerCompiler.h"
#include "tools/automatonCompiler/compilerExceptions.h"
#include "linguisticProcessing/core/Automaton/recognizer.h"
#include "linguisticProcessing/core/Automaton/automatonReaderWriter.h"
#include "common/tools/LimaMainTaskRunner.h"
#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
// #include "common/linguisticData/linguisticData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <string.h>

using namespace std;

using namespace Lima;
using namespace Lima::LinguisticProcessing;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::Automaton;
using namespace Lima::Common;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::LinguisticData;

//****************************************************************************
// declarations
//****************************************************************************
// help mode & usage
static const string USAGE("usage : apply-rules [options] rulesfile textfile\n");

static const string HELP("find named entities based on rules\n"
+USAGE
+"\n"
+"--help : this help page\n"
+"--language=..  : indicates the language for the analysis\n"
+"                 (default is fre)\n"
+"--bin          : the rules file is in binary format (result of the\n"
+"                 compile-rules program)\n"
+"--xml          : read the text file as an xml file\n"
+"--contentTags=..: is --xml, list of tags that contains content fields\n"
+"                 (separated by commas)\n"
+"--encoding=..  : indicates the encoding of input file (default is utf8)\n"
+"--outputOnFile : output is saved on a file named textfile.tagged\n"
+"                 (otherwise printed on stdout)\n"
+"--[testOnFullToken|testOnDicoWord] : indicates if the rules should be\n"
+"                 applied using results of postagging or not\n"
+"\n"
+"rulesfile is the name of the file containing the rules, \n"
+"textfile is the name of the file containing the text to be analyzed\n"
);

#define DEFAULT_COMMON_CONFIG_FILE "lima-common.xml"
#define DEFAULT_LP_CONFIG_FILE "lima-analysis.xml"
#define DEFAULT_LANGUAGE "fre"
#define DEFAULT_ENCODING "utf8"

//****************************************************************************
// GLOBAL variable -> the command line arguments
struct {
  string inputRulesFile;      // name of the rules file
  vector<string> inputTextFiles; // name of the text files to process
  string commonConfigFile;          // config file for linguisticData
  string lpConfigFile;          // config file for linguisticData
  string configDir;          // config dir for linguisticData
  string resourcesPath;       // parameters file for the dictionary
  string language;            // language for the analysis
  string encoding;            // language for the analysis
  StyleOutput outputStyle;    // style of output
  OutputNormalizationType normalizationStyle; // style for output normalization
  vector<LimaString> contentElts;// XML tags indicating content elements
  bool all;                   // load all text and treats it in one part
  bool xml;                   // xml kind
  bool binaryRules;           // the rules are in binary format
  bool outputOnFile;          // the output is written in files
  bool dumpXML;               // dump result of linguistic analysis in XML file
  bool listEntities;          // simple list of the entities found
  bool usePathIterators;      // simple list of the entities found
  bool doPosTagging;          // do greedy postagging and use result
  bool text;                  // apply rules on simple text
  bool tokenize;              // apply rules on tokenized text
  bool testOnFullToken;       // apply rules on FullToken or on DicoWord
  bool help;                  // help mode
} param={string(),
         vector<string>(0),
         DEFAULT_COMMON_CONFIG_FILE,
         DEFAULT_LP_CONFIG_FILE,
         "",
         "",
         DEFAULT_LANGUAGE,
         DEFAULT_ENCODING,
         INLINE,
         KNOWN_NORMALIZATION_ONLY,
         vector<LimaString>(1,Common::Misc::utf8stdstring2limastring("content")),
         true,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         true,
         false};

void readCommandLineArguments(uint64_t argc, char *argv[])
{
  bool languageSpecified(false);
  for(uint64_t i(1); i<argc; i++){
    string s(argv[i]);
    if (s=="-h" || s=="--help")
      param.help=true;
    else if (s=="--doPosTagging")
      param.doPosTagging=true;
    else if (s=="--line")
      param.all=false;
    else if (s=="-xml" || s=="--xml")
      param.xml=true;
    else if (s=="--text")
      param.text=true;
    else if (s=="--testOnFullToken")
      param.testOnFullToken=true;
    else if (s=="--testOnDicoWord")
      param.testOnFullToken=false;
    else if (s=="--tokenize")
      param.tokenize=true;
    else if (s=="--old" || s=="--use-pathiterators")
      param.usePathIterators=true;
    else if (s=="-l" || s=="--listEntities") {
      param.listEntities=true;
      param.outputStyle=OFFLINE;
    }
    else if (s=="--dumpXML")
      param.dumpXML=true;
    else if (s=="--outputOnFile")
      param.outputOnFile=true;
    else if (s=="-bin" || s=="--bin" || s=="--binaryRules")
      param.binaryRules=true;
    else if (s=="-off" || s=="--off")
      param.outputStyle=OFFLINE;
//     else if (s.find("-o",0)==0)
//       param.outputFile=s.substr(2,s.length()-2);
    else if (s.find("--resourcesPath=",0)==0) {
      param.resourcesPath=string(s,16);
    }
    else if (s.find("--language=",0)==0) {
      param.language=s.substr(11,s.length()-11);
      languageSpecified=1;
    }
    else if (s.find("--encoding=",0)==0) {
      param.encoding=s.substr(11,s.length()-11);
    }
    else if (s.find("--normalization=",0)==0) {
      string norm(s,16);
      if (norm == "none") {
    param.normalizationStyle = NO_NORMALIZATION;
      }
      else if (norm == "use-default") {
    param.normalizationStyle = DEFAULT_NORMALIZATION;
      }
      else if (norm == "known-only") {
    param.normalizationStyle = KNOWN_NORMALIZATION_ONLY;
      }
    }
    else if (s.find("--contentTags=",0)==0) {
      param.contentElts.clear();
      LimaString tags(string(s,14));
      uint64_t i(tags.find(',',0));
      while (i!=string::npos) {
        param.contentElts.push_back(LimaString(tags,0,i));
        tags=LimaString(tags,i+1);
        i=tags.find(',',0);
      }
      param.contentElts.push_back(tags);
    }
    else if (s[0]=='-') {
      cerr << "unrecognized option " <<  s
        << endl;
      cerr << USAGE << endl;
      exit(1);
    }
    else { // file names
      if (param.inputRulesFile.empty()) {
    param.inputRulesFile = s;
      }
      else {
    param.inputTextFiles.push_back(s);
      }
    }
  }
  if (param.resourcesPath.empty()) {
    char* resourcesStr =  getenv("LIMA_RESOURCES");
    if (resourcesStr != NULL) { param.resourcesPath = resourcesStr; }
    else { cerr << "$LIMA_RESOURCES not defined" << endl; exit(1); }
  }
  if (param.configDir.empty()) {
    char* configStr =  getenv("LIMA_CONF");
    if (configStr != NULL) { param.configDir = configStr; }
    else { cerr << "$LIMA_CONF not defined" << endl; exit(1); }
  }
}

//**********************************************************************
// local getline

void localGetline(ifstream& file, LimaString& line) {
  string str = Lima::Common::Misc::readLine(file);
  if (param.encoding=="latin1") {
    line = Misc::latin15stdstring2limastring(str);
  }
  else if (param.encoding=="utf8") {
    line = Misc::utf8stdstring2limastring(str);
  }
  else {
    cerr << "Error: encoding not handled: " << param.encoding << endl;
    exit(1);
  }
}

//**********************************************************************
// test if we are inside interesting parts of an XML document

bool searchTag(const LimaString& s, const LimaString& tagToSearch,
           LimaString& strTag, LimaString& strBefore, LimaString& strAfter) {

  uint64_t i(s.find(tagToSearch,0));
  if (i != string::npos) {
    strBefore=LimaString(s,0,i);
    strTag=tagToSearch;
    strAfter=LimaString(s,i+tagToSearch.length());
    return true;
  }
  return false;
}

bool isContentBegin(const LimaString& s, LimaString& strTag,
            LimaString& strBefore, LimaString& strAfter) {
  strTag.clear();
  strBefore.clear();
  strAfter.clear();
  for (vector<LimaString>::const_iterator contentTag(param.contentElts.begin());
       contentTag != param.contentElts.end();
       contentTag++) {
         if (searchTag(s,Common::Misc::utf8stdstring2limastring("<")+*contentTag+Common::Misc::utf8stdstring2limastring(">"),
          strTag,strBefore,strAfter)) {
      return true;
    }
  }
  return false;
}

bool isContentEnd(LimaString s, LimaString& strTag,
          LimaString& strBefore, LimaString& strAfter) {
  strTag.clear();
  strBefore.clear();
  strAfter.clear();
  for (vector<LimaString>::const_iterator contentTag(param.contentElts.begin());
       contentTag != param.contentElts.end();
       contentTag++) {
         if (searchTag(s,Common::Misc::utf8stdstring2limastring("</")+*contentTag+Common::Misc::utf8stdstring2limastring(">"),
          strTag,strBefore,strAfter)) {
      return true;
    }
  }
  return false;
}

// apply the recognizer on the files in mode all : read all text
// first and apply the recognizer on all text
uint64_t applyAll(RecognizerToApply* reco,
                      std::ifstream& file, ostream& output)
{
  uint64_t nbEntitiesFound(0);

  // ------------------------------------------------------------------
  // all-text mode : treats all text 
  // ------------------------------------------------------------------
  LimaString line;
  // load all text in one string and apply the reco on this string
  LimaString contentText();

  ostringstream tmp;   // hack to have the good end of line
  tmp << std::endl;          // (ugly)
  LimaString endline(tmp.str());

  if (param.xml) { // take only text between <content> </content>
    bool inContent(false);
    LimaString stringTagBegin,stringTagEnd,stringBeforeTag,stringAfterTag;
    while (! file.eof()) {
      localGetline(file,line);
      if (isContentBegin(line,stringTagBegin,stringBeforeTag,stringAfterTag)) {
    inContent=true;
    if (param.outputStyle!=OFFLINE) {
      output << stringBeforeTag << stringTagBegin;
    }
    line=stringAfterTag;
      }
      if (isContentEnd(line,stringTagEnd,stringBeforeTag,stringAfterTag)) {
    inContent=false;
    contentText += stringBeforeTag;

        try {
          nbEntitiesFound += reco->applyToText(contentText,output);
        }
        catch (runtime_error& e) {
          string sectionTag;
          sectionTag = Misc::limastring2utf8stdstring(stringTagBegin);
          cerr << "Exception occurred in " << sectionTag << " section : "
               << e.what() << endl;
          output << contentText;
        }

    contentText.clear();

    if (param.outputStyle!=OFFLINE) {
      output << stringTagEnd << stringAfterTag << endl;
    }

      }
      else if (inContent)     {
    contentText += line+endline;
      }
      else if (param.outputStyle!=OFFLINE) {
    if (! file.eof() || line.size()!=0 ) {
      output  << line << endl;
    }
      }
    }
  }
  else { // take all text
    while (! file.eof()) {
      localGetline(file,line);
      contentText += line+endline;
    }
    nbEntitiesFound += reco->applyToText(contentText,output);
  }



  return nbEntitiesFound;
}

// apply the recognizer on the files in mode line :
// apply the recognizer line after line
void applyLine(RecognizerToApply* reco,
               std::ifstream& file, ostream& output)
{
  // ------------------------------------------------------------------
  // line mode : treats one line at a time
  // ------------------------------------------------------------------
  LimaString line;
  while (! file.eof()) {
    localGetline(file,line);
    if (line.length() == 0) { continue; } // skip blank lines
    if (line[0] == LimaChar('#')) { continue; } // skip comments
    //       std::cerr << "[" << line << "]" << endl;

    reco->applyToText(line,output);
    output << endl;
  }
}

void applyRecognizerToFiles(Recognizer* reco, const vector<string>& files) {

  RecognizerToApply* recoToApply=0;
  if (param.text) {
    cerr << "option --text is not available" << endl;
    //recoToApply=new RecognizerOnSimpleText(reco);
  }
  else if (param.tokenize) {
    cerr << "RecognizerOnTokenizedText is not available" << endl;
/*    recoToApply=new RecognizerOnTokenizedText(reco,
                                              LinguisticData::single().language(param.language),
                                              param.resourcesPath);
  */
  }
  else {
    recoToApply=new RecognizerOnAnalyzedText(reco,
                                             Common::LinguisticData::LinguisticData::single().language(param.language),
                                             param.resourcesPath,
                                             param.dumpXML,
                                             !param.binaryRules);
  }

  // transfer parameters to the recognizer to apply
  // (for result output)
  recoToApply->setParameters(param.listEntities, 
                             param.encoding,
                             param.outputStyle,
                             param.normalizationStyle,
                             param.doPosTagging,
                             param.testOnFullToken);

  for (vector<string>::const_iterator fileIn(files.begin());
       fileIn!=files.end(); fileIn++) {
    // open the text file
    std::ifstream file;
    if (! param.inputTextFiles.empty()) {
      file.open((*fileIn).c_str());
    }
    if (!file) {
      cerr << "Error: cannot open file: " << (*fileIn)
       << "... ignored" << endl;
      continue;
    }


    if (param.outputOnFile) {
      string fileOut=(*fileIn)+".tagged";
      ofstream fout(fileOut.c_str());
      if (! fout) {
        cerr << "cannot open file [" << fileOut << "]" << endl;
      }
      if (param.all) {
        uint64_t nbEntitiesFound=applyAll(recoToApply, file, fout);
        cerr << *fileIn << ": " << nbEntitiesFound << " entities found"
             << endl;
      }
      else {
        applyLine(recoToApply,file,fout);
      }
    }
    else { // printing on standard output
      if (param.all) {
        applyAll(recoToApply,file,cout);
      }
      else {
        applyLine(recoToApply,file,cout);
      }
    }
  } // end for each file
  delete recoToApply;
}

//****************************************************************************
//  M A I N
//****************************************************************************
int run(int aargc,char** aargv);

int main(int argc, char **argv)
{
  QCoreApplication a(argc, argv);

  // Task parented to the application so that it
  // will be deleted by the application.
  LimaMainTaskRunner* task = new LimaMainTaskRunner(argc, argv, run, &a);

  // This will cause the application to exit when
  // the task signals finished.
  QObject::connect(task, SIGNAL(finished(int)), &a, SLOT(quit()));

  // This will run the task from the application event loop.
  QTimer::singleShot(0, task, SLOT(run()));

  return a.exec();

}


int run(int argc,char** argv)
{
  QsLogging::initQsLog();
  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();
  
  if (argc<2) {    cerr << USAGE; exit(1); }
  readCommandLineArguments(argc,argv);
  if (param.help) { cerr << HELP; exit(1); }

  deque<string> langs(1,param.language);
  // initialize common
  Common::LinguisticData::LinguisticData::changeable().init(
    param.resourcesPath,
    param.configDir,
    param.commonConfigFile,
    langs);

  // initialize linguistic processing 
    string clientId("lima-coreclient");
    Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig(param.configDir + "/" + param.lpConfigFile);
    LinguisticProcessingClientFactory::changeable().configureClientFactory(
      clientId,
      lpconfig,
      langs);
  
  // initialize linguisticData
  //Common::LinguisticData::LinguisticData::initial().init(param.configFile,param.language);

  // read rules file
  Recognizer* reco = 0;
  if (param.text) {
    cerr << "option --text not available" << endl;
    //reco = new RecognizerText;
  }
  else {
    reco = new Recognizer;
  }
  try {
    if (param.binaryRules) {
      // read recognizer in binary format
      AutomatonReader::changeable().readRecognizer(param.inputRulesFile,*reco);
    }
    else {
      // read recognizer in text format
      RecognizerCompiler compiler(param.inputRulesFile);
      compiler.buildRecognizer(*reco,Common::LinguisticData::LinguisticData::single().language(param.language));
    }
  }
  catch (exception& e) {
    std::cerr << "Error in recognizer construction: "
              << e.what() << endl;
    exit(1);
  }

  //reco.dumpSearchStructure();

//   std::cerr << reco << endl;

  try {
    applyRecognizerToFiles(reco, param.inputTextFiles);
  }
  catch (LinguisticProcessingException& e) { 
    std::cerr << "Error: " << e.what() << endl; 
  }

  // delete the recognizer
  delete reco;
  
  return EXIT_SUCCESS;
}
