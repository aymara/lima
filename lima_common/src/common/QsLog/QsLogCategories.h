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

#include <QtCore/QString>
#include <QtCore/QObject>

#include "QsLog_export.h"


namespace QsLogging
{
LIMA_COMMONQSLOG_EXPORT int initQsLog();

class CategoriesImpl; // d pointer
class LIMA_COMMONQSLOG_EXPORT Categories : public QObject
{
Q_OBJECT
  friend class CategoriesImpl;
public:
   static Categories& instance()
   {
      static Categories staticCategories;
      return staticCategories;
   }

    bool configure(const QString& fileName);
    Level levelFor(const QString& category) const;

private Q_SLOTS:
  void configureFileChanged ( const QString & path );

private:
  Categories(QObject* parent = 0);
   Categories(const Categories&);
   virtual ~Categories();
   Categories& operator=(const Categories&);
   

   CategoriesImpl* d;
};

} // end namespace


#endif // QSLOGCATEGORIES_H
