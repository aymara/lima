// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
