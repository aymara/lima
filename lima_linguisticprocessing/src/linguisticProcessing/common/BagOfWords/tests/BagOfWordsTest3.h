// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef BAGOFWORDSTEST3_H
#define BAGOFWORDSTEST3_H

#include <QtTest/QTest>

class BagOfWordsTest3: public QObject
{
  Q_OBJECT
private Q_SLOTS:
  void initTestCase();

  // BoWText with a BoWTerm and a BoWToken
  void test_BoWText();

  // BoWText with a BoWTerm, a BoWNamedEntity and a BoWToken
  void test_BoWText2();

private:
};

#endif // BAGOFWORDSTEST3_H
