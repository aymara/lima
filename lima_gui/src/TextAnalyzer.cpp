/// This sould include LIMA at some point

#include "TextAnalyzer.h"
#include "ConllParser.h"

#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"

#include <deque>
#include <iostream>
#include <string>

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing;

TextAnalyzer::TextAnalyzer(QObject* p) : QObject(p)
{
  //initializeAnalyzer();
}

void TextAnalyzer::initializeAnalyzer() {
  std::string configDir = qgetenv("LIMA_CONF").constData();
  LTELL("Config Dir is " << configDir);
  if (configDir == "") {
    configDir = "/home/jocelyn/Lima/lima/../Dist/lima-gui/debug/share/config/lima";
  }
  
  std::deque<std::string> langs = {"eng","fre"};
  std::deque<std::string> pipelines = {"main"};
  
  // initialize common
  std::string resourcesPath = qgetenv("LIMA_RESOURCES").constData();
  if( resourcesPath.empty() )
    resourcesPath = "/usr/share/apps/lima/resources/";
  std::string commonConfigFile("lima-common.xml");
  
  std::ostringstream oss;
  std::ostream_iterator<std::string> out_it (oss,", ");
  std::copy ( langs.begin(), langs.end(), out_it );
  Common::MediaticData::MediaticData::changeable().init(
    resourcesPath,
    configDir,
    commonConfigFile,
    langs);
  
  
  std::cout << "Langs:" << std::endl;
  for (unsigned int i = 0;i < langs.size(); i++) {
    std::cout << langs[i] << std::endl;
  }
  
  // initialize linguistic processing
  std::string clientId("lima-coreclient");
  std::string lpConfigFile("lima-analysis.xml");
  Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig(configDir + "/" + lpConfigFile);
  
  LinguisticProcessingClientFactory::changeable().configureClientFactory(
    clientId,
    lpconfig,
    langs,
    pipelines);
  
  m_analyzer = std::dynamic_pointer_cast<AbstractLinguisticProcessingClient>(LinguisticProcessingClientFactory::single().createClient(clientId)); 
  
  std::cout << "Pipelines:" << std::endl;
  for (unsigned int i=0; i < pipelines.size(); i++) {
    std::cout << pipelines[i] << std::endl;
  }
}

void TextAnalyzer::resetAnalyzer() {
  //
  initializeAnalyzer();
}

void TextAnalyzer::analyzeText() {
  std::cout << m_text.toStdString() << std::endl;
//   m_analyzer->analyze()
}

void TextAnalyzer::analyzeFile() {
  std::string filepath = split(m_file.toStdString(),':')[1];
  CONLL_List data = ConllParser::getConllData(filepath);
  ConllParser::show_dependencies(data);
}

/////////////// EXPERIMENTS
/////////////// 
/////////////// 
/////////////// 

void TextAnalyzer::tr_analyzeFile() {
  std::cout << "textanalyzer::tr_analyze" << std::endl;
  
  std::string filepath = m_file.toStdString();

  // PARAMETERS :
  // Text
  std::vector<std::string> contentText = {"Autant en emporte le vent."};// = parseFile(filepath);
  
  // Metadata
  std::map<std::string, std::string> metaData;
  metaData["FileName"] = filepath;
  metaData["Lang"] = "fre";
  
  // Pipeline
  std::string pipeline = "main";
  
  // Handlers 
  std::set<std::string> dumpers;
  
  dumpers.insert("text");
  
  std::map<std::string, AbstractAnalysisHandler*> handlers;
  
  SimpleStreamHandler* simpleStreamHandler = 0;
  
  if (dumpers.find("text") != dumpers.end())
  {
    simpleStreamHandler = new SimpleStreamHandler();
    simpleStreamHandler->setOut(&std::cout);
    handlers.insert(std::make_pair("simpleStreamHandler", simpleStreamHandler));
  }
  
  std::set<std::string> inactiveUnits;
  
  m_analyzer->analyze(QString::fromUtf8(contentText[0].c_str()), metaData, pipeline, handlers, inactiveUnits);
  
  if (simpleStreamHandler)
    delete simpleStreamHandler;
  
}
/*
void TextAnalyzer::freeAll(std:::map<std::string, AbstractAnalysisHandler*>& handlers) {
  
}*/
