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
    bool decodeSourceString ();
    bool matches (QStringList args);
    QString encodeToString () { return ""; };
    QString getColumnData(int column);

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_NGRAMENTRY_H
