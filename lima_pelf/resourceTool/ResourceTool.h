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

    void closeEvent (QCloseEvent* event);

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
