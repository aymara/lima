// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_RESULTSWIDGET_H
#define LIMA_PELF_RESULTSWIDGET_H

#include <QtCore/QtDebug>
#include <QtWidgets/QTableView>
#include <QtWidgets/QMenu>
#include <QtGui/QContextMenuEvent>

#include "Pipeline.h"
#include "ResultsModel.h"
#include "BenchmarkingResult.h"
#include "CommentEditDlg.h"

namespace Lima {
namespace Pelf {

class ResultsWidget : public QTableView
{

Q_OBJECT

public:

    ResultsWidget (QWidget* parent);
    void init (Pipeline* p);
    void updateView ();
    void keyPressEvent (QKeyEvent* event);
    void contextMenuEvent (QContextMenuEvent* event);
    inline void setViewEnabled(bool enabled) {viewEnabled = enabled;}
    
public Q_SLOTS:

    void contextView ();
    void contextComment ();
    void contextDelete ();
    
Q_SIGNALS:

    void resultsChanged ();
    void viewResult (int row);
    void compareWithPrevious();
    void compareWithReference();

    private:

    Pipeline* pipeline;
    QAction* viewAction;
    QAction* commentAction;
    QAction* deleteAction;
    QAction* m_compareWithReferenceAction;
    QAction* m_compareWithPreviousAction;

    bool viewEnabled;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_RESULTSWIDGET_H
