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
/***************************************************************************
 *   Copyright (C) 2007 by CEA LIST / LVIC   *
 *   Gael.de-Chalendar@cea.fr   *
 ***************************************************************************/

#include <QtGui>
#include "annoqt.h"
#include "annotationConfigurationHandler.h"
#include "specificEntitiesHandler.h"
#include "annotationEditWidget.h"

#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <iostream>

#define VERSION "0.2.3"

Annoqt::Annoqt() : m_colorNames2EntityTypes(), m_entityTypes2ColorNames(), m_currentColor(QColor(Qt::black)), m_currentAnnotationConfigurationFile(),m_currentDirectory(),
m_entities(),
m_entitiesMap(),
m_currentEntity(0)
{
  QFont font = QApplication::font();
  font.setPointSize(12);
  QApplication::setFont(font);

  QSplitter *splitter = new QSplitter( this );
  setCentralWidget( splitter );

  m_listWidget = new QListWidget();
//   m_listWidget->setSortingEnabled(true);
  splitter->addWidget( m_listWidget );

  m_textEdit = new AnnotationEditWidget(this);
  m_textEdit->setReadOnly ( true );
  splitter->addWidget( m_textEdit );
  connect(m_textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(slotTextCursorPositionChanged()));

  QList<int> l;
  l.push_back( 100 );
  l.push_back( 400 );
  splitter->setSizes( l );

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();

  readSettings();

  connect( m_textEdit->document(), SIGNAL( contentsChanged() ),
           this, SLOT( documentWasModified() ) );

  connect( m_listWidget, SIGNAL( clicked( const QModelIndex & ) ),
          this, SLOT( slotTypesListItemclicked( const QModelIndex & ) ) );
}

