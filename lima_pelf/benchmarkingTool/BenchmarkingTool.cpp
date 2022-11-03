// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "BenchmarkingTool.h"
#include "BenchmarkingXmlReader.h"
#include "BenchmarkingXmlWriter.h"
#include "syanotprocess.h"
#include "Pipeline.h"
#include "PipelineUnit.h"
#include "PipelineEditFileDlg.h"
#include "PipelineConfigureDlg.h"
#include "EvaluationResult.h"
#include "CommentEditDlg.h"
#include "ResultsModel.h"

#include <QtCore/QTemporaryFile>
#include <QtCore/QtDebug>
#include <QtCore/QSettings>
#include <QtCore/QFile>
#include <QtCore/QSharedMemory>
#include <QtCore/QTimer>
#include <QtCore/QProcess>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QDirModel>
#include <QtWidgets/QLabel>
#include <QtWidgets/QCheckBox>

#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_marker.h>

using namespace Lima::Pelf;

BenchmarkingTool::BenchmarkingTool (QWidget* p) :
    QMainWindow(p),
    Ui::BenchmarkingToolWindow(),
    pelfSharedMemory(new QSharedMemory("PelfRtBt", this)),
    currentViewSelection(-1),
    viewerCommandLine(QString()),
    m_comparingWithPrevious(false),
    m_currentBenchmarkingResult(0),
    m_previousBenchmarkingResult(0),
    m_oneShotAnalysisProcess(new QProcess(this)),
    m_showErrorStatementDetailsProcess(new QProcess(this)),
    m_viewProcess(new QProcess(this)),
    m_tmpFile(0)
{
  settings = new QSettings("CEA LIST LIMA", "Pelf Benchmarking Tool", this);
  setupUi(this);
  createActions();
  createMenus();
  (void)statusBar();
  setWindowIcon(QIcon(":/hi64-app-pelf-bench.png"));

  connect(m_oneShotAnalysisProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(commandFinished(int, QProcess::ExitStatus)));
  connect(m_oneShotAnalysisProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(commandError(QProcess::ProcessError)));

  m_showErrorStatementDetailsProcess->setProcessChannelMode(QProcess::MergedChannels);
  connect(m_showErrorStatementDetailsProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(commandFinished(int, QProcess::ExitStatus)));
  connect(m_showErrorStatementDetailsProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(commandError(QProcess::ProcessError)));

  connect(m_viewProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(commandFinished(int, QProcess::ExitStatus)));
  connect(m_viewProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(commandError(QProcess::ProcessError)));
}

BenchmarkingTool::~BenchmarkingTool()
{
  delete settings;
}

void BenchmarkingTool::init ()
{
    qDebug() << "Starting application, loading settings...";
    restoreGeometry(settings->value("geometry").toByteArray());
    EvaluationResult::initDimensions();
    initPelfSharedMemory();
    pipelineView->init();
    pipeline = pipelineView->pipeline;
    connect(pipeline, SIGNAL(unitsChanged()), this, SLOT(pipelineUnitsChanged()));
    connect(pipeline, SIGNAL(resultsChanged()), this, SLOT(updateResultsViews()));
    connect(pipeline, SIGNAL(finishedBenchmarking()), this, SLOT(pipelineFinisehdBenchmarking()));
    resultsView->init(pipeline);

    connect(resultsView, SIGNAL(resultsChanged()), this, SLOT(updateResultsViews()));
    connect(resultsView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(updateResultsViewSelection()));
    connect(resultsView->verticalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(slotResultsViewVerticalHeaderSectionClicked(int)));
    connect(resultsView, SIGNAL(compareWithPrevious()), this, SLOT(slotCompareWithPrevious()));;
    connect(resultsView, SIGNAL(compareWithReference()), this, SLOT(slotCompareWithReference()));

    connect(resultsView, SIGNAL(viewResult (int)), this, SLOT(viewResult(int)));
    connect(pipelineUnitDisplayCb, SIGNAL(currentIndexChanged(int)), this, SLOT(updateResultsViews()));
    connect(measureDisplayCb, SIGNAL(currentIndexChanged(int)), this, SLOT(updateResultsTableView()));
    updateResultsTableView();
    resetEvaluationCurves();
    updateDimensionsWidgets();
    show();

    connect(pipelineView, SIGNAL(clicked(QModelIndex)),this, SLOT(slotPipelineUnitActivated(QModelIndex)));
    connect(statementsTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)),this, SLOT(slotErrorStatementActivated(QTreeWidgetItem*, int)));
    connect(statementsTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this, SLOT(slotShowErrorStatementDetails(QTreeWidgetItem*, int)));
    connect(launchAnalysisButton, SIGNAL(clicked()), this, SLOT(slotOneShotAnalysis()));
    connect(newFileButton, SIGNAL(clicked()), this, SLOT(slotNewTextFile()));
    connect(choseFileButton, SIGNAL(clicked()), this, SLOT(slotChoseTextFile()));
    connect(saveFileButton, SIGNAL(clicked()), this, SLOT(slotSaveTextFile()));
    connect(recentFilesList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(slotTextFileActivated(QListWidgetItem*)));
    connect(removeFileButton, SIGNAL(clicked()), this, SLOT(slotRemoveTextFile()));

    pipelineSplitter->restoreState(settings->value ("pipelineSplitter").toByteArray());
    outputSplitter->restoreState(settings->value ("outputSplitter").toByteArray());
    textFilesSplitter->restoreState(settings->value ("textFilesSplitter").toByteArray());
    errorsSplitter->restoreState(settings->value ("errorsSplitter").toByteArray());
    measuresSplitter->restoreState(settings->value ("measuresSplitter").toByteArray());
    pipeGraphsSplitter->restoreState(settings->value ("pipeGraphsSplitter").toByteArray());

    QStringList textFiles = settings->value ("textFiles").toStringList();
    Q_FOREACH (QString textFile, textFiles)
    {
      recentFilesList->addItem(textFile);
    }

    qDebug() << "Ready";
}

void BenchmarkingTool::slotPipelineUnitActivated(const QModelIndex& index)
{
  qDebug() << "BenchmarkingTool::slotPipelineUnitActivated" << index.data().toString();

  // seulement si index different de courant
  if (index != m_currentPipelineUnit)
  {
    m_currentPipelineUnit = index;
  }
}

