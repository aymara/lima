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
#ifndef LIMA_PELF_RESOURCEREADERTABLEWIDGET_H
#define LIMA_PELF_RESOURCEREADERTABLEWIDGET_H

#include <QtCore/QtDebug>
#include <QtCore/QFileInfo>
#include <QtWidgets/QTableView>
#include <QtWidgets/QHeaderView>

#include "ResourceReaderSimpleWidget.h"
#include "ResourceEditorTableModel.h"

namespace Lima {
namespace Pelf {

class ResourceReaderTableWidget : public QTableView, public ResourceReaderSimpleWidget
{

Q_OBJECT

public:

  ResourceReaderTableWidget (QWidget* parent);
  void init (ResourceReaderTableModel* rrtm);

public Q_SLOTS:
  void sortByColumn (int column);

private Q_SLOTS:
  void slotResourceFileModified(const QString&);

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_RESOURCEREADERTABLEWIDGET_H
