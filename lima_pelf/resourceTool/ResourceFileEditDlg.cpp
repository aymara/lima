// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "ResourceFileEditDlg.h"
using namespace Lima::Pelf;

ResourceFileEditDlg::ResourceFileEditDlg (QWidget* p) :
    QDialog(p),
    Ui::ResourceTextFileEditDialog(),
    isTabbedWidget(false)
{
    setupUi(this);
    connect(searchEntry,SIGNAL(textEdited(QString)),this,SLOT(slotSearchEntryEdited(QString)));
}

void ResourceFileEditDlg::init (QString rfp, bool showDialog)
{
    resourceFilePath = rfp;
    loadFile();
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(saveFile()));
    if(showDialog)
      show();
}

void ResourceFileEditDlg::accept ()
{
    if(QMessageBox::question(
        this,
        tr("Overwrite resource text file ?"),
        tr("A resource text file is currently edited and will be overwritten, do you really want to overwrite it?"),
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel
    ) == QMessageBox::Ok)
    {
        if(isTabbedWidget)
            saveFile();
        else
            QDialog::accept();
    }
}

void ResourceFileEditDlg::reject ()
{
    if(isTabbedWidget)
    {
        if(QMessageBox::question(
           this,
           tr("Reload resource file form disk ?"),
           tr("By reloading file from disk you will lose any non-saved modifications, are you sure you want to reload file ?"),
           QMessageBox::Ok | QMessageBox::Cancel,
           QMessageBox::Cancel
        ) == QMessageBox::Ok)
            loadFile();
    }
    else
        QDialog::reject();
}

void ResourceFileEditDlg::loadFile ()
{
    qDebug() << "Loading resource text file for editing : " << resourceFilePath;
    QFile resourceFile(resourceFilePath);
    QFileInfo resourceFileInfo(resourceFilePath);
    if ((resourceFileInfo.exists() && !resourceFileInfo.isFile()) || !resourceFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug() << "Error opening resource file " << resourceFilePath << " in read/write mode";
        return;
    }
    if(resourceFileInfo.exists() && resourceFileInfo.size() > 0)
    {
        textEdit->setPlainText(QString::fromUtf8(resourceFile.readAll()));
        setWindowTitle(windowTitle()+" - "+resourceFileInfo.fileName());
    }
    else
        QMessageBox::warning(
            this,
            tr("Resource text file does not exists or is empty"),
            tr("File does not exist or is empty, it will be created or filled")
        );
    resourceFile.close();
}

void ResourceFileEditDlg::saveFile ()
{
    qDebug() << "ResourceFileEditDlg::saveFile:" << resourceFilePath;
    QFile resourceFile(resourceFilePath);
    if (!resourceFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QMessageBox::warning(
            this,
            tr("Impossible to write to file"),
            tr("Writing to file has failed, please check file path and permissions")
        );
        return;
    }
    QByteArray plainTextUtf8 = textEdit->toPlainText().toUtf8();
    int plainTextUtf8Size = qstrlen(plainTextUtf8);
    resourceFile.resize(plainTextUtf8Size);
    resourceFile.write(plainTextUtf8, plainTextUtf8Size);
    resourceFile.close();
    qDebug() << "ResourceFileEditDlg::saveFile saved";
}

void ResourceFileEditDlg::setAsTabbedWidget ()
{
    isTabbedWidget = true;
    buttonBox->button(QDialogButtonBox::Ok)->setText("Save file");
    buttonBox->button(QDialogButtonBox::Cancel)->setText("Reload file from disk");
}

void ResourceFileEditDlg::slotSearchEntryEdited(const QString & text)
{
  qDebug() << "ResourceFileEditDlg::slotSearchEntryEdited" << text;
  textEdit->find(text);
}

#include "ResourceFileEditDlg.moc"
