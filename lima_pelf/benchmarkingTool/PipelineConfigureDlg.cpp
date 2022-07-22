// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
