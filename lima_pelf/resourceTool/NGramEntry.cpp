// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "NGramEntry.h"
using namespace Lima::Pelf;

int NGramEntry::columnCountPerEntry = 4;
QStringList NGramEntry::columnHeaders = QStringList() << "Gram 1" << "Gram 2" << "Gram 3" << "Proba";
QString NGramEntry::allCategoryName = "[ ALL CATEGORIES ]";
QString NGramEntry::emptyCategoryName = "[ EMPTY CATEGORY ]";
QString NGramEntry::bigramsCategoryName = "[ BIGRAM ]";
QStringList NGramEntry::gramCategoryList = QStringList();

NGramEntry* NGramEntry::factory (QString s1, QString s2, int i)
{
    if(s1.isEmpty())
        return 0;
    NGramEntry* entry = new NGramEntry(s1);
    entry->sourceFile = s2;
    entry->sourceLine = i;
    return entry;
}

NGramEntry::NGramEntry (QString s)
{
    sourceLine = -1;
    sourceString = s;
    displayable = decodeSourceString();
}

bool NGramEntry::decodeSourceString ()
{
    if(sourceString.isEmpty())
        return false;
    QStringList sourceFields = sourceString.split("\t");
    if(sourceFields.size() != columnCountPerEntry && sourceFields.size() != columnCountPerEntry -1)
    {
        qDebug() << "NGram entry error, bad format: " << sourceString;
        return false;
    }
    gram1 = sourceFields[0];
    gram2 = sourceFields[1];
    if(sourceFields.size() == columnCountPerEntry) // Case bigrams
    {
        gram3 = sourceFields[2];
        proba = sourceFields[3];
    }
    else // Case trigrams
    {
        gram3 = bigramsCategoryName;
        proba = sourceFields[2];
    }
    if(gramCategoryList.indexOf(gram1) == -1)
        gramCategoryList << gram1;
    if(gramCategoryList.indexOf(gram2) == -1)
        gramCategoryList << gram2;
    if(gramCategoryList.indexOf(gram3) == -1)
        gramCategoryList << gram3;
    return true;
}

bool NGramEntry::matches (QStringList args)
{
    if(args.size() != 3)
        return false;
    bool match = displayable;
    match &= args[0] == allCategoryName || args[0] == gram1 || (args[0] == emptyCategoryName && gram1.isEmpty());
    match &= args[1] == allCategoryName || args[1] == gram2 || (args[1] == emptyCategoryName && gram2.isEmpty());
    match &= args[2] == allCategoryName || args[2] == gram3 || (args[2] == emptyCategoryName && gram3.isEmpty());
    return match;
}

QString NGramEntry::getColumnData (int column)
{
    QString data;
    switch(column)
    {
    case 0:
        data = gram1;
        break;
    case 1:
        data = gram2;
        break;
    case 2:
        data = gram3;
        break;
    case 3:
        data = proba;
        break;
    }
    return data;
}
