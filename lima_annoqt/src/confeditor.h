// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Gael.de-Chalendar@cea.fr   *
 ***************************************************************************/

#ifndef ANNOQTCONFEDITOR_H
#define ANNOQTCONFEDITOR_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QModelIndex>

class QAction;
class QMenu;
class AnnotationEditWidget;
class QListWidget;
class QListWidgetItem;
class KColorDialog;

class AnnoqtConfEditor:public QMainWindow
{
  Q_OBJECT

public:
  AnnoqtConfEditor();
  ~AnnoqtConfEditor();

public Q_SLOTS:

  void slotTypesListItemclicked(QListWidgetItem* item );

protected:
  void closeEvent( QCloseEvent *event ) override;

private Q_SLOTS:
  void slotNewConfiguration();

  void slotOpen();

  bool slotSave();

  bool slotSaveAs();

  void slotAbout();

  void slotDocumentWasModified();

  void slotCut();

  void slotAddItemAction();
  
  void slotColorSelected( const QColor& color );
  

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

  QListWidget* m_listWidget;

  QList<QColor> m_colors;

  QMap<QString, QString> m_colorNames2EntityTypes;
  QMap<quint32, QString> m_entityTypes2ColorNames;
  QMap<QString, quint32> m_entityNames2Types;
  QMap<quint32, QString> m_entityTypes2Names;

  QColor m_currentColor;
  
  QString m_currentAnnotationConfigurationFile;

  QString m_currentDirectory;

  KColorDialog* m_colorPicker;

  QListWidgetItem* m_currentItem;

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

  QAction *addItemAction;

  QAction *aboutAct;

  QAction *aboutQtAct;

};

#endif
