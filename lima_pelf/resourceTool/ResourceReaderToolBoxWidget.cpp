// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "ResourceReaderToolBoxWidget.h"
using namespace Lima::Pelf;

ResourceReaderToolBoxWidget::ResourceReaderToolBoxWidget (QWidget* p) :
    QToolBox(p)
{
}

void ResourceReaderToolBoxWidget::init (ResourceReaderToolBoxModel* rrtbm)
{
    qDebug() << "ResourceReaderToolBoxWidget::init";
    while(count() > 0)
    {
        QWidget* widgetRemoved = widget(0);
        removeItem(0);
        widgetRemoved->close();
    }
    if (resourceModel!=0)
    {
      disconnect((ResourceReaderToolBoxModel*)resourceModel,SIGNAL(resourceFileModified(QString)),this,SLOT(slotResourceFileModified(QString)));
    }
    ResourceReaderSimpleWidget::init(rrtbm);
    connect(rrtbm,SIGNAL(resourceFileModified(QString)),this,SLOT(slotResourceFileModified(QString)));
    for(int i = 0; i < rrtbm->resourcePaths.size(); i++)
    {
        QString resourcePath = rrtbm->resourcePaths.at(i);
        QFileInfo resourcePathFileInfo(resourcePath);
///@TODO Possible memory leak:  ensure that this object will be deleted
        ResourceFileEditDlg* dialog = new ResourceFileEditDlg();
        dialog->textEdit->setWordWrapMode(QTextOption::NoWrap);
        dialog->setWindowModality(Qt::NonModal);
        dialog->setAsTabbedWidget();
        dialog->init(resourcePath, false);
        sourceIndexes[resourcePath] = addItem(dialog, resourcePathFileInfo.fileName());
    }
}

void ResourceReaderToolBoxWidget::moveCursorToSourceLine (QString sourceFile, int sourceLine)
{
    setCurrentIndex(sourceIndexes[sourceFile]);
    ResourceFileEditDlg* dialog = (ResourceFileEditDlg*)currentWidget();
    dialog->textEdit->setFocus();
    dialog->textEdit->moveCursor(QTextCursor::Start);
    for(int i = 0; i < sourceLine; i++)
        dialog->textEdit->moveCursor(QTextCursor::Down);
    dialog->textEdit->centerCursor();
}

void ResourceReaderToolBoxWidget::slotResourceFileModified(const QString& file)
{
  qDebug() << "ResourceReaderToolBoxWidget::slotResourceFileModified" << file;
  if (resourceModel->installStatus != ResourceReaderSimpleModel::INSTALL_PROCESSING)
  {
    if (QMessageBox::question(
          this,
          tr("Resource Modified"),
          tr("Resource file modified: %1\nDo you want to reload it?").arg(file),
          QMessageBox::Ok | QMessageBox::Cancel,
          QMessageBox::Cancel
        ) == QMessageBox::Ok)
    {
      init((ResourceReaderToolBoxModel*)resourceModel);
    }
  }
}

#include "ResourceReaderToolBoxWidget.moc"