void BenchmarkingTool::updateErrorsWidget()
{
  QString selectedUnitTextPath = pipelineUnitDisplayCb->currentIndex()==0
      ? "" : pipelineUnitDisplayCb->currentText();
  errorsTable->clearContents();
  errorsTable->setRowCount(0);
  statementsTree->clear();
  m_comparingWithPrevious = false;
  m_currentBenchmarkingResult = 0;
  m_previousBenchmarkingResult = 0;

  auto errors = utterancesWithErrors(selectedUnitTextPath);
//   qDebug() << "BenchmarkingTool::updateErrorsWidget utterancesWithErrors:" << errors.size();
  for (const auto& key: errors.keys().toSet())
  {
    auto list = errors.values(key);
    qSort(list);
    for(const auto& value: list)
    {
//       qDebug() << "add child item" << key << value;
      auto childItem = new QTreeWidgetItem();
      childItem->setText(0,key);
      childItem->setText(1,value);
      //childItem->setText(2,"Text to load...");
      statementsTree->addTopLevelItem(childItem);
    }
  }
  statementsTree->sortByColumn(0, Qt::AscendingOrder);
}

void BenchmarkingTool::slotErrorStatementActivated(QTreeWidgetItem* item, int column)
{
  qDebug() << "BenchmarkingTool::slotErrorStatementActivated "
            << item->data(column,Qt::DisplayRole).toString() << " " << column;
  if (item->data(1, Qt::DisplayRole).toString() == "") return;

  // vider le tableau des erreurs
  //qDebug() << "clearing errors list";
  errorsTable->clearContents();
  errorsTable->setRowCount(0);

  auto statementId = item->data(1,Qt::DisplayRole).toString();

  //qDebug() << "BenchmarkingTool::slotErrorStatementActivated " << pipeline->results.size() << " results";
  // obtenir la liste des erreurs pour ce pipeline unit
  // pour chacune, creer si necessaire une entree dans la liste des enonces
  // remplir une ligne de la table
  auto pipelineName = item->data(0,Qt::DisplayRole).toString();

  if (!m_comparingWithPrevious)
  {
    if (pipeline->results.contains(pipeline->startTime))
    {
        auto benchResult = (pipeline->results)[pipeline->startTime];
        auto& puResult = benchResult->resultUnits;
        qDebug() << "BenchmarkingTool::slotErrorStatementActivated "
                  << puResult.size() << " pu results";
        for(auto& pipelineUnit: puResult.keys())
        {
  //           qDebug() << "selectedUnitTextPath: " << selectedUnitTextPath << "; pipelineName: " << pipelineName;
            if (pipelineName != pipelineUnit->name) continue;
            auto resultSet = puResult[pipelineUnit];
            qDebug() << "BenchmarkingTool::slotErrorStatementActivated result set size: "
                      << resultSet->size();
            for (auto& dimensionId: resultSet->keys())
            {
                auto dimEvalResult = (*resultSet)[dimensionId];
                addTypedErrors(statementId,dimensionId,tr("Not found"),
                               dimEvalResult->getRefAbsentFromHyp());
                addTypedErrors(statementId,dimensionId,tr("Found incorrect"),
                               dimEvalResult->getHypAbsentFromRef());
                addTypedErrors(statementId,dimensionId,tr("Wrong type"),
                               dimEvalResult->getTypeError());
            }
        }
    }
  }
  else
  {
    auto errors = getErrors(pipelineName, statementId,
                            m_currentBenchmarkingResult);
    qDebug() << "errors" << pipelineName << statementId << ":" << errors;
    auto previousErrors = getErrors(pipelineName, statementId,
                                    m_previousBenchmarkingResult);
    qDebug() << "previousErrors" << pipelineName << statementId << ":"
              << previousErrors;

    for (const auto& list: errors)
    {
      if (!previousErrors.contains(list))
      {
        auto typeText = list[0];
        auto dimension = list[1];
        auto name = list[2];
        addError(typeText, dimension, name);

      }
    }
    for (const auto& list: previousErrors)
    {
      if (!errors.contains(list))
      {
        auto typeText = list[0];
        auto dimension = list[1];
        auto name = list[2];
        addError(typeText, dimension, name);
      }
    }
  }
}

void BenchmarkingTool::slotShowErrorStatementDetails(QTreeWidgetItem* item, int column)
{
  qDebug() << "BenchmarkingTool::slotShowErrorStatementDetails: should launch analysis and display its details of: "
    << item->data(1,Qt::DisplayRole).toString() << " " << column;

  QString statementId = item->data(1,Qt::DisplayRole).toString();
  if (statementId=="") return;

//   QMessageBox::information(this,tr("Not yet implemented"),
//       tr("Will display result analysis and reference for statement of current pipeline unit: ")+statementId,QMessageBox::Close);
//   return;

  // recuperer nom du pipeline unit, nom de l'enonce et chemin de base du pipeline unit
  qDebug() << "BenchmarkingTool::slotShowErrorStatementDetails working dir is: " << pipeline->workingDir;
  QString pipelineUnit = m_currentPipelineUnit.data().toString();
  QStringList arguments;
  arguments.push_back(pipelineUnit);
  arguments.push_back(statementId);
  qDebug() << "BenchmarkingTool::slotShowErrorStatementDetails arguments are: " << arguments;
  // lancer la commande externe qui:
  // affichera avec syanot les version systeme et reference du pipeline unit
  // extraira la phrase du source
  // lancera l'analyse en mode fullXmlDumper, pipeline easy, langue fre
  // affiche si disponible
  //    dans kgraphviewer les graphes avant desambiguisation, apres analyse syntaxique
  //    dans kate, la sortie du fullXmlDumper
  m_showErrorStatementDetailsProcess->setWorkingDirectory(getenv("PELF_COMMANDS_ROOT"));
  m_showErrorStatementDetailsProcess->start("launchAnalysis.sh", arguments);
}

void BenchmarkingTool::createActions()
 {
     for (int i = 0; i < MaxRecentFiles; ++i)
     {
         recentFileActs[i] = new QAction(this);
         recentFileActs[i]->setVisible(false);
         connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
     }
 }

 void BenchmarkingTool::createMenus()
 {
     QMenu* recentFilesMenu = menuBar()->addMenu(tr("&Recent"));
     for (int i = 0; i < MaxRecentFiles; ++i)
         recentFilesMenu->addAction(recentFileActs[i]);
     updateRecentFileActions();
 }

void BenchmarkingTool::updateRecentFileActions()
{
    QStringList files = settings->value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);
}

QString BenchmarkingTool::strippedName(const QString &fullFileName)
{
    return fullFileName;
//    return QFileInfo(fullFileName).fileName();
}