void Annoqt::closeEvent( QCloseEvent *event )
{
  qDebug() << "Annoqt::closeEvent";
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

void Annoqt::newConfiguration()
{
  qDebug() << "Annoqt::newConfiguration";
  QString fileName = QFileDialog::getOpenFileName( this,
    "Select an Annotation Configuration File", m_currentDirectory.isEmpty()?QString():m_currentDirectory );

  if ( !fileName.isEmpty() )
  {
    m_currentDirectory = QFileInfo(fileName).absoluteDir().absolutePath();
    m_textEdit->clear();
    loadAnnotationConfigurationFile(fileName);
  }
}

void Annoqt::open()
{
  qDebug() << "Annoqt::open";
  if ( maybeSave() )
  {
    QString fileName = QFileDialog::getOpenFileName( this, 
        "Select a Text to Annotate", m_currentDirectory.isEmpty()?QString():m_currentDirectory  );

    if ( !fileName.isEmpty() )
    {
      m_currentDirectory = QFileInfo(fileName).absoluteDir().absolutePath();
      loadFile( fileName );
    }
  }
}

bool Annoqt::save()
{
  qDebug() << "Annoqt::save";
  if ( m_entitiesFileName.isEmpty() )
  {
    return saveAs();
  }
  else
  {
    return saveFile( m_entitiesFileName );
  }
}

bool Annoqt::saveAs()
{
  qDebug() << "Annoqt::saveAs";
  m_entitiesFileName = QFileDialog::getSaveFileName( this );

  if ( m_entitiesFileName.isEmpty() )
    return false;

  return saveFile( m_entitiesFileName );
}

void Annoqt::about()
{
  qDebug() << "Annoqt::about";
  QMessageBox::about( this, tr( "Annotation Tool" ),
                      tr( "The <b>Annotation Tool</b> allows to annotate texts with "
                          "specific entities.<br>Version %1<br>"
                          "Copyright 2007-2009 CEA LIST/LVIC" ).arg(VERSION) );
}

void Annoqt::documentWasModified()
{
  qDebug() << "Annoqt::documentWasModified";
  setWindowModified( true );
}

void Annoqt::createActions()
{
  qDebug() << "Annoqt::createActions";
  newAct = new QAction( QIcon( ":/filenew.xpm" ), tr( "&New" ), this );
  newAct->setShortcut( tr( "Ctrl+N" ) );
  newAct->setStatusTip( tr( "Create a new file" ) );
  connect( newAct, SIGNAL( triggered() ), this, SLOT( newConfiguration() ) );

  openAct = new QAction( QIcon( ":/fileopen.xpm" ), tr( "&Open..." ), this );
  openAct->setShortcut( tr( "Ctrl+O" ) );
  openAct->setStatusTip( tr( "Open an existing file" ) );
  connect( openAct, SIGNAL( triggered() ), this, SLOT( open() ) );

  saveAct = new QAction( QIcon( ":/filesave.xpm" ), tr( "&Save" ), this );
  saveAct->setShortcut( tr( "Ctrl+S" ) );
  saveAct->setStatusTip( tr( "Save the document to disk" ) );
  connect( saveAct, SIGNAL( triggered() ), this, SLOT( save() ) );

  saveAsAct = new QAction( tr( "Save &As..." ), this );
  saveAsAct->setStatusTip( tr( "Save the document under a new name" ) );
  connect( saveAsAct, SIGNAL( triggered() ), this, SLOT( saveAs() ) );

  exitAct = new QAction( tr( "E&xit" ), this );
  exitAct->setShortcut( tr( "Ctrl+Q" ) );
  exitAct->setStatusTip( tr( "Exit the application" ) );
  connect( exitAct, SIGNAL( triggered() ), this, SLOT( close() ) );

  cutAct = new QAction( QIcon( ":/editcut.xpm" ), tr( "Cu&t" ), this );
  cutAct->setShortcut( tr( "Ctrl+X" ) );
  cutAct->setStatusTip( tr( "Cut the current selection's contents to the "
                            "clipboard" ) );
  connect( cutAct, SIGNAL( triggered() ), this, SLOT( removeEntity() ) );

  copyAct = new QAction( QIcon( ":/editcopy.xpm" ), tr( "&Copy" ), this );
  copyAct->setShortcut( tr( "Ctrl+C" ) );
  copyAct->setStatusTip( tr( "Copy the current selection's contents to the "
                             "clipboard" ) );
  connect( copyAct, SIGNAL( triggered() ), m_textEdit, SLOT( copy() ) );

  pasteAct = new QAction( QIcon( ":/editpaste.xpm" ), tr( "&Paste" ), this );
  pasteAct->setShortcut( tr( "Ctrl+W" ) );
  pasteAct->setStatusTip( tr( "Reuse the last used entity" ) );
  connect( pasteAct, SIGNAL( triggered() ), this, SLOT( paste() ) );

  aboutAct = new QAction( tr( "&About" ), this );
  aboutAct->setStatusTip( tr( "Show the application's About box" ) );
  connect( aboutAct, SIGNAL( triggered() ), this, SLOT( about() ) );

  aboutQtAct = new QAction( tr( "About &Qt" ), this );
  aboutQtAct->setStatusTip( tr( "Show the Qt library's About box" ) );
  connect( aboutQtAct, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );

  m_repeatAct = new QAction( tr( "Repeat" ), this );
  m_repeatAct->setStatusTip( tr( "Searches all occurrences of the last annotated text and repeat the same annotation on each" ) );
  connect( m_repeatAct, SIGNAL( triggered() ), this, SLOT( slotSearchAndAnnotate() ) );

  m_gotoAct = new QAction( tr( "Go to..." ), this );
  m_gotoAct->setStatusTip( tr( "Opens a dialog to ask where to move inside the text" ) );
  connect( m_gotoAct, SIGNAL( triggered() ), this, SLOT( slotGoto() ) );


  cutAct->setEnabled( false );
  copyAct->setEnabled( false );
//   connect( m_textEdit, SIGNAL( copyAvailable( bool ) ),
//            cutAct, SLOT( setEnabled( bool ) ) );
  connect( m_textEdit, SIGNAL( copyAvailable( bool ) ),
           copyAct, SLOT( setEnabled( bool ) ) );

  searchAction = new QAction( tr( "Search" ), this );
  searchAction->setShortcut( tr( "Ctrl+F" ) );
  searchAction->setStatusTip( tr( "Search a string of text" ) );
  connect( searchAction, SIGNAL( triggered() ), this, SLOT( slotSearch() ) );

  searchNextAction = new QAction( tr( "Search Next" ), this );
  searchNextAction->setShortcut( tr( "F3" ) );
  searchNextAction->setStatusTip( tr( "Search the next occurrence of the last searched text" ) );
  connect( searchNextAction, SIGNAL( triggered() ), this, SLOT( slotSearchNext() ) );
  
}

void Annoqt::createMenus()
{
  qDebug() << "Annoqt::createMenus";
  fileMenu = menuBar()->addMenu( tr( "&File" ) );
  fileMenu->addAction( newAct );
  fileMenu->addAction( openAct );
  fileMenu->addAction( saveAct );
  fileMenu->addAction( saveAsAct );
  fileMenu->addSeparator();
  fileMenu->addAction( exitAct );

  editMenu = menuBar()->addMenu( tr( "&Edit" ) );
  editMenu->addAction( cutAct );
//   editMenu->addAction( copyAct );
  editMenu->addAction( pasteAct );
  editMenu->addAction( m_repeatAct );
  editMenu->addSeparator();
  editMenu->addAction( searchAction );
  editMenu->addAction( searchNextAction );
  
  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu( tr( "&Help" ) );
  helpMenu->addAction( aboutAct );
  helpMenu->addAction( aboutQtAct );
}

void Annoqt::createToolBars()
{
  qDebug() << "Annoqt::createToolBars";
  fileToolBar = addToolBar( tr( "File" ) );
  fileToolBar->addAction( newAct );
  fileToolBar->addAction( openAct );
  fileToolBar->addAction( saveAct );

  editToolBar = addToolBar( tr( "Edit" ) );
  editToolBar->addAction( cutAct );
//   editToolBar->addAction( copyAct );
  editToolBar->addAction( pasteAct );
  editToolBar->addAction( m_repeatAct );
  editToolBar->addAction( m_gotoAct );
}

void Annoqt::createStatusBar()
{
  qDebug() << "Annoqt::createStatusBar";
  statusBar()->showMessage( tr( "Ready" ) );
}

void Annoqt::readSettings()
{
  qDebug() << "Annoqt::readSettings";
  QSettings settings( "LIMA", "Annotation Tool" );
  QPoint pos = settings.value( "pos", QPoint( 200, 200 ) ).toPoint();
  QSize size = settings.value( "size", QSize( 400, 400 ) ).toSize();
  m_currentDirectory = settings.value( "dirconfig", QString() ).toString();
  QString annotconfig = settings.value( "annotconfig", QString() ).toString();
  if (!annotconfig.isEmpty())
  {
    loadAnnotationConfigurationFile(annotconfig);
  }

  m_entitiesFileName = settings.value( "annotfileconfig", QString() ).toString();
  QString fileconfig = settings.value( "fileconfig", QString() ).toString();
  if (!fileconfig.isEmpty())
  {
    loadFile(fileconfig);
  }
  resize( size );
  move( pos );
}

void Annoqt::writeSettings()
{
  qDebug() << "Annoqt::writeSettings";
  QSettings settings( "LIMA", "Annotation Tool" );
  settings.setValue( "pos", pos() );
  settings.setValue( "size", size() );
  settings.setValue( "dirconfig", m_currentDirectory );
  settings.setValue( "annotconfig", m_currentAnnotationConfigurationFile );
  settings.setValue( "fileconfig", m_curFile );
  settings.setValue( "annotfileconfig", m_entitiesFileName );
}

bool Annoqt::maybeSave()
{
  qDebug() << "Annoqt::maybeSave";
  if ( m_textEdit->document()->isModified() )
  {
    int ret = QMessageBox::warning( this, tr( "Application" ),
                                    tr( "The document has been modified.\n"
                                        "Do you want to save your changes?" ),
                                    QMessageBox::Yes | QMessageBox::Default,
                                    QMessageBox::No,
                                    QMessageBox::Cancel | QMessageBox::Escape );

    if ( ret == QMessageBox::Yes )
      return save();
    else
      if ( ret == QMessageBox::Cancel )
        return false;
  }

  return true;
}

void Annoqt::loadFile( const QString &fileName )
{
  qDebug() << "Annoqt::loadFile" << fileName;
  QFile file( fileName );

  if ( !file.open( QFile::ReadOnly | QFile::Text ) )
  {
    QMessageBox::warning( this, tr( "Application" ),
                          tr( "Cannot read file %1:\n%2." )
                          .arg( fileName )
                          .arg( file.errorString() ) );
    return;
  }

  m_textEdit->clear();
  m_textEdit->document()->setModified( false );
  setWindowModified( false );

  QTextStream in( &file );
  in.setCodec(QTextCodec::codecForName("UTF-8"));
  QApplication::setOverrideCursor( Qt::WaitCursor );


  QTextCursor cursor = m_textEdit->textCursor();
  QTextCharFormat modifier = cursor.charFormat();
  modifier.setBackground(QBrush(QColor(Qt::white)));
  cursor.mergeCharFormat(modifier);
  m_textEdit->setTextCursor(cursor);
  m_textEdit->setTextColor( Qt::black );
  m_text = in.readAll();
  m_textEdit->setPlainText(m_text);
  QApplication::restoreOverrideCursor();

  setCurrentFile( fileName );
  statusBar()->showMessage( tr( "File loaded" ), 2000 );
}

bool Annoqt::saveFile( const QString &fileName )
{
  qDebug() << "Annoqt::saveFile" << fileName;
  QFile file( fileName );

  if ( !file.open( QFile::WriteOnly | QFile::Text ) )
  {
    QMessageBox::warning( this, tr( "Application" ),
                          tr( "Cannot write file %1:\n%2." )
                          .arg( fileName )
                          .arg( file.errorString() ) );
    return false;
  }

  QTextStream out( &file );

  out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << endl;
  out << "<specific_entities>" << endl;
//   out << "<entities docid=\"\" offsetNode=\"0\">" << endl;
  QApplication::setOverrideCursor( Qt::WaitCursor );
//   out << m_textEdit->toPlainText();

// std::cerr<<m_textEdit->document()->toHtml("utf-8").toUtf8().data() << std::endl;
  
  foreach (SpecificEntity* entity, m_entities)
  {
    QString string = entity->string();
    QRegExp rxamp("&(?!amp;)");
    string.replace(rxamp, "&amp;");
    QRegExp rxgt(">");
    string.replace(rxgt, "&gt;");
    QRegExp rxlt("<");
    string.replace(rxlt, "&lt;");
    QRegExp rxquot("\"");
    string.replace(rxquot, "&quot;");
    QRegExp rxapos("'");
    string.replace(rxapos, "&apos;");
    out <<"  <specific_entity>"<<endl
        <<"    <string>" << string << "</string>"<<endl
        <<"    <position>" << entity->position()+1 << "</position>"<<endl
        <<"    <length>" << entity->length() << "</length>"<<endl
        <<"    <type>" << m_entityTypes2Names[entity->type()] << "</type>"<<endl
        <<"</specific_entity>" << endl;
  }

  QApplication::restoreOverrideCursor();
//   out << "</entities>" << endl;
  out << "</specific_entities>" << endl;

  statusBar()->showMessage( tr( "File saved" ), 2000 );
  m_textEdit->document()->setModified( false );
  setWindowModified( false );
  return true;
}

void Annoqt::setCurrentFile( const QString &fileName )
{
  qDebug() << "Annoqt::setCurrentFile" << fileName;
  m_curFile = fileName;
  m_textEdit->document()->setModified( false );
  setWindowModified( false );

  QString shownName;

  if ( m_curFile.isEmpty() )
    shownName = "untitled.txt";
  else
    shownName = strippedName( m_curFile );

  setWindowTitle( tr( "%1[*] - %2" ).arg( shownName ).arg( tr( "Application" ) ) );
}

QString Annoqt::strippedName( const QString &fullFileName )
{
  qDebug() << "Annoqt::strippedName";
  return QFileInfo( fullFileName ).fileName();
}

Annoqt::~Annoqt()

{
  qDebug() << "Annoqt::~Annoqt";
  foreach (SpecificEntity* se, m_entities)
  {
    delete se;
  }
  m_entities.clear();
  m_entitiesMap.clear();
}

void Annoqt::slotTypesListItemclicked( const QModelIndex & index )
{
  qDebug() << "Annoqt::slotTypesListItemclicked";
  if ( index.isValid() )
  {
    m_currentColor = m_colors[index.row()];
    if (!m_lastSearchResult.isNull())
    {
      m_textEdit->undo();
      m_textEdit->setTextCursor(m_lastSearchResult);
      m_lastSearchResult = QTextCursor();
    }
    QTextCursor cursor = m_textEdit->textCursor();
    m_currentEntityString = cursor.selectedText ();
/*    QTextCharFormat modifier = cursor.charFormat();
    modifier.setBackground(QBrush(m_currentColor));
    cursor.mergeCharFormat(modifier);

    m_textEdit->setTextCursor(cursor);
    m_textEdit->setTextColor( Qt::white );*/
    QString colorName = m_currentColor.name().toLower();

    SpecificEntity* entity = new SpecificEntity(cursor.selectionStart(), cursor.selectionEnd()-cursor.selectionStart(), m_entityNames2Types[m_colorNames2EntityTypes[colorName]], m_currentEntityString);
    connect( entity, SIGNAL(triggered(SpecificEntity*)), this, SLOT(specificEntityTriggered(SpecificEntity*)));
    
    qDebug() << "Annoqt::slotTypesListItemclicked Adding new entity " << entity->position() << entity->length() << entity->type() << entity->string();
    m_entities.push_back(entity);
    showEntities();
    cursor = m_textEdit->textCursor();
    cursor.setPosition(entity->position());
    m_textEdit->setTextCursor(cursor);
  }
  m_listWidget->setCurrentItem(0);



}

void Annoqt::removeEntity()

{
  qDebug() << "Annoqt::removeEntity()";
  if ( hasCurrentEntity() )
  {
    qDebug() << "m_currentEntity="<<m_currentEntity;
    m_textEdit->undo();

    int pos = m_textEdit->textCursor().position();
    QTextCursor endCursor = m_textEdit->textCursor();
    endCursor.movePosition(QTextCursor::End);
    if (pos >= endCursor.position())
      pos = 0;
    m_entities.removeAll(m_currentEntity);
    delete m_currentEntity;
    setCurrentEntity();
    showEntities();

    qDebug() << "set position to " << pos;
    QTextCursor cursor = m_textEdit->textCursor();
    cursor.setPosition(pos);
    m_textEdit->setTextCursor(cursor);
  }
  else
  {
    qDebug() << "null current entity";
  }
}

void Annoqt::loadAnnotationConfigurationFile(const QString& fileName)
{
  qDebug() << "Annoqt::loadAnnotationConfigurationFile";
  if (fileName.isEmpty())
      return;

  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
      QMessageBox::warning(this, tr("Annotation Tool"),
                            tr("Cannot read file %1:\n%2.")
                            .arg(fileName)
                            .arg(file.errorString()));
      return;
  }

  m_entityNames2Types.clear();
  m_entityTypes2ColorNames.clear();

  m_currentAnnotationConfigurationFile = fileName;

  m_listWidget->clear();
  m_colors.clear();
  m_textEdit->clear();
  m_textEdit->document()->setModified( false );
  setWindowModified( false );

  QList<QString> recursiveEntityTypes;
  AnnotationConfigurationHandler handler(m_listWidget, &m_colors, &m_colorNames2EntityTypes, &recursiveEntityTypes);
  QXmlSimpleReader reader;
  reader.setContentHandler(&handler);
  reader.setErrorHandler(&handler);

  QXmlInputSource xmlInputSource(&file);
  if (reader.parse(xmlInputSource))
      statusBar()->showMessage(tr("Annotation Configuration File loaded"), 2000);
  qDebug() << "Annoqt::loadAnnotationConfigurationFile recursive entities are" << recursiveEntityTypes;
  for (QMap<QString, QString>::const_iterator it=m_colorNames2EntityTypes.begin();
        it != m_colorNames2EntityTypes.end(); it++)
  {
    int newEntityType = m_entityNames2Types.size();
    m_entityNames2Types.insert(it.value(),newEntityType);
    m_entityTypes2Names.insert(newEntityType,it.value());
    m_entityTypes2ColorNames.insert(m_entityNames2Types[it.value()],it.key());
    if (recursiveEntityTypes.contains(it.value()))
    {
      qDebug() << "Annoqt::loadAnnotationConfigurationFile add " <<newEntityType<< " to recursive entities are";
      m_recursiveEntityTypes.push_back(newEntityType);
    }
  }
  statusBar()->showMessage( tr( "Loaded configuration file " ) + fileName );
}

