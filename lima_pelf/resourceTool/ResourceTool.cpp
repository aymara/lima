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
#include "ResourceTool.h"
#include "ResourceEditorTableModel.h"
#include "ResourceFileEditDlg.h"
#include "DictionnaryEntry.h"
#include "DictionnaryEntryEditDlg.h"
#include "NGramEntry.h"
#include "SyntacticRuleEntry.h"
#include "IdiomaticExpressionEntry.h"
#include "IdiomaticExpressionEntryEditDlg.h"


#include <QtXml/QDomImplementation>
#include <QtXml/QDomDocument>
#include <QtCore/QtDebug>
#include <QtCore/QSettings>
#include <QtCore/QSharedMemory>
#include <QtCore/QTimer>
#include <QtCore/QFile>
#include <QtGui/QMainWindow>


using namespace Lima::Pelf;

ResourceTool::ResourceTool (QWidget* p):
    QMainWindow(p),
    Ui::PelfResourceToolWindow(),
    linguisticDataRoot(""),
    commandsRoot(""),
    evaluationStatus(EVALUATION_READY)
{
    setupUi(this);
}

bool ResourceTool::init ()
{
    qDebug() << "Starting application, loading settings...";
    settings = new QSettings("CEA LIST LIMA", "Pelf Resource Tool");
    restoreGeometry(settings->value("geometry").toByteArray());

    char* mmdist = getenv("LIMA_DIST");
    if (mmdist == 0) mmdist = "/usr";
    if(!xmlFileInit(QString(mmdist)+"/share/config/pelf/pelf-rt-conf.xml"))
    {
      qDebug() << "Unable to load configuration from file "<<(QString(mmdist)+"/share/config/pelf/pelf-rt-conf.xml")<<", aborting";
      return false;
    }

    initPelfSharedMemory();
    if(!dictionnaryResourcePath.isEmpty() && !dictionnaryInstallCommand.isEmpty())
        dictionnaryInit(dictionnaryResourcePath, dictionnaryInstallCommand);
    else
        editorTabsWidget->removeTab(editorTabsWidget->indexOf(dictionnaryTab));
    if(!ngramsResourcePaths.isEmpty() && !ngramsInstallCommand.isEmpty())
    {
        nGramsInit(ngramsResourcePaths, ngramsInstallCommand);
        if(!ngramsSymbolicCodesPath.isEmpty() && !ngramsCorpusPath.isEmpty())
            nGramsCorpusInit(ngramsSymbolicCodesPath, ngramsCorpusPath);
    }
    else
        editorTabsWidget->removeTab(editorTabsWidget->indexOf(nGramsTab));
    if(!syntacticRulesResourcePaths.isEmpty() && !syntacticRulesInstallCommand.isEmpty())
        syntacticRulesInit(syntacticRulesResourcePaths, syntacticRulesInstallCommand);
    else
        editorTabsWidget->removeTab(editorTabsWidget->indexOf(syntacticRulesTab));
    if(!idiomaticExpressionsResourcePath.isEmpty() && !idiomaticExpressionsInstallCommand.isEmpty())
        idiomaticExpressionsInit(idiomaticExpressionsResourcePath, idiomaticExpressionsInstallCommand);
    else
        editorTabsWidget->removeTab(editorTabsWidget->indexOf(idiomaticExpressionsTab));

    connect(evaluationBtn, SIGNAL(clicked(bool)), this, SLOT(confirmEvaluate()));

    messagesSplitter->restoreState(settings->value ("messagesSplitter").toByteArray());
    ngramsCategoriesSplitter->restoreState(settings->value ("ngramsCategoriesSplitter").toByteArray());
    ngramsOccurencesSplitter->restoreState(settings->value ("ngramsOccurencesSplitter").toByteArray());

    
    show();
    return true;
}

