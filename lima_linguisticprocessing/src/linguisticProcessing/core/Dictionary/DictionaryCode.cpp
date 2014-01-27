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

// NAUTITIA
//
// jys 16-DEC-2002
//
// DictionaryCode is the class which decodes linguistic properties
//

#include "linguisticProcessing/core/Dictionary/DictionaryCode.h"

// #include "common/linguisticData/linguisticData.h"
#include "common/linguisticData/languageData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/misc/strwstrtools.h"

// #include "linguisticProcessing/core/Tokenizer/Exceptions.h"
#include "linguisticProcessing/core/Dictionary/ParseBinary.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"

#include <time.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <wchar.h>

#define CODE_HEADER_SIZE 12

using namespace Lima;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::LinguisticData;

namespace Lima
{
namespace LinguisticProcessing
{
namespace Dictionary
{

SimpleFactory<AbstractResource,DictionaryCode> dictionaryCodeFactory(DICTIONARYCODE_CLASSID);

DictionaryCode::DictionaryCode() :
    BinaryEntry(),
    _mask(new uint64_t[LinguisticData::single().getNbCategories()]),
    _catBit(new uint64_t[LinguisticData::single().getNbCategories()]),
    _codeHAccess(0),
    _codes(0),
    m_language()
{ 
}

DictionaryCode::DictionaryCode(const DictionaryCode& dico) :
    AbstractResource(),
    BinaryEntry(),
    _mask(0),
    _catBit(0),
    _codeHAccess(NULL),
    _codes(NULL),
    m_language(std::numeric_limits<unsigned char>::max())
{
  // just to keep inoffensive copy by value
}

void DictionaryCode::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  /** @addtogroup ResourceConfiguration
   * - <b>&lt;group name="..." class="DictionaryCode"&gt;</b>
   *    -  codeListFile : file containing codes list
   *    -  codeFile : compiled code file
   */
  DICTIONARYLOGINIT;
#ifdef DEBUG_CD
  LDEBUG <<  "DictionaryCode::init" << LENDL;
#endif
  m_language=manager->getInitializationParameters().language;
  std::string resourcesPath=Common::LinguisticData::LinguisticData::single().getResourcesPath();
  std::string codesListFileName;
  std::string codeFileName;
  try
  {
    codesListFileName=resourcesPath+"/"+unitConfiguration.getParamsValueAtKey("codeListFile");
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'codeListFile' in DictionaryCode group for language " << (int) m_language << LENDL;
    throw InvalidConfiguration();
  }
  try
  {
    codeFileName=resourcesPath+"/"+unitConfiguration.getParamsValueAtKey("codeFile");
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'codeFile' in DictionaryCode group for language " << (int) m_language << LENDL;
    throw InvalidConfiguration();
  }
  
//  loadCodesMaps(codesListFileName);
  parse(codeFileName);
}


DictionaryCode::~DictionaryCode()
{
  delete[] _mask; _mask = 0;
  delete[] _catBit; _catBit = 0;
  delete[] _codeHAccess; _codeHAccess = 0;
  delete[] _codes; _codes = 0;
}

// Parses dictionary binary codes files, creates access method
//There are  steps :
// 1- read beginning of code file to compute size and check consistency
// 2- create code memory storage
// 3- create category array
// 4- create code arrays
void DictionaryCode::parse(const std::string& codeFileName)
{
  //    std::cerr << "DictionaryCode::parse(const  codeFileName)" << std::endl;
  // 1- read beginning of code file to compute size and check consistency
  //    std::cerr << "1- read beginning of code file to compute size and check consistency" << std::endl;
  ParseBinary parseCode(codeFileName.c_str());
  parseCode.advancePointer(4);            // skip TimeStamp
  // check compatibility number
  if (parseCode.getNumber(4) != S2_DICTIONARY_VERSION)
    throw UnconsistentDictionaryException();
  uint64_t codeSize = parseCode.getNumber(4) - CODE_HEADER_SIZE;// without header
  // 2- create code memory storage
  //    std::cerr << "2- create code memory storage" << std::endl;
  _codes = new unsigned char[codeSize];
  if (_codes == NULL) throw MemoryErrorException();
  FILE *codeFile;
  uint64_t readSize;
  codeFile = fopen(codeFileName.c_str(), "rb");
  if (codeFile == NULL) 
  {
    std::string mes = "Open file error: " + codeFileName;
    throw OpenFileException(mes);
  }
  fseek(codeFile, CODE_HEADER_SIZE, SEEK_SET);            // skip header
  readSize = fread(_codes, 1, codeSize, codeFile);
  fclose(codeFile);
  if (readSize != codeSize) throw ReadErrorException("readSize != codeSize in DictionaryCode::parse");
  // 3- create category array
  //    std::cerr << "3- create category array" << std::endl;
  unsigned char *ptr = _codes;
  uint64_t num = getEncodedNumber(ptr);
  unsigned char *endCategory = num + ptr;
  while (ptr < endCategory)
  {
    uint64_t catEnum = getEncodedNumber(ptr);
    if (catEnum > LinguisticData::single().getNbCategories()-1) 
      throw UnconsistentDictionaryException();
    _mask[catEnum] = getEncodedNumber(ptr);
    _catBit[catEnum] = getEncodedNumber(ptr);
  }
  // 4- create code arrays
  //    std::cerr << "4- create code arrays" << std::endl;
  size_t maxLinguisticCode = LinguisticData::single().languageData(m_language).getNbMicroCategories();
  _codeHAccess = new CodeHAccess[maxLinguisticCode + 1];
  for (uint64_t i=0; i<CODE_H_ACCESS_TABLE_SIZE; i++)
  {
    _hTable[i]=NULL;
  }
  uint64_t num = getEncodedNumber(ptr);
  unsigned char *endCode = num + ptr;
  while (ptr < endCode)
  {
    LinguisticCode codeEnum = (LinguisticCode)getEncodedNumber(ptr);
    uint64_t codeValue = getEncodedNumber(ptr);
    if (codeEnum > maxLinguisticCode)
      throw UnconsistentDictionaryException();
    _codeHAccess[codeEnum]._enum = codeEnum;
    _codeHAccess[codeEnum]._value = codeValue;
    _codeHAccess[codeEnum]._stringAddr = ptr;
    _codeHAccess[codeEnum]._next = _hTable[codeValue%CODE_H_ACCESS_TABLE_SIZE];
    _hTable[codeValue%CODE_H_ACCESS_TABLE_SIZE] = &_codeHAccess[codeEnum];
    nextField(ptr);
  }
  //    std::cerr << "OUT DictionaryCode::parse(const std::string& codeFileName)" << std::endl;
}

// returns true if category of value is the supplied code
bool DictionaryCode::isThisCode(const uint64_t property,
                                const LinguisticCategory category,
                                const LinguisticCode code) const
{
  if (category <= L_NONE || category > (LinguisticData::single().getNbCategories()-1))
    throw BoundsErrorException();
  if (code <= NONE_1 || code >= LinguisticData::single().languageData(m_language).getNbMicroCategories())
    throw BoundsErrorException();
  return (_codeHAccess[code]._value == (property & _mask[category]));
}

// returns category value in position from supplied code and category
uint64_t DictionaryCode::categoryValue(const LinguisticCode code,
    const LinguisticCategory category)
{
  if (category <= L_NONE || category >= LinguisticData::single().languageData(m_language).getNbMicroCategories())
    throw BoundsErrorException();
  if (code <= NONE_1 || code > LinguisticData::single().languageData(m_language).getNbMicroCategories())
    throw BoundsErrorException();
  return (_codeHAccess[code]._value) - (_catBit[category]);
}

uint64_t DictionaryCode::setCategory(
  const uint64_t property,
  const LinguisticCode code,
  const LinguisticCategory category)
{
  uint64_t  newCategoryValue = categoryValue(code, category);
  uint64_t newProperty = ( property & (!_mask[category]) ) & newCategoryValue;
  return newProperty;
}

// Returns code of value for the supplied category
LinguisticCode DictionaryCode::code(const uint64_t property,
                                    const LinguisticCategory category) const
{
  if (category <= L_NONE || category > (LinguisticData::single().getNbCategories()-1))
    throw BoundsErrorException();
  uint64_t codeValue = property & (_mask[category]) | (_catBit[category]);
  CodeHAccess *codeHAccess = _hTable[codeValue%CODE_H_ACCESS_TABLE_SIZE];
  while (codeHAccess != NULL)
  {
    if (codeHAccess->_value == codeValue)
      return codeHAccess->_enum;
    codeHAccess = codeHAccess->_next;
  }
  return NONE_1;
}


/** Returns code of value (not in position) for the supplied category */
LinguisticCode DictionaryCode::unpositionedCode(const uint64_t property,
    const LinguisticCategory category) const
{
  if (category <= L_NONE || category > (LinguisticData::single().getNbCategories()-1))
    throw BoundsErrorException();
  uint64_t codeValue = property;
  uint64_t mask = _mask[category];
  while ((mask & 1) == 0)
  {
    codeValue <<= 1;
    mask >>= 1;
  }
  codeValue |= _catBit[category];
  CodeHAccess *codeHAccess = _hTable[codeValue%CODE_H_ACCESS_TABLE_SIZE];
  while (codeHAccess != NULL)
  {
    if (codeHAccess->_value == codeValue)
      return codeHAccess->_enum;
    codeHAccess = codeHAccess->_next;
  }
  return NONE_1;
}

/** returns the string code of value for supplied category */
LimaString DictionaryCode::stringCode(const uint64_t property,
                                       const LinguisticCategory category) const
{
  if (category <= L_NONE || category > (LinguisticData::single().getNbCategories()-1))
    throw BoundsErrorException();
  uint64_t codeValue = property & _mask[category] | (_catBit[category]);
  CodeHAccess *codeHAccess = _hTable[codeValue%CODE_H_ACCESS_TABLE_SIZE];
  while (codeHAccess != NULL)
  {
    if (codeHAccess->_value == codeValue)
      return getStringNoMove(codeHAccess->_stringAddr);
    codeHAccess = codeHAccess->_next;
  }
  return LimaString();
}

/** returns the string code of value for supplied linguistic code */
LimaString DictionaryCode::stringCode(const LinguisticCode code) const
{
  if (code > LinguisticData::single().languageData(m_language).getNbMicroCategories()) throw BoundsErrorException();
  if ((code == NONE_1) || (_codeHAccess[code]._stringAddr == 0))
    return LimaString();
  LimaString res = getStringNoMove(_codeHAccess[code]._stringAddr);
//  std::cerr << "stringCode(" << code << ") = " << Common::Misc::limastring2utf8stdstring(res) << std::endl;
  return res;
}

/*
void DictionaryCode::loadCodesMaps(const std::string& codesListFileName)
{
  std::ifstream codesListFile(codesListFileName.c_str());

  if (!codesListFile.good())
  {
    std::ostringstream oss;
    oss << "Cannot open " << codesListFileName << std::endl;
    throw std::runtime_error(oss.str().c_str());
  }
  LinguisticCode current = 0;
  while (codesListFile.good() && !codesListFile.eof())
  {
    std::string lineString;
    getline(codesListFile, lineString);
    
    // allow empty lines and comments
    if ( (lineString.size() > 0) && (lineString[0] != '#') )
    {
      // allow to specify numbers
      size_t pos = lineString.find(';');
      if (pos != std::string::npos)
      {
        std::istringstream(lineString.substr(pos+1)) >> current;
        lineString = lineString.substr(0,pos);
      }
      if (m_strCode.find(lineString) != m_strCode.end())
      {
        throw std::runtime_error("duplicated micro category " + lineString + " in " + codesListFileName);
      }
      m_codeStr.insert( std::make_pair(current, lineString));
      m_strCode.insert( std::make_pair(lineString, current));
      current++;
    }
  }
  if (m_strCode.size() != m_codeStr.size())
  {
    throw std::runtime_error("code->str and str->code sizes differ after loading " + codesListFileName);
  }
}
*/
/** Returns code of value for the supplied string */
LinguisticCode DictionaryCode::code(const std::string& strCode) const
{
  return LinguisticData::single().languageData(m_language).getMicroCategory(strCode);
}

} //closing namespace Dictionary
} //closing namespace LinguisticProcessing
} //closing namespace Lima