void Annoqt::paste()
{
  qDebug() << "Annoqt::paste";
  QTextCursor cursor = m_textEdit->textCursor();
  QTextCharFormat modifier = cursor.charFormat();
  modifier.setBackground(QBrush(m_currentColor));
  cursor.mergeCharFormat(modifier);

  m_textEdit->setTextCursor(cursor);
  m_textEdit->setTextColor( Qt::white );
}

void Annoqt::loadEntities(const QString& entitiesFile)
{
  qDebug() << "Annoqt::loadEntities" << entitiesFile;
  SpecificEntitiesHandler handler(m_entities, m_textEdit, m_entityNames2Types);
  QXmlSimpleReader reader;
  reader.setContentHandler(&handler);
  reader.setErrorHandler(&handler);

  QFile efile(entitiesFile);
  if (!efile.open(QFile::ReadOnly | QFile::Text))
  {
    m_entitiesFileName = QFileDialog::getOpenFileName(
        this,
        "Select an Entities File",
        m_currentDirectory.isEmpty()?QString():m_currentDirectory );

    if ( !m_entitiesFileName.isEmpty() )
    {
      m_currentDirectory = QFileInfo(m_entitiesFileName).absoluteDir().absolutePath();
      QFile mfile(m_entitiesFileName);
      if (!mfile.open(QFile::ReadOnly | QFile::Text))
      {
        qDebug() << "Cannot open entities file " << m_entitiesFileName;
        return;
      }
      else
      {
        qDebug() << "Opened entities file " << m_entitiesFileName;
      }
    }
    else
    {
      qDebug() << "Cannot open empty entities file name";
      return;
    }
  }
  else
  {
    m_entitiesFileName = entitiesFile;
  }
  qDebug() << "entities file is " << m_entitiesFileName;

  QFile file(m_entitiesFileName);
  QXmlInputSource xmlInputSource(&file);
  if (reader.parse(xmlInputSource))
  {
    statusBar()->showMessage(tr("Entities File loaded"), 2000);
  }
  else
  {
    statusBar()->showMessage(tr("Error loading entities file"), 2000);
  }

  for (QList<SpecificEntity*>::iterator it = m_entities.begin(); it!=m_entities.end();it++)
  {
    connect( (*it), SIGNAL(triggered(SpecificEntity*)), this, SLOT(specificEntityTriggered(SpecificEntity*)));
  }
  showEntities();
}