void BenchmarkingTool::resetEvaluationCurves ()
{
//     fmeasureQwtPlot->clear();
//     precisionQwtPlot->clear();
//     recallQwtPlot->clear();
    evaluationResultTypeQwtCurves.clear();
    QMap<EvaluationResult::DIMENSION_ID, EvaluationResultDimension*>& dimensions = EvaluationResult::getDimensions();
    Q_FOREACH(EvaluationResult::DIMENSION_ID dimensionId, dimensions.keys())
//     dimensionsIt = dimensions.begin(); dimensionsIt != dimensions.end(); dimensionsIt++)
    {
        EvaluationResultDimension* dimension = dimensions[dimensionId];
///@TODO Possible memory leak:  ensure that this object will be deleted
        QwtPlotCurve* fmeasureQwtCurve = new QwtPlotCurve("F Measure - "+dimension->name);
        fmeasureQwtCurve->setPen(QPen(dimension->color));
        fmeasureQwtCurve->attach(fmeasureQwtPlot);
        evaluationResultTypeQwtCurves[dimensionId].push_back(fmeasureQwtCurve);
///@TODO Possible memory leak:  ensure that this object will be deleted
        QwtPlotCurve* precisionQwtCurve = new QwtPlotCurve("Precision - "+dimension->name);
        precisionQwtCurve->setPen(QPen(dimension->color));
        precisionQwtCurve->attach(precisionQwtPlot);
        evaluationResultTypeQwtCurves[dimensionId].push_back(precisionQwtCurve);
///@TODO Possible memory leak:  ensure that this object will be deleted
        QwtPlotCurve* recallQwtCurve = new QwtPlotCurve("Recall - "+dimension->name);
        recallQwtCurve->setPen(QPen(dimension->color));
        recallQwtCurve->attach(recallQwtPlot);
        evaluationResultTypeQwtCurves[dimensionId].push_back(recallQwtCurve);
    }
    updateEvaluationCurves();
}

void BenchmarkingTool::updateEvaluationCurves (int nbRes)
{
    if(currentViewSelection > nbRes && nbRes > 0)
        currentViewSelection = nbRes;
    double viewedRes = currentViewSelection != -1 ? currentViewSelection + 1 : nbRes;
    if(viewedRes < 1)
        viewedRes = 1;
    double showedRes = 5;
    double firstShowedRes = viewedRes > showedRes ? 0.5 + viewedRes - showedRes : 0.5;
    fmeasureQwtPlot->setAxisScale(QwtPlot::yLeft, 0, 1);
    fmeasureQwtPlot->setAxisScale(QwtPlot::xBottom, firstShowedRes, showedRes + firstShowedRes - 1, 1);
    fmeasureQwtPlot->setAxisMaxMinor(QwtPlot::xBottom, 0);
    fmeasureQwtPlot->replot();
    precisionQwtPlot->setAxisScale(QwtPlot::yLeft, 0, 1);
    precisionQwtPlot->setAxisScale(QwtPlot::xBottom, firstShowedRes, showedRes + firstShowedRes - 1, 1);
    precisionQwtPlot->setAxisMaxMinor(QwtPlot::xBottom, 0);
    precisionQwtPlot->replot();
    recallQwtPlot->setAxisScale(QwtPlot::yLeft, 0, 1);
    recallQwtPlot->setAxisScale(QwtPlot::xBottom, firstShowedRes, showedRes + firstShowedRes - 1, 1);
    recallQwtPlot->setAxisMaxMinor(QwtPlot::xBottom, 0);
    recallQwtPlot->replot();
}

void BenchmarkingTool::updateDimensionsWidgets ()
{
    QGridLayout* dimensionsConstsLayout = new QGridLayout(this);
    QGridLayout* dimensionsRelsLayout = new QGridLayout(this);
    int dimConstsNb = -1, dimRelsNb = -1, cellsPerline = 16;
    QCheckBox* dimensionCheckBox;
    QLabel* dimensionLabel;
    QMap<EvaluationResult::DIMENSION_ID, EvaluationResultDimension*>& dimensions = EvaluationResult::getDimensions();
    Q_FOREACH(EvaluationResult::DIMENSION_ID dimensionId, dimensions.keys())
    {
        EvaluationResultDimension* dimension = dimensions[dimensionId];
        dimensionCheckBox = new QCheckBox(this);
        dimensionCheckBox->setTristate(true);
        dimensionCheckBox->setCheckState(dimension->visibilityState);
        connect(dimensionCheckBox, SIGNAL(stateChanged(int)), dimension, SLOT(updateVisibleChanged(int)));
        connect(dimension, SIGNAL(visibleChanged()), this, SLOT(updateResultsViews()));
        dimensionLabel = new QLabel("<font color=\""+dimension->color.name()+"\">"+dimension->name+"</font>", this);
        QGridLayout* currentLayout = dimension->rel ? dimensionsRelsLayout : dimensionsConstsLayout;
        int dimNb = dimension->rel ? ++dimRelsNb : ++dimConstsNb;
        currentLayout->addWidget(dimensionCheckBox, 2*dimNb/cellsPerline, 2*dimNb%cellsPerline, Qt::AlignRight);
        currentLayout->addWidget(dimensionLabel, 2*dimNb/cellsPerline, 2*dimNb%cellsPerline + 1);
    }
    dimensionsConstsWidget->hide();
    delete dimensionsConstsWidget->layout();
    dimensionsConstsWidget->setLayout(dimensionsConstsLayout);
    dimensionsConstsWidget->show();
    dimensionsRelsWidget->hide();
    delete dimensionsRelsWidget->layout();
    dimensionsRelsWidget->setLayout(dimensionsRelsLayout);
    dimensionsRelsWidget->show();
}

void BenchmarkingTool::closeEvent (QCloseEvent *event)
{
    if(!confirmAbandonModifications())
    {
        event->ignore();
        return;
    }
    qDebug() << "Closing application, saving settings";
    settings->setValue("geometry", saveGeometry());
    settings->setValue("pipelineSplitter", pipelineSplitter->saveState());
    settings->setValue("outputSplitter", outputSplitter->saveState());
    settings->setValue("textFilesSplitter", textFilesSplitter->saveState());
    settings->setValue("errorsSplitter", errorsSplitter->saveState());
    settings->setValue("measuresSplitter", measuresSplitter->saveState());
    settings->setValue("pipeGraphsSplitter", pipeGraphsSplitter->saveState());

    QStringList textFiles;
    for (int i = 0; i < recentFilesList->count(); i++)
    {
      textFiles.push_back(recentFilesList->item(i)->text());
    }
    settings->setValue("textFiles", textFiles);
    QMainWindow::closeEvent(event);
}

