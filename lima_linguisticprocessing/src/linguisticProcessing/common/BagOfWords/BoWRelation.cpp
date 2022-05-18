// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @file     BoWRelation.cpp
 * @author   Gael de Chalendar
 * @date     Tue Oct 31 2006
 * copyright Copyright (C) 2006-2012 by CEA LIST
 *
 ***********************************************************************/


#include "BoWRelation.h"
#include "bowToken.h"
#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include "common/Data/readwritetools.h"

#include <iterator>  // for debug output
#include <algorithm> // function (on refMap)

#include <string>

using namespace std;

namespace Lima
{
namespace Common
{
namespace BagOfWords
{

#define DEFAULT_SEPARATOR L'#'
#define DEFAULT_COMPOUND_SEPARATOR L'_'

//***********************************************************************
// static members initialization
//***********************************************************************
LimaChar BoWRelation::m_separator=DEFAULT_SEPARATOR;
LimaChar BoWRelation::m_compoundSeparator=DEFAULT_COMPOUND_SEPARATOR;
bool BoWRelation::m_useOnlyRealization=false;

//***********************************************************************
// constructors
//***********************************************************************
BoWRelation::BoWRelation() :
    m_realization(),
    m_type(0),
    s_type(0)
{
//   BOWLOGINIT;
//   LDEBUG << "Creating BoWRelation : " << (*this) << " - " << this;
}

BoWRelation::BoWRelation(const LimaString& realization,
                   uint64_t type):
    m_realization(realization),
    m_type(type)
{
  convertSpaces();
//   BOWLOGINIT;
//   LDEBUG << "Creating BoWRelation : " << (*this) << " - " << this;
}

BoWRelation::BoWRelation(const LimaString& realization,
                   uint64_t type,uint64_t syntype):
    m_realization(realization),
    m_type(type),
    s_type(syntype)
{
  convertSpaces();
//   BOWLOGINIT;
//   LDEBUG << "Creating BoWRelation : " << (*this) << " - " << this;
}

BoWRelation::BoWRelation(const BoWRelation& rel) :
    m_realization(rel.m_realization),
    m_type(rel.m_type),
    s_type(rel.s_type)
{
//   BOWLOGINIT;
//   LDEBUG << "Copying BoWRelation : " << rel << " - " << &rel << " ; new one is: " << this;
}


//***********************************************************************
// destructor
//***********************************************************************
BoWRelation::~BoWRelation()
{
}

//***********************************************************************
// member functions
//***********************************************************************
LimaString BoWRelation::getString(void) const
{
  //     BOWLOGINIT;
  //     std::string dummy;
  //     std::wostringstream wossdummy;
  //     wossdummy << m_realization << m_separator;
  //     LDEBUG << "getString: " << Lima::Common::Misc::wstring2string(dummy, wossdummy.str()) << m_type;

  if (m_useOnlyRealization)
  {
    return getRealization();
  }
  else
  {
    std::ostringstream oss;
    oss << m_type;
    return getRealization() + m_separator + Misc::utf8stdstring2limastring(oss.str());
  }
}


// convert the spaces in realization string to a non-space character
// (problems in indexing)
void BoWRelation::convertSpaces(void)
{
  int current(0);
  int i=m_realization.indexOf(" ",current);
  while (i != -1)
  {
    m_realization[i]=m_compoundSeparator;
    current=i+1;
    i=m_realization.indexOf(" ",current);
  }
}

//***********************************************************************
// == operator
// added by Benoit Mathieu
//***********************************************************************

bool BoWRelation::operator==(const BoWRelation& t) const
{
  return ( (m_type==t.m_type) &&
           (m_realization==t.m_realization) && 
           (s_type == t.s_type));
}

bool BoWRelation::operator!=(const BoWRelation& t) const
{
  return !(*this==t);
}


//***********************************************************************
// input functions
//***********************************************************************
LimaString BoWRelation::findNextElement(const LimaString& str,
                                      int& current,
                                      const LimaChar& separator)
{
  int i=str.indexOf(separator,current);
  if (i==-1)
  {
    Lima::LimaString res(str.mid(current));
    if (res.isEmpty())
    {
      string s = Lima::Common::Misc::limastring2utf8stdstring(str);
      throw InputErrorException("no other field in string "+s);
    }
    else
    {
      return res;
    }
  }
  Lima::LimaString res=str.mid(current,i-current);
  current=i+1;
  return res;
}

void BoWRelation::parse(const Lima::LimaString& str, int current)
{
  BOWLOGINIT;
  LDEBUG << "BoWRelation::parse" << str << current << findNextElement(str,current,m_separator);
  m_realization = findNextElement(str,current,m_separator);
  setType(findNextElement(str,current,m_separator).toInt());
}

//***********************************************************************
// binary input/output
//***********************************************************************
void BoWRelation::read(std::istream& file)
{
  Misc::readUTF8StringField(file,m_realization);
  m_type=Misc::readCodedInt(file);
  s_type=Misc::readCodedInt(file);
//   LDEBUG << "BoWRelation::read " <<  Misc::limastring2utf8stdstring(m_realization) << " / " << m_type;
}

void BoWRelation::write(std::ostream& file) const
{
//   LDEBUG << "BoWRelation::write " <<  Misc::limastring2utf8stdstring(m_realization) << " / " << m_type;
  Misc::writeUTF8StringField(file,m_realization);
  Misc::writeCodedInt(file,m_type);
  Misc::writeCodedInt(file,s_type);
}

//**********************************************************************
// output on ostream (mostly for debug)
std::ostream& operator << (std::ostream& os, const BoWRelation& rel)
{
  os << "/" << Lima::Common::Misc::limastring2utf8stdstring(rel.getRealization()) << "-"
  << rel.m_type << "/" << "-" << rel.s_type << "/";
  return os;
}
QDebug& operator << (QDebug& os, const BoWRelation& rel)
{
  os << "/" << rel.getRealization() << "-"
  << rel.m_type << "/" << "-" << rel.s_type << "/";
  return os;
}

//***********************************************************************
// output
//***********************************************************************
std::string BoWRelation::getOutputUTF8String() const
{
  std::ostringstream oss;
  oss << "/" << Misc::limastring2utf8stdstring(getRealization()) << "-"
      << m_type << "/" << "-" << s_type << "/";
  return oss.str();
}

std::string BoWRelation::getIdUTF8String() const
{
  std::ostringstream oss;
  oss << "(" << Misc::limastring2utf8stdstring(m_realization) << "-"
  << m_type << ")";
  return oss.str();
}

} // namespace BagOfWords
} // namespace Common
} // namespace Lima
