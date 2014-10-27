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
#ifndef LIMA_PELF_RESOURCEFILEEDITDLG_H
#define LIMA_PELF_RESOURCEFILEEDITDLG_H

#include <QtCore/QtDebug>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtWidgets/QDialog>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>

#include "ui_pelf-rt-editresourcetextfiledialog.h"

namespace Lima {
namespace Pelf {

class ResourceFileEditDlg : public QDialog, public Ui::ResourceTextFileEditDialog
{

Q_OBJECT

public:

    ResourceFileEditDlg (QWidget* parent = 0);
    void init (QString rfp, bool showDialog = true);
    void accept ();
    void reject ();
    void loadFile ();
    void setAsTabbedWidget ();

public Q_SLOTS:

    void saveFile ();

private Q_SLOTS:
    void slotSearchEntryEdited(const QString & text);
    
private:

    QString resourceFilePath;
    bool isTabbedWidget;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_RESOURCEFILEEDITDLG_H
