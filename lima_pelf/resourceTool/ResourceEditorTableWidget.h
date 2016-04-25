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
#ifndef LIMA_PELF_RESOURCEEDITORTABLEWIDGET_H
#define LIMA_PELF_RESOURCEEDITORTABLEWIDGET_H

#include <QtCore/QtDebug>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtGui/QContextMenuEvent>
#include <QtWidgets/QMessageBox>

#include "ResourceReaderTableWidget.h"

namespace Lima {
namespace Pelf {

class ResourceEditorTableWidget : public ResourceReaderTableWidget
{

Q_OBJECT

public:

    ResourceEditorTableWidget (QWidget* parent);
    void init (ResourceEditorTableModel* retm);
    void confirmInstall ();
    void keyPressEvent (QKeyEvent* event);
    void contextMenuEvent (QContextMenuEvent* event);

public Q_SLOTS:

    void editEntry (const QModelIndex& index);
    void createEntry ();
    void contextEdit ();
    void contextDelete ();

Q_SIGNALS:

    void editEntryDlg (AbstractResourceEntry* are = 0);

private:

    ResourceEditorTableModel* resourceEditorTableModel;
    QAction* editAction;
    QAction* deleteAction;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_RESOURCEEDITORTABLEWIDGET_H
