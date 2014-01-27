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

#ifndef LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_CHARCLASS_H
#define LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_CHARCLASS_H

#include "FlatTokenizerExport.h"
#include "common/Data/LimaString.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{

class LIMA_FLATTOKENIZER_EXPORT CharClass 
{

public:
  CharClass();
  virtual ~CharClass();

  inline const Lima::LimaString& id() const {return m_id;}
  inline const Lima::LimaString& name() const {return m_name;}
  inline const CharClass* superClass() const {return m_super;}

  inline void setId(const Lima::LimaString& id) {m_id = id;}
  inline void setName(const Lima::LimaString& name) {m_name = name;}
  inline void setSuperClass(const CharClass* c) {m_super = c;}

  private:
  Lima::LimaString m_id;
  Lima::LimaString m_name;
  const CharClass* m_super;
};

} // Tokenizer
} // LinguisticProcessing
} // Lima

#endif // LIMA_LINGUISTICPROCESSING_TOKENIZER_CHARCLASS_H
