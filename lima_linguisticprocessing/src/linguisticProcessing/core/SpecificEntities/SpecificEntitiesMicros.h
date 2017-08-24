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
/************************************************************************
 *
 * @file       SpecificEntitiesMicros.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Thu Apr  5 2007
 * copyright   Copyright (C) 2007 by CEA LIST
 * Project     linguisticProcessing
 * 
 * @brief      resource storing microcategories associated with specific entities
 * 
 * 
 ***********************************************************************/

#ifndef SPECIFICENTITIESMICROS_H
#define SPECIFICENTITIESMICROS_H

#include "SpecificEntitiesExport.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "common/MediaticData/EntityType.h"

#include <set>

namespace Lima {
namespace LinguisticProcessing {
namespace SpecificEntities {

#define SPECIFICENTITIESMICROS_CLASSID "SpecificEntitiesMicros"

class LIMA_SPECIFICENTITIES_EXPORT SpecificEntitiesMicros : public AbstractResource
{
  Q_OBJECT
 public:
  SpecificEntitiesMicros(); 
  ~SpecificEntitiesMicros();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override;
  
  const std::set<LinguisticCode>* getMicros(const Common::MediaticData::EntityType& type);

 private:
  std::map<Common::MediaticData::EntityType,std::set<LinguisticCode> > m_micros;

};

} // end namespace
} // end namespace
} // end namespace

#endif
