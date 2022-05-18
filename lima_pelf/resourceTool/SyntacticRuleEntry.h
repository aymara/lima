// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
