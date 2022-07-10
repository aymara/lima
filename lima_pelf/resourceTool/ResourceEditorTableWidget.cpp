// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "ResourceEditorTableWidget.h"
using namespace Lima::Pelf;

ResourceEditorTableWidget::ResourceEditorTableWidget (QWidget* p) :
    ResourceReaderTableWidget(p) {}

void ResourceEditorTableWidget::init (ResourceEditorTableModel* retm)
{
    ResourceReaderTableWidget::init(retm);
    editAction = new QAction(("Edit..."), this);
    connect(editAction, SIGNAL(triggered()),this, SLOT(contextEdit()));
    deleteAction = new QAction(("Delete"), this);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(contextDelete()));
    connect(this, SIGNAL(doubleClicked(QModelIndex&)), this, SLOT(contextEdit()));
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
    auto indexes = selectedIndexes();
    editEntry(indexes.last());
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
