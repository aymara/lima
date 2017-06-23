/**
 * \author Jocelyn VERNAY
 * \file main.cpp
 * \project lima-qt-gui
 * \date 20-06-2017 
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "Writer.h"
#include "TextAnalyzer.h"

int main(int argc, char *argv[])
{
    
    
  
    QGuiApplication app(argc, argv);
    // https://stackoverflow.com/questions/9500280/access-c-function-from-qml
    qmlRegisterType<Writer>("integ_cpp", 1, 0, "Writer");
    qmlRegisterType<TextAnalyzer>("integ_cpp", 1, 0, "TextAnalyzer");
    
    QQmlApplicationEngine engine(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    return app.exec();
}
