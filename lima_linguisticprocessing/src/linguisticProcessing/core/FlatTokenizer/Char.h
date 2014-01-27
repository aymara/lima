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

#ifndef LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_CHAR_H
#define LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_CHAR_H

#include "FlatTokenizerExport.h"
#include "common/Data/LimaString.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{

class CharClass;

class CharPrivate;
class LIMA_FLATTOKENIZER_EXPORT Char
{

public:
  explicit Char(LimaChar code);
  
  virtual ~Char();

  Lima::LimaChar operator()() const;
  Lima::LimaChar code() const;
  Lima::LimaChar surrogate() const;
  const Lima::LimaString& name() const;
  const CharClass* charClass() const;

  void setCode(Lima::LimaChar c);
  void setSurrogate(Lima::LimaChar s);
  void setName(const Lima::LimaString& name);
  void setCharClass(const CharClass* cl);

  const Char* maj() const;
  const Char* min() const;
  const Char* unmark() const;
  const Char* longUnmark() const;
  bool hasLongUnmark() const;

  void setMaj(const Char* maj);
  void setMin(const Char* min);
  void setUnmark(const Char* un);
  void setLongUnmark(const Char* lu);
  
private:
  CharPrivate* m_d;
};

} // Tokenizer
} // LinguisticProcessing
} // Lima

#endif // LIMA_LINGUISTICPROCESSING_TOKENIZER_CHAR_H
