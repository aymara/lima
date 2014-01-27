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
 * @file     Entity.cpp
 * @author   Faiza GARA
 * @date     October 2007
 * @version  $Id: 
 * copyright Copyright (C) 2007 by CEA LIST
 *
 ***********************************************************************/


#include "Entity.h"
#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include "common/Data/readwritetools.h"
#include "common/MediaticData/mediaticData.h"



#include <string>

#include <wchar.h>

using namespace std;
using namespace Lima::LinguisticProcessing::Automaton;
using namespace Lima::Common::MediaticData;

namespace Lima
{
namespace LinguisticProcessing
{
namespace EventAnalysis
{





//***********************************************************************
// constructors
//***********************************************************************
Entity::Entity() :
    m_position(),
    m_length(),
    m_main(false),
    m_features()
{

}

Entity::Entity(
                   const uint64_t position,
                   const uint64_t length,
                   EntityFeatures features):
    m_position(position),
    m_length(length),
    m_main(false),
    m_features(features)
    

{
}


//***********************************************************************
// destructor
//***********************************************************************
Entity::~Entity()
{}








//***********************************************************************
// == operator
// added by Benoit Mathieu
//***********************************************************************

bool Entity::operator==(const Entity& e) const
{

  return ( (getPosition()==e.getPosition()) &&  (getLength()==e.getLength()));
}

bool Entity::operator!=(const Entity& t) const
{
  return !(*this==t);
}




//***********************************************************************
// binary input/output
//***********************************************************************


void Entity::read(std::istream& file)
{
  m_position= Common::Misc::readCodedInt(file);
  m_length= Common::Misc::readCodedInt(file);
  m_main= Common::Misc::readCodedInt(file);
  uint64_t size= Common::Misc::readCodedInt(file);
  std::string name;
  std::string value;
  for (uint64_t i=0; i < size; i++)
  {
    EntityFeature f;
    Common::Misc::readStringField(file,name);
    Common::Misc::readStringField(file,value);
    f.setName(name);
    f.setValue(value);
    m_features.push_back(f);
  }
  // lire le nombre de features et les sauvegarder 
}

void Entity::write(std::ostream& file) const
{
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "Entity::write().."<< LENDL; 
  Common::Misc::writeCodedInt(file,m_position);
  Common::Misc::writeCodedInt(file,m_length);
  Common::Misc::writeCodedInt(file,m_main);
  Common::Misc::writeCodedInt(file,m_features.size());
  LDEBUG << "Entity::write: write features..."<< LENDL; 
  for (Automaton::EntityFeatures::const_iterator 
       featureItr=m_features.begin();
       featureItr!=m_features.end(); featureItr++)
  {
    Common::Misc::writeStringField(file,featureItr->getName());
    Common::Misc::writeStringField(file,featureItr->getValueString());
  }
  // ecrire le nombre de features et pour chaque feature ecrire le type et la valeur
}

std::map<std::string,std::string> Entity::getMapFeatures(void) const
{
  std::map<std::string,std::string> resmap;
  for (Automaton::EntityFeatures::const_iterator 
       featureItr=m_features.begin();
       featureItr!=m_features.end(); featureItr++)
  {
    resmap[featureItr->getName()]=featureItr->getValueString();
  }
  return resmap;
}


//**********************************************************************
// output on ostream (mostly for debug)
std::ostream& operator << (std::ostream& os, const Entity& annot)
{
  LIMA_UNUSED(annot);
  return os;
}


//***********************************************************************
// output
//***********************************************************************

std::string Entity::getOutputUTF8String() const
{
  std::ostringstream oss;
  return oss.str();
}


std::string Entity::getIdUTF8String() const
{
  std::ostringstream oss;
  return oss.str();
}


std::string Entity::toString(std::string parentURI) const
{
  LIMA_UNUSED(parentURI);
  std::ostringstream oss;
  oss << "<rdf:rdf xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\""
      << " xmlns:dc=\"http://purl.org/dc/elements/1.1/\"" << std::endl;
  return oss.str();
  
}

} // namespace EventAnalysis
} // namespace LinguisticProcessing
} // namespace Lima
