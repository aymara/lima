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
/***************************************************************************
 *   Copyright (C) 2004 by CEA LIST                       *
 *                                                                         *
 ***************************************************************************/

#include "PropertyCodeManager.h"
#include "XMLPropertyHandler.h"
#include "SymbolicCodeXMLHandler.h"


#include <bitset>
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
string hexString(const uint64_t x) {
  ostringstream oss;
  oss << std::hex << x;
  return oss.str();
}

//utility functions for debug
string binString(const uint64_t x) {
  std::bitset<64> bx(x);
  ostringstream oss;
  oss << bx; 
  return oss.str();
}

void PropertyCodeManager::readFromXmlFile(const std::string& filename)
{
  PROPERTYCODELOGINIT;
#ifdef DEBUG_LP
  LDEBUG << typeid(*this).name() << "PropertyCodeManager::readFromXmlFile" << filename;
#endif

#ifdef DEBUG_LP
  LDEBUG << typeid(*this).name() << "PropertyCodeManager::readFromXmlFile before creating parser";
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
    LERROR << "An error occurred  Error: Cannot open " << filename ;
    throw std::runtime_error(std::string("PropertyCodeManager::readFromXmlFile Unable to open ") + filename);
  }
  if (!parser->parse( QXmlInputSource(&file)))
  {
    LERROR << "PropertyCodeManager::readFromXmlFile An error occurred  Error: " << parser->errorHandler()->errorString() ;
    throw XMLException(std::string("Error while parsing " + filename + " : " + parser->errorHandler()->errorString().toUtf8().constData()));
  }
#ifdef DEBUG_LP
  LDEBUG << "PropertyCodeManager::readFromXmlFile parsed. before deleting parser";
#endif
  // Compute coding properties
  uint64_t usedBits=0;

  // compute data for properties
  const std::vector<XMLPropertyHandler::PropertyDescription>& properties=handler.getProperties();
#ifdef DEBUG_LP
  LDEBUG << properties.size() << " properties read from xmlfile ";
#endif
  for (std::vector<XMLPropertyHandler::PropertyDescription>::const_iterator desc=properties.begin();
       desc!=properties.end();
       desc++)
  {
    // compute mask
#ifdef DEBUG_LP
    LDEBUG << "compute data for property " << desc->name;
#endif
    std::vector<std::string>::size_type nbvalues=desc->values.size() + 1;
    uint64_t nbBits=computeNbBitsNeeded(nbvalues);
#ifdef DEBUG_LP
    LDEBUG << nbvalues << " values so use " << nbBits << " bits";
#endif
    auto mask=computeMask(usedBits,nbBits);
#ifdef DEBUG_LP
    LDEBUG << "mask is " << hexString(mask) << binString(mask);
#endif

    // compute values
    std::map<std::string,LinguisticCode> symbol2code;
    symbol2code["NONE"] = LinguisticCode(0);
    uint64_t i=1;
    for (std::vector<std::string>::const_iterator valItr=desc->values.begin();
         valItr!=desc->values.end();
         valItr++)
    {
      symbol2code.insert(std::make_pair(*valItr, LinguisticCode(i << usedBits)));
#ifdef DEBUG_LP
      LDEBUG << *valItr << " => " << hexString(i << usedBits) << binString(i << usedBits);
#endif
      i++;
    }
    usedBits+=nbBits;
    m_propertyManagers.insert(std::make_pair(desc->name,
                                             PropertyManager(desc->name,
                                                             mask,
                                                             mask,
                                                             symbol2code)));
  }

  const std::vector<XMLPropertyHandler::SubPropertyDescription>& subproperties=handler.getSubProperties();
#ifdef DEBUG_LP
  LDEBUG << subproperties.size() << " subproperties read from file";
#endif
  for (std::vector<XMLPropertyHandler::SubPropertyDescription>::const_iterator desc=subproperties.begin();
       desc!=subproperties.end();
       desc++)
  {
#ifdef DEBUG_LP
    LDEBUG << "compute data for subproperty " << desc->name;
#endif
    const PropertyManager& parentProp=getPropertyManager(desc->parentName);
    LinguisticCode parentmask=parentProp.getMask();

    const std::vector<std::pair<std::string,std::vector<std::string> > >& subvalues=desc->values;

    // compute mask
    // use enough bit to code the biggest subvalues set
    uint64_t maxsubvalues=0;
    for (std::vector<std::pair<std::string,std::vector<std::string> > >::const_iterator it=subvalues.begin();
         it!=subvalues.end();
         it++)
    {
      if (it->second.size()+1>maxsubvalues) { maxsubvalues=it->second.size()+1; }
    }
    uint64_t nbBits=computeNbBitsNeeded(maxsubvalues);
#ifdef DEBUG_LP
    LDEBUG << "maximum subvalues is " << maxsubvalues << " so use " << nbBits << " bits";
#endif
    LinguisticCode emptynessmask=computeMask(usedBits,nbBits);
    LinguisticCode mask=static_cast<LinguisticCode>(emptynessmask + parentmask);
#ifdef DEBUG_LP
    LDEBUG << "mask = " << hexString(mask) << binString(mask);
    LDEBUG << "emptyness mask = " << hexString(emptynessmask) << binString(emptynessmask);
#endif

    // compute values
    std::map<std::string,LinguisticCode> symbol2code;
    symbol2code["NONE"] = LinguisticCode(0);
    for (std::vector<std::pair<std::string,std::vector<std::string> > >::const_iterator subItr=subvalues.begin();
         subItr!=subvalues.end();
         subItr++)
    {
      auto parentValue=parentProp.getPropertyValue(subItr->first);
      if (parentValue == 0)
      {
        LERROR << "parent value " << subItr->first << " of subproperty " << desc->name << " is unknown !";
      }
#ifdef DEBUG_LP
      LDEBUG << "compute subvalues of " << subItr->first << " (" << hexString(parentValue) << binString(parentValue) << ")";
#endif
      symbol2code.insert(std::make_pair(string(subItr->first)+"-NONE",LinguisticCode(parentValue)));
      uint64_t i=1;
      for (std::vector<std::string>::const_iterator valItr=subItr->second.begin();
           valItr!=subItr->second.end();
           valItr++)
      {
        symbol2code.insert(std::make_pair(*valItr,LinguisticCode((i << usedBits) + parentValue)));
#ifdef DEBUG_LP
        LDEBUG << *valItr << " => " << hexString(symbol2code[*valItr]) << binString(symbol2code[*valItr]);
#endif
        i++;
      }
    }
    m_propertyManagers.insert(std::make_pair(desc->name,
                                             PropertyManager(desc->name,
                                                             mask,
                                                             emptynessmask,
                                                             symbol2code)));
    usedBits+=nbBits;

  }

