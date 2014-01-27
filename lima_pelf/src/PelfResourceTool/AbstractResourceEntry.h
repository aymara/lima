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
