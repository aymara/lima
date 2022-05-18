// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @file     BoWToken.cpp
 * @author   Besancon Romaric
 * @date     Tue Oct  7 2003
 * copyright Copyright (C) 2003 by CEA LIST
 *
 ***********************************************************************/


#include "bowToken.h"
#include "bowTokenPrivate.h"
#include "bowNamedEntity.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include "common/Data/readwritetools.h"

#include "bowTerm.h"        // for the generic
#include "bowNamedEntity.h" // input function

#include <iterator>  // for debug output
#include <algorithm> // function (on refMap)

#include <string>

#include <wchar.h>

using namespace std;

namespace Lima
{
namespace Common
{
namespace BagOfWords
{

uint8_t toInt(const BoWType& bt)
{
  if (bt == BoWType::BOW_NOTYPE) return 0;
  else if (bt == BoWType::BOW_TOKEN) return 1;
  else if (bt == BoWType::BOW_TERM) return 2;
  else if (bt == BoWType::BOW_NAMEDENTITY) return 3;
  else if (bt == BoWType::BOW_PREDICATE) return 4;
  else  return std::numeric_limits<uint8_t>::max();
}


#define DEFAULT_SEPARATOR L'#'
#define DEFAULT_COMPOUND_SEPARATOR L'_'

//***********************************************************************
// static members initialization
//***********************************************************************
LimaChar BoWTokenPrivate::m_separator=DEFAULT_SEPARATOR;
LimaChar BoWTokenPrivate::m_compoundSeparator=DEFAULT_COMPOUND_SEPARATOR;
bool BoWTokenPrivate::m_useOnlyLemma=true; //TODO mettre false quand indexation compound terminee

BoWTokenPrivate::BoWTokenPrivate() :
    m_lemma(),
    m_inflectedForm(),
    m_position(0),
    m_length(0),
    m_vertex(0)
{
//   BOWLOGINIT;
//   LDEBUG << "Creating BoWToken : " << (*this) << " - " << this;
}

BoWTokenPrivate::BoWTokenPrivate(const LimaString& lemma,
                   const LinguisticCode category,
                   const uint64_t position,
                   const uint64_t length):
    m_lemma(lemma),
    m_inflectedForm(),
    m_category(category),
    m_position(position),
    m_length(length),
    m_vertex(0)
{
  convertSpaces();
//   BOWLOGINIT;
//   LDEBUG << "Creating BoWToken : " << (*this) << " - " << this;
}

BoWTokenPrivate::BoWTokenPrivate(const BoWToken& tok) :
    m_lemma(tok.m_d->m_lemma),
    m_inflectedForm(tok.m_d->m_inflectedForm),
    m_category(tok.m_d->m_category),
    m_position(tok.m_d->m_position),
    m_length(tok.m_d->m_length),
    m_vertex(tok.m_d->m_vertex)
{
//   BOWLOGINIT;
//   LDEBUG << "Copying BoWToken : " << tok << " - " << &tok << " ; new one is: " << this;
}

BoWTokenPrivate::BoWTokenPrivate(const LimaString& str,
                   const uint64_t position,
                   const uint64_t length):
  m_inflectedForm(),
  m_position(position),
  m_length(length),
  m_vertex(0)
{
  //uint64_t i(str.find(m_separator));
  // uint64_t i(str.find(m_separator));
  int i(str.indexOf(m_separator));
  if (i == -1)
  {
    m_lemma=str;
    m_category=L_NONE;
  }
  else
  {
    m_lemma=str.mid(0,i);
    m_category=LinguisticCode::fromString(str.midRef(i+1).toString().toStdString());
  }

//   BOWLOGINIT;
//   LDEBUG << "Creating BoWToken : " << (*this) << " - " << this;
}

BoWTokenPrivate::BoWTokenPrivate(const BoWTokenPrivate& tok):
    m_lemma(tok.m_lemma),
    m_inflectedForm(tok.m_inflectedForm),
    m_category(tok.m_category),
    m_position(tok.m_position),
    m_length(tok.m_length),
    m_vertex(tok.m_vertex)
{
}

BoWTokenPrivate& BoWTokenPrivate::operator=(const BoWTokenPrivate& tok)
{
    if (&tok != this)
    {
        m_lemma = tok.m_lemma;
        m_inflectedForm = tok.m_inflectedForm;
        m_category = tok.m_category;
        m_position = tok.m_position;
        m_length = tok.m_length;
        m_vertex = tok.m_vertex;
    }
    return *this;
}

bool BoWTokenPrivate::operator==(const BoWTokenPrivate& tok)
{
    return (
               m_lemma == tok.m_lemma &&
               m_inflectedForm == tok.m_inflectedForm &&
               m_category == tok.m_category &&
               m_position == tok.m_position &&
               m_length == tok.m_length &&
               m_vertex == tok.m_vertex
           );
}

//***********************************************************************
// destructor
//***********************************************************************
BoWTokenPrivate::~BoWTokenPrivate()
{}

//***********************************************************************
// constructors
//***********************************************************************
BoWToken::BoWToken(BoWTokenPrivate& d) :
    m_d(&d)
{
#ifdef DEBUG_LP
    BOWLOGINIT;
    LDEBUG << "BoWToken::BoWToken(BoWTokenPrivate): " << (*this) << " - " << this;
#endif
}

BoWToken::BoWToken() :
    m_d(new BoWTokenPrivate())
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "BoWToken::BoWToken()" << this;
#endif
}

BoWToken::BoWToken(const LimaString& lemma,
                   const LinguisticCode category,
                   const uint64_t position,
                   const uint64_t length):
    m_d(new BoWTokenPrivate(lemma, category, position, length))
{
  m_d->convertSpaces();
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "Creating BoWToken : " << (*this) << " - " << this;
#endif
}

BoWToken::BoWToken(const BoWToken& tok) :
    m_d(new BoWTokenPrivate(tok))
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "BoWToken::BoWToken(BoWToken) : " << tok << " - " << &tok << " ; new one is: " << this;
#endif
}

BoWToken& BoWToken::operator=(const BoWToken& tok)
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "BoWToken::operator=(BoWToken) : " << tok << " - " << &tok << " ; new one is: " << this;
#endif
  *m_d = *tok.m_d;
  return *this;
}