bool ResourceTool::xmlFileInit (QString xmlFilePath)
{
    QFile xmlFile(xmlFilePath);
    if(!xmlFile.open(QFile::ReadOnly)){
        qDebug() << "Unable to open xml configuration file, aborting";
        return false;
    }
    QDomDocument doc;
    QString errorMsg;
    if(!doc.setContent(&xmlFile, false, &errorMsg))
    {
        qDebug() << "Unable to parse xml file, aborting (" << errorMsg << ")";
        xmlFile.close();
        return false;
    }
    xmlFile.close();
    QDomElement configurationNode = doc.documentElement();
    if(configurationNode.tagName() != "configuration" )
    {
        qDebug() << "Unrecognized xml file format, aborting";
        return false;
    }

    QDomElement linguisticDataRootNode = configurationNode.firstChildElement("linguisticDataRoot");
    if(!linguisticDataRootNode.isNull() && !linguisticDataRootNode.attribute("path").isNull())
        linguisticDataRoot = linguisticDataRootNode.attribute("path");
    if(linguisticDataRoot.isEmpty())
        linguisticDataRoot = getenv("LINGUISTIC_DATA_ROOT");
    if(linguisticDataRoot.isEmpty())
    {
      qDebug() << "Linguistic data root is not defined";
      return false;
    }
    setenv("LINGUISTIC_DATA_ROOT", linguisticDataRoot.toUtf8(), 1);
    qDebug() << "Retrieved and set env variable linguistic data root to " << linguisticDataRoot;

    QDomElement commandsRootNode = configurationNode.firstChildElement("commandsRoot");
    if(!commandsRootNode.isNull() && !commandsRootNode.attribute("path").isNull())
        commandsRoot = commandsRootNode.attribute("path");
    if(commandsRoot.isEmpty())
    {
      commandsRoot = getenv("LIMA_DIST");
      if (commandsRoot.isEmpty())
      {
        commandsRoot = QString("/usr");
      }
    }
    commandsRoot += "/share/apps/pelf/commands";
    setenv("PELF_COMMANDS_ROOT", commandsRoot.toUtf8(), 1);
    qDebug() << "Retrieved command root " << commandsRoot;

    QDomElement resourcesNode = configurationNode.firstChildElement("resources");
    if(!resourcesNode.isNull())
    {
        QDomElement dictionnaryNode = resourcesNode.firstChildElement("dictionnary");
        if(!dictionnaryNode.isNull())
        {
            QDomElement resourceFileNode = dictionnaryNode.firstChildElement("resourceFile");
            if(!resourceFileNode.isNull() && !resourceFileNode.attribute("pathSuffix").isNull())
            {
                dictionnaryResourcePath = resourceFileNode.attribute("pathSuffix");
                if (!dictionnaryResourcePath.startsWith('/'))
                  dictionnaryResourcePath.prepend(linguisticDataRoot+"/");
            }
            QDomElement installCommandFileNode = dictionnaryNode.firstChildElement("installCommandFile");
            if(!installCommandFileNode.isNull() && !installCommandFileNode.attribute("pathSuffix").isNull())
                dictionnaryInstallCommand = commandsRoot+"/"+installCommandFileNode.attribute("pathSuffix");
        }
        QDomElement ngramsNode = resourcesNode.firstChildElement("ngrams");
        if(!ngramsNode.isNull())
        {
            QDomElement resourceFileNode = ngramsNode.firstChildElement("resourceFile");
            while(!resourceFileNode.isNull())
            {
                if(!resourceFileNode.isNull() && !resourceFileNode.attribute("pathSuffix").isNull())
								{
									if (resourceFileNode.attribute("pathSuffix").startsWith('/'))
                    ngramsResourcePaths << resourceFileNode.attribute("pathSuffix");
									else
										ngramsResourcePaths << linguisticDataRoot+"/"+resourceFileNode.attribute("pathSuffix");
									resourceFileNode = resourceFileNode.nextSiblingElement("resourceFile");
								}
            }
            QDomElement installCommandFileNode = ngramsNode.firstChildElement("installCommandFile");
            if(!installCommandFileNode.isNull() && !installCommandFileNode.attribute("pathSuffix").isNull())
                ngramsInstallCommand = commandsRoot+"/"+installCommandFileNode.attribute("pathSuffix");
            QDomElement symbolicCodeFileNode = ngramsNode.firstChildElement("symbolicCodeFile");
            if(!symbolicCodeFileNode.isNull() && !symbolicCodeFileNode.attribute("pathSuffix").isNull())
                ngramsSymbolicCodesPath = linguisticDataRoot+"/"+symbolicCodeFileNode.attribute("pathSuffix");
            QDomElement corpusFileNode = ngramsNode.firstChildElement("corpusFile");
            if(!corpusFileNode.isNull() && !corpusFileNode.attribute("pathSuffix").isNull())
            {
              ngramsCorpusPath = corpusFileNode.attribute("pathSuffix");
              if (!ngramsCorpusPath.startsWith('/'))
                ngramsCorpusPath.prepend(linguisticDataRoot+"/");
            }
        }
        QDomElement syntacticRulesNode = resourcesNode.firstChildElement("syntacticRules");
        if(!syntacticRulesNode.isNull())
        {
            QDomElement resourceFileNode = syntacticRulesNode.firstChildElement("resourceFile");
            while(!resourceFileNode.isNull())
            {
                if(!resourceFileNode.isNull() && !resourceFileNode.attribute("pathSuffix").isNull())
                    syntacticRulesResourcePaths << linguisticDataRoot+"/"+resourceFileNode.attribute("pathSuffix");
                resourceFileNode = resourceFileNode.nextSiblingElement("resourceFile");
            }
            QDomElement installCommandFileNode = syntacticRulesNode.firstChildElement("installCommandFile");
            if(!installCommandFileNode.isNull() && !installCommandFileNode.attribute("pathSuffix").isNull())
                syntacticRulesInstallCommand = commandsRoot+"/"+installCommandFileNode.attribute("pathSuffix");
        }
        QDomElement idiomaticExpressionsNode = resourcesNode.firstChildElement("idiomaticExpressions");
        if(!idiomaticExpressionsNode.isNull())
        {
            QDomElement resourceFileNode = idiomaticExpressionsNode.firstChildElement("resourceFile");
            if(!resourceFileNode.isNull() && !resourceFileNode.attribute("pathSuffix").isNull())
                idiomaticExpressionsResourcePath = linguisticDataRoot+"/"+resourceFileNode.attribute("pathSuffix");
            QDomElement installCommandFileNode = idiomaticExpressionsNode.firstChildElement("installCommandFile");
            if(!installCommandFileNode.isNull() && !installCommandFileNode.attribute("pathSuffix").isNull())
                idiomaticExpressionsInstallCommand = commandsRoot+"/"+installCommandFileNode.attribute("pathSuffix");
        }
    }
    return true;
}

