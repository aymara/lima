// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "ResourceReaderSimpleModel.h"
using namespace Lima::Pelf;

ResourceReaderSimpleModel::ResourceReaderSimpleModel() :
    installStatus(INSTALL_READY),
    dataModified(false),
    checkDataModified(false),
    m_installProcess(new QProcess()),
    m_watcher()
{
  m_installProcess->setProcessChannelMode(QProcess::MergedChannels);
}

ResourceReaderSimpleModel::~ResourceReaderSimpleModel()
{
  delete m_installProcess;
}

void ResourceReaderSimpleModel::init (QStringList rp, QString ic)
{
    qDebug() << "ResourceReaderSimpleModel::init" << rp;
    resourcePaths = rp;
    installComand = ic;
    m_watcher.removePaths(m_watcher.files());
//     m_watcher.addPaths(rp);
}

QStringList ResourceReaderSimpleModel::loadDataSourceStrings (QString sourcePath)
{
    QStringList sourceStrings;
    qDebug() << "Loading resource data from " << sourcePath;
    QFile resourceFile(sourcePath);
    if(!resourceFile.exists() || !resourceFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Error opening resource file " << sourcePath << " for reading";
        return sourceStrings;
    }
    while(!resourceFile.atEnd())
        sourceStrings << QString::fromUtf8(resourceFile.readLine()).trimmed();
    resourceFile.close();
    return sourceStrings;
}

void ResourceReaderSimpleModel::loadDataCategories (QString emptyCategoryName)
{
    for(int i = 0; i < availableData.size(); i++) if(availableData.at(i)->displayable)
    {
        QString categoryName = availableData.at(i)->category;
        if(categoryName.isEmpty())
            categoryName = emptyCategoryName;
        if(!emptyCategoryName.isEmpty() && categoryList.indexOf(categoryName) == -1)
            categoryList << categoryName;
    }
    categoryList.sort();
}

int ResourceReaderSimpleModel::searchEntries (QStringList args)
{
    foundData.clear();
    for(int i = 0; i < availableData.size(); i++)
        if(availableData.at(i)->matches(args))
            foundData << i;
    return foundData.size();
}

AbstractResourceEntry* ResourceReaderSimpleModel::getFoundEntry (int entryNo) const
{
    if(entryNo < 0 || entryNo >= (int)foundData.size())
        return 0;
    int index = foundData.at(entryNo);
    if(index < 0 || index >= availableData.size())
        return 0;
    return availableData.at(index);
}

void ResourceReaderSimpleModel::install ()
{
    if(checkDataModified && !dataModified)
    {
        qDebug() << "Data has not been modified for this resource, this is considered as a success";
        return;
    }
    if(installStatus == INSTALL_PROCESSING)
    {
        qDebug() << "An installation is already in progress, aborting...";
        return;
    }
    installStatus = INSTALL_PROCESSING;
    qDebug() << "Installing resource with command " << installComand;
    QObject* thisQobject = dynamic_cast<QObject*>(this);
    // These connect must be here and not in the constructor because this object is not a QObject
    // Only subclasses heriting also from QObject will be successively connected.
    if (thisQobject != 0)
    {
      QObject::connect(m_installProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
          thisQobject, SLOT(installFinished(int,QProcess::ExitStatus))
      );
      QObject::connect(m_installProcess, SIGNAL(error(QProcess::ProcessError)),
          thisQobject, SLOT(installError(QProcess::ProcessError))
      );
    }
    else
    {
      qDebug() << "ResourceReaderSimpleModel: cannot cast to QObject";
    }
    m_installProcess->start(installComand);
}

void ResourceReaderSimpleModel::installFinished (int exitCode, QProcess::ExitStatus exitStatus)
{
    bool success = true;
    if(exitCode != 0 || exitStatus != QProcess::NormalExit)
    {
        qDebug() << "Command error while installing resource, output is :" << m_installProcess->readAll();
        success = false;
    }
    else
        qDebug() << "Resource saved and installed";
    installStatus = INSTALL_READY;
    dataModified = false;
    emitDataInstalled(success);
}

void ResourceReaderSimpleModel::installError (QProcess::ProcessError error)
{
    qDebug() << "Undetermined command error while installing resource" << " (QT code: " << error << ")";
    installStatus = INSTALL_READY;
    emitDataInstalled(false);
}
