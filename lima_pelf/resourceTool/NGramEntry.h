// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_NGRAMENTRY_H
#define LIMA_PELF_NGRAMENTRY_H

#include <QtCore/QtDebug>
#include <QtCore/QStringList>

#include "AbstractResourceEntry.h"

namespace Lima {
namespace Pelf {

class NGramEntry : public AbstractResourceEntry
{

public:

    static int columnCountPerEntry;
    static QStringList columnHeaders;
    static QString allCategoryName;
    static QString emptyCategoryName;
    static QString bigramsCategoryName;
    static QStringList gramCategoryList;

    static NGramEntry* factory (QString s1, QString s2, int i);
    static NGramEntry* factoryEndFile () { return 0; };

    QString gram1;
    QString gram2;
    QString gram3;
    QString proba;

    NGramEntry (QString s = "");
    bool decodeSourceString () override;
    bool matches (QStringList args) override;
    QString encodeToString () override { return ""; };
    QString getColumnData(int column) override;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_NGRAMENTRY_H
