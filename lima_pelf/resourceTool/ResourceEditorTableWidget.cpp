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
#include "ResourceEditorTableWidget.h"
using namespace Lima::Pelf;

ResourceEditorTableWidget::ResourceEditorTableWidget (QWidget* p) :
    ResourceReaderTableWidget(p) {}

void ResourceEditorTableWidget::init (ResourceEditorTableModel* retm)
{
    ResourceReaderTableWidget::init(retm);
    editAction = new QAction(("Edit..."), this);
    connect(editAction, SIGNAL(triggered()), this, SLOT(contextEdit()));
    deleteAction = new QAction(("Delete"), this);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(contextDelete()));
    connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(contextEdit()));
}

void ResourceEditorTableWidget::confirmInstall ()
{
    if(confirmInstallDlg())
        dynamic_cast<ResourceEditorTableModel*>(resourceModel)->install();
}

void ResourceEditorTableWidget::editEntry (const QModelIndex& index)
{
    ResourceEditorTableModel* retm = dynamic_cast<ResourceEditorTableModel*>(resourceModel);
    AbstractResourceEntry* entry = retm->getFoundEntry(index.row());
    if(entry == 0)
        return;
    retm->dataModified = true;
    Q_EMIT editEntryDlg(entry);
}

void ResourceEditorTableWidget::createEntry ()
{
    Q_EMIT editEntryDlg();
}

void ResourceEditorTableWidget::keyPressEvent (QKeyEvent* event)
{
    if(event->key() == Qt::Key_Delete)
        contextDelete();
    else
        QTableView::keyPressEvent(event);
}

void ResourceEditorTableWidget::contextMenuEvent (QContextMenuEvent* event)
{
    QModelIndex currentIndex = indexAt(event->pos());
    if(!currentIndex.isValid() || currentIndex.row() == -1)
        return;
    if(!selectedIndexes().contains(currentIndex))
        selectionModel()->select(currentIndex, QItemSelectionModel::Select);
    QMenu menu(this);
    ResourceEditorTableModel* retm = dynamic_cast<ResourceEditorTableModel*>(resourceModel);
    if(selectedIndexes().size() == retm->columnCountPerEntry)
        menu.addAction(editAction);
    menu.addAction(deleteAction);
    menu.exec(event->globalPos());
}

void ResourceEditorTableWidget::contextEdit ()
{
    ResourceEditorTableModel* retm = dynamic_cast<ResourceEditorTableModel*>(resourceModel);
    if(selectedIndexes().size() != retm->columnCountPerEntry)
        return;
    editEntry(selectedIndexes().last());
}

void ResourceEditorTableWidget::contextDelete ()
{
    if(selectedIndexes().size() <= 0)
        return;
    if(QMessageBox::question(
        this,
        "Delete resource entries ?",
        "Do you really want to delete this resource entries ?",
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel
      ) == QMessageBox::Ok)
    {
        ResourceEditorTableModel* resourceEditorTableModel = dynamic_cast<ResourceEditorTableModel*>(resourceModel);
        resourceEditorTableModel->deleteEntries(selectedIndexes());
    }
}

#include "ResourceEditorTableWidget.moc"
