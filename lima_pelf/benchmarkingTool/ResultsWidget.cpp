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
#include "ResultsWidget.h"
using namespace Lima::Pelf;

ResultsWidget::ResultsWidget (QWidget* p) :
    QTableView(p),
    pipeline(0),
    viewAction(new QAction(tr("View unit analysis..."), this)),
    commentAction(new QAction(tr("Change comment..."), this)),
    deleteAction(new QAction(tr("Delete"), this)),
    m_compareWithReferenceAction(new QAction(tr("Compare with reference..."), this)),
    m_compareWithPreviousAction(new QAction(tr("Compare with previous..."), this)),
    viewEnabled(false)
{
  connect(viewAction, SIGNAL(triggered()), this, SLOT(contextView()));
  connect(commentAction, SIGNAL(triggered()), this, SLOT(contextComment()));
  connect(deleteAction, SIGNAL(triggered()), this, SLOT(contextDelete()));
  connect(m_compareWithReferenceAction, SIGNAL(triggered()), this, SIGNAL(compareWithReference()));
  connect(m_compareWithPreviousAction, SIGNAL(triggered()), this, SIGNAL(compareWithPrevious()));
}

void ResultsWidget::init (Pipeline* p)
{
    pipeline = p;
}

void ResultsWidget::updateView ()
{
    ResultsModel* resultsModel = new ResultsModel(this, pipeline);
    ResultsModel* oldModel = (ResultsModel*)model();
    setModel(resultsModel);
    delete oldModel;
}

void ResultsWidget::keyPressEvent (QKeyEvent* event)
{
    if(event->key() == Qt::Key_Delete)
        contextDelete();
    else
        QTableView::keyPressEvent(event);
}

void ResultsWidget::contextMenuEvent (QContextMenuEvent* event)
{
    QModelIndex currentIndex = indexAt(event->pos());
    if(!currentIndex.isValid() || currentIndex.row() == -1)
        return;
    QMenu menu(this);
    if(viewEnabled && currentIndex.row() == (int)pipeline->getResultsVector().size() - 1)
        menu.addAction(viewAction);
    menu.addAction(commentAction);
    menu.addAction(deleteAction);
    menu.addSeparator();
    menu.addAction(m_compareWithReferenceAction);
    menu.addAction(m_compareWithPreviousAction);
    menu.exec(event->globalPos());
}

void ResultsWidget::contextComment ()
{
    if(selectedIndexes().size() <= 0)
        return;
    int row = selectedIndexes().first().row();
    QList<BenchmarkingResult*> resultsVector = pipeline->getResultsVector();
    if(row < 0 || row >= (int)resultsVector.size())
        return;
    BenchmarkingResult* benchmarkingResult = resultsVector[row];
///@TODO Possible memory leak:  ensure that this object will be deleted
    CommentEditDlg* commentEditDlg = new CommentEditDlg();
    commentEditDlg->init(benchmarkingResult);
}

void ResultsWidget::contextDelete ()
{
    if(selectedIndexes().size() <= 0)
        return;
    int row = selectedIndexes().first().row();
    QList<BenchmarkingResult*> resultsVector = pipeline->getResultsVector();
    if(row < 0 || row >= (int)resultsVector.size())
        return;
    QDateTime benchmarkingResultTime = resultsVector[row]->time;
    if(benchmarkingResultTime.isNull() || (benchmarkingResultTime == pipeline->startTime && pipeline->processing))
    {
        QMessageBox::warning(
            this,
            "Cannot delete benchmarking evaluation",
            "Impossible to delete this benchmarking evaluation, either it is currently running or it has not been found"
        );
        return;
    }
    if(QMessageBox::question(
        this,
        "Delete benchmark evaluation ?",
        "Do you really want to delete this benchmark evaluation results ?",
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel
      ) == QMessageBox::Ok)
    {
        pipeline->results.remove(benchmarkingResultTime);
        emit resultsChanged();
    }
}

void ResultsWidget::contextView ()
{
    if(selectedIndexes().size() <= 0)
        return;
    emit viewResult(selectedIndexes().first().row());
}


#include "ResultsWidget.moc"
