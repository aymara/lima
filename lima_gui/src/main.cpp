/**
 * \author Jocelyn VERNAY
 * \file main.cpp
 * \project lima-qt-gui
 * \date 20-06-2017
 */

#include "common/tools/FileUtils.h"


#include "LimaGuiApplication.h"
#include "LimaConfiguration.h"
#include "ConllListModel.h"
#include "NamedEntitiesParser.h"

#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include "common/time/traceUtils.h"
#include "common/tools/FileUtils.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
 #include <QQmlContext>

#define qmlExpose

using namespace Lima::Common;
using namespace Lima::Gui;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    std::string configDir = qgetenv("LIMA_CONF").constData();
    if (configDir == "") {
      configDir = "/home/jocelyn/Lima/lima/../Dist/lima-gui/debug/share/config/lima";
    }
  //   configDir = configDir + "/../water" + ":" + configDir;
    LTELL("Config Dir is " << configDir);

    QStringList projects;
    projects << QString("lima");
    QStringList paths;
    paths << QString((configDir + "/../water").c_str());
    paths << QString(configDir.c_str());
  //  paths = Lima::Common::Misc::buildConfigurationDirectoriesList(projects, paths);

    QString concatenatedPaths;
    for (auto& qstr : paths) {
      if (concatenatedPaths.length()) concatenatedPaths += QString(":");
      concatenatedPaths += qstr;
    }

    LTELL("TRUE FINAL PATH: " << concatenatedPaths.toStdString());
    QString configPath = concatenatedPaths;


    //// INITIATING LIMA ////
//    QStringList configDirs = Misc::buildConfigurationDirectoriesList(QStringList() << "lima",QStringList());
//    QString configPath = configDirs.join(LIMA_PATH_SEPARATOR);
    
    QsLogging::initQsLog(configPath);
    Lima::AmosePluginsManager::single();
    Lima::AmosePluginsManager::changeable().loadPlugins(configPath);
    std::cerr << "Amose plugins initialized" << std::endl;
    QsLogging::initQsLog();
    

    /// REGISTERING QML Types from C++
    ///
    // https://stackoverflow.com/questions/9500280/access-c-function-from-qml
    // qmlRegisterType<Writer>("integ_cpp", 1, 0, "Writer");
    qmlRegisterType<LimaGuiApplication>("integ_cpp", 1, 0, "LimaGuiApplication");
    qmlRegisterType<LimaConfiguration>("integ_cpp", 1, 0, "LimaConfiguration");
    qmlRegisterType<LimaConfigurationNodeModel>("integ_cpp", 1, 0, "LimaConfigurationNodeModel");
    qmlRegisterType<ConllListModel>("integ_cpp", 1, 0, "ConllListModel");
    qmlRegisterType<NamedEntitiesParser>("integ_cpp", 1, 0, "NamedEntitiesParser");
    ////


    QQmlApplicationEngine engine;

    // https://stackoverflow.com/questions/19516851/using-qqmlcontextsetcontextobject-to-make-a-c-object-visible-to-qml
    //new QQmlContext(engine.rootContext());

    LimaGuiApplication lga;



    /// we add the app as a context property so that it can be accessed from anywhere,
    /// without instantiating in QML

    engine.rootContext()->setContextProperty("textAnalyzer", &lga);

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    int result = app.exec();
    return result;

//  LimaConfiguration lc;
//  return 0;
}
