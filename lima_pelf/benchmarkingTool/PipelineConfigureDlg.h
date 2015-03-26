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

public slots:

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
