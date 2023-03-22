// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDICTABSTRACTACCESSRESOURCE_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDICTABSTRACTACCESSRESOURCE_H

#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"

namespace Lima
{
namespace Common
{
  class AbstractAccessByString;
}
namespace LinguisticProcessing
{
namespace AnalysisDict
{

/**
@author Benoit Mathieu
*/
class LIMA_LINGUISTICRESOURCES_EXPORT AbstractAccessResource : public AbstractResource
{
  Q_OBJECT
public:

  explicit AbstractAccessResource(QObject* parent = 0);

  virtual ~AbstractAccessResource();
  
  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override = 0;

  virtual std::shared_ptr<Common::AbstractAccessByString> getAccessByString() const = 0;

  inline bool isMainKeys() const;
  inline void setMainKeys(bool flag);

Q_SIGNALS:
  void accessFileReloaded ( std::shared_ptr<Common::AbstractAccessByString> fsaAccess );
  
protected:
  bool m_isMainKeys;

};

bool AbstractAccessResource::isMainKeys() const {
  return m_isMainKeys;
}

void AbstractAccessResource::setMainKeys(bool flag) {
  m_isMainKeys=flag;
}


}

}

}

#endif
