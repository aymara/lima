// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_PIPELINEWIDGET_H
#define LIMA_PELF_PIPELINEWIDGET_H

#include <QtCore/QtDebug>
#include <QtWidgets/QListView>
#include <QtGui/QDropEvent>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenu>

#include "Pipeline.h"
#include "PipelineEditFileDlg.h"

namespace Lima {
namespace Pelf {

class PipelineWidget : public QListView
{

Q_OBJECT

public:

    Pipeline* pipeline;
    int latestDragIndexRow;

    PipelineWidget (QWidget *parent = 0);
    void init ();
    void dragMoveEvent (QDragMoveEvent* event);
    void dropEvent (QDropEvent* event);
    void keyPressEvent (QKeyEvent* event);
    void contextMenuEvent (QContextMenuEvent* event);

public Q_SLOTS:

    void editUnit (const QModelIndex& index);
    void contextEdit ();
    void contextMoveUp ();
    void contextMoveDown ();
    void contextDelete ();

private:

    QAction* editAction;
    QAction* moveUpAction;
    QAction* moveDownAction;
    QAction* deleteAction;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_PIPELINEWIDGET_H
