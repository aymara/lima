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

  void emitDataInstalled (bool success) { Q_EMIT dataInstalled(success); }; // ResourceReaderSimpleModel needed Q_EMIT function

Q_SIGNALS:
  void dataInstalled (bool success); // ResourceReaderSimpleModel needed signal
  void resourceFileModified(const QString & path);
  
public Q_SLOTS:
  // ResourceReaderSimpleModel needed slots
  virtual void installFinished (int ec, QProcess::ExitStatus es) { ResourceReaderSimpleModel::installFinished(ec, es); };
  virtual void installError (QProcess::ProcessError e) { ResourceReaderSimpleModel::installError(e); };

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_RESOURCEREADERTOOLBOXMODEL_H
