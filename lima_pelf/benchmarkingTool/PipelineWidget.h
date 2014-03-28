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
#ifndef LIMA_PELF_PIPELINEWIDGET_H
#define LIMA_PELF_PIPELINEWIDGET_H

#include <QtCore/QtDebug>
#include <QtGui/QListView>
#include <QtGui/QDropEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QMenu>

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

public slots:

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
