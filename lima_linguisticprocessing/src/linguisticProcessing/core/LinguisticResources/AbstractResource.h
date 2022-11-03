// Copyright 2002-2014 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LINGUISTICPROCESSING_LINGUISTICRESOURCES_ABSTRACTRESOURCES_H
#define LIMA_LINGUISTICPROCESSING_LINGUISTICRESOURCES_ABSTRACTRESOURCES_H


#include "LinguisticResourcesExport.h"
#include "AbstractResourceBase.h"

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
class LIMA_LINGUISTICRESOURCES_EXPORT AbstractResource : 
    public AbstractResourceBase, 
    public InitializableObject<AbstractResource,ResourceInitializationParameters>
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
  
};

} // LinguisticProcessing
} // Lima


#endif
