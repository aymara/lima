// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "PipelineWidget.h"

#include "PipelineUnitDelegate.h"

using namespace Lima::Pelf;

PipelineWidget::PipelineWidget (QWidget* p) :
    QListView(p),
    pipeline(new Pipeline(this))
{
    setModel(pipeline);
}

void PipelineWidget::init ()
{
    setItemDelegate(new PipelineUnitDelegate(this, pipeline));
    connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(editUnit(const QModelIndex&)));
    editAction = new QAction(("Edit..."), this);
    connect(editAction, SIGNAL(triggered()), this, SLOT(contextEdit()));
    moveUpAction = new QAction(("Move up"), this);
    connect(moveUpAction, SIGNAL(triggered()), this, SLOT(contextMoveUp()));
    moveDownAction = new QAction(("Move down"), this);
    connect(moveDownAction, SIGNAL(triggered()), this, SLOT(contextMoveDown()));
    deleteAction = new QAction(("Remove"), this);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(contextDelete()));
}

void PipelineWidget::dragMoveEvent (QDragMoveEvent* event)
{
    QModelIndex currentIndex = indexAt(event->pos());
    if(currentIndex.row() != latestDragIndexRow)
    {
        pipeline->setDropIndicator(currentIndex);
        latestDragIndexRow = currentIndex.row();
        reset();
    }
}

void PipelineWidget::dropEvent (QDropEvent* event)
{
    if(event->possibleActions() & Qt::MoveAction)
        pipeline->moveUnits(selectedIndexes(), indexAt(event->pos()));
    pipeline->clearDropIndicator();
}

void PipelineWidget::keyPressEvent (QKeyEvent* event)
{
    if(event->key() == Qt::Key_Delete)
        pipeline->confirmDeleteUnits(selectedIndexes());
    else
        QListView::keyPressEvent(event);
}

void PipelineWidget::editUnit (const QModelIndex& index)
{
///@TODO Possible memory leak:  ensure that this object will be deleted
    PipelineEditFileDlg* pipelineEditFileDlg = new PipelineEditFileDlg();
    pipelineEditFileDlg->init(pipeline);
    pipelineEditFileDlg->setUnit(pipeline->getUnit(index), index.row());
}

void PipelineWidget::contextMenuEvent (QContextMenuEvent* event)
{
    QModelIndex currentIndex = indexAt(event->pos());
    if(!currentIndex.isValid() || currentIndex.row() == -1)
        return;
    if(!selectedIndexes().contains(currentIndex))
        selectionModel()->select(currentIndex, QItemSelectionModel::Select);
    QMenu menu(this);
    if(selectedIndexes().size() == 1)
        menu.addAction(editAction);
    menu.addAction(moveUpAction);
    menu.addAction(moveDownAction);
    menu.addAction(deleteAction);
    menu.exec(event->globalPos());
}

void PipelineWidget::contextEdit () {
    if(selectedIndexes().size() != 1)
        return;
    QModelIndex targetIndex = selectedIndexes().last();
    editUnit(targetIndex);
}

void PipelineWidget::contextMoveUp () {
    if(selectedIndexes().size() <= 0)
        return;
    QModelIndex targetIndex = selectedIndexes().last().sibling(selectedIndexes().first().row() - 1, 0);
    pipeline->moveUnits(selectedIndexes(), targetIndex);
}

void PipelineWidget::contextMoveDown () {
    if(selectedIndexes().size() <= 0)
        return;
    QModelIndex targetIndex = selectedIndexes().last().sibling(selectedIndexes().last().row() + 2, 0);
    pipeline->moveUnits(selectedIndexes(), targetIndex);
}

void PipelineWidget::contextDelete () {
    if(selectedIndexes().size() <= 0)
        return;
    pipeline->confirmDeleteUnits(selectedIndexes());
}

#include "PipelineWidget.moc"
