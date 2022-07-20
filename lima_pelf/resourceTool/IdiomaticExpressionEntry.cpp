// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "IdiomaticExpressionEntry.h"
using namespace Lima::Pelf;

int IdiomaticExpressionEntry::columnCountPerEntry = 5;
QStringList IdiomaticExpressionEntry::columnHeaders = QStringList() << "Trigger" << "Category" << "Automaton" << "Lemma" << "Cont";
QString IdiomaticExpressionEntry::allCategoryName = "[ ALL CATEGORIES ]";
QString IdiomaticExpressionEntry::emptyCategoryName = "[ EMPTY CATEGORY ]";

IdiomaticExpressionEntry* IdiomaticExpressionEntry::factory (QString s1, QString s2, int i)
{
    IdiomaticExpressionEntry* entry = new IdiomaticExpressionEntry(s1);
    entry->sourceFile = s2;
    entry->sourceLine = i;
    return entry;
}

IdiomaticExpressionEntry::IdiomaticExpressionEntry (QString s)
{
    sourceLine = -1;
    sourceString = s;
    displayable = decodeSourceString();
}

bool IdiomaticExpressionEntry::decodeSourceString ()
{
    if(sourceString.isEmpty())
        return false;
    QStringList sourceFields = sourceString.split(";");
    if(sourceFields.size() != 7)
    {
        qDebug() << "Automaton rule entry error, bad format: " << sourceString;
        return false;
    }
    null1 = sourceFields[0].trimmed();
    null2 = sourceFields[1].trimmed();
    contextual = sourceFields[2].trimmed();
    trigger = sourceFields[3].trimmed();
    automaton = sourceFields[4].trimmed();
    category = sourceFields[5].trimmed();
    lemma = sourceFields[6].trimmed();
    if(trigger.isEmpty())
        return false;
    return true;
}

bool IdiomaticExpressionEntry::matches (QStringList args)
{
    if(args.size() != 2)
        return false;
    bool match = displayable;
    match &= args[0].isEmpty() || trigger.contains(args[0]) || automaton.contains(args[0]) || lemma.contains(args[0]);
    match &= args[1] == allCategoryName || args[1] == category || (args[1] == emptyCategoryName && category.isEmpty());
    return match;
}

QString IdiomaticExpressionEntry::encodeToString ()
{
    if(!displayable)
        return sourceString;
    return null1+";"+null2+";"+contextual+";"+trigger+";"+automaton+";"+category+";"+lemma;
}

QString IdiomaticExpressionEntry::getColumnData (int column)
{
    QString data;
    switch(column)
    {
        case 0:
            data = trigger;
            break;
        case 1:
            data = category;
            break;
        case 2:
            data = automaton;
            break;
        case 3:
            data = lemma;
            break;
        case 4:
            data = contextual;
            break;
    }
    return data;
}
