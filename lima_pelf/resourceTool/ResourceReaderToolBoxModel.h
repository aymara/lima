// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_RESOURCEREADERTOOLBOXMODEL_H
#define LIMA_PELF_RESOURCEREADERTOOLBOXMODEL_H

#include <QtCore/QtDebug>
#include <QtCore/QObject>
#include <QtCore/QFile>

#include "ResourceReaderSimpleModel.h"
#include "AbstractResourceEntry.h"

namespace Lima {
namespace Pelf {

class ResourceReaderToolBoxModel : public QObject, public ResourceReaderSimpleModel
{

Q_OBJECT

public:

  ResourceReaderToolBoxModel (QObject * parent = 0);
  virtual ~ResourceReaderToolBoxModel();

  void emitDataInstalled (bool success) override { Q_EMIT dataInstalled(success); }; 

Q_SIGNALS:
  void dataInstalled (bool success); // ResourceReaderSimpleModel needed signal
  void resourceFileModified(const QString & path);
  
public Q_SLOTS:
  // ResourceReaderSimpleModel needed slots
  virtual void installFinished (int ec, QProcess::ExitStatus es) override { ResourceReaderSimpleModel::installFinished(ec, es); };
  virtual void installError (QProcess::ProcessError e) override { ResourceReaderSimpleModel::installError(e); };

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_RESOURCEREADERTOOLBOXMODEL_H
