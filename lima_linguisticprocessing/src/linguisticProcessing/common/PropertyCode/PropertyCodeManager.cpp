/*
    Copyright 2002-2020 CEA LIST

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
 *   Copyright (C) 2004 by CEA LIST                                        *
 *                                                                         *
 ***************************************************************************/

#include "PropertyCodeManager.h"
#include "XMLPropertyHandler.h"
#include "SymbolicCodeXMLHandler.h"


#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <typeinfo>

using namespace std;

namespace Lima
{
namespace Common
{
namespace PropertyCode
{

//utility functions for debug
string hexString(const uint64_t x)
{
  ostringstream oss;
  oss << hex << x;
  return oss.str();
}

#ifdef uint128_t
string hexString(const uint128_t x)
{
  ostringstream oss;
  oss << hex << x;
  return oss.str();
}
#endif

class PropertyCodeManagerPrivate
{
  friend class PropertyCodeManager;

  std::map<std::string,PropertyManager> m_propertyManagers;

  /**
   * @brief Compute the number of bit needed to encode nbvalues
   * @param nbvalues nbvalues to encode
   * @return uint8_t
   */
  uint8_t computeNbBitsNeeded(uint64_t nbvalues) const;

  /**
   * The mask of a property is an integer that show which bits are used.
   * It has all bits to 0, excepts the nbbits from start bits, which are used to code
   * the property.
   * @brief Compute a mask
   * @param startBit first bit used
   * @param nbBits nb bits to use
   * @return LinguisticCode
   */
  LinguisticCode computeMask(uint8_t startBit,uint8_t nbBits) const;

};

PropertyCodeManager::PropertyCodeManager() :
    m_d(new PropertyCodeManagerPrivate())
{
}

PropertyCodeManager::~PropertyCodeManager()
{
    delete m_d;
}

PropertyCodeManager::PropertyCodeManager(const PropertyCodeManager& pcm)
{
  m_d = new PropertyCodeManagerPrivate();
  m_d->m_propertyManagers = pcm.m_d->m_propertyManagers;
}

PropertyCodeManager& PropertyCodeManager::operator=(const PropertyCodeManager& pcm)
{
  m_d->m_propertyManagers = pcm.m_d->m_propertyManagers;
  return *this;
}

void PropertyCodeManager::readFromXmlFile(const std::string& filename)
{
  PROPERTYCODELOGINIT;
#ifdef DEBUG_LP
  LDEBUG << typeid(*this).name()
         << "PropertyCodeManager::readFromXmlFile" << filename;
#endif

#ifdef DEBUG_LP
  LDEBUG << typeid(*this).name()
         << "PropertyCodeManager::readFromXmlFile before creating parser";
#endif
  QScopedPointer< QXmlSimpleReader > parser(new QXmlSimpleReader());
//   parser->setValidationScheme(SAXParser::Val_Auto);
//   parser->setDoNamespaces(false);
//   parser->setDoSchema(false);
//   parser->setValidationSchemaFullChecking(false);

  //
  //  Create the handler object and install it as the document and error
  //  handler for the parser-> Then parse the file and catch any exceptions
  //  that propogate out
  //
  XMLPropertyHandler handler;
#ifdef DEBUG_LP
  LDEBUG << "PropertyCodeManager::readFromXmlFile before parsing";
#endif
  parser->setContentHandler(&handler);
  parser->setErrorHandler(&handler);
  QFile file(filename.c_str());
  if (!file.open(QIODevice::ReadOnly))
  {
    LERROR << "An error occurred  Error: Cannot open" << filename ;
    throw std::runtime_error(
      std::string("PropertyCodeManager::readFromXmlFile Unable to open ")
                  + filename);
  }
  if (!parser->parse( QXmlInputSource(&file)))
  {
    LERROR << "PropertyCodeManager::readFromXmlFile An error occurred  Error:"
           << parser->errorHandler()->errorString() ;
    throw XMLException(std::string("Error while parsing " + filename + " : "
                + parser->errorHandler()->errorString().toUtf8().constData()));
  }
#ifdef DEBUG_LP
  LDEBUG << "PropertyCodeManager::readFromXmlFile parsed. before deleting parser";
#endif
  // Compute coding properties
  uint8_t usedBits=0;

  // compute data for properties
  const auto& properties = handler.getProperties();
#ifdef DEBUG_LP
  LDEBUG << properties.size() << " properties read from xmlfile ";
#endif
  for (auto desc = properties.cbegin();
       desc != properties.cend();
       desc++)
  {
    // compute mask
#ifdef DEBUG_LP
    LDEBUG << "compute data for property " << desc->name;
#endif
    std::vector<std::string>::size_type nbvalues=desc->values.size() + 1;
    uint8_t nbBits = m_d->computeNbBitsNeeded(nbvalues);
#ifdef DEBUG_LP
    LDEBUG << nbvalues << " values so use " << nbBits << " bits";
#endif
    LinguisticCode mask = m_d->computeMask(usedBits,nbBits);
#ifdef DEBUG_LP
    LDEBUG << "mask is " << mask.toHexString();
#endif

    // compute values
    std::map<std::string,LinguisticCode> symbol2code;
    symbol2code["NONE"] = L_NONE;
    uint64_t i = 1;
    for (auto valItr = desc->values.cbegin();
         valItr != desc->values.cend();
         valItr++)
    {
      LinguisticCode new_code = LinguisticCode::fromUInt(i) << usedBits;
      symbol2code.insert(std::make_pair(*valItr, new_code));
#ifdef DEBUG_LP
      LDEBUG << *valItr << " => " << new_code.toHexString();
#endif
      i++;
    }
    usedBits += nbBits;
    if (usedBits >= sizeof(LinguisticCode)*8)
    {
      std::runtime_error("Error: using more than the number of bits available.");
    }
    m_d->m_propertyManagers.insert(
      std::make_pair(desc->name,
                     PropertyManager(desc->name, mask, mask, symbol2code)));
  }

  const auto& subproperties = handler.getSubProperties();
#ifdef DEBUG_LP
  LDEBUG << subproperties.size() << " subproperties read from file";
#endif
  for (auto desc = subproperties.cbegin();
       desc != subproperties.cend();
       desc++)
  {
#ifdef DEBUG_LP
    LDEBUG << "compute data for subproperty " << desc->name;
#endif
    const auto& parentProp = getPropertyManager(desc->parentName);
    LinguisticCode parentmask = parentProp.getMask();

    const auto& subvalues = desc->values;

    // compute mask
    // use enough bit to code the biggest subvalues set
    uint64_t maxsubvalues=0;
    for (auto it=subvalues.cbegin();
         it!=subvalues.cend();
         it++)
    {
      if (it->second.size()+1>maxsubvalues)
      {
        maxsubvalues=it->second.size()+1;
      }
    }
    auto nbBits = m_d->computeNbBitsNeeded(maxsubvalues);
#ifdef DEBUG_LP
    LDEBUG << "maximum subvalues is " << maxsubvalues << " so use "
           << nbBits << " bits";
#endif
    LinguisticCode emptynessmask = m_d->computeMask(usedBits,nbBits);
    LinguisticCode mask = emptynessmask | parentmask;
#ifdef DEBUG_LP
    LDEBUG << "mask = " << mask.toHexString();
    LDEBUG << "emptyness mask = " << emptynessmask.toHexString();
#endif

    // compute values
    std::map<std::string,LinguisticCode> symbol2code;
    symbol2code["NONE"] = L_NONE;
    for (auto subItr = subvalues.cbegin();
         subItr!=subvalues.cend();
         subItr++)
    {
      LinguisticCode parentValue=parentProp.getPropertyValue(subItr->first);
      if (parentValue == L_NONE)
      {
        LERROR << "parent value " << subItr->first << " of subproperty "
               << desc->name << " is unknown !";
      }
#ifdef DEBUG_LP
      LDEBUG << "compute subvalues of " << subItr->first << " ("
             << parentValue.toHexString() << ")";
#endif
      symbol2code.insert(std::make_pair(string(subItr->first)+"-NONE",
                                        LinguisticCode(parentValue)));
      uint64_t i = 1;
      for (auto valItr = subItr->second.cbegin();
           valItr!=subItr->second.cend();
           valItr++)
      {
        symbol2code.insert(std::make_pair(*valItr, (LinguisticCode::fromUInt(i) << usedBits) | parentValue));
#ifdef DEBUG_LP
        LDEBUG << *valItr << " => " << symbol2code[*valItr].toHexString();
#endif
        i++;
      }
    }
    m_d->m_propertyManagers.insert(std::make_pair(desc->name,
                                             PropertyManager(desc->name,
                                                             mask,
                                                             emptynessmask,
                                                             symbol2code)));
    usedBits += nbBits;

  }

#ifdef DEBUG_LP
  LDEBUG << "have used " << usedBits << " bits to code all properties";
#endif
  if (usedBits > sizeof(LinguisticCode)*8)
  {
    LERROR << "needs" << usedBits
           << "bits to code all properties !! Encoding may be Invalid !!";
    throw std::runtime_error(
      "Needs too much bits to code all properties !! Encoding would be Invalid !!");
  }

}

const PropertyManager& PropertyCodeManager::getPropertyManager(
  const std::string& propertyName) const
{
  auto it =m_d->m_propertyManagers.find(propertyName);
  if (it == m_d->m_propertyManagers.end())
  {
    PROPERTYCODELOGINIT;
    LERROR << "Ask for unknown property accessor:" << propertyName;
    throw InvalidConfiguration();
  }
  return it->second;
}

const std::shared_ptr<PropertyAccessor> PropertyCodeManager::getPropertySetAccessor(
  const std::set<std::string>& propertyNames) const
{
  ostringstream os;
  std::copy(propertyNames.begin(), propertyNames.end(),
            std::ostream_iterator<string>(os,":"));
#ifdef DEBUG_LP
  PROPERTYCODELOGINIT;
  LDEBUG << "create propertysetAccessor for " << os.str();
#endif
  LinguisticCode propertySetMask;
  LinguisticCode propertySetEmptyNessMask;
  for (auto propItr = propertyNames.cbegin();
       propItr != propertyNames.cend();
       propItr++)
  {
    const auto& man = getPropertyManager(*propItr);
    propertySetMask |= man.getMask();
    propertySetEmptyNessMask |= man.getEmptyNessMask();
  }
#ifdef DEBUG_LP
  LDEBUG << "propertysetMask is " << propertySetMask.toHexString();
  LDEBUG << "propertysetEmptyNessMask is "
         << propertySetEmptyNessMask.toHexString();
#endif
  return std::shared_ptr<PropertyAccessor>(
    new PropertyAccessor(os.str(),
                         propertySetMask,
                         propertySetEmptyNessMask));
}

uint8_t PropertyCodeManagerPrivate::computeNbBitsNeeded(uint64_t nbvalues) const
{
  uint8_t nbBits=1;
  for (uint64_t tmp=2; tmp < nbvalues; tmp*=2)
  {
    nbBits++;
  }
  if (nbBits > sizeof(LinguisticCode)*8)
  {
    PROPERTYCODELOGINIT;
    LERROR << "Error: nb bits needed ("<<nbBits<<") is larger than sizeof LinguisticCode="
           << sizeof(LinguisticCode)*8;
    throw std::runtime_error(
      "Error: nb bits needed is larger than sizeof LinguisticCode");
  }
  return nbBits;
}

LinguisticCode PropertyCodeManagerPrivate::computeMask(uint8_t startBit,
                                                       uint8_t nbBits) const
{
  if (startBit + nbBits > LinguisticCode::size())
  {
    PROPERTYCODELOGINIT;
    LERROR << "Error: cannot compute mask. Would use more bits ("
           << (startBit + nbBits) <<") than available ("
           << sizeof(LinguisticCode)*8 << ")";
    throw std::runtime_error("Error: cannot compute mask. Would use more bits than available");
  }
  LinguisticCode mask;
  for (uint8_t currentBit = startBit;
       currentBit < startBit + nbBits;
       currentBit++)
  {
    mask |= (LinguisticCode::fromUInt(1) << currentBit);
  }
  return mask;
}

LinguisticCode PropertyCodeManager::encode(const std::map<std::string,
                                           std::string>& propValues) const
{
#ifdef DEBUG_LP
  PROPERTYCODELOGINIT;
#endif
  LinguisticCode coded;
#ifdef DEBUG_LP
  LDEBUG << "encode";
#endif
  for (auto it = propValues.cbegin(); it != propValues.cend(); it++)
  {
    const auto& man = getPropertyManager(it->first);
    man.getPropertyAccessor().writeValue(man.getPropertyValue(it->second),coded);
 #ifdef DEBUG_LP
   LDEBUG << it->first << " : " << it->second << " coded = " << coded.toHexString();
 #endif
 }
  return coded;
}

void PropertyCodeManager::convertSymbolicCodes(
  const std::string& symbolicCodeFile,
  std::map<std::string,LinguisticCode>& conversionTable) const
{
#ifdef DEBUG_LP
  PROPERTYCODELOGINIT;
  LDEBUG << "convert Symbolic Code file " << symbolicCodeFile;
#endif

  QScopedPointer< QXmlSimpleReader > parser ( new QXmlSimpleReader() );
//   parser->setValidationScheme(SAXParser::Val_Auto);
//   parser->setDoNamespaces(false);
//   parser->setDoSchema(false);
//   parser->setValidationSchemaFullChecking(false);

  //
  //  Create the handler object and install it as the document and error
  //  handler for the parser-> Then parse the file and catch any exceptions
  //  that propogate out
  //
  SymbolicCodeXMLHandler handler(*this,conversionTable);
  parser->setContentHandler(&handler);
  parser->setErrorHandler(&handler);
  QFile file(symbolicCodeFile.c_str());
  if (!file.open(QIODevice::ReadOnly))
  {
    PROPERTYCODELOGINIT;
    LERROR << "An error occurred  Error: Cannot open " << symbolicCodeFile ;
    throw std::runtime_error(
      std::string("PropertyCodeManager::convertSymbolicCodes Unable to open ")
                  + symbolicCodeFile);
  }
  if (!parser->parse( QXmlInputSource(&file)))
  {
    PROPERTYCODELOGINIT;
    LERROR << "PropertyCodeManager::convertSymbolicCodes An error occurred parsing"
           << symbolicCodeFile
           << ". Error: "
           << parser->errorHandler()->errorString() ;
    throw XMLException(
      std::string("Error while parsing " + symbolicCodeFile + " : "
                  + parser->errorHandler()->errorString().toUtf8().constData()));
  }
}

const std::map<std::string,PropertyManager>& PropertyCodeManager::getPropertyManagers() const
{
  return m_d->m_propertyManagers;
}

const PropertyAccessor& PropertyCodeManager::getPropertyAccessor(
  const std::string& propertyName) const
{
  return getPropertyManager(propertyName).getPropertyAccessor();
}

} // PropertyCode
} // Common
} // Lima
