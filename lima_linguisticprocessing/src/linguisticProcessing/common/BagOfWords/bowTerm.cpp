// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @file     bowTerm.cpp
 * @author   Besancon Romaric
 * @date     Tue Oct  7 2003
 * copyright Copyright (C) 2003 by CEA LIST
 *
 ***********************************************************************/

#include "bowTerm.h"
#include "bowComplexTokenPrivate.h"
#include "common/Data/strwstrtools.h"
#include <string>

namespace Lima {
namespace Common {
namespace BagOfWords {

class BoWTermPrivate : public BoWComplexTokenPrivate
{
public:
  friend class BoWTerm;

  BoWTermPrivate() : BoWComplexTokenPrivate() {}
  BoWTermPrivate(const BoWTermPrivate& btp) : BoWComplexTokenPrivate(btp) {}
};


//**********************************************************************
// constructors
//**********************************************************************
BoWTerm::BoWTerm():BoWComplexToken()
{
}

BoWTerm::BoWTerm(const BoWTerm& t):BoWComplexToken(*new BoWTermPrivate())
{
  static_cast<BoWTermPrivate&>(*m_d).copy(t);
}

BoWTerm::BoWTerm(BoWTermPrivate& d) :
BoWComplexToken(d)
{
}


BoWTerm::BoWTerm(const Lima::LimaString& lemma,
                 const LinguisticCode category,
                 const uint64_t position,
                 const uint64_t length):
    BoWComplexToken(*new BoWComplexTokenPrivate())
{
  m_d->m_lemma = lemma;
  m_d->m_category = category;
  m_d->m_position = position;
  m_d->m_length = length;
}

BoWTerm::BoWTerm(const Lima::LimaString& lemma,
                 const LinguisticCode category,
                 const uint64_t position,
                 const uint64_t length,
                 std::deque< boost::shared_ptr< BoWToken > >& parts,
                 const uint64_t head):
    BoWComplexToken(*new BoWComplexTokenPrivate())
{
  m_d->m_lemma = lemma;
  m_d->m_category = category;
  m_d->m_position = position;
  m_d->m_length = length;
  static_cast<BoWComplexTokenPrivate *>(m_d)->m_head = head;
  for (auto i=parts.begin(); i!=parts.end(); i++)
  {
    addPart(*i);
  }
  static_cast<BoWComplexTokenPrivate*>(m_d)->m_head = head;
}

BoWTerm::~BoWTerm()
{
  BoWComplexToken::clear();
}

BoWTerm& BoWTerm::operator=(const BoWTerm& t)
{
  BoWComplexToken::operator=(t);
  return *this;
}

BoWTerm* BoWTerm::clone() const
{
  return new BoWTerm(*(new BoWTermPrivate(static_cast<BoWTermPrivate&>(*(this->m_d)))));
}


//**********************************************************************
// input/output functions
//**********************************************************************
std::string BoWTerm::getOutputUTF8String(const Common::PropertyCode::PropertyManager* macroManager) const {
  std::ostringstream oss;
  oss << BoWToken::getOutputUTF8String(macroManager) << "->" << getUTF8StringParts(macroManager);
  return oss.str();
}

std::string BoWTerm::getIdUTF8String() const {
  std::ostringstream oss;
  oss << BoWToken::getOutputUTF8String() << "->" << getUTF8StringParts();
  return oss.str();
}

std::ostream& operator<<(std::ostream& os, const BoWTerm& t) {
  os << static_cast<BoWToken>(t) << "->" << t.getstdstringParts();
  return os;
}

} // namespace BagOfWords
} // namespace Common
} // namespace Lima
