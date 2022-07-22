// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    LIMA_EXCEPTION( "no list " << list.c_str()
           << " for micros in named entities normalization");
  }
  return &((*it).second);
}


} // end namespace
} // end namespace
} // end namespace
