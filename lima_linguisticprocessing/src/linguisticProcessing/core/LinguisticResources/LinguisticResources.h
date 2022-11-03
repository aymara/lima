// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/


#ifndef LIMA_LINGUISTICPROCESSING_LINGUISTICRESOURCES_LINGUISTICRESOURCES_H
#define LIMA_LINGUISTICPROCESSING_LINGUISTICRESOURCES_LINGUISTICRESOURCES_H

#include "LinguisticResourcesExport.h"
#include "AbstractResource.h"

namespace Lima
{
  namespace Common {
    namespace XMLConfigurationFiles {
      class ModuleConfigurationStructure;
    }
  }
namespace LinguisticProcessing
{

class LinguisticResourcesPrivate;
/**
  * @brief singleton that holds all languages resources
  */
class LIMA_LINGUISTICRESOURCES_EXPORT LinguisticResources : public Singleton<LinguisticResources>
{
friend class Singleton<LinguisticResources>;

public:

  virtual ~LinguisticResources();

  void clearResources();

  AbstractResource* getResource(
    MediaId lang,
    const std::string& id) const;

  void initLanguage(
    MediaId lang,
    Common::XMLConfigurationFiles::ModuleConfigurationStructure& confModule,
    bool registerMainKeysInStringPool);

  Common::XMLConfigurationFiles::ModuleConfigurationStructure&
    getModuleConfiguration(MediaId lang);

private:

  LinguisticResources();
  LinguisticResources(const LinguisticResources& lr);
  LinguisticResources& operator=(const LinguisticResources& lr);

  LinguisticResourcesPrivate* m_d;
};


} // LinguisticProcessing
} // Lima

#endif
