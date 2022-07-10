// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    void accept () override;
    void reject () override;
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
