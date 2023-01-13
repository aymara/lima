// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// clazy:skip

#include "ShiftFromTest0.h"
#include <QtTest/QTest>

QTEST_MAIN ( ShiftFromTest );

#include "common/Handler/shiftFrom.h"

#include <QXmlStreamReader>

using namespace Lima;

void ShiftFromTest::ShiftFromTest0()
{
    // 28, 38, 61, 73, 83
    QString xml = "<DOC><TEXT>>uncoded entities&lt; text &lt;word></TEXT><TEXT2> pop&Dagger;hop &amp; tok &#1983;</TEXT2></DOC>";
    qDebug() << xml;
    ShiftFrom sf(xml);
    qDebug() << "ShiftFromTest0 sf:" << sf;

    auto textPosition = xml.indexOf("text");
    qDebug() << "'text' index in string:" << textPosition;
    qDebug() << "'text' index in string noent:" << sf.xml_noent().indexOf("text");
    auto wordPosition = xml.indexOf("word");
    qDebug() << "'word' index in string:" << wordPosition;
    qDebug() << "'word' index in string noent:" << sf.xml_noent().indexOf("word");
    auto tokPosition = xml.indexOf("tok");
    qDebug() << "'tok' index in string:" << tokPosition;
    qDebug() << "'tok' index in string noent:" << sf.xml_noent().indexOf("tok");
    qDebug() << "";

    QXmlStreamReader reader(sf.xml_noent());
    while (!reader.atEnd())
    {
        reader.readNextStartElement();
        auto offset = reader.characterOffset();
        if (reader.isStartElement() && reader.name() == QLatin1String("TEXT"))
        {
          auto txt = reader.readElementText();
          auto limaText = sf.rebuild_text(txt, offset);
          qDebug() << "text to send to LIMA:" << limaText;
          qDebug() << "";
          QVERIFY(limaText == ">uncoded entities< text <word>");

          auto indexOfLt1 = limaText.indexOf("<");
          auto xindexOfLt1 = xml.indexOf("&lt;");
          qDebug() << "xindexOfLt1:" << xindexOfLt1 << "-" << sf.correct_offset(offset, indexOfLt1);
          QVERIFY(xindexOfLt1==sf.correct_offset(offset, indexOfLt1));

          auto indexOfLt2 = limaText.indexOf("<", 19);
          auto xindexOfLt2 = xml.indexOf("&lt;", 32);
          qDebug() << "xindexOfLt2:" << xindexOfLt2 << "-" << sf.correct_offset(offset, indexOfLt2);
          QVERIFY(xindexOfLt2==sf.correct_offset(offset, indexOfLt2));

          auto indexOfText = limaText.indexOf("text");
          QVERIFY(textPosition==sf.correct_offset(offset, indexOfText));

          auto indexOfWord = limaText.indexOf("word");
          QVERIFY(wordPosition==sf.correct_offset(offset, indexOfWord));

        }
        if (reader.isStartElement() && reader.name() == QLatin1String("TEXT2"))
        {
          auto txt = reader.readElementText();
          auto limaText = sf.rebuild_text(txt, offset);
          qDebug() << "text to send to LIMA:" << limaText;
          qDebug() << "";
          QVERIFY(limaText == " pop‡hop & tok \u07BF");
          auto indexOfTok = limaText.indexOf("tok");
          QVERIFY(tokPosition==sf.correct_offset(offset, indexOfTok));
          return;
        }
    }
    QVERIFY(false);
}