void BenchmarkingTool::logDebugMsg (QtMsgType type, const char* m)
{
    QString msgHtml;
    QString msg = QString::fromUtf8(m);
    switch (type)
    {
        case QtDebugMsg:
            msgHtml = "<font style=\"color: blue;\">"+msg +"</font>";
            break;
        case QtWarningMsg:
            msgHtml = "<font style=\"color: orange;\">"+msg+"</font>";
            break;
        case QtCriticalMsg:
            msgHtml = "<font style=\"color: red;\">"+msg+"</font>";
            break;
        case QtFatalMsg:
            msgHtml = "<font style=\"color: red;\">"+msg+"</font>";
            abort();
    }
    logTextBrowser->append(msgHtml);
}

void BenchmarkingTool::pipelineUnitsChanged ()
{
    pipelineUnitDisplayCb->clear();
    pipelineUnitDisplayCb->addItem("All pipeline units");
    const QList<PipelineUnit*>& pipelineUnits = pipeline->getUnits();
    int unitId = 0;
    Q_FOREACH(PipelineUnit* unit, pipelineUnits)
    {
        pipelineUnitDisplayCb->addItem(unit->name, unitId);
        unitId++;
    }
}

void BenchmarkingTool::updateResultsViews()
{
//     qDebug() << "BenchmarkingTool::updateResultsViews";
    resetEvaluationCurves();
    int nbRes = pipeline->results.size() + 1;
    QList<EvaluationResultDimension*> dimensions = EvaluationResult::getDimensionsVisible();
    QString selectedUnitTextPath;
    resultsView->setViewEnabled(pipelineUnitDisplayCb->currentIndex() > 0 && !viewerCommandLine.isEmpty());
    if(pipelineUnitDisplayCb->currentIndex() > 0)
        selectedUnitTextPath = pipeline->getUnits()[pipelineUnitDisplayCb->currentIndex() - 1]->textPath;
    ResultsModel::selectedUnitTextPath = selectedUnitTextPath;
    Q_FOREACH(EvaluationResultDimension* dimension, dimensions)
    {
        EvaluationResult::DIMENSION_ID dimensionId = (EvaluationResult::DIMENSION_ID)dimension->id;
        int noResults = 1;
        double xFmeasure[nbRes], yFmeasure[nbRes], xPrecision[nbRes], yPrecision[nbRes], xRecall[nbRes], yRecall[nbRes];
        Q_FOREACH (BenchmarkingResult* result, pipeline->results)
        {
            QMap<PipelineUnit*, EvaluationResultSet*>& puResult = result->resultUnits;
            int nbPus = 0;
            double sumFc = 0, sumFp = 0, sumCr = 0;
            Q_FOREACH (PipelineUnit* pipelineUnit, puResult.keys())
            {
                if(selectedUnitTextPath.isEmpty() || selectedUnitTextPath == pipelineUnit->textPath)
                {
                    EvaluationResultSet* resultSet = puResult[pipelineUnit];
                    if(resultSet->contains(dimensionId))
                    {
                        EvaluationResult* result = (*resultSet)[dimensionId];
                        sumFc += result->fc;
                        sumFp += result->fp;
                        sumCr += result->cr;
                        nbPus++;
                    }
                }
            }
            xFmeasure[noResults] = xPrecision[noResults] = xRecall[noResults] = noResults;
            yPrecision[noResults] = sumFp != 0 ? sumFc/sumFp : 0;
            yRecall[noResults] = sumCr != 0 ? sumFc/sumCr : 0;
            if(yPrecision[noResults] + yRecall[noResults] == 0)
                yFmeasure[noResults] = 0;
            else
                yFmeasure[noResults] = 2*yPrecision[noResults]*yRecall[noResults]/(yPrecision[noResults] + yRecall[noResults]);
            QwtPlotMarker* marker;
            QwtSymbol* symbol = dimension->markerSymbol;
            if(noResults == currentViewSelection)
                symbol = dimension->markerSymbolSelected;
///@TODO Possible memory leak:  ensure that this object will be deleted
            marker = new QwtPlotMarker();
            marker->setSymbol(symbol);
            marker->setXValue(noResults);
            marker->setYValue(yFmeasure[noResults]);
            marker->attach(fmeasureQwtPlot);
///@TODO Possible memory leak:  ensure that this object will be deleted
            marker = new QwtPlotMarker();
            marker->setSymbol(symbol);
            marker->setXValue(noResults);
            marker->setYValue(yPrecision[noResults]);
            marker->attach(precisionQwtPlot);
///@TODO Possible memory leak:  ensure that this object will be deleted
            marker = new QwtPlotMarker();
            marker->setSymbol(symbol);
            marker->setXValue(noResults);
            marker->setYValue(yRecall[noResults]);
            marker->attach(recallQwtPlot);
            noResults++;
        }
        yFmeasure[0] = yFmeasure[1];
        yPrecision[0] = yPrecision[1];
        yRecall[0] = yRecall[1];
        xFmeasure[0] = xPrecision[0] = xRecall[0] = 0.5;
// This was working with previous versions of Qwt. To be updated.
//         evaluationResultTypeQwtCurves[dimensionId][0]->setData(xFmeasure, yFmeasure, noResults);
//         evaluationResultTypeQwtCurves[dimensionId][1]->setData(xPrecision, yPrecision, noResults);
//         evaluationResultTypeQwtCurves[dimensionId][2]->setData(xRecall, yRecall, noResults);
    }
    updateEvaluationCurves(nbRes);
    updateResultsTableView();
    updateErrorsWidget();
}

void BenchmarkingTool::updateResultsTableView ()
{
    ResultsModel::measureDisplay = (ResultsModel::MEASURE_DISPLAY)measureDisplayCb->currentIndex();
    resultsView->updateView();
    resultsView->resizeColumnsToContents();
}

void BenchmarkingTool::updateResultsViewSelection ()
{
    qDebug() << "BenchmarkingTool::updateResultsViewSelection";
    if(resultsView->selectionModel()->selectedRows(0).size() <= 0)
        return;
    if (currentViewSelection != resultsView->selectionModel()->selectedRows(0).first().row() + 1)
    {
      currentViewSelection = resultsView->selectionModel()->selectedRows(0).first().row() + 1;
      updateErrorsWidget();
    }
}

void BenchmarkingTool::viewResult (int row)
{
    if(viewerCommandLine.isEmpty())
        return;
    QList<BenchmarkingResult*> resultsVector = pipeline->getResultsVector();
    if(row < 0 || row >= (int)resultsVector.size())
        return;
    PipelineUnit* selectedUnit = pipeline->getUnits()[pipelineUnitDisplayCb->currentIndex() - 1];
    QString commandLine = viewerCommandLine;
    commandLine.replace("$TEXT_PATH", selectedUnit->textPath);
    commandLine.replace("$REFERENCE_PATH", selectedUnit->referencePath);
    m_viewProcess->start(commandLine);
}

