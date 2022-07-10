// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_IDIOMATICEXPRESSIONENTRYEDITDLG_H
#define LIMA_PELF_IDIOMATICEXPRESSIONENTRYEDITDLG_H

#include <QtCore/QtDebug>
#include <QtWidgets/QDialog>
#include <QtWidgets/QPushButton>

#include "ui_pelf-rt-editidiomaticexpressionentrydialog.h"

#include "ResourceEditorTableModel.h"
#include "IdiomaticExpressionEntry.h"

namespace Lima {
namespace Pelf {

class IdiomaticExpressionEntryEditDlg : public QDialog, public Ui::IdiomaticExpressionEntryEditDialog
{

Q_OBJECT

public:

    IdiomaticExpressionEntryEditDlg (QWidget* parent = 0);
    void init (ResourceEditorTableModel* rem, AbstractResourceEntry* are = 0);

Q_SIGNALS:
    void updateEntry (AbstractResourceEntry* de);

public Q_SLOTS:

    void submit ();
    void checkValidity();

private:

    IdiomaticExpressionEntry* idiomaticExpressionEntry;
    bool isNew;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_IDIOMATICEXPRESSIONENTRYEDITDLG_H
