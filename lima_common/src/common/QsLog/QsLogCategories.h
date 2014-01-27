/*
    Copyright 2002-2013 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/
#ifndef QSLOGCATEGORIES_H
#define QSLOGCATEGORIES_H

#include "QsLog.h"

#include <QString>

#ifdef WIN32

#ifdef LIMA_COMMONQSLOG_EXPORTING
#define LIMA_COMMONQSLOG_EXPORT    __declspec(dllexport)
#else
#define LIMA_COMMONQSLOG_EXPORT    __declspec(dllimport)
#endif

#else // Not WIN32

#define LIMA_COMMONQSLOG_EXPORT

#endif

namespace QsLogging
{
LIMA_COMMONQSLOG_EXPORT int initQsLog();

class CategoriesImpl; // d pointer
class LIMA_COMMONQSLOG_EXPORT Categories
{
  friend class CategoriesImpl;
public:
   static Categories& instance()
   {
      static Categories staticCategories;
      return staticCategories;
   }

    bool configure(const QString& fileName);
    Level levelFor(const QString& category) const;

private:
  Categories();
   Categories(const Categories&);
   ~Categories();
   Categories& operator=(const Categories&);
   
   CategoriesImpl* d;
};

} // end namespace


#endif // QSLOGCATEGORIES_H