void BenchmarkingTool::fileNew ()
{
    if(!confirmAbandonModifications())
        return;
    qDebug() << "BenchmarkingTool::fileNew New benchmarking";
    qDebug() << "BenchmarkingTool::fileNew clearing";
    pipeline->clear();
    qDebug() << "BenchmarkingTool::fileNew reset file name";
    currentFileName = "";
    qDebug() << "BenchmarkingTool::fileNew calling resetViews";
    resetViews();
}

void BenchmarkingTool::fileLoad ()
{
    if(!confirmAbandonModifications())
        return;
    qDebug() << "Loading benchmarking";
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Load benchmarking pipeline",
        "",
        "Pelf Benchmarking Pipeline (*.lbp);;All Files (*)"
    );
    if(fileName.isEmpty())
        return;
    loadFileStream(fileName);
}

void BenchmarkingTool::loadFileStream(const QString& fileName)
{
    pipeline->clear();
    BenchmarkingXmlReader reader(pipeline);
    if(!reader.read(fileName))
    {
        qDebug() << "Unable to parse xml file, aborting";
        return;
    }

    QStringList files = settings->value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings->setValue("recentFileList", files);
    updateRecentFileActions();

    resetViews();
    currentFileName = fileName;
    qDebug() << "Benchmarking loaded";
}

void BenchmarkingTool::openRecentFile()
{
    if(!confirmAbandonModifications())
        return;
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        loadFileStream(action->data().toString());
}

void BenchmarkingTool::fileSave ()
{
    if(currentFileName.isEmpty())
    {
        fileSaveAs();
        return;
    }
    QFile dumpFile(currentFileName);
    if(!dumpFile.open(QFile::WriteOnly | QFile::Truncate)){
        qDebug() << "Unable to open file, aborting";
        return;
    }
    qDebug() << "Saving benchmarking";
    BenchmarkingXmlWriter(pipeline, viewerCommandLine, &dumpFile).write();
    dumpFile.close();
    qDebug() << "Benchmarking saved";
}

void BenchmarkingTool::fileSaveAs ()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Save benchmarking pipeline",
        "",
        "Pelf Benchmarking Pipeline (*.lbp);;All Files (*)"
    );
    if(fileName.isEmpty())
        return;
    currentFileName = fileName;
    fileSave();
}

bool BenchmarkingTool::confirmAbandonModifications ()
{
  if(pipeline->dirty())
  {
    return QMessageBox::question(
        this,
        "Close benchmarking",
        "The benchmarking has been changed, do you really want to abandon modifications ?",
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel
      ) == QMessageBox::Ok;
  }
  return true;
}

void BenchmarkingTool::resetViews()
{
    qDebug() << "BenchmarkingTool::resetViews";
    pipelineView->reset();
    updateResultsViews();
    errorsTable->clearContents();
    errorsTable->setRowCount(0);
}

void BenchmarkingTool::pipelineAddFile ()
{
///@TODO Possible memory leak:  ensure that this object will be deleted
    PipelineEditFileDlg* pipelineEditFileDlg = new PipelineEditFileDlg();
    pipelineEditFileDlg->init(pipeline);
}

void BenchmarkingTool::pipelineConfigure ()
{
    qDebug() << "Configuring pipeline...";
///@TODO Possible memory leak:  ensure that this object will be deleted
    PipelineConfigureDlg* pipelineConfigureDlg = new PipelineConfigureDlg();
    pipelineConfigureDlg->init(
        pipeline,
        pipeline->workingDir,
        pipeline->analyzerCommand->commandLine,
        pipeline->evaluatorCommand->commandLine,
        pipeline->concurrentProcesses
    );
}

void BenchmarkingTool::pipelineClearFiles ()
{
    if(pipeline->isEmpty())
        return;
    if(QMessageBox::question(
        this,
        "Clear pipeline files ?",
        "Do you really want to delete all files from pipeline ?",
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel
      ) == QMessageBox::Ok)
    {
        pipeline->clearUnits();
        qDebug() << "Pipeline files cleared";
    }
}

void BenchmarkingTool::pipelineFinisehdBenchmarking ()
{
    currentViewSelection = -1;
    updateResultsViews();
    pipelineView->reset();
    sendResourceToolMsg("RTCMD:EVALUATION_FINISHED;");
    fileSave();
}

bool BenchmarkingTool::checkIsEmpty ()
{
    if(pipeline->isEmpty())
    {
        QMessageBox::warning(
            this,
            "Pipeline is empty",
            "No files into pipeline, please add files before trying to benchmark anything"
        );
        return false;
    }
    return true;
}

void BenchmarkingTool::pipelineStartBenchmarking ()
{
    if(!checkIsEmpty() || !pipeline->resetBenchmarking())
    {
        sendResourceToolMsg("RTCMD:EVALUATION_ABORTED;");
        return;
    }
///@TODO Possible memory leak:  ensure that this object will be deleted
    CommentEditDlg* commentEditDlg = new CommentEditDlg();
///@TODO Possible memory leak:  ensure that this object will be deleted
    BenchmarkingResult* benchmarkingResult = new BenchmarkingResult();
    commentEditDlg->init(benchmarkingResult, pipeline);
}

void BenchmarkingTool::pipelineStartBenchmarking (QString comment)
{
    if(!checkIsEmpty() || !pipeline->resetBenchmarking())
    {
        sendResourceToolMsg("RTCMD:EVALUATION_ABORTED;");
        return;
    }
///@TODO Possible memory leak:  ensure that this object will be deleted
    BenchmarkingResult* benchmarkingResult = new BenchmarkingResult();
    benchmarkingResult->comment = comment;
    if(pipeline != 0)
        pipeline->startBenchmarking(benchmarkingResult);
}

void BenchmarkingTool::pipelinePauseBenchmarking ()
{
    if(pipeline->isEmpty())
        return;
    pipeline->pauseBenchmarking();
}

void BenchmarkingTool::pipelineResumeBenchmarking ()
{
    if(pipeline->isEmpty())
        return;
    pipeline->resumeBenchmarking();
}

void BenchmarkingTool::pipelineResetBenchmarking ()
{
    if(pipeline->isEmpty())
        return;
    pipeline->resetBenchmarking();
}

