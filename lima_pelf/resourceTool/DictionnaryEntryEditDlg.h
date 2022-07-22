// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_DICTIONNARYENTRYEDITDLG_H
#define LIMA_PELF_DICTIONNARYENTRYEDITDLG_H

#include <QtCore/QtDebug>
#include <QtWidgets/QDialog>
#include <QtWidgets/QPushButton>

#include "ui_pelf-rt-editdictionnaryentrydialog.h"

#include "ResourceEditorTableModel.h"
#include "DictionnaryEntry.h"

namespace Lima {
namespace Pelf {

class DictionnaryEntryEditDlg : public QDialog, public Ui::DictionnaryEntryEditDialog
{

Q_OBJECT

public:

    DictionnaryEntryEditDlg (QWidget* parent = 0);
    void init (ResourceEditorTableModel* rem, AbstractResourceEntry* are = 0);

Q_SIGNALS:
    void updateEntry (AbstractResourceEntry* de);

public Q_SLOTS:

    void submit ();
    void checkValidity();

private:

    DictionnaryEntryEditDialog ui;
    DictionnaryEntry* dictionnaryEntry;
    bool isNew;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_DICTIONNARYENTRYEDITDLG_H
