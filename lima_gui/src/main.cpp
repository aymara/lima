// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 * \author Jocelyn VERNAY
 * \file main.cpp
 * \project lima-qt-gui
 * \date 20-06-2017
 */

#include "common/tools/FileUtils.h"

#include "LimaGuiApplication.h"
#include "config/LimaConfiguration.h"
#include "config/ConfigurationTree.h"
#include "config/ConfigurationTreeModel.h"
#include "ConllListModel.h"
#include "NamedEntitiesParser.h"

#include "common/LimaVersion.h"
#include "common/LimaCommon.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include "common/tools/FileUtils.h"

#ifdef DEBUG_LIMA_GUI
  #ifndef QT_QML_DEBUG
    #define QT_QML_DEBUG
  #endif
#endif
#include <QtQuick>
#include <QApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtGlobal>

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::Misc;
using namespace Lima::Gui;
using namespace Lima::Gui::Config;
using namespace Lima::LinguisticProcessing;

#define QML_EXPORT_PACKAGE "integ_cpp"
#define QML_REGISTER(type) qmlRegisterType<type>(QML_EXPORT_PACKAGE, \
                                                  1, 0, #type)

int main(int argc, char *argv[])
{
#ifdef DEBUG_LIMA_GUI
  qputenv("QT_LOGGING_RULES", "*=true");
#else
  qputenv("QT_LOGGING_RULES", "*=false");
#endif
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("LIMA");
  QCoreApplication::setOrganizationDomain("lima.org");
  QCoreApplication::setApplicationName("LIMA GUI");

  QSettings settings;
  QFileInfo settingsFile(settings.fileName());
  QString LIMA_USER_CONFIG(settingsFile.absoluteDir().absolutePath()+"/configs");
  QDir configDir(LIMA_USER_CONFIG);
  if (!configDir.exists())
  {
    configDir.mkpath(LIMA_USER_CONFIG);
  }

  QStringList configDirs = buildConfigurationDirectoriesList(QStringList({"lima"}),
                                                             QStringList()
                                                              << LIMA_USER_CONFIG);
  QString configPath = configDirs.join(LIMA_PATH_SEPARATOR);

  QStringList resourcesDirs = buildResourcesDirectoriesList(QStringList({"lima"}),
                                                            QStringList());
  QString resourcesPath = resourcesDirs.join(LIMA_PATH_SEPARATOR);

  QCommandLineParser parser;
  parser.setApplicationDescription("LIMA Graphical User Interface");
  parser.addHelpOption();
  parser.addVersionOption();
  QCommandLineOption lpConfigFileOption(
      QStringList() << "lp-config-file",
      QCoreApplication::translate("main", "The main media analysis configuration file"),
      "lpConfigFile", "lima-analysis.xml");
  if (!parser.addOption(lpConfigFileOption))
  {
    std::cerr << "Error adding lpConfigFile option" << std::endl;
    return 1;
  }
  QCommandLineOption commonConfigFileOption(
      QStringList() << "common-config-file",
      QCoreApplication::translate("main", "The main common configuration file"),
      "commonConfigFile", "lima-common.xml");
  if (!parser.addOption(commonConfigFileOption))
  {
    std::cerr << "Error adding commonConfigFileOption option" << std::endl;
    return 1;
  }
  QCommandLineOption configPathOption(
      QStringList() << "config-dir" << "config-path",
      QCoreApplication::translate("main", "The colon-separated paths to configuration files"),
      "configPath", configPath);
  if (!parser.addOption(configPathOption))
  {
    std::cerr << "Error adding configPathOption option" << std::endl;
    return 1;
  }
  QCommandLineOption resourcesPathOption(
      QStringList() << "resources-dir" << "resources-path",
      QCoreApplication::translate("main", "The colon-separated paths to media resources"),
      "resourcesPath", resourcesPath);
  if (!parser.addOption(resourcesPathOption))
  {
    std::cerr << "Error adding resourcesPathOption option" << std::endl;
    return 1;
  }
  QCommandLineOption mediasOption(
      QStringList() << "l" << "language" << "m" << "media",
      QCoreApplication::translate("main",
          "A media (or language) to activate. Must be repeated for each of them. The\n"
          "first encountered value of this option will be the default one when a tag\n"
          "in an analyzed file is not associated to any media in the configuration."),
          "media");
  if (!parser.addOption(mediasOption))
  {
    std::cerr << "Error adding mediasOption option" << std::endl;
    return 1;
  }
  QCommandLineOption pipelineOption(
      QStringList() << "p" << "pipeline",
      QCoreApplication::translate("main", "The analysis pipeline to use."),
      "pipeline", "main");
  if (!parser.addOption(pipelineOption))
  {
    std::cerr << "Error adding pipelineOption option" << std::endl;
    return 1;
  }
  QCommandLineOption clientOption(
      QStringList() << "c" << "client",
      QCoreApplication::translate("main", "The analysis client to use."),
      "client", "lima-coreclient");
  if (!parser.addOption(clientOption))
  {
    std::cerr << "Error adding clientOption option" << std::endl;
    return 1;
  }
  parser.process(QCoreApplication::arguments());

  if (parser.isSet(configPathOption))
    configPath = parser.value(configPathOption);

  QsLogging::initQsLog(configPath);
  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();
  Lima::AmosePluginsManager::changeable().loadPlugins(configPath);

  LOGINIT("Lima::Gui");
  LINFO << "Config path is " << configPath;
  LDEBUG << "Options set are" << parser.optionNames();

  QML_REGISTER(LimaConfiguration);
  QML_REGISTER(ConfigurationTreeModel);
  QML_REGISTER(ConllListModel);
  QML_REGISTER(NamedEntitiesParser);

  QQmlApplicationEngine engine;

  LimaGuiApplication lga(parser);
  lga.selectLimaConfiguration("lima-lp-eng.xml");
  LimaConfigurationSharedPtr configuration = lga.configuration();
  ConfigurationNode* root = new ConfigurationNode(configuration->configuration());
  ConfigurationTree* tree = new ConfigurationTree(root);
  ConfigurationTreeModel* treeModel = new ConfigurationTreeModel(*tree);
  engine.rootContext()->setContextProperty("configurationModel", treeModel);

  /// we add the app as a context property so that it can be accessed from anywhere,
  /// without instantiating in QML
  engine.rootContext()->setContextProperty("textAnalyzer", &lga);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
  engine.load(QUrl(QStringLiteral("qrc:///qml/main.qml")));
#else
  engine.load(QUrl(QStringLiteral("qrc:///qml-old/main.qml")));
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
  app.setWindowIcon(QIcon(":qml/resources/lima.png"));
#endif
  int result = app.exec();
  return result;

//  LimaConfiguration lc;
//  return 0;
}
