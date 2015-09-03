#ifndef BAGOFWORDSTEST3_H
#define BAGOFWORDSTEST3_H

#include <QtTest/QtTest>

class BagOfWordsTest3: public QObject
{
  Q_OBJECT
private slots:
  void initTestCase();

  // BoWText with a BoWTerm and a BoWToken
  void test_BoWText();

  // BoWText with a BoWTerm, a BoWNamedEntity and a BoWToken
  void test_BoWText2();

private:
};

#endif // BAGOFWORDSTEST3_H
