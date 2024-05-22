// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef PROPERTYCODETEST0_H
#define PROPERTYCODETEST0_H

#include <QtTest/QTest>
#include <QProcess>
#include <QCoreApplication>
#include <QDebug>

class LimaAnalyzerTestForgedPipeline: public QObject
{
  Q_OBJECT
private Q_SLOTS:
  void initTestCase();

  void test_forged_pipeline();

private:
};

#endif // PROPERTYCODETEST0_H
