// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_RESOURCETOOL_H
#define LIMA_PELF_RESOURCETOOL_H

#include "ui_pelf-rt.h"

class QSettings;
class QSharedMemory;

namespace Lima {
namespace Pelf {

class ResourceTool : public QMainWindow, public Ui::PelfResourceToolWindow
{

Q_OBJECT

public:

    enum EVALUATION_STATUS
    {
        EVALUATION_READY = 0,
        EVALUATION_INSTALLING = 1,
        EVALUATION_BENCHMARKING = 2
    };

    ResourceTool (QWidget* parent = 0);
    bool init ();
    bool xmlFileInit (QString xmlFilePath);
    void dictionnaryInit (QString resourcePath, QString installComand);
    void nGramsInit (QStringList resourcePaths, QString installComand);
    void nGramsInitCategoryCb (QComboBox* categoryCb, bool allowBigrams = false);
    void nGramsCorpusInit (QString ngramsSymbolicCodesPath, QString ngramsCorpusPath);
    void syntacticRulesInit (QStringList resourcePaths, QString installComand);
    void syntacticRulesLoadData (QStringList resourcePaths, QString installComand);
    void syntacticRulesConnectBtnSignals ();
    void syntacticRulesGotoCurrentMatch ();
    void idiomaticExpressionsInit (QString resourcePath, QString installComand);
    bool isInstalling ();
    bool checkInstallingReady ();
    bool checkEvaluateReady ();
    void sendBenmarkingToolMsg (QString msg);
    void initPelfSharedMemory ();
    QString readPelfSharedMemory ();
    void writePelfSharedMemory (QString msg = "");
    bool popPelfSharedMemory (QString msg);
    void logDebugMsg (QtMsgType type, const char* msg);

public Q_SLOTS:

    void dictionnaryEditDlg (AbstractResourceEntry* are = 0);
    void dictionnarySearch ();
    void dictionnaryConfirmInstall ();
    void nGramsShiftSearchLeft ();
    void nGramsShiftSearchRight ();
    void nGramsSearch ();
    void nGramsShowCorpus ();
    void nGramsSearchReset ();
    void nGramsEditCorpus ();
    void nGramsConfirmInstall ();
    void nGramsInstalled (bool success);
    void syntacticRulesSearch ();
    void syntacticRulesPreviousMatch ();
    void syntacticRulesNextMatch ();
    void syntacticRulesConfirmSaveAndInstall ();
    void syntacticRulesSaveAndInstall();
    void syntacticRulesInstalled (bool success);
    void idiomaticExpressionsEditDlg (AbstractResourceEntry* are = 0);
    void idiomaticExpressionsSearch ();
    void idiomaticExpressionsConfirmInstall ();
    void resourceNotInstalled ();
    void resourceInstalled (bool success);
    void checkBenmarkingToolMsg ();
    void confirmEvaluate ();

protected:

    void closeEvent (QCloseEvent* event) override;

private:

    QSettings* settings;
    PelfResourceToolWindow ui;
    QString linguisticDataRoot;
    QString commandsRoot;
    EVALUATION_STATUS evaluationStatus;
    QSharedMemory* pelfSharedMemory;

    QString dictionnaryResourcePath;
    QString dictionnaryInstallCommand;
    ResourceEditorTableModel* dictionnaryModel;

    QStringList ngramsResourcePaths;
    QString ngramsInstallCommand;
    ResourceReaderTableModel* nGramsModel;
    QString ngramsSymbolicCodesPath;
    QString ngramsCorpusPath;
    QMap<QString, QString> ngramsSymbolicCodes;
    QMap<QString, QStringList> ngramsMatches;

    QStringList syntacticRulesResourcePaths;
    QString syntacticRulesInstallCommand;
    int syntacticRulesCurrentMatch;
    ResourceReaderToolBoxModel* syntacticRulesModel;

    QString idiomaticExpressionsResourcePath;
    QString idiomaticExpressionsInstallCommand;
    ResourceEditorTableModel* idiomaticExpressionsModel;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_RESOURCETOOL_H
