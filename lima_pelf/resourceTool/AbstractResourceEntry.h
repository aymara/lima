// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_ABSTRACTRESOURCEENTRY_H
#define LIMA_PELF_ABSTRACTRESOURCEENTRY_H

#include <QtCore/QtDebug>

namespace Lima {
namespace Pelf {

class AbstractResourceEntry
{

public:

    QString category;
    bool displayable;
    QString sourceString;
    QString sourceFile;
    int sourceLine;

    virtual bool decodeSourceString () = 0;
    virtual bool matches (QStringList args) = 0;
    virtual QString encodeToString () = 0;
    virtual QString getColumnData (int column) = 0;
    bool headerLessThan (AbstractResourceEntry* entry, int column, Qt::SortOrder order)
    {
        if(column == -1)
            return sourceLine != -1 && (entry->sourceLine == -1 || sourceLine < entry->sourceLine);
        if(order == Qt::AscendingOrder)
            return QString::localeAwareCompare(getColumnData(column), entry->getColumnData(column)) > 0;
        else
            return QString::localeAwareCompare(getColumnData(column), entry->getColumnData(column)) < 0;
    };

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_ABSTRACTRESOURCEENTRY_H
