/*
    Copyright 2002-2014 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/


#ifndef LIMA_LINGUISTICPROCESSING_LINGUISTICRESOURCES_ABSTRACTRESOURCES_H
#define LIMA_LINGUISTICPROCESSING_LINGUISTICRESOURCES_ABSTRACTRESOURCES_H


#include "LinguisticResourcesExport.h"

#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"
#include "common/AbstractFactoryPattern/InitializableObject.h"
#include "common/tools/LimaFileSystemWatcher.h"

#include <QtCore/QObject>

namespace Lima {
namespace LinguisticProcessing {

struct ResourceInitializationParameters {
  MediaId language;
};

class  AbstractResourcePrivate;
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
class LIMA_LINGUISTICRESOURCES_EXPORT AbstractResource : public QObject, public InitializableObject<AbstractResource,ResourceInitializationParameters>
{
  Q_OBJECT
public:
  explicit AbstractResource ( QObject* parent = 0 );
  virtual ~AbstractResource();
  AbstractResource(const AbstractResource&);
    
  /**
  * @brief initialize with parameters from configuration file.
  * @param unitConfiguration @IN : <group> tag in xml configuration file that
  *        contains parameters to initialize the object.
  * @param manager @IN : manager that asked for initialization and carries parameters.
  * @throw InvalidConfiguration when parameters are invalids.
  */
  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override = 0;
  
protected:
  /**
   * @brief Accessor to the file watcher used to register resource files to be watched for changes.
   * @return The file watcher used to register resource files to be watched for changes.
   */
  LimaFileSystemWatcher& resourceFileWatcher();

Q_SIGNALS:
  /**
   * @brief Signal triggered whenever one of the registered files changes on disk.
   * @param path The full path to the changed file
   */
  void resourceFileChanged ( const QString & path );


protected:
  AbstractResourcePrivate* m_d;

};

} // LinguisticProcessing
} // Lima


#endif
