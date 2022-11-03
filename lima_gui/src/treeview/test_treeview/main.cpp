// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtCore/QUrl>

#include "treemodel.h"

/// The github project that helped : https://github.com/hvoigt/qt-qml-treeview

#define QML_EXPORT_PACKAGE "hello"
#define QML_REGISTER(type) qmlRegisterType<type>(QML_EXPORT_PACKAGE, 1, 0, #type)
int main(int argc, char *argv[])
{
//   QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication app(argc, argv);

  QQmlApplicationEngine engine;

  QML_REGISTER(TreeModel);

  engine.load(QUrl(QLatin1String("qrc:///main.qml")));
  if (engine.rootObjects().isEmpty())
    return -1;


  return app.exec();
}
