/*
    Copyright 2015 CEA LIST

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
#include "BagOfWordsTest3.h"

#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include <QtCore/QObject>

#include "common/QsLog/QsLog.h"
#include "common/QsLog/QsLogDest.h"
#include "common/QsLog/QsLogCategories.h"
#include "common/QsLog/QsDebugOutput.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/bowNamedEntity.h"
#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"

#include <sstream>

using namespace Lima::Common::BagOfWords;
using namespace Lima::Common::MediaticData;

void BagOfWordsTest3::initTestCase()
{
  QsLogging::initQsLog();
  // Necessary to initialize factories under Windows
  Lima::AmosePluginsManager::single();
}

void BagOfWordsTest3::test_BoWText()
{
  qDebug() << "BagOfWordsTest3::test_BoWText";
  QString bt1Lemma;

  std::stringstream stream;
  {
    QSharedPointer< BoWToken > bt1(new BoWToken(QString::fromUtf8("lemma"), 1, 10, 5));
    bt1->setInflectedForm(QString::fromUtf8("lemma"));
    QSharedPointer< BoWToken > bt2(new BoWToken(QString::fromUtf8("démocratique"), 1, 10, 12));
    bt2->setInflectedForm(QString::fromUtf8("démocratique"));
    QSharedPointer< BoWToken > bt3(new BoWToken(QString::fromUtf8("word"), 1, 10, 4));
    bt3->setInflectedForm(QString::fromUtf8("word"));

    QSharedPointer< BoWToken > bt4(new BoWToken(QString::fromUtf8("autre"), 1, 10, 4));
    bt4->setInflectedForm(QString::fromUtf8("autre"));

    bt1Lemma = bt1->getLemma();

    QSharedPointer< BoWTerm > term(new BoWTerm);
    term->addPart(bt1);
    QSharedPointer< BoWTerm > term2(new BoWTerm);
    term2->addPart(bt2);
    term2->addPart(bt3);
    term->addPart(term2);

    BoWText text;
    text.push_back(term);
    text.push_back(bt4);

    BoWBinaryWriter writer;
    writer.writeBoWText(stream, text);
  }

  BoWText text;
  BoWBinaryReader reader;
  reader.readBoWText(stream, text);

  QVERIFY( text.size() == 2 );
  QSharedPointer<BoWTerm> rterm = qSharedPointerCast<BoWTerm>(text[0]);
  QVERIFY( rterm->getParts().size() == 2 );

  QVERIFY( bt1Lemma == rterm->getParts()[0].getBoWToken()->getLemma() );
}

void BagOfWordsTest3::test_BoWText2()
{
  qDebug() << "BagOfWordsTest3::test_BoWText2";
  QString name;

  std::stringstream stream;
  {
    QSharedPointer< BoWToken > bt1(new BoWToken(QString::fromUtf8("lemma"), 1, 10, 5));
    bt1->setInflectedForm(QString::fromUtf8("lemma"));
    QSharedPointer< BoWToken > bt2(new BoWToken(QString::fromUtf8("démocratique"), 1, 10, 12));
    bt2->setInflectedForm(QString::fromUtf8("démocratique"));
    QSharedPointer< BoWToken > bt3(new BoWToken(QString::fromUtf8("word"), 1, 10, 4));
    bt3->setInflectedForm(QString::fromUtf8("word"));

    QSharedPointer< BoWToken > bt4(new BoWToken(QString::fromUtf8("autre"), 1, 10, 4));
    bt4->setInflectedForm(QString::fromUtf8("autre"));

    QSharedPointer< BoWTerm > term(new BoWTerm);
    term->addPart(bt1);
    QSharedPointer< BoWTerm > term2(new BoWTerm);
    term2->addPart(bt2);
    term2->addPart(bt3);
    term->addPart(term2);

    QSharedPointer< BoWNamedEntity > ne(new BoWNamedEntity);
    ne->setNamedEntityType(EntityType(EntityTypeId(1),EntityGroupId(3)));
    ne->addFeature("surname","John");
    ne->addFeature("name","Doe");
    name = ne->getFeatures().at("name");
    QSharedPointer< BoWToken > btsurname(new BoWToken(QString::fromUtf8("John"), 1, 10, 5));
    bt1->setInflectedForm(QString::fromUtf8("John"));
    QSharedPointer< BoWToken > btname(new BoWToken(QString::fromUtf8("Doe"), 1, 10, 12));
    bt2->setInflectedForm(QString::fromUtf8("Doe"));
    ne->addPart(btsurname);
    ne->addPart(btname);

    BoWText text;
    text.push_back(term);
    text.push_back(bt4);
    text.push_back(ne);

    BoWBinaryWriter writer;
    writer.writeBoWText(stream, text);
  }

  BoWText text;
  BoWBinaryReader reader;
  reader.readBoWText(stream, text);

  QVERIFY( text.size() == 3 );
  QSharedPointer<BoWNamedEntity> rne = qSharedPointerCast<BoWNamedEntity>(text[2]);
  QVERIFY( rne->getParts().size() == 2 );

  QVERIFY( name == rne->getFeatures().at("name") );
}


QTEST_MAIN(BagOfWordsTest3)