void Annoqt::slotSearch()
{
  qDebug() << "Annoqt::slotSearch";
  if (!m_lastSearchResult.isNull())
  {
    m_textEdit->undo();
  }
  
  QString searchText = QInputDialog::getText(this, tr("Search Text"), tr("Enter the text to search&nbsp;:"));
  
  if (searchText.isEmpty())
  {
    return;
  }

  QTextDocument* doc = m_textEdit->document();
  m_lastSearchResult = doc->find(searchText, 0);

  if (!m_lastSearchResult.isNull())
  {
    qDebug() << searchText << "found at" << m_lastSearchResult.position();
    setCurrentEntity();
    QTextCharFormat modifier = m_lastSearchResult.charFormat();
    modifier.setUnderlineColor(Qt::black);
    modifier.setUnderlineStyle (QTextCharFormat::WaveUnderline);
    modifier.setFontUnderline(true);
    modifier.setFontOverline(true);
    modifier.setBackground(QBrush(Qt::black));
    modifier.setForeground(QBrush(Qt::white));
    
    m_lastSearchResult.beginEditBlock();
    m_lastSearchResult.mergeCharFormat(modifier);
    m_lastSearchResult.endEditBlock();
    m_textEdit->setTextCursor(m_lastSearchResult);
  }
  else
  {
    qDebug() << searchText << "NOT found";
  }
}

