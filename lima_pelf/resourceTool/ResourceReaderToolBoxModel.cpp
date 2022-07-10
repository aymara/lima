// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "ResourceReaderToolBoxModel.h"
using namespace Lima::Pelf;

ResourceReaderToolBoxModel::ResourceReaderToolBoxModel (QObject * parent) :
    QObject(parent),
    ResourceReaderSimpleModel()
{
  qDebug() << "ResourceReaderToolBoxModel::ResourceReaderToolBoxModel";
  connect(m_installProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
      this, SLOT(installFinished(int,QProcess::ExitStatus)));
  connect(m_installProcess, SIGNAL(error(QProcess::ProcessError)),
      this, SLOT(installError(QProcess::ProcessError)));
  connect(&m_watcher, SIGNAL(fileChanged(QString)), this, SIGNAL(resourceFileModified(QString)));
}

ResourceReaderToolBoxModel::~ResourceReaderToolBoxModel ()
{
}

#include "ResourceReaderToolBoxModel.moc"