void ResourceTool::dictionnaryInit (QString resourcePath, QString installComand)
{
  qDebug() << "ResourceTool::dictionnaryInit" << resourcePath << installComand;
///@TODO Possible memory leak:  ensure that this object will be deleted
    dictionnaryModel = new ResourceEditorTableModel();
    dictionnaryModel->init(resourcePath, installComand);
    dictionnaryModel->columnCountPerEntry = DictionnaryEntry::columnCountPerEntry;
    dictionnaryModel->columnHeaders = DictionnaryEntry::columnHeaders;
    dictionnaryModel->loadData<DictionnaryEntry>();
    dictionnarySearchCb->addItem(DictionnaryEntry::allCategoryName);
    for(int i = 0; i < dictionnaryModel->categoryList.size(); i++)
        dictionnarySearchCb->addItem(dictionnaryModel->categoryList.at(i));
    dictionnaryView->init(dictionnaryModel);
    connect(dictionnaryModel, SIGNAL(dataChanged()), this, SLOT(dictionnarySearch()));
    connect(dictionnaryModel, SIGNAL(dataInstalled(bool)), this, SLOT(resourceInstalled(bool)));
    connect(dictionnarySearchBtn, SIGNAL(clicked(bool)), this, SLOT(dictionnarySearch()));
    connect(dictionnarySearchInp, SIGNAL(returnPressed()), this, SLOT(dictionnarySearch()));
    connect(dictionnaryView, SIGNAL(editEntryDlg(AbstractResourceEntry*)), this, SLOT(dictionnaryEditDlg(AbstractResourceEntry*)));
    connect(dictionnaryAddBtn, SIGNAL(clicked(bool)), dictionnaryView, SLOT(createEntry()));
    connect(dictionnaryInstallBtn, SIGNAL(clicked(bool)), this, SLOT(dictionnaryConfirmInstall()));
    dictionnarySearch();
}

void ResourceTool::dictionnaryEditDlg (AbstractResourceEntry* are)
{
///@TODO Possible memory leak:  ensure that this object will be deleted
    DictionnaryEntryEditDlg* dictionnaryEntryEditDlg = new DictionnaryEntryEditDlg();
    dictionnaryEntryEditDlg->init(dictionnaryModel, are);
}

void ResourceTool::dictionnarySearch ()
{
    QStringList searchArgs;
    searchArgs << dictionnarySearchInp->text();
    searchArgs << dictionnarySearchCb->currentText();
    int nbEntries = dictionnaryModel->searchEntries(searchArgs);
    dictionnaryCountLbl->setText(QString::number(nbEntries)+" entries found");
    dictionnaryView->resizeColumnsToContents();
}

void ResourceTool::dictionnaryConfirmInstall ()
{
    if(!checkEvaluateReady())
        return;
    dictionnaryView->confirmInstall();
}

void ResourceTool::nGramsInit (QStringList resourcePaths, QString installComand)
{
  qDebug() << "ResourceTool::nGramsInit" << resourcePaths << installComand;
///@TODO Possible memory leak:  ensure that this object will be deleted
    nGramsModel = new ResourceReaderTableModel();
    nGramsModel->init(resourcePaths, installComand);
    nGramsModel->columnCountPerEntry = NGramEntry::columnCountPerEntry;
    nGramsModel->columnHeaders = NGramEntry::columnHeaders;
    nGramsModel->loadData<NGramEntry>();
    NGramEntry::gramCategoryList.sort();
    nGramsInitCategoryCb(nGramsSearchCb1);
    nGramsInitCategoryCb(nGramsSearchCb2);
    nGramsInitCategoryCb(nGramsSearchCb3, true);
    nGramsView->init(nGramsModel);
    connect(nGramsModel, SIGNAL(dataChanged()), this, SLOT(nGramsSearch()));
    connect(nGramsModel, SIGNAL(dataInstalled(bool)), this, SLOT(nGramsInstalled(bool)));
    connect(nGramsResetBtn, SIGNAL(clicked(bool)), this, SLOT(nGramsSearchReset()));
    connect(nGramsShiftLeftBtn, SIGNAL(clicked(bool)), this, SLOT(nGramsShiftSearchLeft()));
    connect(nGramsShiftRightBtn, SIGNAL(clicked(bool)), this, SLOT(nGramsShiftSearchRight()));
    connect(nGramsSearchBtn, SIGNAL(clicked(bool)), this, SLOT(nGramsSearch()));
    connect(nGramsView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(nGramsShowCorpus()));
    connect(nGramsEditCorpusBtn, SIGNAL(clicked(bool)), this, SLOT(nGramsEditCorpus()));
    connect(nGramsInstallBtn, SIGNAL(clicked(bool)), this, SLOT(nGramsConfirmInstall()));
    nGramsSearch();
}