#ifdef DEBUG_LP
  LDEBUG << "have used" << usedBits << "bits to code all properties";
#endif
  if (usedBits > 64)
  {
    LERROR << "needs" << usedBits 
           << "bits to code all properties !! Encoding may be Invalid !!";
    throw InvalidConfiguration(std::string("needs ")+QString::number(usedBits).toUtf8().toStdString()+" bits to code all properties !! Encoding may be Invalid !!");
  }

}

const PropertyManager& PropertyCodeManager::getPropertyManager(const std::string& propertyName) const
{
  map<string,PropertyManager>::const_iterator it=m_propertyManagers.find(propertyName);
  if (it == m_propertyManagers.end())
  {
    PROPERTYCODELOGINIT;
    LERROR << "Ask for unknown property accessor : " << propertyName;
    throw InvalidConfiguration();
  }
  return it->second;
}

const PropertyAccessor* PropertyCodeManager::getPropertySetAccessor(const std::set<std::string>& propertyNames) const
{
  ostringstream os;
  copy(propertyNames.begin(),propertyNames.end(),ostream_iterator<string>(os,":"));
#ifdef DEBUG_LP
  PROPERTYCODELOGINIT;
  LDEBUG << "create propertysetAccessor for " << os.str();
#endif
  LinguisticCode propertySetMask(0);
  LinguisticCode propertySetEmptyNessMask(0);
  for (set<string>::const_iterator propItr=propertyNames.begin();
       propItr!=propertyNames.end();
       propItr++)
  {
    const PropertyManager& man=getPropertyManager(*propItr);
    propertySetMask |= man.getMask();
    propertySetEmptyNessMask |= man.getEmptyNessMask();
  }
#ifdef DEBUG_LP
  LDEBUG << "propertysetMask is " << hexString(propertySetMask) << binString(propertySetMask);
  LDEBUG << "propertysetEmptyNessMask is " << hexString(propertySetEmptyNessMask) << binString(propertySetEmptyNessMask);
#endif
  return new PropertyAccessor(os.str(),propertySetMask,propertySetEmptyNessMask);
}

uint64_t PropertyCodeManager::computeNbBitsNeeded(uint64_t nbvalues) const
{
  uint64_t nbBits=1;
  for (uint64_t tmp=2;
       tmp < nbvalues;
       tmp*=2)
  {
    nbBits++;
  }
  return nbBits;
}

LinguisticCode PropertyCodeManager::computeMask(uint64_t startBit,uint64_t nbBits) const
{
  LinguisticCode mask(0);
  for (uint64_t currentBit=startBit;
       currentBit < startBit+nbBits;
       currentBit++)
  {
    mask += static_cast<LinguisticCode>(std::pow(2.0,(uint64_t)currentBit));
  }
  return mask;
}

LinguisticCode PropertyCodeManager::encode(const std::map<std::string,std::string>& propValues) const
{
#ifdef DEBUG_LP
  PROPERTYCODELOGINIT;
#endif
  LinguisticCode coded(0);
#ifdef DEBUG_LP
  LDEBUG << "encode";
#endif
  for (map<string,string>::const_iterator it=propValues.begin();
       it!=propValues.end();
       it++)
  {
    const PropertyManager& man=getPropertyManager(it->first);
    man.getPropertyAccessor().writeValue(man.getPropertyValue(it->second),coded);
 #ifdef DEBUG_LP
   LDEBUG << it->first << " : " << it->second << " coded = " << hexString(coded) << binString(coded);
 #endif
 }
  return coded;
}

void PropertyCodeManager::convertSymbolicCodes(const std::string& symbolicCodeFile,std::map<std::string,LinguisticCode>& conversionTable) const
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
    throw std::runtime_error(std::string("PropertyCodeManager::convertSymbolicCodes Unable to open ") + symbolicCodeFile);
  }
  if (!parser->parse( QXmlInputSource(&file)))
  {
    PROPERTYCODELOGINIT;
    LERROR << "PropertyCodeManager::convertSymbolicCodes An error occurred parsing" << symbolicCodeFile << ". Error: " << parser->errorHandler()->errorString() ;
    throw XMLException(std::string("Error while parsing " + symbolicCodeFile + " : " + parser->errorHandler()->errorString().toUtf8().constData()));
  }
}

} // PropertyCode
} // Common
} // Lima
