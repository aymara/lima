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

#include "common/tools/FileUtils.h"
#include "common/Data/strwstrtools.h"

#include <QtGlobal>

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

CharChart::CharChart() : AbstractResource(), m_classes(), m_chars(), 
    m_unicodeCategories(),
    m_unicodeCategories2LimaClasses()
{
#if QT_VERSION < 0x050000
  m_unicodeCategories
  << "NoCategory";
#endif
  m_unicodeCategories
  << "Mark_NonSpacing"
  << "Mark_SpacingCombining"
  << "Mark_Enclosing"
  << "Number_DecimalDigit"
  << "Number_Letter"
  << "Number_Other"
  << "Separator_Space"
  << "Separator_Line"
  << "Separator_Paragraph"
  << "Other_Control"
  << "Other_Format"
  << "Other_Surrogate"
  << "Other_PrivateUse"
  << "Other_NotAssigned"
  << "Letter_Uppercase"
  << "Letter_Lowercase"
  << "Letter_Titlecase"
  << "Letter_Modifier"
  << "Letter_Other"
  << "Punctuation_Connector"
  << "Punctuation_Dash"
  << "Punctuation_Open"
  << "Punctuation_Close"
  << "Punctuation_InitialQuote"
  << "Punctuation_FinalQuote"
  << "Punctuation_Other"
  << "Symbol_Math"
  << "Symbol_Currency"
  << "Symbol_Modifier"
  << "Symbol_Other";
  
// c_all, c_del, c_b, c_par, c_dot, c_comma, c_slash, c_hyphen, c_lowline, c_quote, c_fraction, c_percent, c_del1, c_plus, c_del2, c_Mm, c_degree, c_M, c_A, c_O, c_S, c_N, c_V, c_m, c_o, c_l_o, c_a, c_s, c_n, c_a_t, c_5, c_other, m_pattern, m_end_pattern, m_line, m_parag, unknwn
//   m_unicodeCategories2LimaClasses.insert("Mark_NonSpacing","");
//   m_unicodeCategories2LimaClasses.insert("Mark_SpacingCombining","");
//   m_unicodeCategories2LimaClasses.insert("Mark_Enclosing","");
  m_unicodeCategories2LimaClasses.insert("Number_DecimalDigit","c_5");
  m_unicodeCategories2LimaClasses.insert("Number_Letter","c_5");
  m_unicodeCategories2LimaClasses.insert("Number_Other","c_5");
  m_unicodeCategories2LimaClasses.insert("Separator_Space","c_b");
  m_unicodeCategories2LimaClasses.insert("Separator_Line","c_par");
  m_unicodeCategories2LimaClasses.insert("Separator_Paragraph","c_par");
  m_unicodeCategories2LimaClasses.insert("Other_Control","c_b");
//   m_unicodeCategories2LimaClasses.insert("Other_Format","");
//   m_unicodeCategories2LimaClasses.insert("Other_Surrogate","");
  m_unicodeCategories2LimaClasses.insert("Other_PrivateUse","c_hyphen");
  m_unicodeCategories2LimaClasses.insert("Other_NotAssigned","unknwn");
  m_unicodeCategories2LimaClasses.insert("Letter_Uppercase","c_M");
  m_unicodeCategories2LimaClasses.insert("Letter_Lowercase","c_m");
  m_unicodeCategories2LimaClasses.insert("Letter_Titlecase","c_M");
  m_unicodeCategories2LimaClasses.insert("Letter_Modifier","c_Mm");
  m_unicodeCategories2LimaClasses.insert("Letter_Other","c_Mm");
  m_unicodeCategories2LimaClasses.insert("Punctuation_Connector","c_hyphen");
  m_unicodeCategories2LimaClasses.insert("Punctuation_Dash","c_hyphen");
  m_unicodeCategories2LimaClasses.insert("Punctuation_Open","c_par");
  m_unicodeCategories2LimaClasses.insert("Punctuation_Close","c_par");
  m_unicodeCategories2LimaClasses.insert("Punctuation_InitialQuote","c_quote");
  m_unicodeCategories2LimaClasses.insert("Punctuation_FinalQuote","c_quote");
  m_unicodeCategories2LimaClasses.insert("Punctuation_Other","c_dot");
  m_unicodeCategories2LimaClasses.insert("Symbol_Math","c_plus");
  m_unicodeCategories2LimaClasses.insert("Symbol_Currency","c_del1");
  m_unicodeCategories2LimaClasses.insert("Symbol_Modifier","c_del1");
  m_unicodeCategories2LimaClasses.insert("Symbol_Other","c_del1");
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
  LDEBUG << "Creating a CharChart (loads file)";
  MediaId language=manager->getInitializationParameters().language;

  try {
    QString charChartFileName=Common::Misc::findFileInPaths(Common::MediaticData::MediaticData::single().getResourcesPath().c_str(),unitConfiguration.getParamsValueAtKey("charFile").c_str());
    loadFromFile(charChartFileName.toUtf8().constData());

  } catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no parameter 'charFile' in charchart group for language " << (int) language << " !";
    throw InvalidConfiguration();
  }
}



