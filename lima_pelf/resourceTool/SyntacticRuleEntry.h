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
#ifndef LIMA_PELF_SYNTACTICRULEENTRY_H
#define LIMA_PELF_SYNTACTICRULEENTRY_H

#include <QtCore/QtDebug>
#include <QtCore/QStringList>

#include "AbstractResourceEntry.h"

namespace Lima {
namespace Pelf {

class SyntacticRuleEntry : public AbstractResourceEntry
{

public:

    static QString allCategoryName;
    static QString emptyCategoryName;
    static QStringList matchNames;
    static QStringList relationNames;
    static SyntacticRuleEntry* factoryCurrentSyntacticRule;

    static SyntacticRuleEntry* factory (QString s1, QString s2, int i);
    static SyntacticRuleEntry* factoryEndFile ();

    QString trigger;
    QString left;
    QString right;
    QStringList dependencyCreates;

    SyntacticRuleEntry (QString s = "");
    bool decodeSourceString () override { return true; };
    bool matches (QStringList args) override;
    QString encodeToString () override { return ""; };
    QString getColumnData(int ) override { return ""; };

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_SYNTACTICRULEENTRY_H
