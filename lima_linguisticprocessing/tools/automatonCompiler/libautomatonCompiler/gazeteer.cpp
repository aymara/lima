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
* File        : gazeteer.cpp
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Thu Oct 24 2002
* Copyright   : (c) 2002 by CEA
* Version     : $Id: gazeteer.cpp 8081 2007-04-13 17:07:06Z mesnardo $
*
******************************************************************************/


#include "gazeteer.h"
#include "ruleFormat.h"
#include "recognizerCompiler.h"
#include "linguisticProcessing/core/Automaton/automatonCommon.h" // for exceptions
#include "tstring.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

#define AUCLOGINIT LOGINIT("Automaton::Compiler")

/***********************************************************************/
// constructors
/***********************************************************************/
Gazeteer::Gazeteer():
std::vector<LimaString>(0),
m_alias(),
m_automatonString() {
}

Gazeteer::Gazeteer(const Gazeteer& g):
std::vector<LimaString>(g),
m_alias(g.m_alias),
m_automatonString(g.m_automatonString)
{
}

/***********************************************************************/
// destructor
/***********************************************************************/
Gazeteer::~Gazeteer() {
}

/***********************************************************************/
// copy
/***********************************************************************/
Gazeteer& Gazeteer::operator = (const Gazeteer& g) {
  if (this != &g) {
    std::vector<LimaString>::operator=(g);
    m_alias = g.alias();
    m_automatonString=g.m_automatonString;
  }
  return (*this);
}

/***********************************************************************/
// operations on gazeteers
// TODO: unused operation, to be deleted?
Gazeteer& Gazeteer::add(const Gazeteer& g) {
  insert(end(),g.begin(),g.end());
  return *this;
}
/***********************************************************************/


/***********************************************************************/
// build the automatonString corresponding to the gazeteer
/***********************************************************************/
void Gazeteer::buildAutomatonString(const std::vector<Gazeteer>& gazeteers,
                                    const std::vector<SubAutomaton>& subAutomatons) {

  //build it directly (avoid generate and reparse string)
  m_automatonString.setType(ALTERNATIVE);
  for (Gazeteer::const_iterator it=begin();it!=end();it++) {
    m_automatonString.getParts().push_back(AutomatonString(*it,gazeteers,subAutomatons));
  }
}

/***********************************************************************/
// input and ouput
/***********************************************************************/
// return the regexp string of the automaton that recognize the gazeteer
LimaString Gazeteer::stringAutomaton(const LimaString& constraint) const {
  if (empty()) {
    return LimaString(); 
  }
  LimaString output(Common::Misc::utf8stdstring2limastring("("));
  Gazeteer::const_iterator it=begin();
  output += (*it)+constraint;
  for (it++; it!=end();it++) {
    output += LimaChar('|')+(*it)+constraint;
  }
  output += LimaChar(')');
  return output;
}
  
void Gazeteer::readFromFile(const std::string& filename) {
  RecognizerCompiler reco(filename);
  read(reco);
}

void Gazeteer::read(RecognizerCompiler& reco) {
  LimaString stringBegin=readName(reco);
  if (m_alias.isEmpty()) {
#ifdef DEBUG_LP
    AUCLOGINIT;
    LDEBUG << "No more gazeteer defined in file " 
           << reco.getFilename();
#endif
    return;
  }
  readValues(reco,stringBegin);
}

LimaString Gazeteer::readName(RecognizerCompiler& reco) {
  LimaString s;
//   AUCLOGINIT;
  while (! reco.endOfFile()) {
    reco.readline(s);
//     LDEBUG << "Gazeteer: line read=[" << s << "]";
    if (s.length() == 0) { continue; } // skip blank lines
    if (s[0] == CHAR_COMMENT) { continue; } // skip comments
    if (s[0] == CHAR_BEGIN_NAMEGAZ) {
      int offsetEqual(findSpecialCharacter(s,CHAR_EQUAL_GAZ,0));
      if (offsetEqual != -1) {
        LimaString alias = s.mid(1,offsetEqual-1);
        setAlias(alias);
//         LDEBUG << "Gazeteer: reading gazeteer: " << alias;
        int offsetParOpen(findSpecialCharacter(s,CHAR_OPEN_GAZ,
                                                        offsetEqual));
        if (offsetParOpen != -1) {
          return s.mid(offsetParOpen+1,s.length()-offsetParOpen-1);
        }
        return LimaString();
        //     else {
      }
    }
  }
  return LimaString();
}

void Gazeteer::readValues(RecognizerCompiler& reco,
                          const LimaString& stringBegin) {
#ifdef DEBUG_LP
  AUCLOGINIT;
#endif
  LimaString s(stringBegin);
  do {
    s = s.trimmed();
    int offset(0);
    int previousOffset(0);
#ifdef DEBUG_LP
    LDEBUG << "Gazeteer: line read=" << s;
#endif
    offset=findSpecialCharacter(s,CHAR_OPEN_GAZ,previousOffset);
    if (offset != -1) {
      previousOffset=offset+1;
      if (offset > 1) {
        AUCLOGINIT;
        LWARN << "reading list: characters before opening parenthesis will be ignored in: " << s;
      }
    }
    offset=findSpecialCharacter(s,CHAR_WORDSEP_GAZ,previousOffset);
    while (offset != -1) {
      addWord(s.mid(previousOffset,offset-previousOffset));
      previousOffset=offset+1;
      offset=findSpecialCharacter(s,CHAR_WORDSEP_GAZ,previousOffset);
    }
    int offsetParClose(findSpecialCharacter(s,CHAR_CLOSE_GAZ,0));
    if (offsetParClose != -1) {
      if (previousOffset < offsetParClose) { // possibly a last element
        addWord(s.mid(previousOffset,offsetParClose-previousOffset));
      }
      break;
    }
    else if (previousOffset < s.length()) {
      addWord(s.mid(previousOffset));
    }
    reco.readline(s);
  } while (! reco.endOfFile());
}

ostream& operator << (ostream& os, const Gazeteer& g) {
  os << Common::Misc::limastring2utf8stdstring(g.alias()) << "=(";
  if (! g.empty()) {
    os << Common::Misc::limastring2utf8stdstring(g[0]);
    for (std::vector<LimaString>::size_type i(1); i<g.size(); i++) {
      os << ";" << Common::Misc::limastring2utf8stdstring(g[i]);
    }
  }
  os << ")";
  return os;
}

QDebug& operator << (QDebug& os, const Gazeteer& g) {
  os << (g.alias()) << "=(";
  if (! g.empty()) {
    os << (g[0]);
    for (std::string::size_type i(1); i<g.size(); i++) {
      os << ";" << (g[i]);
    }
  }
  os << ")";
  return os;
}



} // end namespace
} // end namespace
} // end namespace
