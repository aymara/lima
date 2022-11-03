// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_PIPELINECONFIGUREDLG_H
#define LIMA_PELF_PIPELINECONFIGUREDLG_H

#include <QtCore/QtDebug>
#include <QtCore/QFileInfo>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFileDialog>

#include "ui_pelf-bt-configurepipelinedialog.h"
#include "Pipeline.h"
#include "PipelineUnit.h"

namespace Lima {
namespace Pelf {

class PipelineConfigureDlg : public QDialog, public Ui::PipelineConfigureDialog
{

Q_OBJECT

public:

    int unitIndex;

    PipelineConfigureDlg (QWidget* parent = 0);
    void init (Pipeline* p, QString workingDir, QString analyzerCmd, QString evaluatorCmd, int cp);

public Q_SLOTS:

    void workingDirBrowse ();
    void analyzerCmdBrowse ();
    void evaluatorCmdBrowse ();
    void checkFilePaths ();
    void submit ();

private:

    PipelineConfigureDialog ui;
    Pipeline* pipeline;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_PIPELINECONFIGUREDLG_H
