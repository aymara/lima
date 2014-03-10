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
#include "PipelineConfigureDlg.h"
using namespace Lima::Pelf;

PipelineConfigureDlg::PipelineConfigureDlg (QWidget* p) :
    QDialog(p),
    Ui::PipelineConfigureDialog()
{
    setupUi(this);
}

void PipelineConfigureDlg::init (Pipeline* p, QString workingDir, QString analyzerCmd, QString evaluatorCmd, int cp)
{
    pipeline = p;
    workingDirPathInp->setText(workingDir);
    connect(workingDirBrowseBtn, SIGNAL(clicked()), this, SLOT(workingDirBrowse()));
    connect(workingDirPathInp, SIGNAL(textChanged(const QString&)), this, SLOT(checkFilePaths()));
    analyzerCmdPathInp->setText(analyzerCmd);
    connect(analyzerCmdBrowseBtn, SIGNAL(clicked()), this, SLOT(analyzerCmdBrowse()));
    connect(analyzerCmdPathInp, SIGNAL(textChanged(const QString&)), this, SLOT(checkFilePaths()));
    evaluatorCmdPathInp->setText(evaluatorCmd);
    connect(evaluatorCmdBrowseBtn, SIGNAL(clicked()), this, SLOT(evaluatorCmdBrowse()));
    connect(evaluatorCmdPathInp, SIGNAL(textChanged(const QString&)), this, SLOT(checkFilePaths()));
    concurrentProcesses->setValue(cp);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(submit()));
    checkFilePaths();
    show();
}

void PipelineConfigureDlg::workingDirBrowse ()
{
    QString fileName = QFileDialog::getExistingDirectory(
        this,
        "Choose working directory...",
        workingDirPathInp->text(),
        QFileDialog::ShowDirsOnly
    );
    if(!fileName.isNull())
        workingDirPathInp->setText(fileName);
}

void PipelineConfigureDlg::analyzerCmdBrowse ()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Choose analyzer command...",
        analyzerCmdPathInp->text(),
        "All Files (*)"
    );
    if(!fileName.isEmpty())
        analyzerCmdPathInp->setText(fileName);
}

void PipelineConfigureDlg::evaluatorCmdBrowse ()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Choose evaluator command...",
        evaluatorCmdPathInp->text(),
        "All Files (*)"
    );
    if(!fileName.isEmpty())
        evaluatorCmdPathInp->setText(fileName);
}

void PipelineConfigureDlg::checkFilePaths ()
{
    bool checked = true;
    if(!workingDirPathInp->text().isEmpty())
    {
        QFileInfo fileInfo(workingDirPathInp->text());
        checked = checked && fileInfo.exists() && fileInfo.isDir();
    }
    checked = checked && !analyzerCmdPathInp->text().isEmpty();
//     checked = checked && !evaluatorCmdPathInp->text().isEmpty();
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checked);
}

void PipelineConfigureDlg::submit ()
{
    pipeline->replaceConfiguration(
        workingDirPathInp->text(),
        analyzerCmdPathInp->text(),
        evaluatorCmdPathInp->text(),
        concurrentProcesses->value()
    );
}

#include "PipelineConfigureDlg.moc"
