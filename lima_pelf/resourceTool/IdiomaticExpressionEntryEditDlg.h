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
