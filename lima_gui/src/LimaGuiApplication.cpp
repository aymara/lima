/*
    Copyright 2017 CEA LIST

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
/**
 * \file    LimaGuiApplication.cpp
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 *
 */

#include "LimaGuiApplication.h"
#include "LimaGuiCommon.h"

#include "ConllParser.h"
#include "Threads.h"
#include "config/LimaConfiguration.h"
#include "tools/extract/FileTextExtractor.h"

#include <common/MediaticData/mediaticData.h>
#include <common/tools/FileUtils.h>
#include <common/XMLConfigurationFiles/configurationStructure.h>
#include <linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h>

#include <deque>
#include <iostream>
#include <string>
#include <cstdlib>

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>


using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing;
using namespace Lima::Gui::Config;
using namespace Lima::Gui::Tools;

namespace Lima
{
namespace Gui
{

LimaGuiApplication::LimaGuiApplication(const QCommandLineParser& options,
                                       QObject* parent) :
    QObject(parent),
    m_configuration(nullptr),
    m_options(options)
{
  LIMAGUILOGINIT;
  LDEBUG << "LimaGuiApplication::LimaGuiApplication";

  loadLimaConfigurations();

   auto ith = new InitializeThread(this);
   ith->start();

//  initializeLimaAnalyzer();
}

/// PUBLIC METHODS

QString qstr_parseFile(const std::string& path)
{
  QString tump;
  QFile file(QString(path.c_str()));
  if (file.open(QFile::ReadOnly))
  {
    QTextStream qts(&file);
    tump = qts.readAll();
    file.close();
  }
  else
  {
    LIMAGUILOGINIT;
    LINFO << "didn't open : " << path;
    LERROR << "Error opening file: " << strerror(errno);
  }
  return tump;
}

/// expecting the format : "file:<actual_url>"
std::string cleanUrl(const std::string& url)
{
  std::string str;
  std::vector<std::string> tmpStrList = split(url,':');
  if (!tmpStrList.size()) {
    LIMAGUILOGINIT;
    LERROR << "FILE DIALOG URL FORMAT ERROR : " << url;
  }
  else {
    str = tmpStrList[1];
  }

  return str;
}

QString cleanUrl(const QString& url)
{
  return QString(cleanUrl(url.toStdString()).c_str());
}

//////////////////////////////

bool LimaGuiApplication::openMultipleFiles(const QStringList& urls)
{
  bool result = false;
  for (const auto& url : urls) {
    result |= openFile(url);
  }
  return result;
}

bool LimaGuiApplication::openFile(const QString& filepath)
{
  LIMAGUILOGINIT;
  LINFO << ("OPENING FILE");
  /// FileDialog returns something like : "file:///C:/..."
  /// so we need to remove the unnecessary content
  std::vector<std::string> tmpStrList = split(filepath.toStdString(),':');
  if (!tmpStrList.size()) {
    LERROR << "FILE DIALOG URL FORMAT ERROR : " << filepath.toStdString();
    return false;
  }

  std::string path = tmpStrList[1];

  for (auto& file : m_openFiles)
  {
    if (file.url == path)
    {
      LINFO << ("This file is already open.");
      return false;
    }
  }

  tmpStrList = split(path, '/');
  if (!tmpStrList.size())
  {
    LERROR << "FILE NAME FORMAT ERROR : " << path;
    return false;
  }

  std::string filename = tmpStrList[tmpStrList.size() - 1];

  tmpStrList = split(filename, '.');

  std::string extension = tmpStrList[tmpStrList.size() - 1];

  if (!extractTextFromFile(path, m_fileContent, extension))
    return false;

//  if (tmpStrList.size()) {
//    if (extension == "txt") {
//    }
//    else if (extension == "docx") {
//      std::cout << extension << " : This type of file isn't handled by this application." << std::endl;
//      return false;
//    }
//    else if (extension == "") {
////      return PdfTextExtractor(path).text();
//      std::cout << extension << " : This type of file isn't handled by this application." << std::endl;
//      return false;
//    }
//    else
//      m_fileContent = qstr_parseFile(path);
//  }
//  else {
////     The file has no extension.
//    m_fileContent = qstr_parseFile(path);
//  }


  LimaGuiFile lgf;
  lgf.name = filename;
  lgf.url = path;
  lgf.text = m_fileContent.toStdString();

  m_openFiles.push_back(lgf);

  m_fileUrl = QString(path.c_str());
  m_fileName = QString(filename.c_str());

  /// qml part : open a new tab {title= m_fileName; content=m_fileContent}
  return true;
}

void LimaGuiApplication::closeFile(const QString& filename, bool save)
{
  if (save)
  {}
//  if (save) {
//    saveFile(filename);
//  }

  for (auto it = m_openFiles.begin(); it != m_openFiles.end(); ++it)
  {
    if (it->name == filename.toStdString())
    {
      m_openFiles.erase(it);
      return;
    }
  }

  LIMAGUILOGINIT;
  LERROR << "This file doesn't exist : " << filename.toStdString();
  /// qml part : close tab
}

LimaGuiFile* LimaGuiApplication::getFile(const std::string& filename)
{
  for (auto& file : m_openFiles)
  {
    if (file.name == filename)
    {
      return &file;
    }
  }
  return nullptr;
}


/// ANALYZER METHODS
///

void LimaGuiApplication::analyzeText(const QString& content, QObject* target)
{
  beginNewAnalysis(content, target);
}

void LimaGuiApplication::beginNewAnalysis(const QString& content, QObject* target)
{
  auto at = new AnalysisThread(this, content);
  if (target) at->setResultView(target);
  at->start();
}

void LimaGuiApplication::analyze(const QString& content)
{
  LIMAGUILOGINIT;

  // PARAMETERS :
  // Text

  // Metadata
  std::map<std::string, std::string> metaData;
  metaData["FileName"] = "";
  metaData["Lang"] = m_language.toStdString();
  LINFO << "lang=" << metaData["Lang"];

  // Handlers

  // we need to figure out what handlers to instantiate from the analyzer

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
  try 
  {
    LDEBUG << "LimaGuiApplication::analyze" << m_pipeline << content;
    m_analyzer->analyze(content, metaData, m_pipeline.toStdString(), handlers, inactiveUnits);
  }
  catch (const LinguisticProcessingException& e)
  {
    LIMAGUILOGINIT;
    LERROR << "LimaGuiApplication::analyze catched LinguisticProcessingException:"
            << e.what();
    *out << "Analyzer error:" << e.what() << std::endl;
  }

  if (simpleStreamHandler)
  {
    delete simpleStreamHandler;
  }
}

void LimaGuiApplication::analyzeFile(const QString& filename, QObject* target)
{
  if (filename != m_fileName)
  {
    if (!selectFile(filename))
    {
      return;
    }
  }
  analyzeText(m_fileContent, target);
}

void LimaGuiApplication::analyzeFileFromUrl(const QString& url, QObject* target) {
  if (openFile(url))
  {
    analyzeText(m_fileContent, target);
    closeFile(m_fileName);
  }
  else
  {
    LIMAGUILOGINIT;
    LERROR << ("Couldn't open file");
  }
}

bool LimaGuiApplication::selectFile(const QString& filename)
{
  LimaGuiFile* lgf = getFile(filename.toStdString());
  if (lgf)
  {
//    m_fileContent = qstr_parseFile(lgf->url);
    m_fileContent = QString(lgf->text.c_str());
    m_fileName = QString(lgf->name.c_str());
    m_fileUrl = QString(lgf->url.c_str());
    return true;
  }
  else
  {
    LIMAGUILOGINIT;
    LINFO << "This file doesn't exist : " << filename.toStdString();
  }
  return false;
}


/// INITIALIZATION METHODS

bool LimaGuiApplication::initializeLimaAnalyzer()
{

  LIMAGUILOGINIT;
  LDEBUG << "LimaGuiApplication::initializeLimaAnalyzer";
  QStringList projects;
  projects << QString("lima");

  QString lpConfigFile = m_options.value("lp-config-file");
  QString commonConfigFile = m_options.value("common-config-file");
  QString resourcesPath = m_options.value("resources-path");
  QString configPath = m_options.value("config-path");
  QString clientId = m_options.value("client");

  auto configFilePath = Misc::findFileInPaths(configPath, lpConfigFile, ':');

  Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig(
    configFilePath);

  std::deque<std::string> languages = lpconfig.getModuleGroupListValues(
                                              clientId.toStdString(),
                                              "mediaProcessingDefinitionFiles",
                                              "available");
  if (m_options.isSet("language"))
  {
    for(const auto& media: m_options.values("language"))
            languages.push_back(media.toUtf8().constData());
  }
  for (auto& l : languages)
  {
    m_languages << QString(l.c_str());
  }
  m_languages.removeDuplicates();
  if (!m_languages.isEmpty())
    m_language = m_languages[0];

  std::deque<std::string> pipelines;
  if (m_options.isSet("pipeline"))
  {
    for(const auto& pipeline: m_options.values("pipeline"))
      pipelines.push_back(pipeline.toUtf8().constData());
  }
  else
  {
    auto& pipelinesGroup = lpconfig.getModuleGroupConfiguration(
                                        clientId.toStdString(),
                                        "pipelines");
    auto& pipelinesMaps = pipelinesGroup.getMaps();
    for (auto it = pipelinesMaps.cbegin() ; it != pipelinesMaps.cend(); it++)
    {
      pipelines.push_back((*it).first);
    }
  }
  for (auto& p : pipelines)
  {
    m_pipelines << QString(p.c_str());
  }
  if (!m_pipelines.isEmpty())
    m_pipeline = m_pipelines[0];


  // initialize common
  LINFO << "Ressources path is " << resourcesPath;

  LINFO << "LOADING RESOURCES";

//  std::ostringstream oss;
//  std::ostream_iterator<std::string> out_it (oss,", ");
//  std::copy ( langs.begin(), langs.end(), out_it );
  Common::MediaticData::MediaticData::changeable().init(
    resourcesPath.toStdString(),
    configPath.toStdString(),
//    configDir,
    commonConfigFile.toStdString(),
        languages);


  // initialize linguistic processing

  LINFO << "LOADING CONFIGURATION FILES";
  try
  {
    LinguisticProcessingClientFactory::changeable().configureClientFactory(
        clientId.toStdString(),
        lpconfig,
        languages,
        pipelines);
  }
  catch (const Lima::InvalidConfiguration& e)
  {
    LIMAGUILOGINIT;
    QString errorMessage;
    QTextStream qts(&errorMessage);
    qts << e.what() << endl
            << "\tconfig file:" << lpConfigFile << endl
            << "\tcommon config file:" << commonConfigFile  << endl
            << "\tconfig path:" << configPath << endl
            << "\tclient id  :" << clientId;
    LERROR << errorMessage;
    Q_EMIT(error(tr("Invalid configuration"), errorMessage));
    return false;
  }
  m_analyzer = std::dynamic_pointer_cast<AbstractLinguisticProcessingClient>(
    LinguisticProcessingClientFactory::single().createClient(
      clientId.toStdString()));

  m_clients["default"] = m_analyzer;
  LINFO << "configureClientFactory DONE";

  return true;
}

bool LimaGuiApplication::resetLimaAnalyzer()
{
  // delete m_analzer;
  return initializeLimaAnalyzer();
}

void LimaGuiApplication::setTextBuffer(const std::string& str)
{
  m_text = QString::fromUtf8(str.c_str());
  textChanged();
}

void LimaGuiApplication::writeInConsole(const std::string& str)
{
  m_consoleOutput += QString(str.c_str());
}

void LimaGuiApplication::test() {
  LIMAGUILOGINIT;
  LDEBUG << "This is a warning";
  LDEBUG << "All your bases are belong to us";
}

/// BUFFERS ACCESSERS
LimaConfigurationSharedPtr LimaGuiApplication::configuration() const
{
  return m_configuration;
}

QString LimaGuiApplication::fileContent() const
{ return m_fileContent; }

QString LimaGuiApplication::fileName() const
{ return m_fileName; }

QString LimaGuiApplication::fileUrl() const
{ return m_fileUrl; }

QString LimaGuiApplication::text() const
{ return m_text; }

QString LimaGuiApplication::consoleOutput() const
{ return m_consoleOutput; }

QStringList LimaGuiApplication::languages() const
{ return m_languages; }

QString LimaGuiApplication::language() const
{ return m_language; }

QStringList LimaGuiApplication::pipelines() const
{ return m_pipelines; }

QString LimaGuiApplication::pipeline() const
{ return m_pipeline; }

void LimaGuiApplication::setFileContent(const QString& s)
{ m_fileContent = s; }

void LimaGuiApplication::setFileName(const QString& s)
{ m_fileName = s; }

void LimaGuiApplication::setFileUrl(const QString& s)
{ m_fileUrl = s; }

void LimaGuiApplication::setText(const QString& s)
{m_text = s; textChanged();}

void LimaGuiApplication::setConsoleOuput(const QString& s)
{ m_consoleOutput = s;}

void LimaGuiApplication::setLanguage(const QString& s)
{
  if (m_languages.contains(s))
  {
    m_language = s;
    languageChanged();
  }
  else
  {
    LIMAGUILOGINIT;
    LERROR << "'" << s.toStdString() << "' is not a supported language.";
  }
}

void LimaGuiApplication::setPipeline(const QString& s)
{
  if (m_pipelines.contains(s))
  {
    m_pipeline = s;
    pipelineChanged();
  }
  else
  {
    LIMAGUILOGINIT;
    LERROR << "'" << s.toStdString() << "' is not a supported pipeline.";
  }
}

void LimaGuiApplication::toggleAnalyzerState()
{
  m_analyzerAvailable = !m_analyzerAvailable;
  readyChanged();
}

void LimaGuiApplication::setAnalyzerState(bool bo)
{
  m_analyzerAvailable = bo;
  readyChanged();
}

bool LimaGuiApplication::available()
{
  return m_analyzerAvailable;
}

void LimaGuiApplication::registerQmlObject(QString s, QObject* qo)
{
  if (qo) {
    m_qmlObjects[s] = qo;
  }
}

QObject* LimaGuiApplication::getQmlObject(const QString& name)
{
  if (m_qmlObjects.find(name) != m_qmlObjects.end())
  {
    return m_qmlObjects[name];
  }
  else
  {
    return nullptr;
  }
}


//void LimaGuiApplication::generateAnalyzer(const LimaConfiguration& config) {
void LimaGuiApplication::configure()
{
  // @TODO use findFileInPAths LIMA_CONF and LIMA_RESOURCES can join several
  // paths
  QString configdir = qgetenv("LIMA_CONF");
  std::string resources = qgetenv("LIMA_RESOURCES").constData();

  std::deque<std::string> languages = {"fre", "eng"};
  std::deque<std::string> pipelines = {"main", "easy"};

  std::string commonConfigFile = "lima-common.xml";

  Lima::Common::MediaticData::MediaticData::changeable().init(
    resources,
    configdir.toUtf8().constData(),
    commonConfigFile,
    languages);

  std::string client = "lima-coreclient";
  QString lpConfigFile("lima-analysis.xml");
  XMLConfigurationFiles::XMLConfigurationFileParser lpconfig(
    configdir + "/" + lpConfigFile);

  LinguisticProcessingClientFactory::changeable().configureClientFactory(
    client,
    lpconfig,
    languages,
    pipelines);


}

QString LimaGuiApplication::highlightNamedEntities(const QString& text)
{
  // text is raw conll
//   auto entities = getNamedEntitiesFromConll(text.toStdString());
//  std::string result = highlightNamedEntities()
  return QString();
}

QStringList LimaGuiApplication::getNamedEntitiesList(const QString& text)
{
  QStringList nEntities;
  auto entities = getNamedEntitiesFromConll(text.toStdString());
  for (auto& pair : entities)
  {
    nEntities << QString(pair.first.c_str());
  }

  return nEntities;
}

void LimaGuiApplication::loadLimaConfigurations()
{
  LIMAGUILOGINIT;
  LDEBUG << "LimaGuiApplication::loadLimaConfigurations";

  QStringList configDirs = m_options.value("config-path").split(":");
  for (const auto& configDir: configDirs)
  {
    QFileInfoList list = QDir(configDir).entryInfoList(QStringList() << "*.xml",
                                                QDir::Files
                                                | QDir::NoDotAndDotDot
                                                | QDir::Readable);
    if (list.isEmpty())
    {
      LWARN << "LimaGuiApplication::loadLimaConfigurations No configuration file to load";
    }

    for (int i=0; i<list.size(); i++)
    {
      QFileInfo fileInfo = list.at(i);

      if (!fileInfo.isDir())
      {
        LDEBUG << "LimaGuiApplication::loadLimaConfigurations loading"
                << fileInfo.fileName();
        LimaConfigurationSharedPtr newconfig(new LimaConfiguration(fileInfo));
        m_configurations[newconfig->name()] = newconfig;
      }
      else
      {
        LDEBUG << "LimaGuiApplication::loadLimaConfigurations"
                << fileInfo.fileName() << "is a directory";
      }
    }
  }
}

void LimaGuiApplication::selectLimaConfiguration(const QString& name)
{
  if (m_configurations.find(name) != m_configurations.end())
  {
    m_configuration = m_configurations[name];
    return setLimaConfiguration(*m_configurations[name]);
  }
  else if (name == "default")
  {
    m_analyzer = m_clients["default"];
  }
  else {
    LIMAGUILOGINIT;
    LERROR << "There is no such configuration" << name;
  }
}

void LimaGuiApplication::setLimaConfiguration(const LimaConfiguration& config)
{
  /// This will create a new analyzer
  /// If an analyzer with the same name already exists, ask the user if it should reload it
  /// This kind of warning could toggled in a config file

  LIMAGUILOGINIT;

  if (m_clients.find(config.name()) != m_clients.end())
  {
    std::cout << "A client already exists for this configuration. Reload the configuration ? (y/n)" << std::endl;
    char c;
    std::cin >> c;
    if (c == 'n' || c == 'N') {
      return;
    }
  }

  /// Create a new client
  /// The thing is that we may need to create additional dumpers
  /// Need to know how to handle paths
  ///

  LDEBUG << "Creating " << config.name() << " client ...";

  LDEBUG << "(WIP)";

  // m_analyzer = m_clients[config.getName()];

}

void LimaGuiApplication::createLimaConfiguration(const LimaConfiguration& newconfig)
{
  LIMA_UNUSED(newconfig)
  /// This will take in a limaconfiguration created by the user
  /// and save it as files properly
  ///
  /// +
  /// add it to current configurations
  ///
//  if (created == success) {
//    m_configurations[newconfig.name()] = newconfig;
//  }

}

//void LimaGuiApplication::test() {
//  Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser fp()
//}

} // END namespace Gui
} // END namespace Lima

