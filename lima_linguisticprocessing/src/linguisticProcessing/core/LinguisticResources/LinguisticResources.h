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
