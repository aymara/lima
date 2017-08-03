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
#ifndef LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_CHARCHART_H
#define LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_CHARCHART_H

#include "FlatTokenizerExport.h"
#include "common/Data/LimaString.h"

#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"

#ifdef WIN32
#undef min
#undef max
#endif

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{

class Char;
class CharClass;

#define FLATTOKENIZERCHARCHART_CLASSID "FlatTokenizerCharChart"

class LIMA_FLATTOKENIZER_EXPORT CharChart : public AbstractResource
{
  Q_OBJECT

public:
  explicit CharChart();
  virtual ~CharChart();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure&,
    Manager*)
  ;

  const std::vector<CharClass*>& classes() const  {return m_classes;}
  const std::vector<Char*>& chars() const {return m_chars;}
  std::vector<CharClass*>& classes() {return m_classes;}
  std::vector<Char*>& chars() {return m_chars;}

  /** Gets the Class of the specified character. If specified character was not
   * defined, try to use the mapping with Qt Unicode categories and if this 
   * fails too, class named unknwn is returned. */
  const CharClass* charClass(const Lima::LimaChar& c) const;
  const CharClass* charClass (const LimaChar& c1, const LimaChar& c2) const;

  /** Gets the upper case corresponding to the specified
   character. If specified character was not defined,
   InvalidCharException is raised. */
  Lima::LimaChar maj(const Lima::LimaChar& c) const;

  /** Gets the lower case corresponding to the specified
   character. If specified character was not defined,
   InvalidCharException is raised. */
  Lima::LimaChar min(const Lima::LimaChar& c) const;

  LimaString unmarkWithMapping(const LimaString& str,std::vector<unsigned char>& mapping) const;
  
  /** @brief Gets the unmark character corresponding to the specified character.
   * First take the lowercase version before unmarking.
   If specified character was not defined,
   InvalidCharException is raised.
   If unmark replacement is less or more than one character,
   0 is returned. In that case, replacement by string must be called */
  Lima::LimaChar unmark(const Lima::LimaChar& c) const;

  LimaString unmark(const LimaString& str) const;
  
  /** Gets the unmark string corresponding to the specified
   character. If specified character was not defined,
   InvalidCharException is raised.
   Null string, one, two and more characters string can be returned. */
  Lima::LimaString unmarkByString (const Lima::LimaChar& c) const;

  /** @brief Converts src to its lowercase equivalent */
  Lima::LimaString toLower(const Lima::LimaString& src) const;

  const CharClass* classNamed(const Lima::LimaString& name) const;

  bool loadFromFile(const std::string& fileName);

  inline const std::map< LimaChar, std::map< LimaChar, Char* > >& surrogates() const {return m_surrogates;}
  inline std::map< LimaChar, std::map< LimaChar, Char* > >& surrogates() {return m_surrogates;}

private:
  CharChart(const CharChart&) : AbstractResource() {}
  
  /// @brief char accessor with lazy initializer.
  /// If the the does not currently exist, it will be created
  Char* lazyGetChar(const LimaChar& code);
  
  std::vector<CharClass*> m_classes;
  std::vector<Char*> m_chars;

  std::map< LimaChar, std::map< LimaChar, Char* > > m_surrogates;
  
  /// names of unicode char catgories at QChar::Categories positions (Qt 5.4)
  QVector<QString> m_unicodeCategories;
  
  /// mapping of m_unicodeCategories names to Lima char classes names.
  /// currently a default list of Lima names is used but the mapping 
  /// should be loaded or completed from the resource file
  QMap<QString,QString> m_unicodeCategories2LimaClasses;
};

} // Tokenizer
} // LinguisticProcessing
} // Lima

#endif // LIMA_LINGUISTICPROCESSING_TOKENIZER_CHARCHART_H
