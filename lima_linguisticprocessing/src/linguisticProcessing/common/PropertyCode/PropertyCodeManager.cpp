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


#include <math.h>
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
  oss << hex << x;
  return oss.str();
}

PropertyCodeManager::PropertyCodeManager()
{}

void PropertyCodeManager::readFromXmlFile(const std::string& filename)
{
  PROPERTYCODELOGINIT;
  
  // check that file exists
  {
    ifstream fin(filename.c_str(), std::ifstream::binary);
    if (!fin.good()) {
      LERROR << "invalid XMLPropertyCode file " << filename;
      throw InvalidConfiguration();
    }
    fin.close();
  }
  
  LDEBUG << typeid(*this).name() << "PropertyCodeManager::readFromXmlFile before creating parser";
  QXmlSimpleReader* parser = new QXmlSimpleReader();
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
  LDEBUG << "PropertyCodeManager::readFromXmlFile before parsing";
  parser->setContentHandler(&handler);
  parser->setErrorHandler(&handler);
  QFile file(filename.c_str());
  if (!file.open(QIODevice::ReadOnly))
  {
    LERROR << "An error occurred  Error: Cannot open " << filename ;
    return;
  }
  if (!parser->parse( QXmlInputSource(&file)))
  {
    LERROR << "An error occurred  Error: " << parser->errorHandler()->errorString() ;
    return;
  }
  LDEBUG << "PropertyCodeManager::readFromXmlFile parsed. before deleting parser";
  delete parser;
  // Compute coding properties
  uint64_t usedBits=0;

  // compute data for properties
  const std::vector<XMLPropertyHandler::PropertyDescription>& properties=handler.getProperties();
  LINFO << properties.size() << " properties read from xmlfile ";
  for (std::vector<XMLPropertyHandler::PropertyDescription>::const_iterator desc=properties.begin();
       desc!=properties.end();
       desc++)
  {
    // compute mask
    LINFO << "compute data for property " << desc->name;
	std::vector<std::string>::size_type nbvalues=desc->values.size() + 1;
    uint64_t nbBits=computeNbBitsNeeded(nbvalues);
    LDEBUG << nbvalues << " values so use " << nbBits << " bits";
    uint64_t mask=computeMask(usedBits,nbBits);
    LDEBUG << "mask is " << hexString(mask);
    
    // compute values
    std::map<std::string,LinguisticCode> symbol2code;
    symbol2code["NONE"] = LinguisticCode(0);
    uint64_t i=1;
    for (std::vector<std::string>::const_iterator valItr=desc->values.begin();
         valItr!=desc->values.end();
         valItr++)
    {
      symbol2code.insert(std::make_pair(*valItr, LinguisticCode(i << usedBits)));
      LDEBUG << *valItr << " => " << hexString(i << usedBits);
      i++;
    }
    usedBits+=nbBits;
    m_propertyManagers.insert(std::make_pair(desc->name,PropertyManager(desc->name,static_cast<LinguisticCode>(mask),static_cast<LinguisticCode>(mask),symbol2code)));
  }

  const std::vector<XMLPropertyHandler::SubPropertyDescription>& subproperties=handler.getSubProperties();
  LINFO << subproperties.size() << " subproperties read from file";
  for (std::vector<XMLPropertyHandler::SubPropertyDescription>::const_iterator desc=subproperties.begin();
       desc!=subproperties.end();
       desc++)
  {
    LINFO << "compute data for subproperty " << desc->name;
    const PropertyManager& parentProp=getPropertyManager(desc->parentName);
    uint64_t parentmask=parentProp.getMask();

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
    LDEBUG << "maximum subvalues is " << maxsubvalues << " so use " << nbBits << " bits";
    uint64_t emptynessmask=computeMask(usedBits,nbBits);
    uint64_t mask=emptynessmask + parentmask;
    LDEBUG << "mask = " << hexString(mask);
    LDEBUG << "emptyness mask = " << hexString(emptynessmask);

    // compute values
    std::map<std::string,LinguisticCode> symbol2code;
    symbol2code["NONE"] = LinguisticCode(0);
    for (std::vector<std::pair<std::string,std::vector<std::string> > >::const_iterator subItr=subvalues.begin();
         subItr!=subvalues.end();
         subItr++)
    {
      uint64_t parentValue=parentProp.getPropertyValue(subItr->first);
      if (parentValue == 0)
      {
        LERROR << "parent value " << subItr->first << " of subproperty " << desc->name << " is unknown !";
      }
      LDEBUG << "compute subvalues of " << subItr->first << " (" << hexString(parentValue) << ")";
      symbol2code.insert(std::make_pair(string(subItr->first)+"-NONE",LinguisticCode(parentValue)));
      uint64_t i=1;
      for (std::vector<std::string>::const_iterator valItr=subItr->second.begin();
           valItr!=subItr->second.end();
           valItr++)
      {
        symbol2code.insert(std::make_pair(*valItr,LinguisticCode((i << usedBits) + parentValue)));
        LDEBUG << *valItr << " => " << hexString(symbol2code[*valItr]);
        i++;
      }
    }
    m_propertyManagers.insert(std::make_pair(desc->name,PropertyManager(desc->name,static_cast<LinguisticCode>(mask),static_cast<LinguisticCode>(emptynessmask),symbol2code)));
    usedBits+=nbBits;

  }

  LINFO << "have used " << usedBits << " bits to code all properties";
  if (usedBits > 32)
  {
    LERROR << "needs " << usedBits << " bits to code all properties !! Encoding may be Invalid !!";
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
  PROPERTYCODELOGINIT;
  LDEBUG << "create propertysetAccessor for " << os.str();
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
  LDEBUG << "propertysetMask is " << hexString(propertySetMask);
  LDEBUG << "propertysetEmptyNessMask is " << hexString(propertySetEmptyNessMask);
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

uint64_t PropertyCodeManager::computeMask(uint64_t startBit,uint64_t nbBits) const
{
  uint64_t mask(0);
  for (uint64_t currentBit=startBit;
       currentBit < startBit+nbBits;
       currentBit++)
  {
    mask += (uint64_t) pow(2.0,(int)currentBit);
  }
  return mask;
}

LinguisticCode PropertyCodeManager::encode(const std::map<std::string,std::string>& propValues) const
{
  PROPERTYCODELOGINIT;
  LinguisticCode coded(0);
  LDEBUG << "encode";
  for (map<string,string>::const_iterator it=propValues.begin();
       it!=propValues.end();
       it++)
  {
    const PropertyManager& man=getPropertyManager(it->first);
    man.getPropertyAccessor().writeValue(man.getPropertyValue(it->second),coded);
    LDEBUG << it->first << " : " << it->second << " coded = " << hexString(coded);
  }
  return coded;
}

void PropertyCodeManager::convertSymbolicCodes(const std::string& symbolicCodeFile,std::map<std::string,LinguisticCode>& conversionTable) const
{
  PROPERTYCODELOGINIT;
  LINFO << "convert Symbolic Code file " << symbolicCodeFile;

  QXmlSimpleReader* parser = new QXmlSimpleReader();
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
    LERROR << "An error occurred  Error: Cannot open " << symbolicCodeFile ;
    return;
  }
  if (!parser->parse( QXmlInputSource(&file)))
  {
    LERROR << "An error occurred parsing" << symbolicCodeFile << ". Error: " << parser->errorHandler()->errorString() ;
    throw std::runtime_error(parser->errorHandler()->errorString().toUtf8().constData());
  }
  delete parser;
}

} // PropertyCode
} // Common
} // Lima
