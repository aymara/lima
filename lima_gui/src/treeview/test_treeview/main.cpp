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
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

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

  engine.load(QUrl(QLatin1String("qrc:/main.qml")));
  if (engine.rootObjects().isEmpty())
    return -1;


  return app.exec();
}
