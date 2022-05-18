// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
