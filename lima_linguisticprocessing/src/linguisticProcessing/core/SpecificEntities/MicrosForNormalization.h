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
            Manager* manager)
    ;
  
  const std::set<LinguisticCode>* getMicros(const std::string& group);

 private:
  std::map<std::string,std::set<LinguisticCode> > m_micros;
};

} // end namespace
} // end namespace
} // end namespace

#endif
