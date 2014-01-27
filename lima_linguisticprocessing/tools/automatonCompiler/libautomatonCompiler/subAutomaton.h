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
/************************************************************************
 *
 * @file       subAutomaton.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Jan 24 2005
 * @version    $Id: subAutomaton.h 2223 2005-03-15 16:56:41Z romaric $
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * Project     automatonCompiler
 * 
 * @brief      a class for defining sub-automatons
 * 
 * 
 ***********************************************************************/

#ifndef SUBAUTOMATON_H
#define SUBAUTOMATON_H

#include "automatonString.h"
#include <map>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class Gazeteer;

// a class for defining sub-automatons (just text manipulation)
class SubAutomaton {
public: 
  SubAutomaton();
  ~SubAutomaton();
  
  bool operator==(const LimaString& name) const { return m_name==name; }

  const LimaString& getName() const {return m_name;} 
  const LimaString& getPattern() const {return m_pattern;} 
  const std::map<LimaString,LimaString>& getAttributes() const { return m_attributes;}
  
  bool findAttribute(const LimaString& name, 
                     LimaString& value) const;
  
  const AutomatonString& getAutomatonString() const { return m_automatonString;}
 
  void setAutomatonName(const LimaString& s) {m_name=s;} 
  void setPattern(const LimaString& s) {m_pattern=s;} 
  void addAttribute(const LimaString& name,
                    const LimaString& value) {m_attributes[name]=value;}

  void buildAutomatonString(const std::vector<Gazeteer>& gazeteers,
                            const std::vector<SubAutomaton>& subAutomatons);

  friend std::ostream& operator<<(std::ostream& os, const SubAutomaton& a);
  friend QDebug& operator<<(QDebug& os, const SubAutomaton& a);
  
private:
  LimaString m_name;
  LimaString m_pattern;
  std::map<LimaString,LimaString> m_attributes;   
  AutomatonString m_automatonString;
};

} // end namespace
} // end namespace
} // end namespace

#endif