bool BoWToken::operator==(const BoWToken& tok)
{
  return( *m_d == *tok.m_d);
}

BoWToken::BoWToken(const LimaString& str,
                   const uint64_t position,
                   const uint64_t length):
    m_d(new BoWTokenPrivate(str, position, length))
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "BoWToken::BoWToken" <<  this << str << position << length;
#endif
  //uint64_t i(str.find(m_separator));
  // uint64_t i(str.find(m_separator));
  int i(str.indexOf(m_d->m_separator));
  if (i == -1)
  {
    m_d->m_lemma=str;
    m_d->m_category=L_NONE;
  }
  else
  {
    m_d->m_lemma=str.left(i);
    m_d->m_category=LinguisticCode::fromString(str.midRef(i+1).toString().toStdString());
  }

//   BOWLOGINIT;
//   LDEBUG << "Creating BoWToken : " << (*this) << " - " << this;
}

BoWToken* BoWToken::clone() const
{
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "BoWToken::clone " << this;
#endif
  return new BoWToken(*(new BoWTokenPrivate(static_cast<BoWTokenPrivate&>(*(this->m_d)))));
}

// BoWToken* BoWToken::clone(const std::map<BoWToken*,BoWToken*>& /*unused refmap*/) const
// {
  //     return new BoWToken(*this);
  // }



//***********************************************************************
// destructor
//***********************************************************************
BoWToken::~BoWToken()
{
  delete m_d;
}


//***********************************************************************
// member functions
//***********************************************************************
Lima::LimaString BoWToken::getLemma(void) const {return m_d->m_lemma;};
Lima::LimaString BoWToken::getInflectedForm(void) const {return m_d->m_inflectedForm;};
LinguisticCode BoWToken::getCategory(void) const {return m_d->m_category;};
uint64_t BoWToken::getPosition(void) const {return m_d->m_position;};
uint64_t BoWToken::getLength(void)   const {return m_d->m_length;};

/** size of the BoWToken is the number of parts in the token */
uint64_t BoWToken::size(void) const { return 1; }

void BoWToken::setLemma(const Lima::LimaString& lemma)  {m_d->m_lemma = lemma;};
void BoWToken::setInflectedForm(const Lima::LimaString& inflectedForm)  {
  m_d->m_inflectedForm = inflectedForm;};
void BoWToken::setCategory(LinguisticCode c)  {m_d->m_category = c;};
void BoWToken::setPosition(const uint64_t pos){m_d->m_position = pos;};
void BoWToken::setLength(const uint64_t len)  {m_d->m_length = len;};

BoWType BoWToken::getType() const { return BoWType::BOW_TOKEN; }

uint64_t BoWToken::getVertex() const {return m_d->m_vertex;}
void BoWToken::setVertex(uint64_t vertex) {m_d->m_vertex = vertex;}

void BoWToken::setUseOnlyLemma(const bool b) { BoWTokenPrivate::m_useOnlyLemma=b; }
bool BoWToken::getUseOnlyLemma() { return BoWTokenPrivate::m_useOnlyLemma; }


void BoWToken::setInternalSeparator(const LimaChar c)
{
    BoWTokenPrivate::m_separator=c;
}
void BoWToken::setCompoundSeparator(const LimaChar c)
{
    BoWTokenPrivate::m_compoundSeparator=c;
}

