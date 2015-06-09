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

 ************************************************************************
 *
 * @file       AbstractTaggerFactory.h
 * @author     Grandjean Alex (alex.grandjean@cea.fr)
 * @date       Thu Mar 27 2014
 * copyright   Copyright (C) 2006-2014 by CEA LIST
 * Project     Lima
 * 
 * @brief       Implement Factory to generate Tagger for CRF labelling
 * 
 * 
 ***********************************************************************/

#ifndef ABSTRACTTAGGERFACTORY_H
#define ABSTRACTTAGGERFACTORY_H

#include <string>
#include "common/AbstractFactoryPattern/RegistrableFactory.h"
#include "AbstractTagger.h"
#include "SpecificEntitiesCRFExport.h"

namespace Lima {
namespace LinguisticProcessing {
 
class LIMA_SPECIFICENTITIESCRF_EXPORT AbstractTaggerFactory : public RegistrableFactory<AbstractTaggerFactory> 
{

 public:
  AbstractTaggerFactory(const std::string& factoryId);
  virtual AbstractTagger* create()=0;
  

};
  
template<typename AbTag> 
class LIMA_SPECIFICENTITIESCRF_EXPORT TaggerFactory : public AbstractTaggerFactory
{
public:
  TaggerFactory(const std::string& factoryId):
  AbstractTaggerFactory(factoryId) {}
  AbstractTagger* create() {
    AbstractTagger* newFeature=new AbTag();
    return newFeature;
  }
private:
};  

}
}

#endif
