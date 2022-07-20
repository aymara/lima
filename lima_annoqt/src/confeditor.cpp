// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Gael.de-Chalendar@cea.fr   *
 ***************************************************************************/

#include "confeditor.h"
#include "annotationConfigurationHandler.h"
#include "kcolorvalueselector.h"
#include "kcolordialog.h"

#include <QMessageBox>
#include <QApplication>
#include <QToolBar>
#include <QStatusBar>
#include <QSplitter>
#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>
#include <iostream>
#include <QAction>
#include <QDebug>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QTextCodec>

AnnoqtConfEditor::AnnoqtConfEditor() : m_colorNames2EntityTypes(), m_entityTypes2ColorNames(), m_currentColor(QColor(Qt::black)), m_currentAnnotationConfigurationFile(),m_currentDirectory(),
m_currentItem(0)
{
  QFont font = QApplication::font();
  font.setPointSize(12);
  QApplication::setFont(font);

  QSplitter *splitter = new QSplitter( this );
  setCentralWidget( splitter );

  m_listWidget = new QListWidget();
  splitter->addWidget( m_listWidget );

  m_colorPicker = new KColorDialog(this);
  m_colorPicker->setColor( Qt::white );
  splitter->addWidget( m_colorPicker );

  QList<int> l;
  l.push_back( 100 );
  l.push_back( 600 );
  splitter->setSizes( l );

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();

  readSettings();

  connect( m_listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
           this, SLOT(slotTypesListItemclicked(QListWidgetItem*) ) );

  connect( m_colorPicker, SIGNAL(colorSelected(QColor&)), this, SLOT(slotColorSelected(QColor&)) );
}

AnnoqtConfEditor::~AnnoqtConfEditor()
{
}

void AnnoqtConfEditor::closeEvent( QCloseEvent *event )
{
  if ( maybeSave() )
  {
    writeSettings();
    event->accept();
  }
  else
  {
    event->ignore();
  }
}

void AnnoqtConfEditor::slotNewConfiguration()
{
  m_listWidget->clear();
  m_currentAnnotationConfigurationFile = QString();
  m_currentDirectory = QString();
  m_currentItem = 0;
}

void AnnoqtConfEditor::slotOpen()
{
  if ( maybeSave() )
  {
    QString fileName = QFileDialog::getOpenFileName( this,
        "Select a Text to Annotate", m_currentDirectory.isEmpty()?QString():m_currentDirectory  );

    if ( !fileName.isEmpty() )
    {
      m_currentDirectory = QFileInfo(fileName).absoluteDir().absolutePath();
      loadAnnotationConfigurationFile( fileName );
    }
  }
}

bool AnnoqtConfEditor::slotSave()
{
  if ( m_currentAnnotationConfigurationFile.isEmpty() )
  {
    return slotSaveAs();
  }
  else
  {
    return saveFile( m_currentAnnotationConfigurationFile );
  }
}

bool AnnoqtConfEditor::slotSaveAs()
{
  QString fileName = QFileDialog::getSaveFileName( this );

  if ( fileName.isEmpty() )
    return false;

  return saveFile( fileName );
}

void AnnoqtConfEditor::slotAbout()
{
  QMessageBox::about( this, tr( "Annotation Configuration Tool" ),
                      tr( "The <b>Annotation Configure Tool</b> allows to configure the annotation tool.<br>"
                          "Copyright 2008-2019 CEA LIST" ) );
}

void AnnoqtConfEditor::slotDocumentWasModified()
{
  setWindowModified( true );
}

void AnnoqtConfEditor::createActions()
{
  newAct = new QAction( QIcon( ":/filenew.xpm" ), tr( "&New" ), this );
  newAct->setShortcut( tr( "Ctrl+N" ) );
  newAct->setStatusTip( tr( "Create a new file" ) );
  connect( newAct, SIGNAL( triggered() ), this, SLOT( slotNewConfiguration() ) );

  openAct = new QAction( QIcon( ":/fileopen.xpm" ), tr( "&Open..." ), this );
  openAct->setShortcut( tr( "Ctrl+O" ) );
  openAct->setStatusTip( tr( "Open an existing file" ) );
  connect( openAct, SIGNAL( triggered() ), this, SLOT( slotOpen() ) );

  saveAct = new QAction( QIcon( ":/filesave.xpm" ), tr( "&Save" ), this );
  saveAct->setShortcut( tr( "Ctrl+S" ) );
  saveAct->setStatusTip( tr( "Save the document to disk" ) );
  connect( saveAct, SIGNAL( triggered() ), this, SLOT( slotSave() ) );

  saveAsAct = new QAction( tr( "Save &As..." ), this );
  saveAsAct->setStatusTip( tr( "Save the document under a new name" ) );
  connect( saveAsAct, SIGNAL( triggered() ), this, SLOT( slotSaveAs() ) );

  exitAct = new QAction( tr( "E&xit" ), this );
  exitAct->setShortcut( tr( "Ctrl+Q" ) );
  exitAct->setStatusTip( tr( "Exit the application" ) );
  connect( exitAct, SIGNAL( triggered() ), this, SLOT( close() ) );

  cutAct = new QAction( QIcon( ":/editcut.xpm" ), tr( "Cu&t" ), this );
  cutAct->setShortcut( tr( "Ctrl+X" ) );
  cutAct->setStatusTip( tr( "Cut the current selection's contents to the "
                            "clipboard" ) );
  connect( cutAct, SIGNAL( triggered() ), this, SLOT( slotCut() ) );

  addItemAction = new QAction( QIcon( ":/item.xpm" ), tr( "New entity type" ), this );
  addItemAction->setShortcut( tr( "Ctrl+E" ) );
  addItemAction->setStatusTip( tr( "Add a new entity type" ) );
  connect( addItemAction, SIGNAL( triggered() ), this, SLOT( slotAddItemAction() ) );

  aboutAct = new QAction( tr( "&About" ), this );
  aboutAct->setStatusTip( tr( "Show the application's About box" ) );
  connect( aboutAct, SIGNAL( triggered() ), this, SLOT( slotAbout() ) );

  aboutQtAct = new QAction( tr( "About &Qt" ), this );
  aboutQtAct->setStatusTip( tr( "Show the Qt library's About box" ) );
  connect( aboutQtAct, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );

  cutAct->setEnabled( true );
}