void ResourceTool::nGramsInitCategoryCb (QComboBox* categoryCb, bool allowBigrams)
{
  categoryCb->addItem(NGramEntry::allCategoryName);
  for(int i = 0; i < NGramEntry::gramCategoryList.size(); i++)
      if(allowBigrams || NGramEntry::gramCategoryList.at(i) != NGramEntry::bigramsCategoryName)
        categoryCb->addItem(NGramEntry::gramCategoryList.at(i));
}

void ResourceTool::nGramsCorpusInit (QString symbolicCodesPath, QString corpusPath)
{
    qDebug() << "Loading NGrams symbolic codes: " << symbolicCodesPath;
    nGramsSymbolicCodesEdit->clear();
    ngramsSymbolicCodes.clear();
    QFile ngramsSymbolicCodesFile(symbolicCodesPath);
    if(!ngramsSymbolicCodesFile.exists() || !ngramsSymbolicCodesFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Error opening NGrams symbolic codes file " << ngramsSymbolicCodesPath << " for reading";
//         return;
    }
    else
    {
      while(!ngramsSymbolicCodesFile.atEnd())
      {
          QString symbolicCode = QString::fromUtf8(ngramsSymbolicCodesFile.readLine()).trimmed();
          QStringList symbolicCodeParts = symbolicCode.split('\t');
          if(symbolicCodeParts.size() == 2)
          {
              nGramsSymbolicCodesEdit->appendPlainText(symbolicCode);
              ngramsSymbolicCodes[symbolicCodeParts[0]] = symbolicCodeParts[1];
          }
      }
      ngramsSymbolicCodesFile.close();
      nGramsSymbolicCodesEdit->moveCursor(QTextCursor::Start);
      nGramsSymbolicCodesEdit->ensureCursorVisible();
    }
    qDebug() << "Loading NGrams corpus path: " << corpusPath;
    ngramsMatches.clear();
    QFile ngramsCorpusFile(corpusPath);
    if(!ngramsCorpusFile.exists() || !ngramsCorpusFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Error opening NGrams corpus file " << ngramsCorpusPath << " for reading";
        return;
    }
    QMap<QString, QStringList> curMatches;
    QMap<QString, QStringList>::iterator curMatchesIt, curMatchesItEnd;
    QString curText, prevText, prevPrevText, curCode, prevCode, prevPrevCode, curCat, prevCat, prevPrevCat;
    QString curSentence = "> ";
    while(!ngramsCorpusFile.atEnd())
    {
        QString currentWord = QString::fromUtf8(ngramsCorpusFile.readLine()).trimmed();
        QStringList currentWordParts = currentWord.split('\t');
        if(currentWordParts.size() == 2)
        {
            curCode = currentWordParts[1].trimmed();
            curCat = curCode;
            if(ngramsSymbolicCodes.contains(curCode))
                curCat = ngramsSymbolicCodes[curCode];
            curMatchesItEnd = curMatches.end();
            curText = currentWordParts[0].trimmed();
						curText += "<i>/" + curCat + "</i>";
//             qDebug() << "Entry" << curText << curCat;
            for(curMatchesIt = curMatches.begin(); curMatchesIt != curMatchesItEnd; curMatchesIt++)
                for(int i = 0; i < curMatchesIt.value().size(); i++)
                    curMatchesIt.value()[i] = curMatchesIt.value()[i] + curText + " ";
            if(!prevPrevCat.isEmpty())
            {
                QString triText = curSentence;
                triText += " <b>[ " +prevPrevText + " ";
                triText += prevText + " ";
								triText += curText + "]</b> ";
                curMatches[prevPrevCat + "/" + prevCat + "/" + curCat] << triText;
                curMatches[prevPrevCat + "/" + prevCat + "/" + NGramEntry::bigramsCategoryName] << triText;
                curSentence += prevPrevText + " ";
            }
            prevPrevText = prevText;
            prevPrevCode = prevCode;
						prevPrevCat = prevCat;
            prevText = curText;
            prevCode = curCode;
            prevCat = curCat;
            if(curCat == "L_PONCTU_FORTE" || curCat == "SENT")
            {
                curSentence = "<br/>> ";
                prevPrevText = prevText = "";
                for(curMatchesIt = curMatches.begin(); curMatchesIt != curMatchesItEnd; curMatchesIt++)
                    ngramsMatches[curMatchesIt.key()] << curMatchesIt.value();
                curMatches.clear();
            }
        }
    }
    ngramsCorpusFile.close();
}

