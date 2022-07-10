// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LINGUISTICPROCESSING_LINGUISTICRESOURCES_ABSTRACTRESOURCEBASE_H
#define LIMA_LINGUISTICPROCESSING_LINGUISTICRESOURCES_ABSTRACTRESOURCEBASE_H


#include "LinguisticResourcesExport.h"

#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"
#include "common/AbstractFactoryPattern/InitializableObject.h"
#include "common/tools/LimaFileSystemWatcher.h"

#include <QtCore/QObject>

namespace Lima {
namespace LinguisticProcessing {

class  AbstractResourceBasePrivate;
/** 
  * @brief resource abstraction. All resource should inherit from this class
  * @author Benoit Mathieu <mathieub@zoe.cea.fr>
  * 
  * @ref AbstractResource inherits from @ref QObject to be able to trigger the resourceFileChanged
  * signal whenever one of the files it has registered changes. The @ref init function should
  * use the @ref LimaFileSystemWatcher accessed using the protected @ref resourceFileWatcher
  * accessor to register its resource files. Then it can connect a slot to the 
  * @ref resourceFileChanged signal to do whatever necessary with this changed resource, for 
  * example reloading it.
  */

class LIMA_LINGUISTICRESOURCES_EXPORT AbstractResourceBase : 
    public QObject
{
  Q_OBJECT
public:
  explicit AbstractResourceBase ( QObject* parent = 0 );
  virtual ~AbstractResourceBase();
  AbstractResourceBase(const AbstractResourceBase&);

  QString getResourceFileName(const QString& paramName);

protected:
  /**
   * @brief Accessor to the file watcher used to register resource files to be 
   * watched for changes.
   * @return The file watcher used to register resource files to be watched for 
   * changes.
   */
  LimaFileSystemWatcher& resourceFileWatcher();

Q_SIGNALS:
  /**
   * @brief Signal triggered whenever one of the registered files changes on
   * disk.
   * @param path The full path to the changed file
   */
  void resourceFileChanged ( const QString & path );


protected:
  AbstractResourceBasePrivate* m_d;

private:
  AbstractResourceBase& operator=(const AbstractResourceBase&);
};

} // LinguisticProcessing
} // Lima


#endif