void BenchmarkingTool::checkResourceToolMsg ()
{
    if(popPelfSharedMemory("BTCMD:EVALUATE;"))
    {
        pipelineStartBenchmarking("Pelf Resource Tool signal");
    }
}

void BenchmarkingTool::sendResourceToolMsg (QString msg)
{
    if(!pelfSharedMemory->isAttached())
        return;
    pelfSharedMemory->lock();
    writePelfSharedMemory(msg.prepend(readPelfSharedMemory()));
    pelfSharedMemory->unlock();
}

void BenchmarkingTool::initPelfSharedMemory ()
{
///@TODO Possible memory leak:  ensure that this object will be deleted
    if(pelfSharedMemory->isAttached())
        pelfSharedMemory->detach();
    if(!pelfSharedMemory->create(1000))
        if(!pelfSharedMemory->attach())
            qDebug() << "Unable to create or attach shared memory (" << pelfSharedMemory->error() << ")";
    if(!pelfSharedMemory->isAttached())
        return;
    writePelfSharedMemory();
///@TODO Possible memory leak:  ensure that this object will be deleted
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkResourceToolMsg()));
    timer->start(1000);
}

QString BenchmarkingTool::readPelfSharedMemory ()
{
    return QString(QByteArray((char*)pelfSharedMemory->data(), pelfSharedMemory->size()));
}

void BenchmarkingTool::writePelfSharedMemory (QString msg)
{
    memcpy(pelfSharedMemory->data(), msg.toUtf8(), qMin(pelfSharedMemory->size(), msg.size() + 1));
}

bool BenchmarkingTool::popPelfSharedMemory (QString msg)
{
    pelfSharedMemory->lock();
    QString pelfSharedMemoryMsgs = readPelfSharedMemory();
    bool foundMsg;
    if(pelfSharedMemoryMsgs.contains(msg))
    {
        writePelfSharedMemory(pelfSharedMemoryMsgs.remove(msg));
        foundMsg = true;
    }
    else
        foundMsg = false;
    pelfSharedMemory->unlock();
    return foundMsg;
}

void BenchmarkingTool::slotOneShotAnalysis()
{
  if (m_oneShotAnalysisProcess->state() != QProcess::NotRunning)
  {
    QMessageBox::warning(this, tr("One Shot Analysis Error"), tr("Cannot start one shot analysis: process still running"));
    qWarning() << "Cannot start one shot analysis: process still running";
    return;
  }
  if (m_tmpFile != 0)
  {
    delete m_tmpFile;
  }
  m_tmpFile = new QTemporaryFile();
  if (m_tmpFile->open())
  {
    qDebug() << "BenchmarkingTool::slotOneShotAnalysis: " << m_tmpFile->fileName();
    m_tmpFile->write(textToAnalyze->document()->toPlainText().toUtf8()+'\n');
    QStringList arguments;
    qDebug() << "Triggering one shot analysis (file " << m_tmpFile->fileName() << ")";
    arguments.push_back(m_tmpFile->fileName());
//     m_oneShotAnalysisProcess->setProcessChannelMode(QProcess::MergedChannels);
    m_oneShotAnalysisProcess->setWorkingDirectory(QString(getenv("LIMA_DIST"))+"/share/apps/pelf/commands/viewer");
    m_oneShotAnalysisProcess->setProcessChannelMode(QProcess::ForwardedChannels);
    m_oneShotAnalysisProcess->start(QString(getenv("LIMA_DIST"))+"/share/apps/pelf/commands/viewer/oneShotAnalysis.sh", arguments);
    // should destroy tmpFile at process end
    m_tmpFile->close();
  }
  else
  {
    delete m_tmpFile;
    m_tmpFile = 0;
  }
}

void BenchmarkingTool::slotChoseTextFile()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Chose a text file to analyze"),QString(),"Text files (*.txt);;All files (*)");
  if (fileName.isNull())
    return;
  recentFilesList->addItem(fileName);
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly))
    return;
  textToAnalyze->document()->setPlainText(QString::fromUtf8(file.readAll().data()));
  file.close();
}

void BenchmarkingTool::slotTextFileActivated(QListWidgetItem* item)
{
  QFile file(item->text());
  if (!file.open(QIODevice::ReadOnly))
    return;
  textToAnalyze->document()->setPlainText(QString::fromUtf8(file.readAll().data()));
  file.close();
}

void BenchmarkingTool::slotRemoveTextFile()
{
  QList<QListWidgetItem *> items = recentFilesList->selectedItems();
  Q_FOREACH(QListWidgetItem* item, items)
  {
    recentFilesList->takeItem(recentFilesList->row(item));
  }
}

void BenchmarkingTool::slotResultsViewVerticalHeaderSectionClicked(int)
{
  qDebug() << "BenchmarkingTool::slotResultsViewVerticalHeaderSectionClicked";
  updateErrorsWidget();
}

void BenchmarkingTool::slotCompareWithReference()
{
  qDebug() << "BenchmarkingTool::slotCompareWithReference";
  PipelineUnit* selectedUnit = pipeline->getUnits()[pipelineUnitDisplayCb->currentIndex() - 1];

  QString referenceFilename = selectedUnit->referencePath;
  referenceFilename.replace("$LBP_PATH/","");
  compareWith(referenceFilename);
}

void BenchmarkingTool::slotCompareWithPrevious()
{
  qDebug() << "BenchmarkingTool::slotCompareWithPrevious";
  QList<BenchmarkingResult*> resultsVector = pipeline->getResultsVector();

  BenchmarkingResult* benchmarkingResult = 0;
  int row = resultsView->selectionModel()->selectedRows().first().row();
  if(row > 0 && row < (int)resultsVector.size())
  {
    benchmarkingResult = resultsVector[row];
  }
  else
  {
    QMessageBox::warning(this,tr("Error"),tr("There is no previous run."));
    return;
  }
    qDebug() << "Result" << row << "selected";


  BenchmarkingResult* previousBenchmarkingResult = resultsVector[row-1];
  QString selectedUnitTextPath = pipelineUnitDisplayCb->currentText();
  QString previousFilename = QString(QLatin1String("output/")) + QString::number(previousBenchmarkingResult->time.toTime_t()) + "/" + selectedUnitTextPath + ".aligned.easy.xml";
  compareWith(previousFilename);
  updateErrorsWidget(benchmarkingResult, previousBenchmarkingResult);
}