void Annoqt::slotSearchNext()
{
  qDebug() << "Annoqt::slotSearchNext";
  if (m_lastSearchResult.isNull())
  {
    return;
  }
  m_textEdit->undo();

  QTextDocument* doc = m_textEdit->document();
  m_lastSearchResult = doc->find(m_lastSearchResult.selectedText (), m_lastSearchResult.position()+1);

  if (!m_lastSearchResult.isNull())
  {
    qDebug() << m_lastSearchResult.selectedText() << "found again at" << m_lastSearchResult.position();
    setCurrentEntity();
    QTextCharFormat modifier = m_lastSearchResult.charFormat();
    modifier.setUnderlineColor(Qt::black);
    modifier.setUnderlineStyle (QTextCharFormat::WaveUnderline);
    modifier.setFontUnderline(true);
    modifier.setFontOverline(true);
    modifier.setBackground(QBrush(Qt::black));
    modifier.setForeground(QBrush(Qt::white));
    
    m_lastSearchResult.beginEditBlock();
    m_lastSearchResult.mergeCharFormat(modifier);
    m_lastSearchResult.endEditBlock();
    m_textEdit->setTextCursor(m_lastSearchResult);
  }
  else
  {
    qDebug() << m_lastSearchResult.selectedText() << "NOT found again";
  }
}