void ResourceTool::nGramsShiftSearchLeft ()
{
  nGramsSearchCb1->setCurrentIndex(nGramsSearchCb2->currentIndex());
  nGramsSearchCb2->setCurrentIndex(nGramsSearchCb3->currentIndex());
  nGramsSearchCb3->setCurrentIndex(0);
  nGramsSearch();
}

void ResourceTool::nGramsShiftSearchRight ()
{
  nGramsSearchCb3->setCurrentIndex(nGramsSearchCb2->currentIndex());
  nGramsSearchCb2->setCurrentIndex(nGramsSearchCb1->currentIndex());
  nGramsSearchCb1->setCurrentIndex(0);
  nGramsSearch();
}

void ResourceTool::nGramsSearch ()
{
    QStringList searchArgs;
    searchArgs << nGramsSearchCb1->currentText();
    searchArgs << nGramsSearchCb2->currentText();
    searchArgs << nGramsSearchCb3->currentText();
    int nbEntries = nGramsModel->searchEntries(searchArgs);
    nGramsCountLbl->setText(QString::number(nbEntries)+" entries found");
    nGramsView->resizeColumnsToContents();
    nGramsShowCorpus();
}

void ResourceTool::nGramsShowCorpus ()
{
    QStringList matchArgs;
    if(nGramsView->selectionModel()->selectedRows(0).size() > 0)
    {
        matchArgs << nGramsView->selectionModel()->selectedRows(0).first().data().toString();
        matchArgs << nGramsView->selectionModel()->selectedRows(1).first().data().toString();
        matchArgs << nGramsView->selectionModel()->selectedRows(2).first().data().toString();
        QString matchArgsCode = matchArgs.join("/");
        if(ngramsMatches.contains(matchArgsCode))
        {
            QStringList matches = ngramsMatches[matchArgsCode];
            QString matchesNb = QString::number(matches.size());
						nGramsMatchesEdit->clear();
            nGramsMatchesEdit-> appendHtml("Found "+matchesNb+" distinct matches in corpus :\n"+matches.join("\n"));
            return;
        }
    }
    nGramsMatchesEdit->setPlainText("No matches found in corpus (or empty selection)");
}

void ResourceTool::nGramsSearchReset ()
{
    nGramsSearchCb1->setCurrentIndex(0);
    nGramsSearchCb2->setCurrentIndex(0);
    nGramsSearchCb3->setCurrentIndex(0);
    nGramsSearch();
}

void ResourceTool::nGramsEditCorpus ()
{
///@TODO Possible memory leak:  ensure that this object will be deleted
    ResourceFileEditDlg* dialog = new ResourceFileEditDlg();
    dialog->init(linguisticDataRoot+"/disambiguisationMatrices/fre/corpus/corpus_fr_ajout.txt");
    dialog->textEdit->setTabStopWidth(180);
    dialog->setWindowModality(Qt::NonModal);
    dialog->show();
}

void ResourceTool::nGramsConfirmInstall ()
{
    if(!checkEvaluateReady())
        return;
    nGramsView->confirmInstall();
}

void ResourceTool::nGramsInstalled (bool success)
{
    resourceInstalled(success);
    nGramsCorpusInit(ngramsSymbolicCodesPath, ngramsCorpusPath);
    nGramsModel->availableData.clear();
    nGramsModel->loadData<NGramEntry>();
    nGramsSearch();
}

void ResourceTool::syntacticRulesInit (QStringList resourcePaths, QString installComand)
{
    syntacticRulesLoadData(resourcePaths, installComand);
    syntacticRulesConnectBtnSignals();
}