void BenchmarkingTool::compareWith(const QString& otherFilename)
{
  qDebug() << "BenchmarkingTool::compareWith" << otherFilename;
  QString selectedUnitTextPath = pipelineUnitDisplayCb->currentText();

  QString outputFilename = QString(QLatin1String("output/%1/%2.aligned.easy.xml")).arg(pipeline->startTime.toTime_t()).arg(selectedUnitTextPath);

  qDebug() << "BenchmarkingTool::compareWith";
  QMultiMap<QString,QString> utterancesSet = utterancesWithErrors(selectedUnitTextPath);
  QString utterances;
  Q_FOREACH (const QString& utt, utterancesSet.values(selectedUnitTextPath))
  {
    utterances += utt + ",";
  }

  SyanotProcessFactory::instance(pipeline->workingDir, utterances, otherFilename, outputFilename);
}

QMultiMap<QString,QString> BenchmarkingTool::utterancesWithErrors(const QString& selectedUnitTextPath)
{
//   qDebug() << "BenchmarkingTool::utterancesWithErrors " << selectedUnitTextPath;
  QMultiMap<QString,QString> utterances;
//   qDebug() << "BenchmarkingTool::utterancesWithErrors " << pipeline->results.size() << " results";
  // obtenir la liste des erreurs pour ce pipeline unit
  // pour chacune, creer si necessaire une entree dans la liste des enonces
  // remplir une ligne de la table

//     QString selectedUnitTextPath = pipelineView->currentIndex().data().toString();
  BenchmarkingResult* benchmarkingResult = 0;
  if (resultsView->selectionModel()->selectedRows().empty() )
  {
    if (!pipeline->results.contains(pipeline->startTime))
      return utterances;
//     qDebug() << "No result selected; use result with pipeline time" << pipeline->startTime;
    benchmarkingResult = pipeline->results[pipeline->startTime];
  }
  else
  {
    int row = resultsView->selectionModel()->selectedRows().first().row();
    QList<BenchmarkingResult*> resultsVector = pipeline->getResultsVector();
    if(row >= 0 && row < (int)resultsVector.size())
    {
      benchmarkingResult = resultsVector[row];
    }
//     qDebug() << "Result" << row << "selected";
  }
  if (benchmarkingResult != 0)
  {
    utterances = utterancesWithErrors(benchmarkingResult, selectedUnitTextPath);
  }
  return utterances;
}

QMultiMap<QString,QString> BenchmarkingTool::utterancesWithErrors(BenchmarkingResult* benchmarkingResult, const QString& selectedUnitTextPath)
{
//   qDebug() << "BenchmarkingTool::utterancesWithErrors " << benchmarkingResult << selectedUnitTextPath;
  QMultiMap<QString,QString> utterances;
  if (benchmarkingResult == 0) return utterances;
//   qDebug() << "BenchmarkingTool::utterancesWithErrors " << pipeline->results.size() << " results";
  // obtenir la liste des erreurs pour ce pipeline unit
  // pour chacune, creer si necessaire une entree dans la liste des enonces
  // remplir une ligne de la table

  // continue if the current result is not the one selected and, if none
  // is selected, if it is not the one with the pipeline startTime, in other
  // words if it is not the last one
  QMap<PipelineUnit*, EvaluationResultSet*> puResult = benchmarkingResult->resultUnits;
//       qDebug() << "BenchmarkingTool::utterancesWithErrors " << puResult.size() << " pu results";
  Q_FOREACH(PipelineUnit* pipelineUnit, puResult.keys())
  {
      QString pipelineName = pipelineUnit->name;
//           qDebug() << "selectedUnitTextPath: " << selectedUnitTextPath << "; pipelineName: " << pipelineName;
      if (!selectedUnitTextPath.isEmpty() && selectedUnitTextPath != pipelineName) continue;
      EvaluationResultSet* resultSet = puResult[pipelineUnit];
      //qDebug() << "Selecting unit result set (size: " << resultSet->results.size() << ")";
      Q_FOREACH(EvaluationResult::DIMENSION_ID dimensionId, resultSet->keys())
      {
          EvaluationResult* result = (*resultSet)[dimensionId];
          // sentence/group or sentence/relation ids
          QMultiMap<QString, QString>&  absent = result->getRefAbsentFromHyp();
//               qDebug() << "absents ("<<dimensionId<<"): "<< absent.size() << absent;
          QMultiMap<QString, QString>&  fals = result->getHypAbsentFromRef();
//               qDebug() << "false ("<<dimensionId<<"): "<< fals.size() << fals;
          QMultiMap<QString, QString>&  type = result->getTypeError();
//               qDebug() << "type ("<<dimensionId<<"): "<< type.size() << type;
          QSet<QString> keys;
          keys.unite(QSet<QString>::fromList(absent.keys()));
          keys.unite(QSet<QString>::fromList(fals.keys()));
          keys.unite(QSet<QString>::fromList(type.keys()));
//               qDebug() << "utterances insert" << pipelineName << keys;
          Q_FOREACH(const QString& key, keys)
          {
            if (!utterances.values(pipelineName).contains(key))
              utterances.insert(pipelineName,key);
          }
      }
  }
  return utterances;
}

void BenchmarkingTool::addError(const QString& type, const QString& dimension, const QString& name)
{
  qDebug() << "BenchmarkingTool::addError" << type << dimension << name;
  errorsTable->setRowCount(errorsTable->rowCount()+1);
  int row = errorsTable->rowCount() - 1;

  QTableWidgetItem *tItem = new QTableWidgetItem(type);
  errorsTable->setItem(row, 0, tItem);
  QTableWidgetItem *dItem = new QTableWidgetItem(dimension);
  errorsTable->setItem(row, 1, dItem);
  QTableWidgetItem *nItem = new QTableWidgetItem(name);
  errorsTable->setItem(row, 2, nItem);
}

void BenchmarkingTool::addTypedErrors(const QString& statementId, EvaluationResult::DIMENSION_ID dim, const QString& typeText, QMultiMap<QString, QString>& errors)
{
  qDebug() << "BenchmarkingTool::addTypedErrors" << statementId << dim << typeText << errors;
  QMultiMap<QString, QString>::const_iterator ait = errors.begin();
  for (; ait != errors.end(); ait++)
  {
    if (ait.key() == statementId)
    {
      QString dimension = EvaluationResult::dimensions[dim]->name;
      QString name = ait.value();
      //QString text = "Text to load...";
      addError(typeText,dimension,name);
    }
  }
}