void Annoqt::slotSearchAndAnnotate()
{
  qDebug() << "Annoqt::slotSearchAndAnnotate";
  searchTextAndAnnotate(m_currentEntityString);
}

void Annoqt::searchTextAndAnnotate(const QString &text)
{
  qDebug() << "Annoqt::searchTextAndAnnotate";
  if (text.isEmpty())
  {
    return;
  }
  if (!m_lastSearchResult.isNull())
  {
    m_textEdit->undo();
    m_lastSearchResult = QTextCursor();
  }

  QTextDocument* doc = m_textEdit->document();
  QTextCursor cursor = doc->find(text, 0);


  while (!cursor.isNull())
  {
    int middlePos = (cursor.position()+cursor.anchor())/2;
    QTextBlock middleBlock = doc->findBlock(middlePos);
    
    // two different blocks would mean not in the same entity
    if (doc->findBlock(cursor.position()) == doc->findBlock(cursor.anchor())
        // automatic changes only on untagged text (thus white)
        && cursor.charFormat().background().color() == QColor(Qt::white) )
    {
      SpecificEntity* se = new SpecificEntity(cursor.anchor(),text.size(),m_entityNames2Types[m_colorNames2EntityTypes[m_currentColor.name().toLower()]],text);
      m_entities.push_back(se);
    }
    cursor = doc->find(text, cursor);
  }
  showEntities();
}

void Annoqt::slotGoto()
{
  qDebug() << "Annoqt::slotGoto";
  bool ok;
  int offset = QInputDialog::getInteger(this, tr("Jump to input"),
                                        tr("Go to:"), 0, 0, m_textEdit->document()->toPlainText().size(),
                                          1,&ok);
  if (ok)
  {
    QTextCursor cursor = m_textEdit->textCursor ();
    cursor.setPosition(offset);
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.movePosition(QTextCursor::EndOfLine,QTextCursor::KeepAnchor);
    m_textEdit->setTextCursor(cursor);
  }
}

void Annoqt::computeEntitiesMap()
{
  qDebug() << "Annoqt::computeEntitiesMap";
  m_entitiesMap.clear();
  foreach (SpecificEntity* entity, m_entities)
  {
    for (quint32 i = entity->position(); i < entity->position()+entity->length(); i++)
    {
      if (!m_entitiesMap.contains(i))
      {
        m_entitiesMap.insert(i, QList<SpecificEntity*>());
      }
      m_entitiesMap[i].push_back(entity);
    }
  }
}

void Annoqt::slotTextCursorPositionChanged()
{
//   qDebug() << "Annoqt::slotTextCursorPositionChanged. It's now " << m_textEdit->textCursor().position();
}

void Annoqt::showEntities()
{
  qDebug() << "Annoqt::showEntities";
  m_textEdit->setUpdatesEnabled(false);

  if (!m_lastSearchResult.isNull())
  {
    m_textEdit->undo();
    m_lastSearchResult = QTextCursor();
  }

  QTextCursor lengthCursor = m_textEdit->textCursor();
  lengthCursor.movePosition(QTextCursor::End);

  computeEntitiesMap();
  hideAll();
  EntitiesSpan currentSpan;
  currentSpan.second = QList<SpecificEntity*>();
  qDebug() << "Annoqt::showEntities lengthCursor.position" << lengthCursor.position();
  for (int i = 0; i < lengthCursor.position(); i++)
  {
//     qDebug() << "Annoqt::showEntities i" << i;
    if ( (!m_entitiesMap.contains(i) && !currentSpan.second.empty()) // just at the first free char after some entity(ies)
        || (m_entitiesMap.contains(i) && m_entitiesMap[i] != currentSpan.second) // there is entites at i, but the entities list there is different than the previous one
        || (m_entitiesMap.contains(i) && i == lengthCursor.position()-1) ) // we are at the end of the document and there is entity(ies)
    {
      showSpanEntities(currentSpan);
      currentSpan = EntitiesSpan();
      currentSpan.first.first = i;
      currentSpan.second = m_entitiesMap[i];
    }
    else
    {
      currentSpan.first.second = i;
    }
  }
  m_textEdit->setUpdatesEnabled(true);
}

