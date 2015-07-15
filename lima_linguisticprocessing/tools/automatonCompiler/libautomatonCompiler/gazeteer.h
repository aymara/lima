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
/******************************************************************************
*
* File        : gazeteer.h
* Project     : DETECT - NamedEntities
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Thu Oct 24 2002
* Copyright   : (c) 2002 by CEA
* Version     : $Id: gazeteer.h 2799 2006-03-07 16:22:12Z romaric $
*
* Description : a class for the definition of gazeteers
*
******************************************************************************/

#ifndef GAZETEER_H
#define GAZETEER_H

#include "automatonString.h"
#include <iostream>
#include <string>
#include <vector>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class RecognizerCompiler;

class Gazeteer : public std::vector<LimaString>
{
 public:
  Gazeteer(); 
  Gazeteer(const Gazeteer&);
  ~Gazeteer();
  Gazeteer& operator = (const Gazeteer&);
  
  friend std::ostream& operator << (std::ostream&, const Gazeteer&);
  friend QDebug& operator << (QDebug&, const Gazeteer&);
  bool operator==(const LimaString& alias) const { return m_alias==alias; }

  uint64_t numberOfWords() const;
  const LimaString& alias() const;
  LimaString stringAutomaton(const LimaString& constraint=LimaString()) const;
  const AutomatonString& getAutomatonString() const { return m_automatonString; }

  void setAlias(const LimaString&);
  void addWord(const LimaString&);
  // TODO: unused operation, to be deleted?
  Gazeteer& add(const Gazeteer&);
  void buildAutomatonString(const std::vector<Gazeteer>& gazeteers,
                            const std::vector<SubAutomaton>& subAutomatons);
  
  void readFromFile(const std::string& filename);
  void read(RecognizerCompiler& reco);
  LimaString readName(RecognizerCompiler& reco);
  void readValues(RecognizerCompiler& reco,
                  const LimaString& stringBegin=LimaString());
 private:
  LimaString m_alias;
  AutomatonString m_automatonString;
};

/***********************************************************************/
// inline access functions
/***********************************************************************/
inline uint64_t Gazeteer::numberOfWords() const { return size(); }
inline void Gazeteer::addWord(const LimaString& s) { push_back(s); }
inline const LimaString& Gazeteer::alias() const { return m_alias; }
inline void Gazeteer::setAlias(const LimaString& a) { m_alias = a; }


} // end namespace
} // end namespace
} // end namespace

#endif
