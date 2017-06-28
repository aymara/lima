
#include "LimaGuiApplication.h"
#include "ConllParser.h"

#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"

#include <deque>
#include <iostream>
#include <string>

#include <QFile>

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing;

LimaGuiApplication::LimaGuiApplication(QObject* parent) : QObject(parent) {
  //initializeLimaAnalyzer();
}

/// PUBLIC METHODS

QString qstr_parseFile(std::string path) {
  QString tump;
  QFile file(QString(path.c_str()));
  if (file.open(QFile::ReadOnly)) {
    QTextStream qts(&file);
    tump = qts.readAll();
    file.close();
  }
  else {
    std::cout << "didn't open : " << path << std::endl;
    std::cout << "Error opening file: " << strerror(errno) << std::endl;
  }
  return tump;
}

/// expecting the format : "file:<actual_url>"
std::string cleanUrl(std::string url) {
  std::string str;
  std::vector<std::string> tmpStrList = split(url,':');
  if (!tmpStrList.size()) {
    LTELL("FILE DIALOG URL FORMAT ERROR : " << url);
  }
  else {
    str = tmpStrList[1];
  }
  
  return str;
}

QString cleanUrl(QString url) {
  return QString(cleanUrl(url.toStdString()).c_str());
}

//////////////////////////////
//////////////////////////////
//////////////////////////////
//////////////////////////////

bool LimaGuiApplication::openMultipleFiles(QStringList urls) {
  bool result = false;
  for (const auto& url : urls) {
    result |= openFile(url);
  }
  return result;
}

bool LimaGuiApplication::openFile(QString filepath) {
  /// FileDialog returns something like : "file:///C:/..."
  /// so we need to remove the unnecessary content
  std::vector<std::string> tmpStrList = split(filepath.toStdString(),':');
  if (!tmpStrList.size()) {
    LTELL("FILE DIALOG URL FORMAT ERROR : " << filepath.toStdString());
  }
  
  std::string path = tmpStrList[1];
  
  tmpStrList = split(path, '/');
  if (!tmpStrList.size()) {
    LTELL("FILE NAME FORMAT ERROR : " << path);
  }
  
  std::string filename = tmpStrList[tmpStrList.size() - 1];
  
  m_fileContent = qstr_parseFile(path);
  
  LimaGuiFile lgf;
  lgf.name = filename;
  lgf.url = path;
  
  m_openFiles.push_back(lgf);
  
  m_fileUrl = QString(path.c_str());
  m_fileName = QString(filename.c_str());
  
  /// qml part : open a new tab {title= m_fileName; content=m_fileContent}
  
}

bool LimaGuiApplication::saveFile(QString filename) {
  
  LimaGuiFile* lgfile = getFile(filename.toStdString());
  
  if (!lgfile) {
    LTELL("This file doesn't exist : " << filename.toStdString());
    return false;
  }
  
  QFile file(QString(lgfile->url.c_str()));
  if (file.open(QFile::WriteOnly | QFile::Text)) {
    QTextStream qts(&file);
    qts << m_fileContent;
    file.close();
  }
  else {
    std::cout << "didn't open : " << lgfile->url << std::endl;
    std::cout << "Error opening file: " << strerror(errno) << std::endl;
  }
}

bool LimaGuiApplication::saveFileAs(QString filename, QString newUrl) {
  /// check if file is open
  LimaGuiFile* lgfile = getFile(filename.toStdString());
  
  if (!lgfile) {
    LTELL("This file doesn't exist : " << filename.toStdString());
    return false;
  }
  
  /// check new url
  std::vector<std::string> tmpStrList = split(newUrl.toStdString(),':');
  if (!tmpStrList.size()) {
    LTELL("FILE DIALOG URL FORMAT ERROR : " << newUrl.toStdString());
  }
  
  std::string path = tmpStrList[1];
  
  QFile file(newUrl);
  if (file.open(QFile::WriteOnly | QFile::Text)) {
    QTextStream qts(&file);
    qts << m_fileContent;
    file.close();
  }
  else {
    std::cout << "didn't open : " << newUrl << std::endl;
    std::cout << "Error opening file: " << strerror(errno) << std::endl;
  }
}

void LimaGuiApplication::closeFile(QString filename, bool save) {
  
  if (save) {
    saveFile(filename);
  }  
  
  for (std::vector<LimaGuiFile>::const_iterator it = m_openFiles.begin(); it != m_openFiles.end(); ++it) {
    if (it->name == filename.toStdString()) {
      m_openFiles.erase(it);
      return;
    }
  }
  
  LTELL("This file doesn't exist : " << filename.toStdString());
  /// qml part : close tab
}

LimaGuiFile* LimaGuiApplication::getFile(std::string filename) {
  for (auto& file : m_openFiles) {
    if (file.name == filename) {
      return &file;
    }
  }
  return nullptr;
}


/// ANALYZER METHODS

void LimaGuiApplication::analyzeText(QString content) {
  
  // PARAMETERS :
  // Text
 
  // Metadata
  std::map<std::string, std::string> metaData;
  metaData["FileName"] = "";
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
  // QString::fromUtf8(contentText.c_str())
  m_analyzer->analyze(content, metaData, pipeline, handlers, inactiveUnits);
  
  if (simpleStreamHandler)
    delete simpleStreamHandler;
}

void LimaGuiApplication::analyzeFile(QString filename) {
  if (filename != m_fileName) {
    if (!selectFile(filename)) {
      return;
    }
  }
  analyzeText(m_fileContent);
}

void LimaGuiApplication::analyzeFileFromUrl(QString url) {
  if (openFile(url)) {
    analyzeText(m_fileContent);
    closeFile(m_fileName);
  }
}

bool LimaGuiApplication::selectFile(QString filename) {
  LimaGuiFile* lgf = getFile(filename.toStdString());
  if (lgf) {
    m_fileContent = qstr_parseFile(lgf->url);
    m_fileName = QString(lgf->name.c_str());
    m_fileUrl = QString(lgf->url.c_str());
    return true;
  }
  else {
    LTELL("This file doesn't exist : " << filename.toStdString());
  }
  return false;
}


/// INITIALIZATION METHODS

void LimaGuiApplication::initializeLimaAnalyzer() {
  
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

void LimaGuiApplication::resetLimaAnalyzer() {
  // delete m_analzer;
  initializeLimaAnalyzer();
}
