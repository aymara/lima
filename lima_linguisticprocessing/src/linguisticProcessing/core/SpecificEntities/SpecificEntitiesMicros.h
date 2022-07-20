// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
