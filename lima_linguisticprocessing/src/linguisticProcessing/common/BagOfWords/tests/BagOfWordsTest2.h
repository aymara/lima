#ifndef BAGOFWORDSTEST2_H
#define BAGOFWORDSTEST2_H

#include <QtTest/QtTest>

class BagOfWordsTest2: public QObject
{
  Q_OBJECT
private slots:
  void initTestCase();

  void test_indexElementDefaultConstructor();
  void test_indexElementConstructor1();
  void test_indexElementConstructor2();
  void test_indexElementCopyConstructor();
  void test_indexElementOperatorAffect();

private:
};

#endif // BAGOFWORDSTEST2_H
