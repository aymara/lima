// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    void keyPressEvent (QKeyEvent* event)  override;
    void contextMenuEvent (QContextMenuEvent* event) override;

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
