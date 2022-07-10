// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef BAGOFWORDSTEST2_H
#define BAGOFWORDSTEST2_H

#include <QtTest/QTest>

class BagOfWordsTest2: public QObject
{
  Q_OBJECT
private Q_SLOTS:
  void initTestCase();

  void test_indexElementDefaultConstructor();
  void test_indexElementConstructor1();
  void test_indexElementConstructor2();
  void test_indexElementCopyConstructor();
  void test_indexElementOperatorAffect();

private:
};

#endif // BAGOFWORDSTEST2_H
