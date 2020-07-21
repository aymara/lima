#ifndef ENHANCEDANALYSISDICTIONARYTEST1_H
#define ENHANCEDANALYSISDICTIONARYTEST1_H

#include <QtTest/QtTest>

class EnhancedAnalysisDictionaryTest1: public QObject
{
  Q_OBJECT
private Q_SLOTS:
  void initTestCase();

  void testEng();
  void testFre();
  void testPor();

private:
  void testLanguage(
    const QString& language,
    const QString& lpConfFileName,
    const QString& entryKey,
    const QStringList& refForms,
    const QStringList& refLemmas,
    const QStringList& refNormalized,
    const QString& micro);

};

#endif // ENHANCEDANALYSISDICTIONARYTEST1_H
