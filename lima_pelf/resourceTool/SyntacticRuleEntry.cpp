// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "SyntacticRuleEntry.h"
using namespace Lima::Pelf;

QString SyntacticRuleEntry::allCategoryName = "[ ALL CATEGORIES ]";
QString SyntacticRuleEntry::emptyCategoryName = "[ EMPTY CATEGORY ]";
QStringList SyntacticRuleEntry::matchNames = QStringList();
QStringList SyntacticRuleEntry::relationNames = QStringList();
SyntacticRuleEntry* SyntacticRuleEntry::factoryCurrentSyntacticRule = 0;

SyntacticRuleEntry* SyntacticRuleEntry::factory (QString s1, QString s2, int i)
{
    s1 = s1.trimmed();
    if(s1.isEmpty())
       return 0;
    SyntacticRuleEntry* factoryLatestSyntacticRule = 0;
    if(*s1.begin() == '#')
    {
        factoryLatestSyntacticRule = factoryCurrentSyntacticRule;
        factoryCurrentSyntacticRule = 0;
    }
    else
    {
        QRegExp automatonRegExp("^([^:]*):([^:]*):([^:]*):([^:]*):([^:]*)$");
        if(automatonRegExp.indexIn(s1) != -1)
        {
            factoryLatestSyntacticRule = factoryCurrentSyntacticRule;
///@TODO Possible memory leak:  ensure that this object will be deleted
            factoryCurrentSyntacticRule = new SyntacticRuleEntry(s1);
            factoryCurrentSyntacticRule->sourceFile = s2;
            factoryCurrentSyntacticRule->sourceLine = i;
            QStringList automatonParts = automatonRegExp.capturedTexts();
            factoryCurrentSyntacticRule->left = automatonParts[1];
            factoryCurrentSyntacticRule->trigger = automatonParts[2];
            factoryCurrentSyntacticRule->right = automatonParts[3];
            factoryCurrentSyntacticRule->category = automatonParts[4];
            QString recongnizeElements = automatonParts[1]+":"+automatonParts[2]+":"+automatonParts[3];
            int automatonElementMatchIndex = 0;
            QRegExp automatonElementRegExp("(\\$|@)[a-zA-Z_-]+");
            while(automatonElementRegExp.indexIn(recongnizeElements, automatonElementMatchIndex) != -1)
            {
                QStringList elements = automatonElementRegExp.capturedTexts();
                QString matchName = elements[0];
                if(matchNames.indexOf(matchName) == -1)
                    matchNames << matchName;
                automatonElementMatchIndex += automatonElementRegExp.matchedLength();
            }
        }
        else if(factoryCurrentSyntacticRule != 0)
        {
            factoryCurrentSyntacticRule->sourceString += s1;
            QRegExp creationRegExp("^\\+CreateRelationBetween\\([^,]*,[^,]*,\"([^,]*)\"\\)$");
            if(creationRegExp.indexIn(s1) != -1)
            {
                QStringList elements = creationRegExp.capturedTexts();
                QString relationName = elements[1];
                factoryCurrentSyntacticRule->dependencyCreates << relationName;
                if(relationNames.indexOf(relationName) == -1)
                    relationNames << relationName;
            }
        }
    }
    return factoryLatestSyntacticRule;
}

SyntacticRuleEntry* SyntacticRuleEntry::factoryEndFile ()
{
    SyntacticRuleEntry* factoryLatestSyntacticRule = factoryCurrentSyntacticRule;
    factoryCurrentSyntacticRule = 0;
    return factoryLatestSyntacticRule;
}

SyntacticRuleEntry::SyntacticRuleEntry (QString s)
{
    sourceLine = -1;
    sourceString = s;
    displayable = true;
}

bool SyntacticRuleEntry::matches (QStringList args)
{
    if(args.size() != 4)
        return false;
    bool match = displayable;
    match &= args[0].isEmpty() || sourceString.contains(args[0]);
    match &= args[1] == allCategoryName || args[1] == category || (args[1] == emptyCategoryName && category.isEmpty());
    match &= args[2] == allCategoryName || trigger.contains(args[2]) || left.contains(args[2]) || right.contains(args[2]);
    match &= args[3] == allCategoryName || dependencyCreates.contains(args[3]);
    return match;
}
