// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Gael.de-Chalendar@cea.fr   *
 ***************************************************************************/

#ifndef ANNOQT_H
#define ANNOQT_H

#include "specificEntity.h"

#include <QMainWindow>
#include <QCloseEvent>
#include <QModelIndex>
#include <QTextCursor>

class QAction;
class QMenu;
class AnnotationEditWidget;
class QListWidget;
class QListWidgetItem;

class Annoqt:public QMainWindow
{
  Q_OBJECT
  typedef QPair< QPair<quint32, quint32>, QList<SpecificEntity*> > EntitiesSpan;

public:
  Annoqt();
  ~Annoqt();

  void selectEventAt(quint32 position, const QPoint& eventPos);

public Q_SLOTS:

  void slotTypesListItemclicked(QListWidgetItem *item);

  void removeEntity();

  void slotTextCursorPositionChanged();

protected:
  void closeEvent( QCloseEvent *event ) override;

private Q_SLOTS:
  void newConfiguration();

  void open();

  bool save();

  bool saveAs();

  void about();

  void documentWasModified();

  void paste();

  void slotSearchAndAnnotate();

  void slotGoto();

  void specificEntityTriggered(SpecificEntity* se);

  void slotSearch();

  void slotSearchNext();
  
private:
  void createActions();

  void createMenus();

  void createToolBars();

  void createStatusBar();

  void readSettings();

  void writeSettings();

  bool maybeSave();

  void loadFile( const QString &fileName );

  bool saveFile( const QString &fileName );

  void setCurrentFile( const QString &fileName );

  QString strippedName( const QString &fullFileName );

  void loadAnnotationConfigurationFile(const QString& fileName);

  void loadEntities(const QString& entitiesFile);

  void searchTextAndAnnotate(const QString &text);

  void computeEntitiesMap();

  void showEntities();

  void hideAll();

  QIcon iconFactory(const QColor& color);

  QAction* menuItemFor(QMenu* menu, SpecificEntity& se);

  void showRecursiveEntity(SpecificEntity* se, QTextCursor& cursor);
  void showSpanEntities(EntitiesSpan& span);
  void showCurrentEntityAsSelected();

  void setCurrentEntity(SpecificEntity* se = 0);
  inline bool isCurrentEntity(SpecificEntity* se) {return m_currentEntity == se;}
  inline bool hasCurrentEntity() {return m_currentEntity != 0;}
  
  QListWidget* m_listWidget;

  QList<QColor> m_colors;

  QMap<QString, QString> m_colorNames2EntityTypes;
  QMap<quint32, QString> m_entityTypes2ColorNames;
  QMap<QString, quint32> m_entityNames2Types;
  QMap<quint32, QString> m_entityTypes2Names;

  AnnotationEditWidget* m_textEdit;

  QColor m_currentColor;

  QString m_currentEntityString;

  QString m_curFile;

  QString m_currentAnnotationConfigurationFile;

  QString m_currentDirectory;

  QMenu *fileMenu;

  QMenu *editMenu;

  QMenu *helpMenu;

  QToolBar *fileToolBar;

  QToolBar *editToolBar;

  QAction *newAct;

  QAction *openAct;

  QAction *saveAct;

  QAction *saveAsAct;

  QAction *exitAct;

  QAction *cutAct;

  QAction *copyAct;

  QAction *pasteAct;

  QAction *aboutAct;

  QAction *aboutQtAct;

  QAction *m_repeatAct;

  QAction *m_gotoAct;

  QAction* searchAction;

  QAction* searchNextAction;
  
  QList<SpecificEntity*> m_entities;
  QMap< quint32, QList<SpecificEntity* > > m_entitiesMap;

  SpecificEntity* m_currentEntity;

  QTextCursor m_lastSearchResult;

  QString m_entitiesFileName;

  QList<SpecificEntity*> m_entitiesWithFrame;

  QList<quint32> m_recursiveEntityTypes;

  QString m_text;
};

#endif
