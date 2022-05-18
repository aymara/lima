// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