// Gets the Class of the specified character.
// If specified character was not defined,
// try to use the mapping with Qt Unicode categories and if this fails too,
// class named unknwn is returned
const CharClass* CharChart::charClass (const LimaChar& c) const
{
    if (c.unicode() >= m_chars.size() || m_chars[c.unicode()] == 0 || m_chars[c.unicode()]->charClass() == 0)
    {
      if (c.category() < m_unicodeCategories.size())
      {
        QString unicodeCategory = m_unicodeCategories[c.category()];
        if (m_unicodeCategories2LimaClasses.contains(unicodeCategory))
        {
#ifdef DEBUG_LP
          TOKENIZERLOGINIT;
          LDEBUG << "CharChart::charClass using unicode category" << unicodeCategory  << "and LIMA class" << m_unicodeCategories2LimaClasses[unicodeCategory] ;
#endif
          return classNamed(m_unicodeCategories2LimaClasses[unicodeCategory]);
        }
      }
      TOKENIZERLOGINIT;
      LNOTICE << "CharChart::charClass undefined char: " << c;
      return classNamed(utf8stdstring2limastring("unknwn"));
    }
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LTRACE << "CharChart::charClass" << c << c.unicode() << m_chars.size() << m_chars[c.unicode()]<< m_chars[c.unicode()]->charClass();
#endif
    return m_chars[c.unicode()]->charClass();
}

const CharClass* CharChart::charClass (const LimaChar& c1, const LimaChar& c2) const
{
  if (m_surrogates.find(c1) == m_surrogates.end())
  {
      TOKENIZERLOGINIT;
      LNOTICE << "CharChart::charClass undefined 32 bytes char: " << c1  << c2 ;
      return classNamed(utf8stdstring2limastring("unknwn"));
  }
  const std::map<LimaChar,Char*>& c1classes = (*(m_surrogates.find(c1))).second;
  if (c1classes.find(c2) == c1classes.end())
  {
      TOKENIZERLOGINIT;
      LNOTICE << "CharChart::charClass undefined 32 bytes char: " << c1  << c2 ;
      return classNamed(utf8stdstring2limastring("unknwn"));
  }
  else
  {
    return (*(c1classes.find(c2))).second->charClass();
  }
}

// Gets the upper case corresponding of the specified
// character.
LimaChar CharChart::maj(const LimaChar& c) const
{
  return c.toUpper();
}

// Gets the lower case corresponding of the specified
// character. 
LimaChar CharChart::min (const LimaChar& c) const
{
  return c.toLower();
}

// Gets the unmark character corresponding to the specified
// character. If specified character was not defined,
// InvalidCharException is raised.
LimaChar CharChart::unmark (const LimaChar& c) const {
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
#endif
  if (c.unicode() >= m_chars.size())
        throw InvalidCharException();
  if (m_chars[c.unicode()] == 0)
      return LimaChar();
  if (m_chars[c.unicode()]->charClass() == 0)
      throw InvalidCharException();
  if (m_chars[c.unicode()]->longUnmark() != 0)
      return LimaChar();
  if (m_chars[c.unicode()]->min() != 0 
      && m_chars[c.unicode()]->min()->unmark() != 0)
    return (*(m_chars[c.unicode()]->min()->unmark()))();
  if (m_chars[c.unicode()]->unmark() != 0)
    return (*(m_chars[c.unicode()]->unmark()))();
  return LimaChar();
}

// Gets the unmark string corresponding of the specified
// character. If specified character was not defined,
// InvalidCharException is raised.
// Null string, one, two and more characters string can be returned.
LimaString CharChart::unmarkByString (const LimaChar& c) const
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
	LDEBUG << "CharChart::unmarkByString" << c;
