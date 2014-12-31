/*
    Copyright 2002-2013 CEA LIST

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
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_LINGUISTICPROCESSING_LINGUISTICRESOURCES_ABSTRACTRESOURCES_H
#define LIMA_LINGUISTICPROCESSING_LINGUISTICRESOURCES_ABSTRACTRESOURCES_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"
#include "common/AbstractFactoryPattern/InitializableObject.h"

#include <QtCore/QObject>
#include <QtCore/QFileSystemWatcher>

namespace Lima {
namespace LinguisticProcessing {

struct ResourceInitializationParameters {
  MediaId language;
};

class AbstractResourcePrivate;
/** 
  * @brief resource abstraction. All resource should inherit from this class
  * @author Benoit Mathieu <mathieub@zoe.cea.fr>
  *
  *
  */
class AbstractResource : public QObject, public InitializableObject<AbstractResource,ResourceInitializationParameters>
{
  Q_OBJECT
public:
  explicit AbstractResource ( QObject* parent = 0 );
  virtual ~AbstractResource();
  AbstractResource(const AbstractResource&);
    
  /**
  * @brief initialize with parameters from configuration file and languageId.
  * @param unitConfiguration @IN : <group> tag in xml configuration file that
  *        contains parameters to initialize the object.
  * @param manager @IN : manager that asked for initialization and carries parameters.
  * @throw InvalidConfiguration when parameters are invalids.
  */
  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) = 0;
  
  QFileSystemWatcher& resourceFileWatcher();

Q_SIGNALS:
  void resourceFileChanged ( const QString & path );


protected:
  AbstractResourcePrivate* m_d;

};

} // LinguisticProcessing
} // Lima


#endif