void AnnoqtConfEditor::createMenus()
{
  fileMenu = menuBar()->addMenu( tr( "&File" ) );
  fileMenu->addAction( newAct );
  fileMenu->addAction( openAct );
  fileMenu->addAction( saveAct );
  fileMenu->addAction( saveAsAct );
  fileMenu->addSeparator();
  fileMenu->addAction( exitAct );

  editMenu = menuBar()->addMenu( tr( "&Edit" ) );
  editMenu->addAction( addItemAction );
  editMenu->addAction( cutAct );
  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu( tr( "&Help" ) );
  helpMenu->addAction( aboutAct );
  helpMenu->addAction( aboutQtAct );
}

void AnnoqtConfEditor::createToolBars()
{
  fileToolBar = addToolBar( tr( "File" ) );
  fileToolBar->addAction( newAct );
  fileToolBar->addAction( openAct );
  fileToolBar->addAction( saveAct );

  editToolBar = addToolBar( tr( "Edit" ) );
  editToolBar->addAction( addItemAction );
  editToolBar->addAction( cutAct );
}

void AnnoqtConfEditor::createStatusBar()
{
  statusBar()->showMessage( tr( "Ready" ) );
}

void AnnoqtConfEditor::readSettings()
{
  QSettings settings( "LIMA", "Annotation Configuration Tool" );
  QPoint pos = settings.value( "pos", QPoint( 200, 200 ) ).toPoint();
  QSize size = settings.value( "size", QSize( 400, 400 ) ).toSize();
  m_currentDirectory = settings.value( "dirconfig", QString() ).toString();
  QString annotconfig = settings.value( "annotconfig", QString() ).toString();
  if (!annotconfig.isEmpty())
  {
    loadAnnotationConfigurationFile(annotconfig);
  }

  QString fileconfig = settings.value( "fileconfig", QString() ).toString();
  if (!fileconfig.isEmpty())
  {
    loadFile(fileconfig);
  }
  resize( size );
  move( pos );
}

void AnnoqtConfEditor::writeSettings()
{
  QSettings settings( "LIMA", "Annotation Configuration Tool" );
  settings.setValue( "pos", pos() );
  settings.setValue( "size", size() );
  settings.setValue( "dirconfig", m_currentDirectory );
  settings.setValue( "annotconfig", m_currentAnnotationConfigurationFile );
}

bool AnnoqtConfEditor::maybeSave()
{
  if ( isWindowModified() )
  {
    int ret = QMessageBox::warning( this, tr( "Application" ),
                                    tr( "The configuration has been modified.\n"
                                        "Do you want to save your changes?" ),
                                    QMessageBox::Yes | QMessageBox::Default,
                                    QMessageBox::No,
                                    QMessageBox::Cancel | QMessageBox::Escape );

    if ( ret == QMessageBox::Yes )
      return slotSave();
    else
      if ( ret == QMessageBox::Cancel )
        return false;
  }

  return true;
}

void AnnoqtConfEditor::loadFile( const QString &fileName )
{
  QFile file( fileName );

  if ( !file.open( QFile::ReadOnly | QFile::Text ) )
  {
    QMessageBox::warning( this, tr( "Application" ),
                          tr( "Cannot read file %1:\n%2." )
                          .arg( fileName, file.errorString() ) );
    return;
  }

  setWindowModified( false );

  QTextStream in( &file );
  in.setCodec(QTextCodec::codecForName("UTF-8"));
  QApplication::setOverrideCursor( Qt::WaitCursor );


  QApplication::restoreOverrideCursor();

  setCurrentFile( fileName );
  statusBar()->showMessage( tr( "File loaded" ), 2000 );
}