void ResourceTool::syntacticRulesLoadData (QStringList resourcePaths, QString installComand)
{
///@TODO Possible memory leak:  ensure that this object will be deleted
    syntacticRulesModel = new ResourceReaderToolBoxModel();
    syntacticRulesModel->init(resourcePaths, installComand);
    syntacticRulesModel->loadData<SyntacticRuleEntry>();
    syntacticRulesCategoryCb->clear();
    syntacticRulesCategoryCb->addItem(SyntacticRuleEntry::allCategoryName);
    for(int i = 0; i < syntacticRulesModel->categoryList.size(); i++)
        syntacticRulesCategoryCb->addItem(syntacticRulesModel->categoryList.at(i));
    SyntacticRuleEntry::matchNames.sort();
    syntacticRulesMatchCb->clear();
    syntacticRulesMatchCb->addItem(SyntacticRuleEntry::allCategoryName);
    for(int i = 0; i < SyntacticRuleEntry::matchNames.size(); i++)
        syntacticRulesMatchCb->addItem(SyntacticRuleEntry::matchNames.at(i));
    SyntacticRuleEntry::relationNames.sort();
    syntacticRulesCreateCb->clear();
    syntacticRulesCreateCb->addItem(SyntacticRuleEntry::allCategoryName);
    for(int i = 0; i < SyntacticRuleEntry::relationNames.size(); i++)
        syntacticRulesCreateCb->addItem(SyntacticRuleEntry::relationNames.at(i));
    syntacticRulesView->init(syntacticRulesModel);
    connect(syntacticRulesModel, SIGNAL(dataInstalled(bool)), this, SLOT(syntacticRulesInstalled(bool)));
}

void ResourceTool::syntacticRulesConnectBtnSignals ()
{
    connect(syntacticRulesSearchBtn, SIGNAL(clicked(bool)), this, SLOT(syntacticRulesSearch()));
    connect(syntacticRulesPreviousMatchBtn, SIGNAL(clicked(bool)), this, SLOT(syntacticRulesPreviousMatch()));
    connect(syntacticRulesNextMatchBtn, SIGNAL(clicked(bool)), this, SLOT(syntacticRulesNextMatch()));
    connect(syntacticRulesInstallBtn, SIGNAL(clicked(bool)), this, SLOT(syntacticRulesConfirmSaveAndInstall()));
    syntacticRulesSearch();
}

void ResourceTool::syntacticRulesSearch ()
{
    QStringList searchArgs;
    searchArgs << syntacticRulesSearchInp->text();
    searchArgs << syntacticRulesCategoryCb->currentText();
    searchArgs << syntacticRulesMatchCb->currentText();
    searchArgs << syntacticRulesCreateCb->currentText();
    int nbEntries = syntacticRulesModel->searchEntries(searchArgs);
    syntacticRulesCountLbl->setText(QString::number(nbEntries)+" entries found");
    syntacticRulesCurrentMatch = 0;
    syntacticRulesGotoCurrentMatch();
}

void ResourceTool::syntacticRulesPreviousMatch ()
{
    if(syntacticRulesCurrentMatch > 0)
        syntacticRulesCurrentMatch--;
    syntacticRulesGotoCurrentMatch();
}

void ResourceTool::syntacticRulesNextMatch ()
{
    if(syntacticRulesCurrentMatch < syntacticRulesModel->foundData.size() - 1)
        syntacticRulesCurrentMatch++;
    syntacticRulesGotoCurrentMatch();
}

void ResourceTool::syntacticRulesGotoCurrentMatch ()
{
    syntacticRulesPreviousMatchBtn->setEnabled(syntacticRulesCurrentMatch > 0);
    syntacticRulesNextMatchBtn->setEnabled(syntacticRulesCurrentMatch < syntacticRulesModel->foundData.size()-1);
    if(syntacticRulesModel->foundData.size() == 0)
        return;
    SyntacticRuleEntry* entry;
    entry = (SyntacticRuleEntry*)syntacticRulesModel->getFoundEntry(syntacticRulesCurrentMatch);
    syntacticRulesView->moveCursorToSourceLine(entry->sourceFile, entry->sourceLine);
}

void ResourceTool::syntacticRulesConfirmSaveAndInstall()
{
    if(!syntacticRulesView->checkInstallingReady())
        return;
    if(QMessageBox::question(
       this,
       "Save, compile and install all syntactic rules resource files ?",
       "All syntactic rules files will be saved, compiled and installed do you want to proceed ?",
       QMessageBox::Ok | QMessageBox::Cancel,
       QMessageBox::Cancel
    ) == QMessageBox::Cancel)
        return;
    if(!checkEvaluateReady())
        return;
    syntacticRulesSaveAndInstall();
}

void ResourceTool::syntacticRulesSaveAndInstall()
{
    for(int i = 0; i < syntacticRulesView->count(); i++)
        ((ResourceFileEditDlg*)syntacticRulesView->widget(i))->saveFile();
    syntacticRulesModel->install();
}

void ResourceTool::syntacticRulesInstalled (bool success)
{
    int currentIndex = syntacticRulesView->currentIndex();
    QTextCursor currentCursor = ((ResourceFileEditDlg*)syntacticRulesView->currentWidget())->textEdit->textCursor();
    resourceInstalled(success);
    syntacticRulesLoadData(syntacticRulesResourcePaths, syntacticRulesInstallCommand);
    syntacticRulesView->setCurrentIndex(currentIndex);
    ((ResourceFileEditDlg*)syntacticRulesView->currentWidget())->textEdit->setTextCursor(currentCursor);
    ((ResourceFileEditDlg*)syntacticRulesView->currentWidget())->textEdit->ensureCursorVisible();
}

