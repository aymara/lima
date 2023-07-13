// Copyright 2002-2018 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_TSTATUS_TR_small, (QLatin1String("t_")));

/***********************************************************************/
// constructors
/***********************************************************************/
Gazeteer::Gazeteer():
std::vector<LimaString>(0),
m_alias(),
m_hasMultiTermWord(false),
m_hasOnlyWords(true),
m_automatonString()
{
}

Gazeteer::Gazeteer(const Gazeteer& g):
std::vector<LimaString>(g),
m_alias(g.m_alias),
m_hasMultiTermWord(g.m_hasMultiTermWord),
m_hasOnlyWords(g.m_hasOnlyWords),
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
    m_hasMultiTermWord=g.m_hasMultiTermWord;
    m_hasOnlyWords=g.m_hasOnlyWords;
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
// add a word in the inherited  std::vector<LimaString>
// check if word is simple word (no category, no Tstatus)
/***********************************************************************/
void Gazeteer::addWord(const LimaString& s, const vector<Gazeteer>& otherGazeteers) {
  if( (s.startsWith(*STRING_TSTATUS_TR))
   || (s.startsWith(*STRING_TSTATUS_TR_small))
   || (s.contains(CHAR_POS_TR))
   || (s.startsWith(CHAR_BEGIN_ENTITY))
  )
  {
    m_hasOnlyWords=false;
  }
  //if( s.contains(CHAR_SEP_RE) )  {
  if (findSpecialCharacter(s,CHAR_SEP_RE)!=-1) { // if spaces are escaped, is not a multi-term
    setHasMultiTermWordFlag();
  }
  if (s.startsWith(CHAR_BEGIN_NAMEGAZ)) {
    // This is a reference to other gazeteer. Let's take copy all its' items.
#ifdef DEBUG_LP
    AUCLOGINIT;
    LINFO << "reference to other gazeteer found:" << s;
#endif

    LimaString otherGazeteerName = s.right(s.size()-1);

#ifdef DEBUG_LP
    LINFO << "otherGazeteerName=" << otherGazeteerName;
#endif

    for (auto &other: otherGazeteers) {
        if (other.alias() == otherGazeteerName) {
            add(other);
            break;
        }
    }
  } else
    push_back(s);
}


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
    vector<Gazeteer> empty;
    readValues(reco, empty, stringBegin);
}

void Gazeteer::readValues(RecognizerCompiler& reco,
                          const vector<Gazeteer>& otherGazeteers,
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
        //AUCLOGINIT;
     //   LWARN << "reading list: characters before opening parenthesis will be ignored in: " << s;
        throw std::runtime_error("found parenthesis in " + s.toStdString());
      }
    }
    offset=findSpecialCharacter(s,CHAR_WORDSEP_GAZ,previousOffset);
    while (offset != -1) {
      addWord(s.mid(previousOffset,offset-previousOffset),otherGazeteers);
      previousOffset=offset+1;
      offset=findSpecialCharacter(s,CHAR_WORDSEP_GAZ,previousOffset);
    }
    int offsetParClose(findSpecialCharacter(s,CHAR_CLOSE_GAZ,0));
    if (offsetParClose != -1) {
      if (previousOffset < offsetParClose) { // possibly a last element
        addWord(s.mid(previousOffset,offsetParClose-previousOffset),otherGazeteers);
      }
      break;
    }
    else if (previousOffset < s.length()) {
      addWord(s.mid(previousOffset),otherGazeteers);
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
