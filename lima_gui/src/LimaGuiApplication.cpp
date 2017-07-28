
#include "LimaGuiApplication.h"
#include "ConllParser.h"
#include "Threads.h"

#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
//#include "XMLConfigurationFiles/configurationStructure.h"

#include <deque>
#include <iostream>
#include <string>

#include <QFile>

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing;

namespace Lima {
namespace Gui {


LimaGuiApplication::LimaGuiApplication(QObject* parent) : QObject(parent) {

   auto ith = new InitializeThread(this);
   ith->start();
}

/// PUBLIC METHODS

QString qstr_parseFile(const std::string& path) {
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
std::string cleanUrl(const std::string& url) {
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

QString cleanUrl(const QString& url) {
  return QString(cleanUrl(url.toStdString()).c_str());
}

//////////////////////////////

bool LimaGuiApplication::openMultipleFiles(const QStringList& urls) {
  bool result = false;
  for (const auto& url : urls) {
    result |= openFile(url);
  }
  return result;
}

bool LimaGuiApplication::openFile(const QString& filepath) {
  LTELL("OPENING FILE");
  /// FileDialog returns something like : "file:///C:/..."
  /// so we need to remove the unnecessary content
  std::vector<std::string> tmpStrList = split(filepath.toStdString(),':');
  if (!tmpStrList.size()) {
    LTELL("FILE DIALOG URL FORMAT ERROR : " << filepath.toStdString());
    return false;
  }
  
  std::string path = tmpStrList[1];
  
  for (auto& file : m_openFiles) {
    if (file.url == path) {
      LTELL("This file is already open.");
      return false;
    }
  }

  tmpStrList = split(path, '/');
  if (!tmpStrList.size()) {
    LTELL("FILE NAME FORMAT ERROR : " << path);
    return false;
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
  return true;
}

void LimaGuiApplication::closeFile(const QString& filename, bool save) {
  
//  if (save) {
//    saveFile(filename);
//  }
  
  for (std::vector<LimaGuiFile>::const_iterator it = m_openFiles.begin(); it != m_openFiles.end(); ++it) {
    if (it->name == filename.toStdString()) {
      m_openFiles.erase(it);
      return;
    }
  }
  
  LTELL("This file doesn't exist : " << filename.toStdString());
  /// qml part : close tab
}

LimaGuiFile* LimaGuiApplication::getFile(const std::string& filename) {
  for (auto& file : m_openFiles) {
    if (file.name == filename) {
      return &file;
    }
  }
  return nullptr;
}


/// ANALYZER METHODS
///

void LimaGuiApplication::analyzeText(const QString& content, QObject* target) {
  beginNewAnalysis(content, target);
}

void LimaGuiApplication::beginNewAnalysis(const QString& content, QObject* target) {
//  LTELL("ANALYYZING :");
//  LTELL(content.toStdString());
  auto at = new AnalysisThread(this, content);
  if (target) at->setResultView(target);
  at->start();
}

void LimaGuiApplication::analyze(const QString& content) {
  
  // PARAMETERS :
  // Text
 
  // Metadata
  std::map<std::string, std::string> metaData;
  metaData["FileName"] = "";
  metaData["Lang"] = m_language.toStdString();
  LTELL("lang=" << metaData["Lang"]);
  
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
    simpleStreamHandler->setOut(out);
    handlers.insert(std::make_pair("simpleStreamHandler", simpleStreamHandler));
  }
  
  std::set<std::string> inactiveUnits;
  // QString::fromUtf8(contentText.c_str())
  m_analyzer->analyze(content, metaData, pipeline, handlers, inactiveUnits);
  
  if (simpleStreamHandler)
    delete simpleStreamHandler;
}

void LimaGuiApplication::analyzeFile(const QString& filename, QObject* target) {
  if (filename != m_fileName) {
    if (!selectFile(filename)) {
      return;
    }
  }
  analyzeText(m_fileContent, target);
}

void LimaGuiApplication::analyzeFileFromUrl(const QString& url, QObject* target) {
  if (openFile(url)) {
    analyzeText(m_fileContent, target);
    closeFile(m_fileName);
  }
  else {
    LTELL("There was a problem somewhere");
  }
}

bool LimaGuiApplication::selectFile(const QString& filename) {
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
//  configDir += "/../custom";
  
  std::deque<std::string> langs = {"eng","fre"};
  std::deque<std::string> pipelines = {"main", "easy"};
  
  // initialize common
  std::string resourcesPath = qgetenv("LIMA_RESOURCES").constData();
  if( resourcesPath.empty() )
    resourcesPath = "/usr/share/apps/lima/resources/";
  std::string commonConfigFile("lima-common.xml");
  
//  std::ostringstream oss;
//  std::ostream_iterator<std::string> out_it (oss,", ");
//  std::copy ( langs.begin(), langs.end(), out_it );
  Common::MediaticData::MediaticData::changeable().init(
    resourcesPath,
    configDir,
    commonConfigFile,
    langs);
  
  for (auto& l : langs) {
    m_languages << QString(l.c_str());
  }
  m_language = "fre";
  
  // initialize linguistic processing
  std::string clientId("lima-coreclient");
  std::string lpConfigFile("lima-analysis.xml");
  Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig(configDir + "/" + lpConfigFile);
  
  /// HERE TO EXPLORE  //////////////////////////////////////////
  ///

//  ConfigurationStructure& cstruct = lpconfig.getConfiguration();

//  for (auto& pair : cstruct) {

//    std::cout << "[" << pair.first << "]" << std::endl;

//    for (auto& mpair : pair.second) {
//        std::cout << "\t(" << mpair.first << ")" << std::endl;


//    }

//  }

  //////////////////////////////////////////////////////////////

  LinguisticProcessingClientFactory::changeable().configureClientFactory(
    clientId,
    lpconfig,
    langs,
    pipelines);

  m_analyzer = std::dynamic_pointer_cast<AbstractLinguisticProcessingClient>(LinguisticProcessingClientFactory::single().createClient(clientId));

//  std::string anotherClientId("lima-sideclient");

//  LinguisticProcessingClientFactory::changeable().configureClientFactory(
//    anotherClientId,
//    lpconfig,
//    langs,
//    pipelines);
  

//  std::cout << "Pipelines:" << std::endl;
//  for (unsigned int i=0; i < pipelines.size(); i++) {
//    std::cout << pipelines[i] << std::endl;
//  }
}

void LimaGuiApplication::resetLimaAnalyzer() {
  // delete m_analzer;
  initializeLimaAnalyzer();
}

void LimaGuiApplication::setTextBuffer(const std::string& str) {
  m_text = QString::fromUtf8(str.c_str());
  textChanged();
}

void LimaGuiApplication::writeInConsole(const std::string& str) {
  m_consoleOutput += QString(str.c_str());
}

void LimaGuiApplication::test() {
  LTELL("This is a warning");
  std::cout << "All your bases are belong to us" << std::endl;
}

/// BUFFERS ACCESSERS

QString LimaGuiApplication::fileContent() const { return m_fileContent; }
QString LimaGuiApplication::fileName() const { return m_fileName; }
QString LimaGuiApplication::fileUrl() const { return m_fileUrl; }
QString LimaGuiApplication::text() const { return m_text; }
QString LimaGuiApplication::consoleOutput() const { return m_consoleOutput; }
QStringList LimaGuiApplication::languages() const { return m_languages; }
QString LimaGuiApplication::language() const { return m_language; }

void LimaGuiApplication::setFileContent(const QString& s) { m_fileContent = s; }
void LimaGuiApplication::setFileName(const QString& s) { m_fileName = s; }
void LimaGuiApplication::setFileUrl(const QString& s) { m_fileUrl = s; }
void LimaGuiApplication::setText(const QString& s) {m_text = s; textChanged();}
void LimaGuiApplication::setConsoleOuput(const QString& s) { m_consoleOutput = s;}

void LimaGuiApplication::setLanguage(const QString& s) {
  if (m_languages.contains(s)) {
    m_language = s;
    languageChanged();
  }
  else {
    LTELL("'" << s.toStdString() << "' is not a supported language.");
  }
}

void LimaGuiApplication::toggleAnalyzerState() {
  m_analyzerAvailable = !m_analyzerAvailable;
  readyChanged();
}

void LimaGuiApplication::setAnalyzerState(bool bo) {
  m_analyzerAvailable = bo;
  readyChanged();
}

bool LimaGuiApplication::available() {
  return m_analyzerAvailable;
}

void LimaGuiApplication::registerQmlObject(QString s, QObject* qo) {
  if (qo) {
    qml_objects[s] = qo;
  }
}

QObject* LimaGuiApplication::getQmlObject(const QString& name) {
  if (qml_objects.find(name) != qml_objects.end()) {
    return qml_objects[name];
  }
  else {
    return nullptr;
  }
}


//void LimaGuiApplication::generateAnalyzer(const LimaConfiguration& config) {
void LimaGuiApplication::configure() {
  std::string configdir = qgetenv("LIMA_CONF").constData();
  std::string resources = qgetenv("LIMA_RESOURCES").constData();

  std::deque<std::string> languages = {"fre", "eng"};
  std::deque<std::string> pipelines = {"main", "easy"};

  std::string commonConfigFile = "lima-common.xml";

  MediaticData::MediaticData::changeable().init(resources, configdir, commonConfigFile, languages);

  std::string client = "lima-coreclient";
  std::string lpConfigFile("lima-analysis.xml");
  XMLConfigurationFiles::XMLConfigurationFileParser lpconfig(configdir + "/" + lpConfigFile);

  LinguisticProcessingClientFactory::changeable().configureClientFactory(client, lpconfig, languages, pipelines);


}

QString LimaGuiApplication::highlightNamedEntities(const QString& text) {
  // text is raw conll

  std::map<std::string, std::vector<std::string> > entities = getNamedEntitiesFromConll(text.toStdString());
//  std::string result = highlightNamedEntities()
  return QString();
}

QStringList LimaGuiApplication::getNamedEntitiesList(const QString& text) {
  QStringList nEntities;
  std::map<std::string, std::vector<std::string> > entities = getNamedEntitiesFromConll(text.toStdString());
  for (auto& pair : entities) {
    nEntities << QString(pair.first.c_str());// << ":" <<
  }

  return nEntities;
}


} // END namespace Gui
} // END namespace Lima