void Annoqt::selectEventAt(quint32 position, const QPoint& eventPos)
{
  qDebug() << "Annoqt::selectEvent   qsdfze  At: "<<position<<", " << eventPos;
	statusBar()->showMessage( "azzaaa" );
// 	statusBar()->showMessage( tr( "Position: %1").arg(position) );
}

void Annoqt::hideAll()
{
  qDebug() << "Annoqt::hideAll";
  m_textEdit->clear();
  QTextCursor cursor;
  m_textEdit->setTextCursor(cursor);
  m_textEdit->setPlainText(m_text);
  m_entitiesWithFrame.clear();
/*  m_textEdit->setTextColor( Qt::black );
  QTextCursor cursor = m_textEdit->textCursor();
  cursor.movePosition(QTextCursor::Start);
  cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);

  QTextCharFormat format = cursor.charFormat();
  format.setBackground(QBrush(Qt::white));
  cursor.setCharFormat(format);
  m_textEdit->setTextCursor(cursor);*/
}

QIcon Annoqt::iconFactory(const QColor& color)
{
  qDebug() << "Annoqt::iconFactory";
  QPixmap pix(10,10);
  pix.fill(color);
  return QIcon(pix);
}

QAction* Annoqt::menuItemFor(QMenu* menu, SpecificEntity& se)
{
  qDebug() << "Annoqt::menuItemFor";
  QColor color(m_entityTypes2ColorNames[se.type()]);
  QIcon icon = iconFactory( color );
  QString text = m_entityTypes2Names[se.type()] + " | \"" + se.string() + "\"";
  QAction* action = menu->addAction(icon, text, &se, SLOT(slotTriggered()));
  return action;
}

void Annoqt::specificEntityTriggered(SpecificEntity* se)
{
  qDebug() << "Annoqt::specificEntityTriggered" << se << se->position() << se->length() << se->type(  ) << se->string();
  if (hasCurrentEntity())
  {
    m_textEdit->undo();
  }
  setCurrentEntity(se);
  showEntities();
  return;
  m_textEdit->setTextColor( Qt::black );

  QTextCursor cursor = m_textEdit->textCursor();
  cursor.setPosition(m_currentEntity->position());
  cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor,m_currentEntity->length());

  QTextCharFormat modifier = cursor.charFormat();
  modifier.setUnderlineColor(Qt::black);
  modifier.setUnderlineStyle (QTextCharFormat::WaveUnderline);
  modifier.setFontUnderline(true);
  modifier.setFontOverline(true);
  modifier.setBackground(QBrush(Qt::black));
  
  cursor.beginEditBlock();
  cursor.mergeCharFormat(modifier);
  cursor.endEditBlock();
  setCurrentEntity(se);
  qDebug() << "Annoqt::specificEntityTriggered current entity is now" << m_currentEntity;
}

void Annoqt::showRecursiveEntity(SpecificEntity* se, QTextCursor& cursor)
{
  qDebug() << "Annoqt::showRecursiveEntity";
  if (!m_recursiveEntityTypes.contains(se->type()))
  {
    return;
  }
  if (!m_entitiesWithFrame.contains(se))
  {
    qDebug() << "Annoqt::showRecursiveEntity recursive se without frame";
    cursor.setPosition(se->position());
    cursor.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,se->length());
    
    QColor color(m_entityTypes2ColorNames[se->type()]);
    qDebug() << "color" << color;
    QTextFrameFormat format;
    format.setBorderStyle(QTextFrameFormat::BorderStyle_Dashed);
    format.setBorderBrush(color);
    format.setBorder(2);
    format.setMargin(5);
    format.setBackground(Qt::white);
    cursor.insertFrame(format);
    m_entitiesWithFrame.push_back(se);
  }
  else
  {
    qDebug() << "Annoqt::showRecursiveEntity recursive se with frame already";
  }

  QColor color ( m_entityTypes2ColorNames[se->type() ] );
  m_textEdit->setTextColor ( color );

  QTextFrame* frame = cursor.currentFrame();
  QTextFrameFormat format = frame->frameFormat();
  if (isCurrentEntity(se))
  {
    qDebug() << "Annoqt::showRecursiveEntity is current: black background";
    format.setBackground(Qt::black);
    format.setForeground(Qt::white);
    m_textEdit->setTextColor (Qt::white);


    QTextCursor cursor = frame->firstCursorPosition();
//     cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor,frame->lastPosition()-frame->firstPosition());

    QTextCharFormat modifier = cursor.charFormat();
    modifier.setBackground(QBrush(Qt::black));

    cursor.beginEditBlock();
    cursor.mergeCharFormat(modifier);
    cursor.endEditBlock();
    m_textEdit->setTextCursor(cursor);
    m_textEdit->setTextColor(Qt::black);
    m_textEdit->undo();
  }
  else
  {
    qDebug() << "Annoqt::showRecursiveEntity is not current: white background";
    format.setBackground(Qt::white);
    format.setForeground(Qt::black);
    m_textEdit->setTextColor (Qt::black);
  }
  frame->setFrameFormat(format);
}

