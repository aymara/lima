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

#include "LimaGuiApplication.h"
#include "Writer.h"

#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include "common/time/traceUtils.h"
#include "common/tools/FileUtils.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
 #include <QQmlContext>

using namespace Lima::Common;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    //// LIMA ////
    QStringList configDirs = Misc::buildConfigurationDirectoriesList(QStringList() << "lima",QStringList());
    QString configPath = configDirs.join(LIMA_PATH_SEPARATOR);
    
    QsLogging::initQsLog(configPath);
    Lima::AmosePluginsManager::single();
    Lima::AmosePluginsManager::changeable().loadPlugins(configPath);
    std::cerr << "Amose plugins initialized" << std::endl;
    QsLogging::initQsLog();
    
    // https://stackoverflow.com/questions/9500280/access-c-function-from-qml
    qmlRegisterType<Writer>("integ_cpp", 1, 0, "Writer");
    qmlRegisterType<LimaGuiApplication>("integ_cpp", 1, 0, "LimaGuiApplication");
    ////


    QQmlApplicationEngine engine;

    // https://stackoverflow.com/questions/19516851/using-qqmlcontextsetcontextobject-to-make-a-c-object-visible-to-qml
    //new QQmlContext(engine.rootContext());

    LimaGuiApplication lga;

    engine.rootContext()->setContextProperty("textAnalyzer", &lga);

    /// By loading the lga here rather than in the qml :
    /// use it as a context property, which would have enabled it to
    /// be referenced directly by other qml objects and have these to be updated
    /// when lga's concerned property was updated :
    ///
    /// c++: m_text = "hello"
    /// qml: Text{ text: textAnalyzer.text}
    ///
    /// Twould then update the Text.text just by altering the member in C++
    /// But it doesn't seem to work
    /// ...
    /// just look up 'property binding'
    /// http://doc.qt.io/qt-5/qtqml-cppintegration-exposecppattributes.html
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    int result = app.exec();
    return result;
}
