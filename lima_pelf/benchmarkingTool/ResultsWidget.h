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
#ifndef LIMA_PELF_RESULTSWIDGET_H
#define LIMA_PELF_RESULTSWIDGET_H

#include <QtCore/QtDebug>
#include <QtGui/QTableView>
#include <QtGui/QMenu>
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