LimaChar BoWToken::getInternalSeparator()
{
    return BoWTokenPrivate::m_separator;
}
LimaChar BoWToken::getCompoundSeparator()
{
    return BoWTokenPrivate::m_compoundSeparator;
}

void BoWToken::addToPosition(const uint64_t offset) {
  m_d->m_position+=offset;
}



LimaString BoWToken::getString(void) const
{
//#ifdef DEBUG_LP
//       BOWLOGINIT;
//#endif

  if (m_d->m_useOnlyLemma)
  {
// #ifdef DEBUG_LP
//     LDEBUG << "BoWToken::getString: m_d->m_useOnlyLemma is 'true'";
//     LDEBUG << "BoWToken::getString: getLemma()=" << getLemma();
// #endif
    return getLemma();
  }
  else
  {
    ostringstream cat;
    cat << m_d->m_category.toString();
//#ifdef DEBUG_LP
//     LDEBUG << "BoWToken::getString: m_d->m_useOnlyLemma is 'false'";
//#endif
/*    ostringstream len;
    len << m_length;
    return m_lemma + m_separator + LimaString(cat.str()) + m_separator + LimaString(len.str());*/
//#ifdef DEBUG_LP
//     LDEBUG << "BoWToken::getString: getLemma()=" << getLemma() << ", cat=" << Misc::utf8stdstring2limastring(cat.str() );
//#endif
    return getLemma() + m_d->m_separator + Misc::utf8stdstring2limastring(cat.str());
  }
}


// convert the spaces in lemma string to a non-space character
// (problems in indexing)
void BoWTokenPrivate::convertSpaces(void)
{
  int current(0);
  int i=m_lemma.indexOf(Misc::utf8stdstring2limastring(" "),current);
  while (i != -1)
  {
    m_lemma[i]=m_compoundSeparator;
    current=i+1;
    i=m_lemma.indexOf(Misc::utf8stdstring2limastring(" "),current);
  }
}

//***********************************************************************
// get a list of (position,length) couples identifying the parts
// of the tokens
//***********************************************************************
Common::Misc::PositionLengthList BoWToken::getPositionLengthList() const
{
  return Common::Misc::PositionLengthList(1, make_pair(
        Common::Misc::Position(m_d->m_position),
        Common::Misc::Length(m_d->m_length)));
}

//***********************************************************************
// == operator
// added by Benoit Mathieu
//***********************************************************************

bool BoWToken::operator==(const BoWToken& t) const
{
  if ((getType()==BoWType::BOW_NAMEDENTITY) && (t.getType()==BoWType::BOW_NAMEDENTITY))
  {
    const BoWNamedEntity* n1=dynamic_cast<const BoWNamedEntity*>(this);
    const BoWNamedEntity* n2=dynamic_cast<const BoWNamedEntity*>(&t);
    if (n1->getNamedEntityType()!=n2->getNamedEntityType()) return false;
  }
  return ( (getType()==t.getType()) &&
           (m_d->m_category==t.m_d->m_category) &&
           (m_d->m_lemma==t.m_d->m_lemma));
}

bool BoWToken::operator!=(const BoWToken& t) const
{
  return !(*this==t);
}


//***********************************************************************
// output
//***********************************************************************
std::ostream& operator << (std::ostream& os, const BoWToken& tok)
{
  os << "(" << Misc::limastring2utf8stdstring(tok.getLemma()) << "-"
     << tok.m_d->m_category.toString() << "-" << tok.m_d->m_position << "-" << tok.m_d->m_length << ")";
  return os;
}

QDebug& operator << (QDebug& os, const BoWToken& tok)
{
  os << "(" << tok.getLemma() << "-"
     << tok.m_d->m_category.toString() << "-" << tok.m_d->m_position << "-" << tok.m_d->m_length << ")";
  return os;
}

std::string BoWToken::getOutputUTF8String(const Common::PropertyCode::PropertyManager* macroManager) const
{
  std::ostringstream oss;
  oss << "(" << Misc::limastring2utf8stdstring(getLemma()) << "-";
  if (macroManager==0)
  {
    oss << m_d->m_category.toString();
  }
  else
  {
    oss << macroManager->getPropertySymbolicValue(m_d->m_category);
  }
  oss << "-" << m_d->m_position << ")";
  return oss.str();
}

std::string BoWToken::getIdUTF8String() const
{
  std::ostringstream oss;
  oss << "(" << Misc::limastring2utf8stdstring(m_d->m_lemma) << "-"
      << m_d->m_category.toString() << "-" << m_d->m_position << ")";
  return oss.str();
}

std::set< uint64_t > BoWToken::getVertices() const
{
  std::set< uint64_t > result;
  result.insert(getVertex());
  return result;
}


} // namespace BagOfWords
} // namespace Common
} // namespace Lima