bool AnnoqtConfEditor::saveFile( const QString &fileName )
{
  QFile file( fileName );

  if ( !file.open( QFile::WriteOnly | QFile::Text ) )
  {
    QMessageBox::warning( this, tr( "Application" ),
                          tr( "Cannot write file %1:\n%2." )
                          .arg( fileName, file.errorString() ) );
    return false;
  }

  QTextStream out( &file );

  out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << QTENDL;
  out << "<annotationConfiguration>" << QTENDL;
  out << "  <entities>" << QTENDL;
  QApplication::setOverrideCursor( Qt::WaitCursor );

  for (int i = 0; i < m_listWidget->count(); i++)
  {
    QListWidgetItem* item = m_listWidget->item(i);
    out << "    <entity name=\""<<item->text()<<"\" color=\""<<item->background().color().name()<<"\"";
    if (item->checkState() == Qt::Checked)
    {
      out << " recursive=\"true\"";
    }
    out << "/>" << QTENDL;
  }



  QApplication::restoreOverrideCursor();
  out << "  </entities>" << QTENDL;
  out << "  <templates>" << QTENDL;
  out << "  </templates>" << QTENDL;
  out << "</annotationConfiguration>" << QTENDL;

  statusBar()->showMessage( tr( "File saved" ), 2000 );
  setWindowModified( false );
  return true;
}

void AnnoqtConfEditor::setCurrentFile( const QString &fileName )
{
  m_currentAnnotationConfigurationFile = fileName;
  setWindowModified( false );

  QString shownName;

  if ( m_currentAnnotationConfigurationFile.isEmpty() )
    shownName = "untitled.xml";
  else
    shownName = strippedName( m_currentAnnotationConfigurationFile );

  setWindowTitle( tr( "%1[*] - %2" ).arg( shownName, tr( "Application" ) ) );
}

QString AnnoqtConfEditor::strippedName( const QString &fullFileName )
{
  return QFileInfo( fullFileName ).fileName();
}

void AnnoqtConfEditor::slotTypesListItemclicked( QListWidgetItem* item )
{
  if ( m_currentItem != 0 )
  {
    m_currentItem->setForeground(Qt::white);
    m_currentItem->setSelected(false);
    QFont font = m_currentItem->font();
    font.setBold(item->checkState() == Qt::Checked);
    font.setItalic(item->checkState() == Qt::Checked);
    m_currentItem->setFont(font);
  }
  if ( item != 0 )
  {
    m_currentItem = item;
    m_currentItem->setForeground(Qt::black);
    m_currentItem->setSelected(true);
    QFont font = m_currentItem->font();
    font.setBold(true);
    font.setItalic(item->checkState() == Qt::Checked);
    m_currentItem->setFont(font);
    m_currentColor = item->background().color();
  }
  m_listWidget->setCurrentItem(0);
}

void AnnoqtConfEditor::slotCut()
{
  if (m_currentItem != 0)
  {
    m_listWidget->takeItem(m_listWidget->row(m_currentItem));
    delete m_currentItem;
    m_currentItem = 0;
  }
}

void AnnoqtConfEditor::loadAnnotationConfigurationFile(const QString& fileName)
{
  if (fileName.isEmpty())
      return;

  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
      QMessageBox::warning(this, tr("Annotation Tool"),
                            tr("Cannot read file %1:\n%2.")
                            .arg(fileName, file.errorString()));
      return;
  }

  m_currentAnnotationConfigurationFile = fileName;

  m_listWidget->clear();
  m_colors.clear();
  setWindowModified( false );

  QList<QString> recursiveEntityTypes;
  AnnotationConfigurationHandler handler(m_listWidget, &m_colors, &m_colorNames2EntityTypes, &recursiveEntityTypes, true);
  QXmlSimpleReader reader;
  reader.setContentHandler(&handler);
  reader.setErrorHandler(&handler);

  QXmlInputSource xmlInputSource(&file);
  if (reader.parse(xmlInputSource))
      statusBar()->showMessage(tr("Annotation Configuration File loaded"), 2000);

  for (auto it = m_colorNames2EntityTypes.begin();
        it != m_colorNames2EntityTypes.end(); it++)
  {
    m_entityNames2Types.insert(it.value(),m_entityNames2Types.size());
    m_entityTypes2Names.insert(m_entityNames2Types.size()-1,it.value());
    m_entityTypes2ColorNames.insert(m_entityNames2Types[it.value()],it.key());
  }
  statusBar()->showMessage( tr( "Loaded configuration file " ) + fileName );
}

void AnnoqtConfEditor::slotColorSelected( const QColor& color )
{
  qDebug() << "AnnoqtConfEditor::slotColorSelected( "<<color<<" )";
  if (m_currentItem != 0)
  {
    m_currentItem->setBackgroundColor  ( color );
    setWindowModified(true);
  }
}

void AnnoqtConfEditor::slotAddItemAction()
{
  QListWidgetItem* item = new QListWidgetItem("New Type", m_listWidget);
  item->setFlags( item->flags() | Qt::ItemIsEditable );
  item->setCheckState(Qt::Unchecked);
  QColor color = m_colorPicker->color();
  item->setBackgroundColor(color);
  setWindowModified(true);
}
