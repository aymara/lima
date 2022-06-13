/*
 *    Copyright 2002-2013 CEA LIST
 * 
 *    This file is part of LIMA.
 * 
 *    LIMA is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Affero General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 * 
 *    LIMA is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 * 
 *    You should have received a copy of the GNU Affero General Public License
 *    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
 */
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
        QRegularExpression automatonRegExp("^([^:]*):([^:]*):([^:]*):([^:]*):([^:]*)$");
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
            QRegularExpression automatonElementRegExp("(\\$|@)[a-zA-Z_-]+");
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
            QRegularExpression creationRegExp("^\\+CreateRelationBetween\\([^,]*,[^,]*,\"([^,]*)\"\\)$");
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
