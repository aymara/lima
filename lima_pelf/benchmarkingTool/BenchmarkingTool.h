// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_BENCHMARKINGTOOL_H
#define LIMA_PELF_BENCHMARKINGTOOL_H


#include "ui_pelf-bt.h"

class QAction;
class QMenu;
class QTemporaryFile;
class QSettings;
class QSharedMemory;
class QwtPlotCurve;

namespace Lima {
namespace Pelf {

class Pipeline;

class BenchmarkingTool : public QMainWindow, public Ui::BenchmarkingToolWindow
{

Q_OBJECT

public:

    BenchmarkingTool (QWidget* parent = 0);
    ~BenchmarkingTool ();
    void init ();
    void resetEvaluationCurves ();
    void updateEvaluationCurves (int nbRes = 0);
    void updateDimensionsWidgets ();
    void logDebugMsg (QtMsgType type, const char* msg);
    bool confirmAbandonModifications ();
    bool checkIsEmpty ();
    void pipelineStartBenchmarking (QString comment);
    void sendResourceToolMsg (QString msg);
    void initPelfSharedMemory ();
    QString readPelfSharedMemory ();
    void writePelfSharedMemory (QString msg = "");
    bool popPelfSharedMemory (QString msg);

public Q_SLOTS:

    void pipelineUnitsChanged ();
    void updateResultsViews ();
    void updateResultsTableView ();
    void updateResultsViewSelection ();
    void viewResult (int row);
    void fileNew (); void on_actionNew_triggered () { fileNew(); };
    void fileLoad (); void on_actionOpen_triggered () { fileLoad(); };
    void fileSave (); void on_actionSave_triggered () { fileSave(); };
    void fileSaveAs (); void on_actionSave_as_triggered () { fileSaveAs(); };
    void resetViews ();
    void pipelineAddFile (); void on_actionAdd_file_triggered () { pipelineAddFile(); };
    void pipelineClearFiles (); void on_actionClear_files_triggered () { pipelineClearFiles(); };
    void pipelineStartBenchmarking (); void on_actionStart_restart_benchmarking_triggered () { pipelineStartBenchmarking(); };
    void pipelinePauseBenchmarking (); void on_actionPause_benchmarking_triggered () { pipelinePauseBenchmarking(); };
    void pipelineResumeBenchmarking (); void on_actionResume_benchmarking_triggered () { pipelineResumeBenchmarking(); };
    void pipelineResetBenchmarking (); void on_actionReset_benchmarking_triggered () { pipelineResetBenchmarking(); };
    void pipelineConfigure (); void on_actionConfigure_pipeline_triggered () { pipelineConfigure(); };
    void pipelineFinisehdBenchmarking ();
    void checkResourceToolMsg ();
    void openRecentFile();
    void slotResultsViewVerticalHeaderSectionClicked(int);
    void slotCompareWithReference();
    void slotCompareWithPrevious();
    void commandFinished (int exitCode, QProcess::ExitStatus exitStatus);
    void commandError (QProcess::ProcessError error);

private Q_SLOTS:
    void slotPipelineUnitActivated(const QModelIndex&);
    void slotErrorStatementActivated(QTreeWidgetItem* item, int column);
    void slotShowErrorStatementDetails(QTreeWidgetItem* item, int column);
    void slotOneShotAnalysis();
    void slotNewTextFile();
    void slotChoseTextFile();
    void slotSaveTextFile();
    void slotRemoveTextFile();
    void slotTextFileActivated(QListWidgetItem* item);

protected:

    void closeEvent (QCloseEvent* event);

private:
    void loadFileStream (const QString& fileName);
    void createActions();
    void createMenus();
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);
    void updateErrorsWidget();
    void updateErrorsWidget(BenchmarkingResult* benchmarkingResult, BenchmarkingResult* previousBenchmarkingResult);
    void compareWith(const QString& otherFilename);
    QMultiMap<QString,QString>  utterancesWithErrors(const QString& selectecUnit = QString());
    QMultiMap<QString,QString> utterancesWithErrors(BenchmarkingResult* benchmarkingResult, const QString& selectedUnitTextPath);
    void addError(const QString& type, const QString& dimension, const QString& name);
    void addTypedErrors(const QString& statementId, EvaluationResult::DIMENSION_ID dim, const QString& text, QMultiMap<QString, QString>& errors);
    QList<QStringList> getErrors(const QString& unit, const QString& utteranceId, BenchmarkingResult* benchResult);


    QSettings* settings;
    BenchmarkingToolWindow ui;
    Pipeline* pipeline;
    QString currentFileName;
    QMap<EvaluationResult::DIMENSION_ID, QList<QwtPlotCurve*> > evaluationResultTypeQwtCurves;
    QSharedMemory* pelfSharedMemory;
    int currentViewSelection;
    QString viewerCommandLine;
    QMenu *recentFilesMenu;

    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];

    QModelIndex m_currentPipelineUnit;

    bool m_comparingWithPrevious;
    BenchmarkingResult* m_currentBenchmarkingResult;
    BenchmarkingResult* m_previousBenchmarkingResult;

    QProcess* m_oneShotAnalysisProcess;
    QProcess* m_showErrorStatementDetailsProcess;
    QProcess* m_viewProcess;
    QTemporaryFile* m_tmpFile;
};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_BENCHMARKINGTOOL_H