void ResourceTool::idiomaticExpressionsInit (QString resourcePath, QString installComand)
{
///@TODO Possible memory leak:  ensure that this object will be deleted
    idiomaticExpressionsModel = new ResourceEditorTableModel();
    idiomaticExpressionsModel->init(resourcePath, installComand);
    idiomaticExpressionsModel->columnCountPerEntry = IdiomaticExpressionEntry::columnCountPerEntry;
    idiomaticExpressionsModel->columnHeaders = IdiomaticExpressionEntry::columnHeaders;
    idiomaticExpressionsModel->loadData<IdiomaticExpressionEntry>();
    idiomaticExpressionsSearchCb->addItem(IdiomaticExpressionEntry::allCategoryName);
    for(int i = 0; i < idiomaticExpressionsModel->categoryList.size(); i++)
        idiomaticExpressionsSearchCb->addItem(idiomaticExpressionsModel->categoryList.at(i));
    idiomaticExpressionsView->init(idiomaticExpressionsModel);
    connect(idiomaticExpressionsModel, SIGNAL(dataChanged()), this, SLOT(idiomaticExpressionsSearch()));
    connect(idiomaticExpressionsModel, SIGNAL(dataInstalled(bool)), this, SLOT(resourceInstalled(bool)));
    connect(idiomaticExpressionsSearchBtn, SIGNAL(clicked(bool)), this, SLOT(idiomaticExpressionsSearch()));
    connect(idiomaticExpressionsSearchInp, SIGNAL(returnPressed()), this, SLOT(idiomaticExpressionsSearch()));
    connect(idiomaticExpressionsView, SIGNAL(editEntryDlg(AbstractResourceEntry*)), this, SLOT(idiomaticExpressionsEditDlg(AbstractResourceEntry*)));
    connect(idiomaticExpressionsAddBtn, SIGNAL(clicked(bool)), idiomaticExpressionsView, SLOT(createEntry()));
    connect(idiomaticExpressionsInstallBtn, SIGNAL(clicked(bool)), this, SLOT(idiomaticExpressionsConfirmInstall()));
    idiomaticExpressionsSearch();
}

void ResourceTool::idiomaticExpressionsEditDlg (AbstractResourceEntry* are)
{
///@TODO Possible memory leak:  ensure that this object will be deleted
    IdiomaticExpressionEntryEditDlg* idiomaticExpressionEntryEditDlg = new IdiomaticExpressionEntryEditDlg();
    idiomaticExpressionEntryEditDlg->init(idiomaticExpressionsModel, are);
}

void ResourceTool::idiomaticExpressionsSearch ()
{
    QStringList searchArgs;
    searchArgs << idiomaticExpressionsSearchInp->text();
    searchArgs << idiomaticExpressionsSearchCb->currentText();
    int nbEntries = idiomaticExpressionsModel->searchEntries(searchArgs);
    idiomaticExpressionsCountLbl->setText(QString::number(nbEntries)+" entries found");
    idiomaticExpressionsView->resizeColumnsToContents();
}

void ResourceTool::idiomaticExpressionsConfirmInstall ()
{
    if(!checkEvaluateReady())
        return;
    idiomaticExpressionsView->confirmInstall();
}

bool ResourceTool::isInstalling ()
{
    return dictionnaryModel->installStatus == ResourceReaderTableModel::INSTALL_PROCESSING
        || nGramsModel->installStatus == ResourceReaderTableModel::INSTALL_PROCESSING
        || syntacticRulesModel->installStatus == ResourceReaderTableModel::INSTALL_PROCESSING
        || idiomaticExpressionsModel->installStatus == ResourceReaderTableModel::INSTALL_PROCESSING;
}

bool ResourceTool::checkInstallingReady ()
{
    if(isInstalling()){
        if(QMessageBox::question(
            this,
            "Abort installation process ?",
            "An installation is currently in progress, interrupting it may cause indesirable results, do you really want to abort ?",
            QMessageBox::Ok | QMessageBox::Cancel,
            QMessageBox::Cancel
          ) == QMessageBox::Cancel)
        {
            return false;
        }
    }
    return true;
}

bool ResourceTool::checkEvaluateReady ()
{
    if(evaluationStatus != EVALUATION_READY)
    {
        if(QMessageBox::question(
            this,
            "Last evaluation is not finished",
            "An evaluation or an installation is currently in progress, this action may cause indesirable results, do you really want to proceed ?",
            QMessageBox::Ok | QMessageBox::Cancel,
            QMessageBox::Cancel
          ) == QMessageBox::Cancel)
        {
            return false;
        }
    }
    return true;
}

