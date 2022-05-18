// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       subAutomaton.cpp
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Jan 24 2005
 * @version    $Id: subAutomaton.cpp 2954 2006-10-10 08:17:03Z gael $
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "subAutomaton.h"
#include "gazeteer.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

SubAutomaton::SubAutomaton():
m_name(),
m_pattern(),
m_attributes(),
m_automatonString() 
{
}

SubAutomaton::~SubAutomaton() 
{
}

bool SubAutomaton::
findAttribute(const LimaString& name, 
              LimaString& value) const {
  std::map<LimaString,LimaString>::const_iterator 
    it=m_attributes.find(name);
  if (it==m_attributes.end()) {
    return false;
  }
  else {
    value=(*it).second;
    return true;
  }
}

void SubAutomaton::
buildAutomatonString(const std::vector<Gazeteer>& gazeteers,
                     const std::vector<SubAutomaton>& subAutomatons) {
  m_automatonString=AutomatonString(m_pattern,gazeteers,subAutomatons);
}

//**********************************************************************
// output
std::ostream& operator<<(std::ostream& os, const SubAutomaton& sub) {
  os << "\"" << Common::Misc::limastring2utf8stdstring(sub.getName()) << "\":"
  << "pattern=/" << Common::Misc::limastring2utf8stdstring(sub.getPattern()) << "/";
  std::map<LimaString,LimaString>::const_iterator
  att=sub.getAttributes().begin(),
  att_end=sub.getAttributes().end();
  for (; att!=att_end; att++) {
    os << ";" << Common::Misc::limastring2utf8stdstring((*att).first) << "=" << Common::Misc::limastring2utf8stdstring((*att).second);
  }
  return os;
}

QDebug& operator<<(QDebug& os, const SubAutomaton& sub) {
  os << "\"" << sub.getName() << "\":"
  << "pattern=/" << sub.getPattern() << "/";
  std::map<LimaString,LimaString>::const_iterator
  att=sub.getAttributes().begin(),
  att_end=sub.getAttributes().end();
  for (; att!=att_end; att++) {
    os << ";" << (*att).first << "=" << (*att).second;
  }
  return os;
}

} // end namespace
} // end namespace
} // end namespace
