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
#include "PipelineEditFileDlg.h"
using namespace Lima::Pelf;

PipelineEditFileDlg::PipelineEditFileDlg (QWidget* p) :
    QDialog(p),
    Ui::PipelineEditFileDialog(),
    unitIndex(-1)
{
    setupUi(this);
}

void PipelineEditFileDlg::init (Pipeline* p)
{
    pipeline = p;
    connect(textFileBrowseBtn, SIGNAL(clicked()), this, SLOT(textFileBrowse()));
    connect(referenceFileBrowseBtn, SIGNAL(clicked()), this, SLOT(referenceFileBrowse()));
    connect(textFilePathInp, SIGNAL(textChanged(const QString&)), this, SLOT(checkFilePaths()));
    connect(referenceFilePathInp, SIGNAL(textChanged(const QString&)), this, SLOT(checkFilePaths()));
    connect(fileTypeCb, SIGNAL(currentIndexChanged(int)), this, SLOT(checkFilePaths()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(submit()));
    checkFilePaths();
    show();
}

void PipelineEditFileDlg::setUnit (PipelineUnit* unit, int index)
{
    unitIndex = index;
    textFilePathInp->setText(unit->getTextPath());
    referenceFilePathInp->setText(unit->referencePath);
    fileTypeCb->setCurrentIndex(fileTypeCb->findText(unit->fileType));
    checkFilePaths();
}

void PipelineEditFileDlg::textFileBrowse ()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Choose text file...",
        textFilePathInp->text(),
        "All Files (*)"
    );
    if(!fileName.isEmpty())
        textFilePathInp->setText(fileName);
}

void PipelineEditFileDlg::referenceFileBrowse ()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Choose reference file...",
        referenceFilePathInp->text(),
        "Easy reference file (*.easyref.xml);;All Files (*)"
    );
    if(!fileName.isEmpty())
        referenceFilePathInp->setText(fileName);
}

void PipelineEditFileDlg::checkFilePaths ()
{
    bool checked = true;
    QFileInfo fileInfo(textFilePathInp->text());
    checked = checked && fileInfo.exists() && fileInfo.isFile() && fileInfo.size() > 0;
    fileInfo = QFileInfo(referenceFilePathInp->text());
    checked = checked && fileInfo.exists() && fileInfo.isFile() && fileInfo.size() > 0;
    checked = checked && fileTypeCb->currentIndex() >= 0;
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checked);
}

void PipelineEditFileDlg::submit ()
{
    pipeline->replaceUnit(
        textFilePathInp->text(),
        referenceFilePathInp->text(),
        fileTypeCb->currentText(),
        PipelineUnit::STATUS_UNPROCESSED,
        unitIndex
    );
}

#include "PipelineEditFileDlg.moc"