#endif
  if (c.unicode() >= m_chars.size())
        throw InvalidCharException();
  if (m_chars[c.unicode()] == 0)
  {
    LimaString result;
    result.push_back(c);
#ifdef DEBUG_LP
    LDEBUG << "CharChart::unmarkByString" << result;
#endif
    return result;
  }
  if (!m_chars[c.unicode()]->charClass())
      throw InvalidCharException();

  LimaString result;
  if (m_chars[c.unicode()]->unmark() != 0 && m_chars[c.unicode()]->unmark() != m_chars[c.unicode()])
    result.push_back(m_chars[c.unicode()]->unmark()->code());
  if (m_chars[c.unicode()]->longUnmark() != 0 && m_chars[c.unicode()]->longUnmark() != m_chars[c.unicode()])
    result.push_back(m_chars[c.unicode()]->longUnmark()->code());
#ifdef DEBUG_LP
  LDEBUG << "CharChart::unmarkByString" << result;
#endif
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
//         else
//           desaccented.push_back(str.at(i));
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
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "toLower("<<src<<") = " << src.toLower();
#endif
  return src.toLower();
}

const CharClass* CharChart::classNamed(const LimaString& name) const
{
// #ifdef DEBUG_LP
//   TOKENIZERLOGINIT;
//   LDEBUG << "Searching class " << limastring2utf8stdstring(name);
// #endif
  std::vector<CharClass*>::const_iterator itcc, itcc_end;
  itcc = m_classes.begin(); itcc_end = m_classes.end();
  for (; itcc != itcc_end; itcc++)
  {
//     LDEBUG << "  looking at " << limastring2utf8stdstring((*itcc)->id());
    if ( (*itcc)->id() == name)
    {
//       LDEBUG << "Found " << limastring2utf8stdstring((*itcc)->id());
      return (*itcc);
    }
  }
  {
    TOKENIZERLOGINIT;
    LERROR << "CharChart::classNamed "<<Common::Misc::limastring2utf8stdstring(name)<<" NOT Found ";
  }
  return 0;
}

bool CharChart::loadFromFile(const std::string& fileName)
{
#ifdef DEBUG_LP
  TOKENIZERLOADERLOGINIT;
  LDEBUG << "Loading CharChart from " << fileName;
#endif
  std::ifstream file(fileName.c_str(), std::ifstream::binary);
  if (!file.good())
  {
    TOKENIZERLOADERLOGINIT;
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
#ifdef DEBUG_LP
      if (newClass->superClass() != 0)
      {
        LDEBUG << "  Loaded class " << ch.name << " < " << Common::Misc::limastring2utf8stdstring(newClass->superClass()->id());
      }
      else
      {
        LDEBUG << "  Loaded class " << ch.name << " < NONE";
      }
#endif
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
        TOKENIZERLOGINIT;
        LERROR << "Error loading char '" << ch.code << "'";
        continue;
      }
      const CharClass* newCharClass = classNamed(Common::Misc::utf8stdstring2limastring(ch.charclass));
      if (newCharClass == 0)
      {
        TOKENIZERLOGINIT;
        LERROR << "Error loading char '" << ch.code << "' : unknown class '" << ch.charclass << "'";
        continue;
      }
      newChar->setCharClass(newCharClass);
      newChar->setName(Common::Misc::utf8stdstring2limastring(ch.name));
#ifdef DEBUG_LP
      LDEBUG << "Adding char" << newChar->name() << newCharClass->name();
#endif
    }
    charIt = charchart.chars.begin(); charItend = charchart.chars.end();
    for (; charIt != charItend; charIt++)
    {
      const charchart_char& ch = *charIt;
      Char* newChar = lazyGetChar(ch.code);
#ifdef DEBUG_LP
      LDEBUG << "Modifiers for" << newChar->name();
#endif
//       const CharClass* newCharClass = classNamed(Common::Misc::utf8stdstring2limastring(ch.charclass));
      std::vector<modifierdef>::const_iterator modIt, modItEnd;
      modIt = ch.modifiers.begin(); modItEnd = ch.modifiers.end();
      for (; modIt != modItEnd; modIt++)
      {
#ifdef DEBUG_LP
        LDEBUG << "  modifier "<< (*modIt).first <<":" << lazyGetChar((*modIt).second)->name();
#endif
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
      if (newChar->code().isHighSurrogate())
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
    TOKENIZERLOGINIT;
    LERROR << "Error while parsing: " << fileName;
  }
  return true;
}

Char* CharChart::lazyGetChar(const LimaChar& code)
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
