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
 * @file       StructuredBoWToBoWDocument.cpp
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Mon Mar  6 2006
 * @version    $Id$
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "StructuredBoWToBoWDocument.h"

using namespace Lima::Common::BagOfWords;
using namespace Lima::Common::Misc;

namespace Lima {
namespace LinguisticProcessing {

#define LPCLIENTSBOWHANDLERLOGINIT LOGINIT("LP::SBoWHandler")

//***********************************************************************
// constructors and destructors
StructuredBoWToBoWDocument::
StructuredBoWToBoWDocument(std::vector<BoWDocument>* bowDocs):
m_bowDocs(bowDocs),
m_inIndexingNode(),
m_currentDocument(0)
{
}

StructuredBoWToBoWDocument::~StructuredBoWToBoWDocument() 
{
  // do not delete pointer
}

//***********************************************************************
void StructuredBoWToBoWDocument::
openSBoWNode(const Lima::Common::Misc::GenericDocumentProperties* /*unused properties*/,
             const std::string& /*unused elementName*/)
{
  m_inIndexingNode.push_back(false);
}

void StructuredBoWToBoWDocument::
openSBoWIndexingNode(const Lima::Common::Misc::GenericDocumentProperties* properties,
                     const std::string& /*unused elementName*/)
{
  BoWDocument newDoc;
  m_bowDocs->push_back(newDoc);
  m_currentDocument=&(m_bowDocs->back());
  if (properties!=0) {
    m_currentDocument->setProperties(static_cast<const Common::Misc::GenericDocumentProperties&>(*properties));
  }
  m_inIndexingNode.push_back(true);
}

void StructuredBoWToBoWDocument::
processSBoWText(const BoWText* boWText, 
                bool /*unused useIterators*/, bool useIndexIterator)
{
  LIMA_UNUSED(useIndexIterator)
  if (! m_inIndexingNode.empty() && 
      m_inIndexingNode.back() && 
      boWText!=0) {
    // clone BoWText (pointers of boWText will be deleted)
    BoWText* newBoWText=new BoWText(*boWText); 
    m_currentDocument->insert(m_currentDocument->end(),
                              newBoWText->begin(),
                              newBoWText->end());
  }
}

void StructuredBoWToBoWDocument::
processProperties(const Common::Misc::GenericDocumentProperties* properties, 
                  bool /*unused useIterators*/, bool /*useIndexIterator*/)
{
  if (m_inIndexingNode.back()) {
    addProperties(*m_currentDocument,properties);
  }
}

void StructuredBoWToBoWDocument::
closeSBoWNode()
{
  m_inIndexingNode.pop_back();
}

void StructuredBoWToBoWDocument::
addProperties(BoWDocument& /*document*/,
              const GenericDocumentProperties* properties)
{
  if (properties == 0 ) { 
    return; 
  }
  std::pair<GenericDocumentProperties::IntPropertiesIterator,GenericDocumentProperties::IntPropertiesIterator> intProps=properties->getIntProperties();
  for (GenericDocumentProperties::IntPropertiesIterator it=intProps.first,it_end=intProps.second;
       it!=it_end; it++) {
    m_currentDocument->setIntValue((*it).first,(*it).second);
  }

  std::pair<GenericDocumentProperties::StringPropertiesIterator,GenericDocumentProperties::StringPropertiesIterator> strProps=properties->getStringProperties();
  for (GenericDocumentProperties::StringPropertiesIterator it=strProps.first,it_end=strProps.second;
       it!=it_end; it++) {
    m_currentDocument->setStringValue((*it).first,(*it).second);
  }

  std::pair<GenericDocumentProperties::DatePropertiesIterator,GenericDocumentProperties::DatePropertiesIterator> dateProps=properties->getDateProperties();
  for (GenericDocumentProperties::DatePropertiesIterator it=dateProps.first,it_end=dateProps.second;
       it!=it_end; it++) {
    m_currentDocument->setDateValue((*it).first,(*it).second);
  }

  std::pair<GenericDocumentProperties::DateIntervalPropertiesIterator,GenericDocumentProperties::DateIntervalPropertiesIterator> dateIntProps=properties->getDateIntervalProperties();
  for (GenericDocumentProperties::DateIntervalPropertiesIterator it=dateIntProps.first,it_end=dateIntProps.second;
       it!=it_end; it++) {
    m_currentDocument->setDateIntervalValue((*it).first,(*it).second);
  }

}


} // end namespace
} // end namespace
