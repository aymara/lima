// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_IDIOMATICEXPRESSIONENTRY_H
#define LIMA_PELF_IDIOMATICEXPRESSIONENTRY_H

#include <QtCore/QtDebug>
#include <QtCore/QStringList>

#include "AbstractResourceEntry.h"

namespace Lima {
namespace Pelf {

class IdiomaticExpressionEntry : public AbstractResourceEntry
{

public:

    static int columnCountPerEntry;
    static QStringList columnHeaders;
    static QString allCategoryName;
    static QString emptyCategoryName;

    static IdiomaticExpressionEntry* factory (QString s1, QString s2, int i);
    static IdiomaticExpressionEntry* factoryEndFile () { return 0; };

    QString null1;
    QString null2;
    QString trigger;
    QString automaton;
    QString lemma;
    QString contextual;

    IdiomaticExpressionEntry (QString s = "");
    bool decodeSourceString () override;
    bool matches (QStringList args) override;
    QString encodeToString () override;
    QString getColumnData(int column) override;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_IDIOMATICEXPRESSIONENTRY_H
