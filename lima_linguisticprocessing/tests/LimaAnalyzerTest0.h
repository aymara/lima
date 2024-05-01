// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef PROPERTYCODETEST0_H
#define PROPERTYCODETEST0_H

#include <QtTest/QTest>
#include <QProcess>
#include <QCoreApplication>
#include <QDebug>

class LimaAnalyzerTest0: public QObject
{
  Q_OBJECT
private Q_SLOTS:
  void initTestCase();

  void test_two_instances();

private:
};

#endif // PROPERTYCODETEST0_H
