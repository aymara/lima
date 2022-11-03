// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_COMMENTEDITDLG_H
#define LIMA_PELF_COMMENTEDITDLG_H

#include <QtCore/QtDebug>
#include <QtWidgets/QDialog>

#include "BenchmarkingResult.h"
#include "Pipeline.h"

#include "ui_pelf-bt-commenteditdialog.h"

namespace Lima {
namespace Pelf {

class CommentEditDlg : public QDialog, public Ui::CommentEditDialog
{

Q_OBJECT

public:

    CommentEditDlg (QWidget* parent = 0);
    void init (BenchmarkingResult* br, Pipeline* p = 0);

public Q_SLOTS:

    void submit ();

private:

    CommentEditDialog ui;
    BenchmarkingResult* benchmarkingResult;
    Pipeline* pipeline;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_COMMENTEDITDLG_H
