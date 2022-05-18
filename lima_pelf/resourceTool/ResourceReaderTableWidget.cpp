// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "ResourceReaderTableWidget.h"
using namespace Lima::Pelf;

ResourceReaderTableWidget::ResourceReaderTableWidget (QWidget* p) :
    QTableView(p), ResourceReaderSimpleWidget() {}

void ResourceReaderTableWidget::init (ResourceReaderTableModel* rrtm)
{
  if (rrtm!=0)
  {
    disconnect(rrtm,SIGNAL(resourceFileModified(QString)),this,SLOT(slotResourceFileModified(QString)));
  }
  ResourceReaderSimpleWidget::init(rrtm);
  connect(horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sortByColumn(int)));
  setModel(rrtm);
  sortByColumn(0);

  connect(rrtm,SIGNAL(resourceFileModified(QString)),this,SLOT(slotResourceFileModified(QString)));
}

void ResourceReaderTableWidget::sortByColumn (int column)
{
  if (resourceModel != 0)
  {
    ((ResourceReaderTableModel*)resourceModel)->sortByHeader(column, horizontalHeader()->sortIndicatorOrder());
  }
}

void ResourceReaderTableWidget::slotResourceFileModified(const QString& file)
{
  qDebug() << "ResourceReaderToolBoxWidget::slotResourceFileModified" << file;
  if (resourceModel->installStatus != ResourceReaderSimpleModel::INSTALL_PROCESSING)
  {
    if (QMessageBox::question(
          this,
          tr("Resource Modified"),
          tr("Resource file modified: %1\nDo you want to reload it?").arg(file),
          QMessageBox::Ok | QMessageBox::Cancel,
          QMessageBox::Cancel
        ) == QMessageBox::Ok)
    {
      init((ResourceReaderTableModel*)resourceModel);
    }
  }
}


#include "ResourceReaderTableWidget.moc"
