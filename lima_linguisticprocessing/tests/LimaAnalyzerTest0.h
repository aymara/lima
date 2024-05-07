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

  void test_unknownLanguage();
  void test_lang_no_prefix();
  void test_several_instances();
  void test_two_same_instances();
  void test_two_different_instances();
  void test_analyzeText_lang_not_str();
  void test_analyzeText_lang_not_init();
  void test_analyzeText_pipeline_none_lang_ud();
  void test_analyzeText_pipeline_none_lang_eng();
  void test_analyzeText_init_with_lang_and_pipe();
  void test_analyzeText_pipeline_not_avail();
  // void test_forged_pipeline();

private:
};

#endif // PROPERTYCODETEST0_H
