// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "DictionnaryEntry.h"
using namespace Lima::Pelf;

int DictionnaryEntry::columnCountPerEntry = 3;
QStringList DictionnaryEntry::columnHeaders = QStringList() << "Lemma" << "Category" << "Normalization";
QString DictionnaryEntry::allCategoryName = "[ ALL CATEGORIES ]";
QString DictionnaryEntry::emptyCategoryName = "[ EMPTY CATEGORY ]";

DictionnaryEntry* DictionnaryEntry::factory (QString s1, QString s2, int i)
{
    DictionnaryEntry* entry = new DictionnaryEntry(s1);
    entry->sourceFile = s2;
    entry->sourceLine = i;
    return entry;
}

DictionnaryEntry::DictionnaryEntry (QString s)
{
    sourceLine = -1;
    sourceString = s;
    if(!s.isEmpty())
    {
        decodeSourceString();
        displayable = true;
    }
}

bool DictionnaryEntry::decodeSourceString ()
{
    if(sourceString.isEmpty())
        return false;
    QStringList sourceFields = sourceString.split("\t");
    if(sourceFields.size() != 4)
    {
        qDebug() << "Dictionnary entry error, bad format: " << sourceString;
        return false;
    }
    lemma = sourceFields[0].trimmed();
    normalization = sourceFields[1].trimmed();
    validation = sourceFields[2].trimmed();
    category = sourceFields[3].trimmed();
    if(lemma.isEmpty() || category.isEmpty())
        return false;
    return true;
}

bool DictionnaryEntry::matches (QStringList args)
{
    if(args.size() != 2)
        return false;
    bool match = displayable;
    match &= args[0].isEmpty() || lemma.contains(args[0]) || normalization.contains(args[0]);
    match &= args[1] == allCategoryName || args[1] == category || (args[1] == emptyCategoryName && category.isEmpty());
    return match;
}

QString DictionnaryEntry::encodeToString ()
{
    if(!displayable)
        return sourceString;
    return null1+";"+null2+";"+lemma+";"+category+";"+normalization+";"+validation;
}

QString DictionnaryEntry::getColumnData (int column)
{
    QString data;
    switch(column)
    {
    case 0:
        data = lemma;
        break;
    case 1:
        data = category;
        break;
    case 2:
        data = normalization;
        break;
    }
    return data;
}
