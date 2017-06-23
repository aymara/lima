#include "../../lima_common/src/common/tools/FileUtils.h"
#include <../../../master/debug/include/common/tools/FileUtils.h>
#include <../../../master/debug/include/common/tools/FileUtils.h>
#include <../../../master/debug/include/common/tools/FileUtils.h>
#include "../../lima_common/src/common/tools/FileUtils.h"
#include "../../lima_common/src/common/tools/FileUtils.h"
#include "../../lima_common/src/common/tools/FileUtils.h"
/**
 * \author Jocelyn VERNAY
 * \file main.cpp
 * \project lima-qt-gui
 * \date 20-06-2017 
 */

#include "Writer.h"
#include "TextAnalyzer.h"

#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include "common/time/traceUtils.h"
#include "common/tools/FileUtils.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>

using namespace Lima::Common;

int main(int argc, char *argv[])
{
    
  
  
    QGuiApplication app(argc, argv);
  
    
    QStringList configDirs = Misc::buildConfigurationDirectoriesList(QStringList() << "lima",QStringList());
    QString configPath = configDirs.join(LIMA_PATH_SEPARATOR);
    
    QsLogging::initQsLog(configPath);
    Lima::AmosePluginsManager::single();
    Lima::AmosePluginsManager::changeable().loadPlugins(configPath);
    std::cerr << "Amose plugins initialized" << std::endl;
    QsLogging::initQsLog();
    
    // https://stackoverflow.com/questions/9500280/access-c-function-from-qml
    qmlRegisterType<Writer>("integ_cpp", 1, 0, "Writer");
    qmlRegisterType<TextAnalyzer>("integ_cpp", 1, 0, "TextAnalyzer");
    
    QQmlApplicationEngine engine(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    return app.exec();
}