void Annoqt::showSpanEntities(EntitiesSpan& currentSpan)
{
  qDebug() << "Annoqt::showSpanEntities: new currentSpan" << "; currentSpan.second size: " << currentSpan.second.size();
  if (!currentSpan.second.empty())
  {
    qDebug() << "Annoqt::showSpanEntities There is current entities";
    if (currentSpan.first.second==0)
    {
      qDebug() << "Annoqt::showSpanEntities Empty current span";
      currentSpan.first.second = currentSpan.first.first;
    }
    qDebug() << "Annoqt::showSpanEntities current span bounds" << currentSpan.first.first << currentSpan.first.second-currentSpan.first.first+1;
    QTextCursor cursor = m_textEdit->textCursor();
    cursor.setPosition(currentSpan.first.first);
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor,currentSpan.first.second-currentSpan.first.first+1);
    QTextCharFormat modifier = cursor.charFormat();
    if (currentSpan.second.size() >= 2)
    { // gradient
      QLinearGradient linearGrad(0.0, 0.0, 0.0, 30);
      linearGrad.setStart(QPointF(0.0,0.0));
      linearGrad.setFinalStop(QPointF(0.0,12.0));
      linearGrad.setSpread(QGradient::ReflectSpread) ;
      for (int p = 0; p < currentSpan.second.size(); p ++)
      {
        SpecificEntity* se = currentSpan.second[p];
        if (m_recursiveEntityTypes.contains(se->type()))
        {
          showRecursiveEntity(se, cursor);
        }
        else
        {
          QColor color(m_entityTypes2ColorNames[se->type()]);
          linearGrad.setColorAt(p*(1.0/(currentSpan.second.size()-1)), color);
        }
      }
      qDebug() << "Annoqt::showSpanEntities set back color to gradient";
      modifier.setBackground(QBrush(linearGrad));
      modifier.setFontUnderline(true);
    }
    else
    {
      SpecificEntity* se = *currentSpan.second.begin();
      qDebug() << "Annoqt::showSpanEntities set back color to" << m_entityTypes2ColorNames[se->type()];
      QColor color(m_entityTypes2ColorNames[se->type()]);
      modifier.setBackground(QBrush(color));
    }
    SpecificEntity* se = *currentSpan.second.begin();
    if (m_recursiveEntityTypes.contains(se->type()))
    {
      showRecursiveEntity(se, cursor);
    }
    else if (isCurrentEntity(se))
    {
      showCurrentEntityAsSelected();
    }
    else
    {
      qDebug() << "Annoqt::showSpanEntities non-recursive nor current: setting color";
      cursor.mergeCharFormat(modifier);
      m_textEdit->setTextCursor(cursor);
      m_textEdit->setTextColor( Qt::white );
      cursor.movePosition(QTextCursor::Start);
      m_textEdit->setTextCursor(cursor);
    }
  }
}

void Annoqt::showCurrentEntityAsSelected()
{
  qDebug() << "Annoqt::showCurrentEntityAsSelected";
  if (!m_recursiveEntityTypes.contains(m_currentEntity->type()))
  {
    qDebug() << "Annoqt::showCurrentEntityAsSelected: entity is NOT recursive";
    QColor color ( m_entityTypes2ColorNames[m_currentEntity->type() ] );
    m_textEdit->setTextColor ( color );

    QTextCursor cursor = m_textEdit->textCursor();
    qDebug() << "Annoqt::showCurrentEntityAsSelected: current entity "<< m_entityTypes2Names[m_currentEntity->type()] << m_currentEntity->position() << m_currentEntity->length();
    cursor.setPosition(m_currentEntity->position());
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor,m_currentEntity->length());

    QTextCharFormat modifier = cursor.charFormat();
    modifier.setUnderlineColor(Qt::black);
    modifier.setUnderlineStyle (QTextCharFormat::WaveUnderline);
    modifier.setFontUnderline(true);
    modifier.setFontOverline(true);
    modifier.setBackground(QBrush(Qt::black));

    cursor.beginEditBlock();
    cursor.mergeCharFormat(modifier);
    cursor.endEditBlock();
    m_textEdit->setTextCursor(cursor);
    m_textEdit->setTextColor(Qt::black);
  }
  else
  {
    QTextCursor cursor = m_textEdit->textCursor();
    cursor.setPosition(m_currentEntity->position()+1);
    showRecursiveEntity(m_currentEntity, cursor);
  }
}

void Annoqt::setCurrentEntity(SpecificEntity* se)
{
  qDebug() << "Annoqt::setCurrentEntity" << (void*)se;
  cutAct->setEnabled(se != 0);
  qDebug() << "Annoqt::setCurrentEntity cutAct enabled now:" << cutAct->isEnabled();
  m_currentEntity = se;
}
