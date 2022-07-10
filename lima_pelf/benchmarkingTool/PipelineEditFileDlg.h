// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_PIPELINEEDITFILEDLG_H
#define LIMA_PELF_PIPELINEEDITFILEDLG_H

#include <QtCore/QtDebug>
#include <QtCore/QFileInfo>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFileDialog>

#include "ui_pelf-bt-editfiledialog.h"
#include "Pipeline.h"
#include "PipelineUnit.h"

namespace Lima {
namespace Pelf {

class PipelineEditFileDlg : public QDialog, public Ui::PipelineEditFileDialog
{

Q_OBJECT

public:

    int unitIndex;

    PipelineEditFileDlg (QWidget* parent = 0);
    void init (Pipeline* bp);
    void setUnit (PipelineUnit* unit, int index);

public Q_SLOTS:

    void textFileBrowse ();
    void referenceFileBrowse ();
    void checkFilePaths ();
    void submit ();

private:

    PipelineEditFileDialog ui;
    Pipeline* pipeline;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_PIPELINEEDITFILEDLG_H
