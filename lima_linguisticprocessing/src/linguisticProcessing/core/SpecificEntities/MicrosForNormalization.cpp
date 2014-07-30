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
 * @file       MicrosForNormalization.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Tue Mar 13 2007
 * copyright   Copyright (C) 2007 by CEA LIST
 * 
 ***********************************************************************/

#include "MicrosForNormalization.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "common/MediaticData/mediaticData.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "linguisticProcessing/common/linguisticData/languageData.h"

using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::PropertyCode;
using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace SpecificEntities {

SimpleFactory<AbstractResource,MicrosForNormalization> 
MicrosForNormalizationFactory(MICROSFORNORMALIZATION_CLASSID);

//***********************************************************************
// constructors and destructors
MicrosForNormalization::MicrosForNormalization():
m_micros()
{
}

MicrosForNormalization::~MicrosForNormalization() 
{
}

void MicrosForNormalization::
init(GroupConfigurationStructure& unitConfiguration,
     Manager* manager)
   
{
  MediaId language=manager->getInitializationParameters().language;
  const PropertyManager& microManager = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyManager("MICRO");

  try {
    deque<string>& microGroups =
      unitConfiguration.getListsValueAtKey("microGroups");
    
    for (deque<string>::const_iterator 
           g=microGroups.begin(),g_end=microGroups.end();
         g!=g_end; g++) {

      deque<string>& microList =
        unitConfiguration.getListsValueAtKey(*g);
      
      for (deque<string>::const_iterator 
             m=microList.begin(),m_end=microList.end();
             m!=m_end; m++) {
        
        m_micros[*g].insert(microManager.getPropertyValue(*m));
      }
    }
  }
  catch (NoSuchList& ) {
    SELOGINIT;
    LERROR << "missing list in 'MicrosForNormalization' group of language " << (int)language << " !";
    throw InvalidConfiguration();
  }
}

const std::set<LinguisticCode>* MicrosForNormalization::
getMicros(const std::string& list)
{
  map<string,set<LinguisticCode> >::const_iterator it=m_micros.find(list);
  if (it==m_micros.end()) {
    SELOGINIT;
    LERROR << "no list " << list
           << " for micros in named entities normalization";
    throw LimaException();
  }
  return &((*it).second);
}


} // end namespace
} // end namespace
} // end namespace
