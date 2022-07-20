// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       MicrosForNormalization.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Tue Mar 13 2007
 * copyright   Copyright (C) 2007 by CEA LIST
 * Project     s2lp
 * 
 * @brief      micro categories used for named entity normalization
 * 
 * 
 ***********************************************************************/

#ifndef MICROSFORNORMALIZATION_H
#define MICROSFORNORMALIZATION_H

#include "SpecificEntitiesExport.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include <map>
#include <set>


namespace Lima {
namespace LinguisticProcessing {
namespace SpecificEntities {

#define MICROSFORNORMALIZATION_CLASSID "MicrosForNormalization"

class LIMA_SPECIFICENTITIES_EXPORT MicrosForNormalization : public AbstractResource
{
  Q_OBJECT
 public:
  MicrosForNormalization(); 
  ~MicrosForNormalization();
  
  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override;
  
  const std::set<LinguisticCode>* getMicros(const std::string& group);

 private:
  std::map<std::string,std::set<LinguisticCode> > m_micros;
};

} // end namespace
} // end namespace
} // end namespace

#endif
