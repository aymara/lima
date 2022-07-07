// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef QSLOGCATEGORIES_H
#define QSLOGCATEGORIES_H

#include "QsLog.h"

#include <QtCore/QString>
#include <QtCore/QObject>

#include "QsLog_export.h"


namespace QsLogging
{
LIMA_COMMONQSLOG_EXPORT bool initQsLog ( const QString& configString = QLatin1String("") );

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

  bool configure(const QString& fileName, bool reload = false);
  Level levelFor(const QString& category) const;
  void connectSignals();

private Q_SLOTS:
  void configureFileChanged ( const QString & path );

private:
   Categories(QObject* parent = 0);
   Categories(const Categories&) = delete;
   virtual ~Categories();
   Categories& operator=(const Categories&) = delete;

   CategoriesImpl* d;
};

} // end namespace


#endif // QSLOGCATEGORIES_H
