// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "CommentEditDlg.h"
using namespace Lima::Pelf;

CommentEditDlg::CommentEditDlg (QWidget* p) :
    QDialog(p),
    Ui::CommentEditDialog()
{
    setupUi(this);
}

void CommentEditDlg::init (BenchmarkingResult* br, Pipeline* p)
{
    benchmarkingResult = br;
    pipeline = p;
    if(pipeline != 0)
    {
        setWindowTitle("New benchmark evaluation");
        commentLabel->setText("Starting a new benchmark evaluation.\n"+commentLabel->text());
    }
    commentEdit->setAcceptRichText(false);
    commentEdit->setPlainText(benchmarkingResult->comment);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(submit()));
    show();
}

void CommentEditDlg::submit ()
{
    benchmarkingResult->comment = commentEdit->toPlainText();
    if(pipeline != 0)
        pipeline->startBenchmarking(benchmarkingResult);
}

#include "CommentEditDlg.moc"
