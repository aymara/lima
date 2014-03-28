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
