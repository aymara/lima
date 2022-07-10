// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_DICTIONNARYENTRY_H
#define LIMA_PELF_DICTIONNARYENTRY_H

#include <QtCore/QtDebug>
#include <QtCore/QStringList>

#include "AbstractResourceEntry.h"

namespace Lima {
namespace Pelf {

class DictionnaryEntry : public AbstractResourceEntry
{

public:

    static int columnCountPerEntry;
    static QStringList columnHeaders;
    static QString allCategoryName;
    static QString emptyCategoryName;

    static DictionnaryEntry* factory (QString s1, QString s2, int i);
    static DictionnaryEntry* factoryEndFile () { return 0; };

    QString null1;
    QString null2;
    QString lemma;
    QString normalization;
    QString validation;

    DictionnaryEntry (QString s = "");
    bool decodeSourceString () override;
    bool matches (QStringList args) override;
    QString encodeToString () override;
    QString getColumnData(int column) override;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_DICTIONNARYENTRY_H
