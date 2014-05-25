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
// jys 21-JUL-2002
//
// Char is the array of valid characters. It is used to find
// corresponding characters class, maj, min, unmark.
// Performances at run-time are very important.

#include "CharChart.h"
#include "Char.h"
#include "CharClass.h"
#include "SpiritCharChartParser.hpp"
#include "common/misc/Exceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/MediaticData/mediaticData.h"

#include "common/Data/strwstrtools.h"

#include <string>
#include <algorithm>
#include <fstream>

#include <boost/spirit/include/qi_match.hpp>

using namespace Lima;
using namespace Common;
using namespace Common::Misc;
using namespace Common::MediaticData;
using namespace Common::XMLConfigurationFiles;


namespace Lima {
namespace LinguisticProcessing {
namespace FlatTokenizer {

SimpleFactory<AbstractResource,CharChart> flatTokenizerCharChartFactory(FLATTOKENIZERCHARCHART_CLASSID);

CharChart::CharChart() : AbstractResource(), m_classes(), m_chars()
{
}

CharChart::~CharChart()
{
  std::vector<CharClass*>::iterator itcc, itcc_end;
  itcc = m_classes.begin(); itcc_end = m_classes.end();
  for (; itcc != itcc_end; itcc++)
  {
    delete *itcc;
  }
  std::vector<Char*>::iterator itc, itc_end;
  itc = m_chars.begin(); itc_end = m_chars.end();
  for (; itc != itc_end; itc++)
  {
    delete *itc;
  }
}


void CharChart::init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
    
{
  TOKENIZERLOGINIT;
  LDEBUG << "Creating a CharChart (loads file)" << LENDL;
  MediaId language=manager->getInitializationParameters().language;

  try {
    std::string resourcePath=Common::MediaticData::MediaticData::single().getResourcesPath();
    std::string charChartFileName=resourcePath + "/" + unitConfiguration.getParamsValueAtKey("charFile");
    loadFromFile(charChartFileName);

  } catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no parameter 'charFile' in charchart group for language " << (int) language << " !" << LENDL;
    throw InvalidConfiguration();
  }
}



// Gets the Class of the specified character.
// If specified character was not defined,
// class named unknwn is returned
const CharClass* CharChart::charClass (LimaChar c) const
{
    if (c.unicode() >= m_chars.size() || m_chars[c.unicode()] == 0 || m_chars[c.unicode()]->charClass() == 0)
    {
      TOKENIZERLOGINIT;
      LNOTICE << "CharChart::charClass undefined char: " << c << LENDL;
      return classNamed(utf8stdstring2limastring("unknwn"));
    }
    return m_chars[c.unicode()]->charClass();
}

const CharClass* CharChart::charClass (LimaChar c1, LimaChar c2) const
{
  if (m_surrogates.find(c1) == m_surrogates.end())
  {
      TOKENIZERLOGINIT;
      LNOTICE << "CharChart::charClass undefined 32 bytes char: " << c1  << c2  << LENDL;
      return classNamed(utf8stdstring2limastring("unknwn"));
  }
  const std::map<LimaChar,Char*>& c1classes = (*(m_surrogates.find(c1))).second;
  if (c1classes.find(c2) == c1classes.end())
  {
      TOKENIZERLOGINIT;
      LNOTICE << "CharChart::charClass undefined 32 bytes char: " << c1  << c2  << LENDL;
      return classNamed(utf8stdstring2limastring("unknwn"));
  }
  else
  {
    return (*(c1classes.find(c2))).second->charClass();
  }
}

// Gets the upper case corresponding of the specified
// character. If specified character was not defined,
// InvalidCharException is raised.

LimaChar CharChart::maj(LimaChar c) const
{
    if (c.unicode() >= m_chars.size())
        throw InvalidCharException();
    if (m_chars[c.unicode()] == 0)
      return c;
    if (!m_chars[c.unicode()]->charClass())
        throw InvalidCharException();
    return m_chars[c.unicode()]->maj() == 0 ? LimaChar() : (*(m_chars[c.unicode()]->maj()))();
}

// Gets the lower case corresponding of the specified
// character. If specified character was not defined,
// InvalidCharException is raised.
LimaChar CharChart::min (LimaChar c) const
{
  if (c.unicode() >= m_chars.size())
        throw InvalidCharException();
  if (m_chars[c.unicode()] == 0)
      return c;
  if (!m_chars[c.unicode()]->charClass())
        throw InvalidCharException();
  return m_chars[c.unicode()]->min() == 0 ? LimaChar() : (*(m_chars[c.unicode()]->min()))();
}

// Gets the unmark character corresponding to the specified
// character. If specified character was not defined,
// InvalidCharException is raised.
LimaChar CharChart::unmark (LimaChar c) const {
  if (c.unicode() >= m_chars.size())
        throw InvalidCharException();
  if (m_chars[c.unicode()] == 0)
      return LimaChar();
  if (m_chars[c.unicode()]->charClass() == 0)
      throw InvalidCharException();
  if (m_chars[c.unicode()]->longUnmark() != 0)
      return LimaChar();
  if (m_chars[c.unicode()]->min() != 0 && m_chars[c.unicode()]->min()->unmark() != 0)
    return (*(m_chars[c.unicode()]->min()->unmark()))();
  if (m_chars[c.unicode()]->unmark() != 0)
    return (*(m_chars[c.unicode()]->unmark()))();
  return LimaChar();
}

// Gets the unmark string corresponding of the specified
// character. If specified character was not defined,
// InvalidCharException is raised.
// Null string, one, two and more characters string can be returned.
LimaString CharChart::unmarkByString (LimaChar c) const
{
  if (c.unicode() >= m_chars.size())
        throw InvalidCharException();
  if (m_chars[c.unicode()] == 0)
    {
      LimaString result;
      result.push_back(c);
      return result;
    }
    if (!m_chars[c.unicode()]->charClass())
        throw InvalidCharException();

    LimaString result;
  if (m_chars[c.unicode()]->unmark() != 0 && m_chars[c.unicode()]->unmark() != m_chars[c.unicode()])
    result.push_back(m_chars[c.unicode()]->unmark()->code());
  if (m_chars[c.unicode()]->longUnmark() != 0 && m_chars[c.unicode()]->longUnmark() != m_chars[c.unicode()])
    result.push_back(m_chars[c.unicode()]->longUnmark()->code());
  return result;
}

LimaString CharChart::unmark(const LimaString& str) const
{
  LimaString desaccented;
  desaccented.reserve(str.size());
  for (int i = 0; i < str.size(); i++)
  {
    try
    {
      LimaChar chr = unmark(str.at(i));
      if (!chr.isNull())
      {
        desaccented.push_back(chr);
        if (m_chars[str.at(i).unicode()]->hasLongUnmark())
          desaccented.push_back(m_chars[str.at(i).unicode()]->longUnmark()->code());
        
      }
      else
      {
        LimaString s = unmarkByString(str.at(i));
        if (!s.isEmpty())
          desaccented.append(s);
        else
          desaccented.push_back(str.at(i));
      }
    }
    // silently discard invalid character
    catch (InvalidCharException) {}
  }
  return desaccented;
}

LimaString CharChart::unmarkWithMapping(const LimaString& str,std::vector<unsigned char>& mapping) const
{
  LimaString desaccented;
  desaccented.reserve(str.size());
  mapping.resize(str.size());
  LimaChar chr;
  LimaString s;
  unsigned char desaccIndex=0;
  for (int i = 0; i < str.size(); i++)
  {
    try
    {
      chr = unmark(str.at(i));
      if (!chr.isNull()) {
        desaccented.push_back(chr);
        mapping[i]=desaccIndex;
        desaccIndex++;
      }
      else
      {
        s = unmarkByString(str.at(i));
        desaccented.append(s);
        mapping[i]=desaccIndex;
        desaccIndex+=s.size();
      }
    }
    // discard invalid character
    catch (InvalidCharException) {}
  }
  return desaccented;
}


/** Converts src to its lowercase equivalent */
LimaString CharChart::toLower(const LimaString& src) const
{
  TOKENIZERLOGINIT;
  LDEBUG << "toLower("<<src<<") = " << src.toLower();
  return src.toLower();
//   LimaString newString;
//   newString.reserve(src.size());
//   for (size_t i = 0; i < src.size(); i++)
//   {
//       LimaChar currentChar = src.at(i);
// //        std::wcerr << currentChar;
//       LimaChar minCurrent;
//       try {minCurrent = min(currentChar);}
//       catch (const InvalidCharException& )
//       {
//         LERROR << "toLower encountered an invalide character";
//         return LimaString();
//       }
// //        std::wcerr << " " << minCurrent << std::endl;
//       newString.push_back(minCurrent.isNull()?currentChar:minCurrent);
//   }
//   LDEBUG << "toLower("<<src<<") = " << newString;
//   return newString;
}

const CharClass* CharChart::classNamed(const LimaString& name) const
{
  TOKENIZERLOGINIT;
//   LDEBUG << "Searching class " << limastring2utf8stdstring(name) << LENDL;
  std::vector<CharClass*>::const_iterator itcc, itcc_end;
  itcc = m_classes.begin(); itcc_end = m_classes.end();
  for (; itcc != itcc_end; itcc++)
  {
//     LDEBUG << "  looking at " << limastring2utf8stdstring((*itcc)->id()) << LENDL;
    if ( (*itcc)->id() == name)
    {
//       LDEBUG << "Found " << limastring2utf8stdstring((*itcc)->id()) << LENDL;
      return (*itcc);
    }
  }
  LERROR << "CharChart::classNamed "<<Common::Misc::limastring2utf8stdstring(name)<<" NOT Found " << LENDL;
  return 0;
}

bool CharChart::loadFromFile(const std::string& fileName)
{
  TOKENIZERLOADERLOGINIT;
  LDEBUG << "Loading CharChart from " << fileName << LENDL;
  std::ifstream file(fileName.c_str(), std::ifstream::binary);
  if (!file.good())
  {
    LERROR << "Unable to open" << fileName;
    return false;
  }
  std::string str;
  Common::Misc::readStream(file, str);
  
  std::string::const_iterator iter = str.begin();
  std::string::const_iterator end = str.end();
  
//   typedef std::string::const_iterator iterator_type;
//   typedef LinguisticProcessing::FlatTokenizer::charchart_parser<iterator_type> charchart_parser;

  charchart charchart;
  /// @ERROR DOES NOT COMPILE UNDER WINDOWS
  charchart_parser<std::string::const_iterator> parser; // Our grammar
  
  bool r = phrase_parse(iter, end, parser, skipper, charchart);
  if (r)
  {
    std::vector<charchart_class>::const_iterator classIt, classItend;
    classIt = charchart.classes.begin(); classItend = charchart.classes.end();
    for (; classIt != classItend; classIt++)
    {
      const charchart_class& ch = *classIt;
      CharClass* newClass = new CharClass();
      newClass->setId(Common::Misc::utf8stdstring2limastring(ch.name));
      newClass->setName(Common::Misc::utf8stdstring2limastring(ch.comment));
      if (ch.parent.is_initialized() && !ch.parent.get().empty()
        && classNamed(Common::Misc::utf8stdstring2limastring(ch.parent.get())) != 0)
      {
        newClass->setSuperClass(classNamed(Common::Misc::utf8stdstring2limastring(ch.parent.get())));
      }
      if (newClass->superClass() != 0)
      {
        LDEBUG << "  Loaded class " << ch.name << " < " << Common::Misc::limastring2utf8stdstring(newClass->superClass()->id()) << LENDL;
      }
      else
      {
        LDEBUG << "  Loaded class " << ch.name << " < NONE" << LENDL;
      }
      m_classes.push_back(newClass);
    }
    std::vector<charchart_char>::const_iterator charIt, charItend;
    charIt = charchart.chars.begin(); charItend = charchart.chars.end();
    for (; charIt != charItend; charIt++)
    {
      const charchart_char& ch = *charIt;
      Char* newChar = lazyGetChar(ch.code);
      if (newChar == 0)
      {
        LERROR << "Error loading char '" << ch.code << "'" << LENDL;
        continue;
      }
      const CharClass* newCharClass = classNamed(Common::Misc::utf8stdstring2limastring(ch.charclass));
      if (newCharClass == 0)
      {
        LERROR << "Error loading char '" << ch.code << "' : unknown class '" << ch.charclass << "'" << LENDL;
        continue;
      }
      newChar->setCharClass(newCharClass);
      newChar->setName(Common::Misc::utf8stdstring2limastring(ch.name));
      LDEBUG << "Adding char" << newChar->name();
    }
    charIt = charchart.chars.begin(); charItend = charchart.chars.end();
    for (; charIt != charItend; charIt++)
    {
      const charchart_char& ch = *charIt;
      Char* newChar = lazyGetChar(ch.code);
      LDEBUG << "Modifiers for" << newChar->name();
//       const CharClass* newCharClass = classNamed(Common::Misc::utf8stdstring2limastring(ch.charclass));
      std::vector<modifierdef>::const_iterator modIt, modItEnd;
      modIt = ch.modifiers.begin(); modItEnd = ch.modifiers.end();
      for (; modIt != modItEnd; modIt++)
      {
        LDEBUG << "  modifier "<< (*modIt).first <<":" << lazyGetChar((*modIt).second)->name();

        switch ( (*modIt).first )
        {
          case MIN:
            newChar->setMin(lazyGetChar((*modIt).second));
            break;
          case MAJ:
            newChar->setMaj(lazyGetChar((*modIt).second));
            break;
          case UNMARK:
            newChar->setUnmark(lazyGetChar((*modIt).second));
            break;
          default: ;
        }
      }
      if (newChar->code() > 0xD800)
      {
        if (surrogates().find(newChar->code()) == surrogates().end())
        {
          surrogates().insert(std::make_pair(newChar->code(),std::map< LimaChar, Char* >()));
        }
        surrogates()[newChar->code()].insert(std::make_pair(newChar->surrogate(),newChar));
      }
    }
  }
  else
  {
    LERROR << "Error while parsing: " << fileName << LENDL;
  }
  return true;
}

Char* CharChart::lazyGetChar(LimaChar code)
{
  Char* newChar = 0;
//   if (code <= 0xD800)
//   {
  if (chars().size() <= code.unicode())
    {
      chars().resize(code.unicode()+1);
    }
    if (chars()[code.unicode()] == 0)
    {
      newChar = new Char(code);
      chars()[code.unicode()] = newChar;
    }
    else
    {
      newChar = chars()[code.unicode()];
    }
//   }
  return newChar;
}

} // Tokenizer
} // LinguisticProcessing
} // Lima
