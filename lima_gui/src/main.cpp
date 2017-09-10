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
#include "common/time/traceUtils.h"
#include "common/tools/FileUtils.h"

#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>

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
    QGuiApplication app(argc, argv);
//     view.setWindowIcon(QIcon("qrc:qml/resources/backup_btnclose_h.png")‌​); 
    QStringList configDirs = buildConfigurationDirectoriesList(QStringList() 
                                                                  << "lima", 
                                                               QStringList());
    QString configPath = configDirs.join(LIMA_PATH_SEPARATOR);

    QStringList resourcesDirs = buildResourcesDirectoriesList(QStringList() 
                                                                << "lima",
                                                              QStringList());
    QString resourcesPath = resourcesDirs.join(LIMA_PATH_SEPARATOR);

    QsLogging::initQsLog(configPath);
    // Necessary to initialize factories
    Lima::AmosePluginsManager::single();
    Lima::AmosePluginsManager::changeable().loadPlugins(configPath);

    LOGINIT("Lima::Gui");
    LINFO << "Config path is " << configPath;

    
    QML_REGISTER(LimaConfiguration);
    QML_REGISTER(ConfigurationTreeModel);
    QML_REGISTER(ConllListModel);
    QML_REGISTER(NamedEntitiesParser);

    QQmlApplicationEngine engine;

    LimaGuiApplication lga;
//     lga.selectLimaConfiguration("lima-lp-eng.xml");
//     LimaConfigurationSharedPtr configuration = lga.configuration();
//     ConfigurationNode* root = new ConfigurationNode(configuration->configuration());
//     ConfigurationTree* tree = new ConfigurationTree(root);
//     ConfigurationTreeModel* treeModel = new ConfigurationTreeModel(*tree);
//     engine.rootContext()->setContextProperty("configurationModel", treeModel);

    /// we add the app as a context property so that it can be accessed from anywhere,
    /// without instantiating in QML
    engine.rootContext()->setContextProperty("textAnalyzer", &lga);

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    app.setWindowIcon(QIcon(":qml/resources/lima_gui.png"));
    int result = app.exec();
    return result;

//  LimaConfiguration lc;
//  return 0;
}