/** Shows only the differences between the two benchmarkings */
void BenchmarkingTool::updateErrorsWidget(BenchmarkingResult* benchmarkingResult, BenchmarkingResult* previousBenchmarkingResult)
{
  QString selectedUnitTextPath = pipelineUnitDisplayCb->currentIndex()==0?"":pipelineUnitDisplayCb->currentText();
//   qDebug() << "BenchmarkingTool::updateErrorsWidget (compare) " << pipelineUnitDisplayCb->currentIndex() << selectedUnitTextPath;

  errorsTable->clearContents();
  errorsTable->setRowCount(0);
  statementsTree->clear();
  m_comparingWithPrevious = true;
  m_currentBenchmarkingResult = benchmarkingResult;
  m_previousBenchmarkingResult = previousBenchmarkingResult;

  QMultiMap<QString,QString> errorUtterances = utterancesWithErrors(benchmarkingResult, selectedUnitTextPath);
  QMultiMap<QString,QString> previousErrorUtterances = utterancesWithErrors(previousBenchmarkingResult, selectedUnitTextPath);

//   qDebug() << "BenchmarkingTool::updateErrorsWidget (compare) AFTER UTTERANCES WITH ERRORS";
  QMap<QString,QString> alreadyInserted;
  Q_FOREACH (const QString& key, errorUtterances.keys().toSet())
  {
//     qDebug() << "key" << key;
    Q_FOREACH(const QString& value, errorUtterances.values(key))
    {
//       qDebug() << "value" << value;
      QList<QStringList> errors = getErrors(key, value, benchmarkingResult);
      qDebug() << "errors" << key << value << ":" << errors;
      QList<QStringList> previousErrors = getErrors(key, value, previousBenchmarkingResult);
      qDebug() << "previousErrors" << key << value << ":" << previousErrors;

      Q_FOREACH (const QStringList& list, errors)
      {
        if (!previousErrors.contains(list) && !(alreadyInserted.contains(key) && alreadyInserted.values(key).contains(value)))
        {
          qDebug() << "add child item" << key << value;
          QTreeWidgetItem * childItem = new QTreeWidgetItem();
          childItem->setText(0,key);
          childItem->setText(1,value);
          //childItem->setText(2,"Text to load...");
          statementsTree->addTopLevelItem(childItem);
          alreadyInserted[key] = value;
        }
      }
      Q_FOREACH (const QStringList& list, previousErrors)
      {
        if (!errors.contains(list) && !(alreadyInserted.contains(key) && alreadyInserted.values(key).contains(value)))
        {
          qDebug() << "add child item" << key << value;
          QTreeWidgetItem * childItem = new QTreeWidgetItem();
          childItem->setText(0,key);
          childItem->setText(1,value);
          //childItem->setText(2,"Text to load...");
          statementsTree->addTopLevelItem(childItem);
          alreadyInserted[key] = value;
        }
      }
    }
  }

  statementsTree->sortByColumn(0,Qt::AscendingOrder);
}

QList<QStringList> BenchmarkingTool::getErrors(const QString& unit, const QString& utteranceId, BenchmarkingResult* benchResult)
{
  qDebug() << "BenchmarkingTool::getErrors" << unit << utteranceId;
  QList<QStringList> result;

  QMap<PipelineUnit*, EvaluationResultSet*>& puResult = benchResult->resultUnits;
  qDebug() << "BenchmarkingTool::getErrors" << puResult.size() << " pu results";
  Q_FOREACH(PipelineUnit* pipelineUnit, puResult.keys())
  {
    QString pipelineName = pipelineUnit->name;
    qDebug() << "unit: " << unit << "; pipelineName: " << pipelineName;
    if (unit != pipelineName) continue;
    EvaluationResultSet* resultSet = puResult[pipelineUnit];
    qDebug() << "BenchmarkingTool::getErrors result set size: " << resultSet->size();
    Q_FOREACH(EvaluationResult::DIMENSION_ID dimensionId, resultSet->keys())
    {
      EvaluationResult* dimEvalResult = (*resultSet)[dimensionId];
      QMultiMap<QString, QString> errors = dimEvalResult->getRefAbsentFromHyp();
      QMultiMap<QString, QString>::const_iterator ait = errors.begin();
      for (; ait != errors.end(); ait++)
      {
        if (ait.key() == utteranceId)
        {
          QStringList error;
          QString dimension = EvaluationResult::dimensions[dimensionId]->name;
          QString name = ait.value();
          error << "Not found" << dimension << name;
          result.push_back(error);
        }
      }
      errors = dimEvalResult->getHypAbsentFromRef();
      ait = errors.begin();
      for (; ait != errors.end(); ait++)
      {
        if (ait.key() == utteranceId)
        {
          QStringList error;
          QString dimension = EvaluationResult::dimensions[dimensionId]->name;
          QString name = ait.value();
          error << "Found incorrect" << dimension << name;
          result.push_back(error);
        }
      }
      errors = dimEvalResult->getTypeError();
      ait = errors.begin();
      for (; ait != errors.end(); ait++)
      {
        if (ait.key() == utteranceId)
        {
          QStringList error;
          QString dimension = EvaluationResult::dimensions[dimensionId]->name;
          QString name = ait.value();
          error << "Wrong type" << dimension << name;
          result.push_back(error);
        }
      }
    }
  }

  return result;
}

void BenchmarkingTool::slotSaveTextFile()
{
  QString fileName;
  if (recentFilesList->currentItem() == 0)
  {
    fileName = QFileDialog::getSaveFileName(this, tr("Choose a file to save the current text"));
  }
  else
  {
    fileName = recentFilesList->currentItem()->text();
  }
  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly))
  {
    qCritical() << "Cannot open" << fileName << "for writing";
    return;
  }
  QTextStream stream(&file);
  stream << textToAnalyze->toPlainText();
  file.close();
}

void BenchmarkingTool::slotNewTextFile()
{
  recentFilesList->setCurrentItem(0);
  textToAnalyze->clear();
}

void BenchmarkingTool::commandFinished (int exitCode, QProcess::ExitStatus exitStatus)
{
  qDebug() << "Finished a process";
}

void BenchmarkingTool::commandError (QProcess::ProcessError error)
{
  switch (error)
  {
  case QProcess::FailedToStart:
    qDebug() << "Command error: The process failed to start. ";
  break;
  case QProcess::Crashed:
    qDebug() << "Command error: The process crashed some time after starting successfully.";
  break;
  case QProcess::Timedout:
    qDebug() << "Command error: The last waitFor...() function timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again.";
  break;
  case QProcess::WriteError:
    qDebug() << "Command error: An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel.";
  break;
  case QProcess::ReadError:
    qDebug() << "Command error: An error occurred when attempting to read from the process. For example, the process may not be running.";
  break;
  case QProcess::UnknownError:;
  default:
    qDebug() << "Undetermined command error (QT code: " << error << ")";
  }
}



#include "BenchmarkingTool.moc"