void ResourceTool::confirmEvaluate ()
{
    if(!checkInstallingReady())
        return;
    if(!checkEvaluateReady())
        return;
    if(QMessageBox::question(
        this,
        "Install resources and evaluate ?",
        "All resources will be saved, compiled and installed, and evaluation will be triggerred do you start this now ?",
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel
      ) == QMessageBox::Cancel)
        return;
    qDebug() << "Starting all resources installations";
    evaluationStatus = EVALUATION_INSTALLING;
    dictionnaryModel->install();
    nGramsModel->install();
    syntacticRulesSaveAndInstall();
    idiomaticExpressionsModel->install();
    resourceInstalled(true);
}

void ResourceTool::resourceNotInstalled ()
{
    if(QMessageBox::question(
       this,
       "Error while installing resource",
       "All resources will be saved, compiled and installed, and evaluation will be triggerred do you start this now ?",
       QMessageBox::Ok | QMessageBox::Cancel,
       QMessageBox::Cancel
    ) == QMessageBox::Cancel)
        return;
}

void ResourceTool::resourceInstalled (bool success)
{
    if(!success)
    {
        if(evaluationStatus != EVALUATION_INSTALLING)
            QMessageBox::warning(
                this,
                "Error while installing resource",
                "The resource could not be installed"
            );
        else if(QMessageBox::question(
           this,
           "Error while installing resource",
           "A resource could not be installed, do you want to abort evaluation ?",
           QMessageBox::Ok | QMessageBox::Cancel,
           QMessageBox::Cancel
        ) == QMessageBox::Ok)
            evaluationStatus = EVALUATION_READY;
    }
    if(isInstalling())
        return;
    if(evaluationStatus != EVALUATION_INSTALLING)
        return;
    if(!pelfSharedMemory->isAttached())
    {
        qDebug() << "No communication with benchmarking tool, aborting evaluation";
        return;
    }
    qDebug() << "Starting evaluation by communicating with benchmarking tool";
    evaluationStatus = EVALUATION_BENCHMARKING;
    sendBenmarkingToolMsg("BTCMD:EVALUATE;");
}

void ResourceTool::checkBenmarkingToolMsg ()
{
    if(popPelfSharedMemory("RTCMD:EVALUATION_ABORTED;"))
    {
        QMessageBox::warning(
            this,
            "Evaluation aborted",
            "Benchmarking tool has aborted evaluation, please check the error in the benchmarking tool"
        );
    }
    if(popPelfSharedMemory("RTCMD:EVALUATION_FINISHED;") && evaluationStatus == EVALUATION_BENCHMARKING)
    {
        evaluationStatus = EVALUATION_READY;
        QMessageBox::warning(
            this,
            "Evaluation finished",
            "Benchmarking tool has finished latest evaluation, you may check results"
        );
    }
}

void ResourceTool::sendBenmarkingToolMsg (QString msg)
{
    if(!pelfSharedMemory->isAttached())
        return;
    pelfSharedMemory->lock();
    writePelfSharedMemory(msg.prepend(readPelfSharedMemory()));
    pelfSharedMemory->unlock();
}

void ResourceTool::initPelfSharedMemory ()
{
///@TODO Possible memory leak:  ensure that this object will be deleted
    pelfSharedMemory = new QSharedMemory("PelfRtBt", this);
    if(pelfSharedMemory->isAttached())
        pelfSharedMemory->detach();
    if(!pelfSharedMemory->create(1000))
        if(!pelfSharedMemory->attach())
            qDebug() << "Unable to create or attach shared memory (" << pelfSharedMemory->error() << ")";
    if(!pelfSharedMemory->isAttached())
        return;
    writePelfSharedMemory();
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkBenmarkingToolMsg()));
    timer->start(1000);
}

QString ResourceTool::readPelfSharedMemory ()
{
    return QString(QByteArray((char*)pelfSharedMemory->data(), pelfSharedMemory->size()));
}

void ResourceTool::writePelfSharedMemory (QString msg)
{
    memcpy(pelfSharedMemory->data(), msg.toUtf8(), qMin(pelfSharedMemory->size(), msg.size() + 1));
}

bool ResourceTool::popPelfSharedMemory (QString msg)
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

void ResourceTool::closeEvent (QCloseEvent *event)
{
    if(!checkInstallingReady() || !checkEvaluateReady())
    {
        event->ignore();
        return;
    }
    qDebug() << "Closing application";
    settings->setValue("geometry", saveGeometry());

    settings->setValue("messagesSplitter", messagesSplitter->saveState());
    settings->setValue("ngramsCategoriesSplitter", ngramsCategoriesSplitter->saveState());
    settings->setValue("ngramsOccurencesSplitter", ngramsOccurencesSplitter->saveState());

    QMainWindow::closeEvent(event);
}

void ResourceTool::logDebugMsg (QtMsgType type, const char* m)
{
    QString msgHtml;
    QString msg = QString::fromUtf8(m);
    switch (type)
    {
    case QtDebugMsg:
        msgHtml = "<font style=\"color: blue;\">"+msg+"</font>";
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

#include "ResourceTool.moc"
