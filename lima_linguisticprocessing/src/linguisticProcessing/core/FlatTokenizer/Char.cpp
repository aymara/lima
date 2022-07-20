// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "Char.h"
#include "CharClass.h"

#include "linguisticProcessing/LinguisticProcessingCommon.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{

class CharPrivate
{

public:
  explicit CharPrivate(LimaChar code);

  virtual ~CharPrivate();

  Lima::LimaChar m_code;
  Lima::LimaChar m_surrogate;

  Lima::LimaString m_name;
  const CharClass* m_class;

  const Char* m_maj;
  const Char* m_min;
  const Char* m_unmark;
  const Char* m_longUnmark;
};

CharPrivate::CharPrivate(Lima::LimaChar code) :
m_code(code), m_surrogate(0), m_name(),
m_class(0), m_maj(0), m_min(0), m_unmark(0), m_longUnmark(0)
{
}

CharPrivate::~CharPrivate()
{
}

Char::Char(Lima::LimaChar code) :
m_d(new CharPrivate(code))
{
}

Char::~Char()
{
  delete m_d;
}

Lima::LimaChar Char::operator()() const {return m_d->m_code;}
Lima::LimaChar Char::code() const {return m_d->m_code;}
Lima::LimaChar Char::surrogate() const {return m_d->m_surrogate;}
const Lima::LimaString& Char::name() const {return m_d->m_name;}
const CharClass* Char::charClass() const {return m_d->m_class;}

void Char::setCode(Lima::LimaChar c) {m_d->m_code = c;}
void Char::setSurrogate(Lima::LimaChar s) {m_d->m_surrogate = s;}
void Char::setName(const Lima::LimaString& name) {m_d->m_name = name;}
void Char::setCharClass(const CharClass* cl) {m_d->m_class = cl;}

const Char* Char::maj() const {return m_d->m_maj;}
const Char* Char::min() const {return m_d->m_min;}
const Char* Char::unmark() const {return m_d->m_unmark==0?m_d->m_min:m_d->m_unmark;}
const Char* Char::longUnmark() const {return m_d->m_longUnmark;}
bool Char::hasLongUnmark() const {return m_d->m_longUnmark != 0;}


void Char::setMaj(const Char* maj)
{
  m_d->m_maj = maj;
  // if this has a maj, then there is a maj/min relation and this is the min
  // allows to discard acronym dots when unmarking
  if (maj != 0 && m_d->m_min == 0)
  {
    m_d->m_min = this;
  }
//   if (maj != 0 && m_d->m_unmark == 0)
//   {
//     m_d->m_unmark = this;
//   }
}

void Char::setMin(const Char* min)
{
  m_d->m_min = min;
  // if this has a min, then there is a maj/min relation and this is the maj
  // allows to discard acronym dots when unmarking
  if (min != 0 && m_d->m_maj == 0)
  {
    m_d->m_maj = this;
  }
//   if (min != 0 && m_d->m_unmark == 0)
//   {
//     m_d->m_unmark = this;
//   }
}
void Char::setUnmark(const Char* un)
{
  TOKENIZERLOADERLOGINIT;
  LDEBUG << "Char::setUnmark" << (void*)this << this->code() << un->code() << (void*)m_d->m_unmark;
  if (m_d->m_unmark == 0)
  {
    m_d->m_unmark = un;
    if (un != 0 && m_d->m_min == 0)
    {
      m_d->m_min = this;
    }
    if (un != 0 && m_d->m_maj == 0)
    {
      m_d->m_maj = this;
    }
  }
  else
  {
    setLongUnmark(un);
  }
}
void Char::setLongUnmark(const Char* lu)
{
  TOKENIZERLOADERLOGINIT;
  LDEBUG << "Char::setLongUnmark" << (void*)this << this->code() << lu->code();
  m_d->m_longUnmark = lu;
  
}

} // Tokenizer
} // LinguisticProcessing
} // Lima
