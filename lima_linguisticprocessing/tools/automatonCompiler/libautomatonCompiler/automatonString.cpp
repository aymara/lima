/*
    Copyright 2002-2019 CEA LIST

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
* File        : automatonString.cpp
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Mon Sep  8 2003
* Copyright   : (c) 2003 by CEA
* Version     : $Id: automatonString.cpp 8076 2007-04-13 17:05:34Z mesnardo $
*
************************************************************************/

#include "automatonString.h"
#include "gazeteer.h"
#include "subAutomaton.h"
#include "compilerExceptions.h"
#include "common/LimaCommon.h"
#include "tstring.h"
#include "common/Data/strwstrtools.h"

using namespace std;
using namespace Lima::Common;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

// to define sub indices in groups
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_GROUP_FIRST, (QLatin1String("first")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_GROUP_CURRENT, (QLatin1String("current")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_GROUP_NEXT, (QLatin1String("next")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_GROUP_LAST, (QLatin1String("last")));

// representing infinite loop on same state (Kleene star)
// by a special max value in optionality {0-n} or {0-N}
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_INFINITY_LC, (QLatin1String("n")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_INFINITY_UC, (QLatin1String("N")));

// for debug only
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_OPEN_DUMMY_SEQUENCE, (Common::Misc::utf8stdstring2limastring("_(")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CLOSE_DUMMY_SEQUENCE, (Common::Misc::utf8stdstring2limastring(")_")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, OUTPUT_NOKEEP_BEGIN, (Common::Misc::utf8stdstring2limastring("[")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, OUTPUT_NOKEEP_END, (Common::Misc::utf8stdstring2limastring("]")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, OUTPUT_NEGATIVE, (Common::Misc::utf8stdstring2limastring("^")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, OUTPUT_QUOTE, (Common::Misc::utf8stdstring2limastring("\"")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, OUTPUT_OR, (Common::Misc::utf8stdstring2limastring(" OR ")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, OUTPUT_NONE, (Common::Misc::utf8stdstring2limastring("none")));

//***********************************************************************
// constructors
//***********************************************************************
AutomatonString::AutomatonString():
m_unit(),
m_type(UNKNOWN_TYPE),
m_parts(),
m_minOccurrences(0),
m_maxOccurrences(0),
m_keep(true),
m_negative(false),
m_constraints(),
m_artificialSequence(false),
m_isSplittedFirst(false),
m_isSplittedLast(false)
{
}

AutomatonString::AutomatonString(const AutomatonString& r):
m_unit(r.m_unit),
m_type(r.m_type),
m_parts(r.m_parts),
m_minOccurrences(r.m_minOccurrences),
m_maxOccurrences(r.m_maxOccurrences),
m_keep(r.m_keep),
m_negative(r.m_negative),
m_constraints(r.m_constraints),
m_artificialSequence(r.m_artificialSequence),
m_isSplittedFirst(r.m_isSplittedFirst),
m_isSplittedLast(r.m_isSplittedLast)
{
}


AutomatonString::
AutomatonString(const LimaString& str):
m_unit(),
m_type(UNKNOWN_TYPE),
m_parts(),
m_minOccurrences(0),
m_maxOccurrences(0),
m_keep(true),
m_negative(false),
m_constraints(),
m_artificialSequence(false),
m_isSplittedFirst(false),
m_isSplittedLast(false)
{
  init(str,std::vector<Gazeteer>(0),std::vector<SubAutomaton>(0),0,str.size());
}

AutomatonString::
AutomatonString(const LimaString& str,
                const std::vector<Gazeteer>& gazeteers,
                const std::vector<SubAutomaton>& subAutomatons):
m_unit(),
m_type(UNKNOWN_TYPE),
m_parts(),
m_minOccurrences(0),
m_maxOccurrences(0),
m_keep(true),
m_negative(false),
m_constraints(),
m_artificialSequence(false),
m_isSplittedFirst(false),
m_isSplittedLast(false)
{
  // a correction of certain syntax simplifications [a b] -> [(a b)]
  LimaString s=syntaxCorrection(str);
#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "AutomatonString: building automaton from : " << s;
#endif

  init(str,gazeteers,subAutomatons,0,str.size());

#ifdef DEBUG_LP
  LDEBUG << "AutomatonString:BUILT:" << getStringDebug();
#endif
}

AutomatonString::
AutomatonString(const LimaString& str,
                const std::vector<Gazeteer>& gazeteers,
                const std::vector<SubAutomaton>& subAutomatons,
                const int begin,
                const int size):
m_unit(),
m_type(UNKNOWN_TYPE),
m_parts(),
m_minOccurrences(0),
m_maxOccurrences(0),
m_keep(true),
m_negative(false),
m_constraints(),
m_artificialSequence(false),
m_isSplittedFirst(false),
m_isSplittedLast(false)
{
  init(str,gazeteers,subAutomatons,begin,size);
}

void AutomatonString::init(const LimaString& str,
                           const std::vector<Gazeteer>& gazeteers,
                           const std::vector<SubAutomaton>& subAutomatons,
                           const int begin,
                           const int size)
{
  parse(str,gazeteers,subAutomatons,begin,size);
}

// constructor in which the string AND the modifier are specified
AutomatonString::AutomatonString(const LimaString& string,
                                 const LimaString& modifier):
m_unit(string),
m_type(UNKNOWN_TYPE),
m_parts(),
m_minOccurrences(0),
m_maxOccurrences(0),
m_keep(true),
m_negative(false),
m_constraints(),
m_artificialSequence(false),
m_isSplittedFirst(false),
m_isSplittedLast(false)
{
  int begin(0);
  int size(modifier.size());
  parseModifiers(modifier,begin,size);
}

//***********************************************************************
// destructor
//***********************************************************************
AutomatonString::~AutomatonString() {
  m_parts.clear();
  m_constraints.clear();
}

//***********************************************************************
// assignment operator
//***********************************************************************
AutomatonString& AutomatonString::operator = (const AutomatonString& r) {
  if (this != &r) {
    m_unit=r.m_unit;
    m_type=r.m_type;
    m_parts=r.m_parts;
    m_minOccurrences=r.m_minOccurrences;
    m_maxOccurrences=r.m_maxOccurrences;
    m_keep=r.m_keep;
    m_negative=r.m_negative;
    m_constraints=r.m_constraints;
    m_artificialSequence=r.m_artificialSequence;
    m_isSplittedFirst=r.m_isSplittedFirst;
    m_isSplittedLast=r.m_isSplittedLast;
  }
  return *this;
}

//***********************************************************************
// reinitialization
//***********************************************************************
void AutomatonString::reinit() {
  m_unit=LimaString();
  m_type=UNKNOWN_TYPE;
  m_parts.clear();
  m_constraints.clear();
  m_minOccurrences=0;
  m_maxOccurrences=0;
  m_keep=true;
  m_negative=false;
  m_artificialSequence=false;
  m_isSplittedFirst=false;
  m_isSplittedLast=false;
}

//***********************************************************************
// correction of certain syntax simplifications
// [a b] -> [(a b)]
// -> helps the parsing : [..]  are considered as modifiers
//***********************************************************************
LimaString AutomatonString::syntaxCorrection(const LimaString& s) const {
  LimaString str(s);

  int nokeepOpen=findSpecialCharacter(str,CHAR_NOKEEP_OPEN_RE,0);

  while (nokeepOpen != -1) {
    int nokeepClose=findSpecialCharacter(str,CHAR_NOKEEP_CLOSE_RE,nokeepOpen+1);
    if (nokeepClose == -1) {
      throw ExpressionSyntaxException("unbalanced brackets in "+
                                      Misc::limastring2utf8stdstring(str));
    }

    int sep=findSpecialCharacter(str,CHAR_SEP_RE,nokeepOpen+1);

    if ( sep != -1 && sep < nokeepClose &&
        (str[nokeepOpen+1] != CHAR_GROUP_OPEN_RE ||
         str[nokeepClose-1] != CHAR_GROUP_CLOSE_RE)) {
      str.insert(nokeepOpen+1,CHAR_GROUP_OPEN_RE);
      str.insert(nokeepClose+1,CHAR_GROUP_CLOSE_RE);
    }
    nokeepOpen=findSpecialCharacter(str,CHAR_NOKEEP_OPEN_RE,nokeepClose+1);
  }

  return str;
}

//***********************************************************************
// initialize the structure from a string
// -> basic parsing function
//***********************************************************************
bool AutomatonString::parse(const LimaString& s,
                            const std::vector<Gazeteer>& gazeteers,
                            const std::vector<SubAutomaton>& subAutomatons,
                            const int begin,
                            const int size) {

  if (s.isEmpty() || size==0 || begin >= s.size()) {
    return false;
  }

  // easier to work on a copy of the substring
  LimaString str = s.mid(begin,size);

#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "AutomatonString:parsing string "
         << str
         << "[" << begin << "," << size << "] in "
         << s
        ;
#endif

  int offsetGroupOpen=findSpecialCharacter(str,CHAR_GROUP_OPEN_RE,0);

  if (offsetGroupOpen == -1) { // no group ()
    if (findSpecialCharacter(str,CHAR_SEP_RE,0) == -1) {
      // no separator either -> single unit
      m_type=UNIT;
      parseUnit(str,gazeteers,subAutomatons);
      return true;
    }
    // simple sequence
    m_type=SEQUENCE;
    m_artificialSequence=true;
    splitOnChar(str,CHAR_SEP_RE,gazeteers,subAutomatons);
    return true;
  }

  // at least one group ()
  // (where things begin to get more complicated)

  // type is a sequence (maybe with just one element, but more safe)
  m_type=SEQUENCE;
  // keep in mind that this sequence has been added by construction
  m_artificialSequence=true;

  // if there is something before the group
  if (offsetGroupOpen>0) {
    // look for separator before group
    int offsetSepBefore=rfindSpecialCharacter(str,CHAR_SEP_RE,
                                                       offsetGroupOpen-1);
    if (offsetSepBefore != -1) {
      splitOnChar(str,CHAR_SEP_RE,gazeteers,subAutomatons,0,offsetSepBefore);
    }
    // else no separator before first parenthesis
    // first part is same group as the parenthesized group (separators)
  }

  int endGroup=addGroup(str,gazeteers,subAutomatons,offsetGroupOpen);

  // parse the rest
  if (endGroup != -1 && endGroup+1<str.size()-1) {
    if (str[endGroup+1] == CHAR_SEP_RE) {
      AutomatonString rest(str,gazeteers,subAutomatons,endGroup+2);
      m_parts.push_back(rest);
    }
    else {
      ostringstream oss;
      oss << "got confused by character " << (endGroup+1)
          << "(" << Common::Misc::limastring2utf8stdstring(LimaString(str[endGroup+1])) << ") while parsing "
          << Misc::limastring2utf8stdstring(str);
      throw ExpressionSyntaxException(oss.str());
    }
  }

  return true;
}

//***********************************************************************
// parse a group
// offset is the offset just after the beginning of the group
// returns the position after the end of the group (including possible
// modifiers)

int AutomatonString::
addGroup(const LimaString& str,
         const std::vector<Gazeteer>& gazeteers,
         const std::vector<SubAutomaton>& subAutomatons,
         const int offset)
{

  AutomatonString subGroup;
  subGroup.parseModifiersPre(str,offset);
  int subGroupEnd=subGroup.parseGroup(str,gazeteers,subAutomatons,offset+1);
  subGroupEnd=subGroup.parseModifiersPost(str,subGroupEnd);
  m_parts.push_back(subGroup);
  return subGroupEnd;
}

int AutomatonString::
parseGroupSequence(const LimaString& str,
                   const std::vector<Gazeteer>& gazeteers,
                   const std::vector<SubAutomaton>& subAutomatons,
                   const int begin,
                   const LimaChar endChar)
{
#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "AutomatonString:parsing sequence group \""
         << str.mid(begin) << "\""
         << " (endChar='" << endChar << "')";
#endif

  int nextSepSeq=findSpecialCharacter(str,CHAR_SEP_RE,begin);
  int subGroupOpen=findSpecialCharacter(str,CHAR_GROUP_OPEN_RE,begin);
  int nextEndChar=findSpecialCharacter(str,endChar,begin);
  int nextGroupClose=findSpecialCharacter(str,CHAR_GROUP_CLOSE_RE,
                                                   begin);

  if (nextSepSeq != -1 && nextSepSeq == begin) {
    // first char is separator => advance
    return parseGroupSequence(str,gazeteers,subAutomatons,begin+1,endChar);
  }

  if ( (nextEndChar != -1 && nextEndChar == begin) ||
      (endChar == CHAR_CHOICESEP_RE && nextGroupClose==begin)) {
    return begin;
  }

  if (subGroupOpen != -1 && (nextSepSeq==-1 || subGroupOpen < nextSepSeq) &&
    (nextEndChar==-1 ||subGroupOpen < nextEndChar)) {
    int subGroupEnd=addGroup(str,gazeteers,subAutomatons,subGroupOpen);
    return parseGroupSequence(str,gazeteers,subAutomatons,subGroupEnd+1,endChar);
  }

  // in case of a sequence subgroup in an alternative group that ends
  // and another alternative group after (a|b c) (d|e)
  if (endChar == CHAR_CHOICESEP_RE &&
    nextGroupClose != - 1 &&
    (nextEndChar==1 || nextGroupClose < nextEndChar) &&
    (nextSepSeq==-1 || nextGroupClose < nextSepSeq)) {
    m_parts.push_back(AutomatonString(str,gazeteers,subAutomatons,begin,nextGroupClose-begin));
    return nextGroupClose;
  }

  if (nextEndChar != -1 && (nextSepSeq == -1 || nextEndChar < nextSepSeq)) {
    if (nextEndChar>begin && begin<str.size()) {
      m_parts.push_back(AutomatonString(str,gazeteers,subAutomatons,begin,nextEndChar-begin));
      return nextEndChar;
    }
  }

  m_parts.push_back(AutomatonString(str,gazeteers,subAutomatons,begin,nextSepSeq-begin));
  return parseGroupSequence(str,gazeteers,subAutomatons,nextSepSeq+1,endChar);

}

int AutomatonString::
parseGroupAlternative(const LimaString& str,
                      const std::vector<Gazeteer>& gazeteers,
                      const std::vector<SubAutomaton>& subAutomatons,
                      const int begin)
{
  int nextSepAlt=findSpecialCharacter(str,CHAR_CHOICESEP_RE,begin);
  int nextSepSeq=findSpecialCharacter(str,CHAR_SEP_RE,begin);
  int subGroupOpen=findSpecialCharacter(str,CHAR_GROUP_OPEN_RE,begin);
  int nextGroupEnd=findSpecialCharacter(str,CHAR_GROUP_CLOSE_RE,begin);


#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "AutomatonString:parsing alternative group" << begin
         << str.mid(begin) << nextSepAlt << nextSepSeq << subGroupOpen << nextGroupEnd;
#endif

  if (nextSepAlt != -1 && nextSepAlt==begin) {
#ifdef DEBUG_LP
    LDEBUG << "AutomatonString:parseAlternative: goto next element";
#endif
    return parseGroupAlternative(str,gazeteers,subAutomatons,begin+1);
  }

  if (nextGroupEnd != -1 && nextGroupEnd == begin) {
#ifdef DEBUG_LP
    LDEBUG << "AutomatonString:parseAlternative: end";
#endif
    return begin;
  }

   if (nextSepSeq != -1 && (nextSepAlt == -1 || nextSepSeq < nextSepAlt) &&
     (nextGroupEnd == -1 || nextSepSeq < nextGroupEnd)) {
    // next element of the alternative is a sequence
    // (maybe complex sequence with subgroups) (a|b (c|d)|e)
    // create a subGroup, parse until next alternative separator
    AutomatonString subGroup;
    subGroup.setType(SEQUENCE);

    if (nextSepAlt == -1) {
      // this sequence element is the last of the alternative
      int subGroupEnd=subGroup.parseGroupSequence(str,gazeteers,subAutomatons,
                                                           begin,CHAR_GROUP_CLOSE_RE);
      m_parts.push_back(subGroup);
      return subGroupEnd;
    }
    else {
      int subGroupEnd=subGroup.parseGroupSequence(str,gazeteers,subAutomatons,
                                                           begin,CHAR_CHOICESEP_RE);
      m_parts.push_back(subGroup);
      if (str[subGroupEnd]==CHAR_GROUP_CLOSE_RE)
        //the sequence group was at the end of alternative
        //but contained alternative subgroups (a|b (c|d))
        return subGroupEnd;
      else if (str[subGroupEnd]==CHAR_CHOICESEP_RE)
        return parseGroupAlternative(str,gazeteers,subAutomatons,subGroupEnd+1);
      else
        throw ExpressionSyntaxException("got confused while parsing "+
                                        Misc::limastring2utf8stdstring(str));
    }
  }

  if (subGroupOpen != -1 &&
    (nextSepSeq == -1 || subGroupOpen < nextSepSeq) &&
    (nextSepAlt == -1 || subGroupOpen < nextSepAlt) &&
    (nextGroupEnd == -1 || subGroupOpen < nextGroupEnd)) {
    int subGroupEnd=addGroup(str,gazeteers,subAutomatons,subGroupOpen);
#ifdef DEBUG_LP
    LDEBUG << "AutomatonString:parseAlternative: parsing subGroup";
    LDEBUG << "AutomatonString:parseAlternative: end of subGroup is "
           << subGroupEnd;
#endif
    return parseGroupAlternative(str,gazeteers,subAutomatons,subGroupEnd+1);
  }

  if (nextGroupEnd != -1 && (nextSepAlt == -1 || nextGroupEnd < nextSepAlt)) {
    if (nextGroupEnd>begin && begin<str.size()) {
      m_parts.push_back(AutomatonString(str,gazeteers,subAutomatons,begin,nextGroupEnd-begin));
      return nextGroupEnd;
    }
  }

  m_parts.push_back(AutomatonString(str,gazeteers,subAutomatons,begin,nextSepAlt-begin));
  return parseGroupAlternative(str,gazeteers,subAutomatons,nextSepAlt+1);
}

int AutomatonString::
parseGroup(const LimaString& str,
           const std::vector<Gazeteer>& gazeteers,
           const std::vector<SubAutomaton>& subAutomatons,
           const int begin)
{

#ifdef DEBUG_LP
  AUCLOGINIT;
#endif
  int nextSepAlt=findSpecialCharacter(str,CHAR_CHOICESEP_RE,begin);
  int nextSepSeq=findSpecialCharacter(str,CHAR_SEP_RE,begin);
  int subGroupOpen=findSpecialCharacter(str,CHAR_GROUP_OPEN_RE,begin);
  int nextGroupEnd=findSpecialCharacter(str,CHAR_GROUP_CLOSE_RE,begin);

#ifdef DEBUG_LP
  LDEBUG << "AutomatonString:parsing unknown group "
         << str.mid(begin)
         << nextSepAlt << nextSepSeq << subGroupOpen << nextGroupEnd;
#endif

  if (nextGroupEnd == begin) {
    return begin;
  }

  if (subGroupOpen != -1 &&
    (nextSepSeq == -1 || subGroupOpen < nextSepSeq) &&
    (nextSepAlt == -1 || subGroupOpen < nextSepAlt) &&
    (nextGroupEnd == -1 || subGroupOpen < nextGroupEnd)) {
    int subGroupEnd=addGroup(str,gazeteers,subAutomatons,subGroupOpen);
    // looking at next separator
    if (str[subGroupEnd+1] == CHAR_CHOICESEP_RE)
    {
      m_type=ALTERNATIVE;
      return parseGroupAlternative(str,gazeteers,subAutomatons,subGroupEnd+2);
    }
    else if (str[subGroupEnd+1] == CHAR_GROUP_CLOSE_RE)
    {
      // end of group
      m_type=SEQUENCE;
      return subGroupEnd+1;
    }
    else if (str[subGroupEnd+1] == CHAR_SEP_RE)
      // dont know yet
      return parseGroup(str,gazeteers,subAutomatons,subGroupEnd+2);
    else
    {
      ostringstream oss;
      oss << "unexpected charater on position " << (subGroupEnd+1)
          << " of string \"" << Misc::limastring2utf8stdstring(str) << "\"";
      throw ExpressionSyntaxException(oss.str());
    }
  }

  if ((nextSepSeq == -1 || nextGroupEnd <= nextSepSeq) && (nextSepAlt == -1 || nextGroupEnd <= nextSepAlt)) {
    // default type for unit group is sequence
    m_type=SEQUENCE;
    m_artificialSequence=true;
    if (nextGroupEnd > begin && begin < str.size()-1) {
      m_parts.push_back(AutomatonString(str,gazeteers,subAutomatons,begin,nextGroupEnd-begin));
    }
    return nextGroupEnd;
  }

  // have to determine the type,
  // look at first separator to guess the type of group
  if (nextSepAlt == -1) {
    // not an alternative group
    m_type=SEQUENCE;
    m_parts.push_back(AutomatonString(str,gazeteers,subAutomatons,begin,nextSepSeq-begin));
    return parseGroupSequence(str,gazeteers,subAutomatons,nextSepSeq+1,CHAR_GROUP_CLOSE_RE);
  }
  else if (nextSepSeq == -1 || nextSepAlt < nextSepSeq) {
    // is an alternative group
    m_type=ALTERNATIVE;

    if (! m_parts.empty()) {
      // specific treatment if some parts already exist
      // (a b|c) -> a previous part, b current part, both of unknown type
      // that should be sequence
      AutomatonString subGroup;
      subGroup.setType(SEQUENCE);
      subGroup.copyParts(*this);
      if (nextSepAlt > begin) {
        subGroup.getParts().push_back(AutomatonString(str,gazeteers,subAutomatons,begin,nextSepAlt-begin));
      }
      m_parts.clear();
      m_parts.push_back(subGroup);
      return parseGroupAlternative(str,gazeteers,subAutomatons,nextSepAlt+1);
    }
    else {
      m_parts.push_back(AutomatonString(str,gazeteers,subAutomatons,begin,nextSepAlt-begin));
      return parseGroupAlternative(str,gazeteers,subAutomatons,nextSepAlt+1);
    }
  }
  else if (nextSepSeq != -1) {
    m_parts.push_back(AutomatonString(str,gazeteers,subAutomatons,begin,nextSepSeq-begin));
    return parseGroup(str,gazeteers,subAutomatons,nextSepSeq+1);
  }

  return begin;
}

// Set the prefix of identifier for each transition in the automaton
void AutomatonString::identifyTransition(const std::string& partId) {
  m_automId = partId;
}

//***********************************************************************
// split the string in parts, according to a given separator
// and add the parts in the expression (used for splitting basic sequences
// or basic alternatives)
//***********************************************************************
void AutomatonString::splitOnChar(const LimaString& str,
                                  const LimaChar c,
                                  const std::vector<Gazeteer>& gazeteers,
                                  const std::vector<SubAutomaton>& subAutomatons,
                                  const int begin,
                                  const int size) {
//   AUCLOGINIT;
//   LDEBUG << "AutomatonString::splitOnChar" << str << c << begin << size;
  int offset(begin);
  int offsetEnd;
  if (size == -1) {
    offsetEnd=str.size()-1;
  }
  else {
    offsetEnd=begin+size-1;
  }

  int offsetNextChar;


  // skip repeted elements
  while (str[offset] == c) { offset++; }

  // split on spaces
  offsetNextChar = findSpecialCharacter(str,c,offset);
  while ((offset < offsetEnd) && (offsetNextChar != -1)) {
    AutomatonString element;
    element.parse(str,gazeteers,subAutomatons,offset,offsetNextChar-offset);
    m_parts.push_back(element);
    offset=offsetNextChar+1;
    offsetNextChar = findSpecialCharacter(str,c,offset);
  }

  if (offset != -1 && offset<=offsetEnd) { // add the last element
    AutomatonString element;
    element.parse(str,gazeteers,subAutomatons,offset,offsetEnd-offset+1);
    m_parts.push_back(element);
    offset=offsetEnd+1;
  }
}

//***********************************************************************
// remove artificial sequences
//***********************************************************************
void AutomatonString::removeArtificialSequences(const bool inSubPart) {
  LIMA_UNUSED(inSubPart);
#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "removing artificial sequences from(inSubPart="<< inSubPart<< "): "
          << getString();
#endif
  switch (m_type) {
  case UNKNOWN_TYPE:
  case UNIT:
  case ALTERNATIVE: {
//     LDEBUG << "no sequence";
    return;
  }
  case SEQUENCE: {
    for (vector<AutomatonString>::iterator part=m_parts.begin();
         part != m_parts.end();) {
      if ((*part).isSequence()) {
        (*part).removeArtificialSequences(true);
#ifdef DEBUG_LP
         LDEBUG << "part without artificial sub-sequences : "
            << (*part).getString();
#endif
      }
      if ((*part).isArtificialSequence()) {
        if ((*part).hasModifiers() || (*part).hasConstraint()) {
          // sequence has modifiers -> if not unique element,
          // has been erroneously set as artificial
          (*part).setArtificialSequence(false);
          continue;
//           if ((*part).getParts().size() == 1) {
//             AutomatonString tmp((*part).getParts()[0]);
//             if ((*part).hasModifiers()) { tmp.copyModifiers(*part); }
//             if ((*part).hasConstraint()) { tmp.copyConstraints(*part); }
//             part=m_parts.insert(part,tmp);
//             part++; // insert returns iterator on inserted element
//             part=m_parts.erase(part);
//             part--; // erase returns iterator following the one erased
//           }
//           else { // keep sequence
//           (*part).setArtificialSequence(false);
//           }
        }
#ifdef DEBUG_LP
        LDEBUG << "sequence is artificial: " << (*part).getString();
#endif
        // insert elements one by one (otherwise, loose the iterator)
		AutomatonString str = *part;
		part = m_parts.erase(part);
		const std::vector<AutomatonString>& parts = str.getParts();
		for (std::vector<AutomatonString>::const_iterator it = parts.begin(); it != parts.end(); ++it) {
			part = m_parts.insert(part, *it);
			++part;
		}
      }
	  else {
		  ++part;
	  }
//       else if (inSubPart) {
//         LDEBUG << "sequence is NOT artificial: " << (*part).getString();
//       }
//       else {
//         LDEBUG << "sequence is no a subpart: " << (*part).getString();
//       }
    }
  }
  default: return;
  }
}

void AutomatonString::removeUnitSequences() {
#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "removeUnitSequences";
  LDEBUG << "removing unit sequences from " << getString();
#endif
  switch (m_type) {
  case UNKNOWN_TYPE:
  case UNIT: {
    break;
  }
  case ALTERNATIVE:
  case SEQUENCE: {
    if (m_parts.size()==1) {
      // only one part: remove one level in hierarchy

      // make a copy (otherwise, pb when reassigning m_parts)
      AutomatonString part=m_parts.front();

      // for properties, have to combine
      m_negative=m_negative ^ part.isNegative(); // xor for negative
      m_keep=m_keep && part.isKept(); // and for isKept
      // add constraints from part
      addConstraints(part.getConstraints());

      // get artificial sequence from part
      m_artificialSequence = part.isArtificialSequence();

      // for occurrences
      if (m_maxOccurrences == 0) { // no occurences
        m_minOccurrences=part.getMinOccurrences();
        m_maxOccurrences=part.getMaxOccurrences();
      }
      else if (part.getMaxOccurrences() != 0) {
        m_minOccurrences*=part.getMinOccurrences();
        if (m_maxOccurrences == INFINITE_OCC ||
            part.getMaxOccurrences() == INFINITE_OCC) {
          m_maxOccurrences=INFINITE_OCC;
        }
        else {
          m_maxOccurrences*=part.getMaxOccurrences();
        }
      }
      // otherwise not changed (no occurrences specified for part)

      // copy type, unit and parts
      m_type=part.getType();
      m_unit=part.getUnitString();
      m_parts=part.getParts();

      // reapply (descend in the hierarchy)
      removeUnitSequences();
    }
    else {
      for (vector<AutomatonString>::iterator part=m_parts.begin();
           part != m_parts.end(); part++) {
        (*part).removeUnitSequences();
      }
    }
  }
  default: return;
  }
}

//***********************************************************************
// parse the possible modifiers of the group or unit
// return the offset and the size of the string without the modifiers
//***********************************************************************
int AutomatonString::parseModifiersPre(const LimaString& s,
                                                const int begin) {
  if (s.isEmpty() || begin==0 || begin == -1) {
    return begin;
  }

  int previous(begin);

  if (s[previous-1] == CHAR_NEGATIVE_RE) {
    m_negative = true;
    previous--;
  }

  if (previous>0 && s[previous-1] == CHAR_NOKEEP_OPEN_RE) {
    m_keep=false;
    previous--;
  }

  return previous;
}

int AutomatonString::parseModifiersPost(const LimaString& s,
                                                 const int begin) {
  if (s.isEmpty() || begin >= s.size()-1) {
    return begin;
  }

  int end(begin);

#ifdef DEBUG_LP
  AUCLOGINIT;
#endif
  if (s[begin+1] == CHAR_OPTIONAL_RE) {
    m_minOccurrences = 0;
    m_maxOccurrences = 1;
    end++;
  }
  else if (s[begin+1] == CHAR_CARDINALITY_OPEN_RE) {
    int offsetOpen=begin+1;
    int offsetClose=findSpecialCharacter(s,CHAR_CARDINALITY_CLOSE_RE,
                                                  begin);
    int offsetMinus=findSpecialCharacter(s,CHAR_CARDINALITY_UNTIL_RE,
                                                   begin);
#ifdef DEBUG_LP
    LDEBUG << "AutomatonString::parseModifiersPost offsets" << offsetOpen << offsetMinus << offsetClose << s.mid(offsetOpen+1,offsetMinus-offsetOpen-1);
#endif
    m_minOccurrences=s.midRef(offsetOpen+1,offsetMinus-offsetOpen-1).toInt();

    // max occurrences can be infinite
    LimaString maxString=s.mid(offsetMinus+1,
                              offsetClose-offsetMinus-1);

    if (maxString == *STRING_INFINITY_LC ||
        maxString == *STRING_INFINITY_UC) {
      m_maxOccurrences=INFINITE_OCC;
    }
    else {
      m_maxOccurrences=maxString.toInt();
    }
    end=offsetClose;
  }


  if (end+1<s.size() && s[end+1] == CHAR_NOKEEP_CLOSE_RE) {
    // keep has already been set on pre modifier
    end++;
  }
#ifdef DEBUG_LP
  LDEBUG << "AutomatonString::parseModifiersPost got" << m_minOccurrences << m_maxOccurrences << end ;
#endif

  return end;
}

bool AutomatonString::parseModifiers(const LimaString& s,
                                     int& begin,
                                     int& size) {

  if (s.isEmpty() || begin >= s.size()-1 || size==0) {
    return false;
  }

  bool foundModifiers(false);

  if (s[begin] == CHAR_NOKEEP_OPEN_RE) {
    // the unit is not kept in recognized expressions
    if (s[s.length()-1] != CHAR_NOKEEP_CLOSE_RE) {
      throw ExpressionSyntaxException("got confused while reading expression "
                      +Misc::limastring2utf8stdstring(s));
    }
    m_keep=false;
    begin++;
    size-=2;
    foundModifiers=true;
  }

  if (s[begin] == CHAR_NEGATIVE_RE) {
    m_negative = true;
    begin++;
    foundModifiers=true;
  }

  int last=begin+size-1;

  // optional
  if (rfindSpecialCharacter(s,CHAR_OPTIONAL_RE,last) == last) {
    m_minOccurrences = 0;
    m_maxOccurrences = 1;
    size--;
    foundModifiers=true;
  }
  // min and max occurrences
  else if (rfindSpecialCharacter(s,CHAR_CARDINALITY_CLOSE_RE,last) == last) {
    int offsetOpen=rfindSpecialCharacter(s,CHAR_CARDINALITY_OPEN_RE,
                                                  last);
    int offsetClose=last;
    int offsetMinus=rfindSpecialCharacter(s,CHAR_CARDINALITY_UNTIL_RE,
                                                   last);
    m_minOccurrences=s.midRef(offsetOpen+1, offsetMinus-offsetOpen-1).toInt();

    // max occurrences can be infinite
    LimaString maxString=s.mid(offsetMinus+1,
                                      offsetClose-offsetMinus-1);

    if (maxString == *STRING_INFINITY_LC ||
        maxString == *STRING_INFINITY_UC) {
      m_maxOccurrences=INFINITE_OCC;
    }
    else {
      m_maxOccurrences=maxString.toInt();
    }
    size=offsetOpen-begin;
    foundModifiers=true;
  }


  return foundModifiers;
}

//***********************************************************************
// parse a unit string
//***********************************************************************
void AutomatonString::parseUnit(const LimaString& str,
                                const std::vector<Gazeteer>& gazeteers,
                                const std::vector<SubAutomaton>& subAutomatons,
                                const int begin,
                                const int size) {

#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "AutomatonString: parsing unit" << str << begin << size;
#endif

  int newBegin(begin);
  int newSize(size);

  if (newSize == -1) { // if not specified, until the end
    newSize=str.size()-begin;
  }

  parseModifiers(str,newBegin,newSize);

  if (str[newBegin] == CHAR_BEGIN_NAMEGAZ) {
    //copy automaton string of the corresponding gazeteer
    vector<Gazeteer>::const_iterator it=
      find(gazeteers.begin(),gazeteers.end(),str.mid(newBegin+1,newSize-1));
    if (it==gazeteers.end()) {
      ostringstream oss;
      oss << "unknown class " << Common::Misc::limastring2utf8stdstring(str.mid(newBegin+1,newSize-1));
      throw AutomatonCompilerException(oss.str());
    }
    const Gazeteer& gazeteer = *it;
    //if( !gazeteer.hasMultiTermWord() && gazeteer.hasNoCategoryNorTstatus() ) {
    if( gazeteer.hasNotOnlyWords() ) {
#ifdef DEBUG_LP
      LDEBUG << "AutomatonString: set type(SIMPLE_GAZETEER)";
#endif
      setType(SIMPLE_GAZETEER);
      // m_parts is empty!;
      // m_unit=gazeteer.getName();
      m_unit=str.mid(newBegin,newSize);
    }
    else {
      // copy only type, parts and unit (other are set by modifiers)
#ifdef DEBUG_LP
      LDEBUG << "AutomatonString: set type(" << (*it).getAutomatonString().getType() << ")";
#endif
      setType((*it).getAutomatonString().getType());
      m_parts=(*it).getAutomatonString().getParts();
      m_unit=(*it).getAutomatonString().getUnitString();
    }
  }
  else if (str[newBegin] == CHAR_BEGIN_NAMESUB) {
#ifdef DEBUG_LP
    LDEBUG << "AutomatonString: searching subautomaton" << str.mid(newBegin+1,newSize-1) << "in" << subAutomatons.size() << "subautomatons";
#endif
    // copy automaton string of the corresponding subAutomaton
    vector<SubAutomaton>::const_iterator it= subAutomatons.begin();
    for(;it!=subAutomatons.end();it++)
    {
#ifdef DEBUG_LP
      LDEBUG << "AutomatonString: one subAutomaton named:" << (*it).getName();
#endif
      if ((*it).getName() == str.mid(newBegin+1,newSize-1)) break;

    }
    if (it==subAutomatons.end()) {
      AUCLOGINIT;
      LERROR << "unknown class " << str.mid(newBegin+1,newSize-1);
      ostringstream oss;
      oss << "unknown class " << Common::Misc::limastring2utf8stdstring(str.mid(newBegin+1,newSize-1));
      throw AutomatonCompilerException(oss.str());
    }
    // copy only type, parts and unit (other are set by modifiers)
    setType((*it).getAutomatonString().getType());
    m_parts=(*it).getAutomatonString().getParts();
    m_unit=(*it).getAutomatonString().getUnitString();
  }
  else {
    setType(UNIT);
    m_unit=str.mid(newBegin,newSize);
  }
}


//***********************************************************************
// get a modifier string
//***********************************************************************
LimaString AutomatonString::applyModifiers(const LimaString& s) const {
  LimaString result;
  if (! m_keep)   { result+=*OUTPUT_NOKEEP_BEGIN; }
  if (m_negative) { result+=*OUTPUT_NEGATIVE; }
  result+=s;

  // also constraints
  for (uint64_t i(0); i<m_constraints.size(); i++) {
    result+=m_constraints[i].str();
  }

  if (m_maxOccurrences != 0) {
    QString s; QTextStream oss(&s);
    oss << CHAR_CARDINALITY_OPEN_RE << m_minOccurrences
        << CHAR_CARDINALITY_UNTIL_RE;
    if (m_maxOccurrences==INFINITE_OCC) {
      oss << STRING_INFINITY_LC;
    }
    else {
      oss << m_maxOccurrences;
    }
    oss << CHAR_CARDINALITY_CLOSE_RE;
    result+=oss.readAll();
  }
  if (! m_keep)   { result+=*OUTPUT_NOKEEP_END; }

  return result;
}

//***********************************************************************
// test if one part is nokeep [..]
//***********************************************************************
bool AutomatonString::hasNoKeepInParts() const {
  if (isUnit()) { return false; }
  else {
    for (uint64_t i(0); i<m_parts.size(); i++) {
      if (! m_parts[i].isKept()) {
        return true;
      }
    }
  }
  return false;
}

//***********************************************************************
// get the string of the element
//***********************************************************************
LimaString AutomatonString::getString() const {
//   AUCLOGINIT;
  switch(m_type) {
  case UNIT: {
    return applyModifiers(m_unit);
  }
  case SIMPLE_GAZETEER: {
    return applyModifiers(m_unit);
  }
  case SEQUENCE: {
    LimaString str;
    if (m_parts.size()) {
      str=m_parts[0].getString();
      for (uint64_t i(1); i<m_parts.size(); i++) {
        str+=CHAR_SEP_RE+m_parts[i].getString();
      }
    }
    if (hasNoKeepInParts()) {
      return applyModifiers(str);
    }
    else {
      return applyModifiers(CHAR_GROUP_OPEN_RE+str+CHAR_GROUP_CLOSE_RE);
    }
  }
  case ALTERNATIVE: {
    LimaString str;
    if (m_parts.size()) {
      str=m_parts[0].getString();
      for (uint64_t i(1); i<m_parts.size(); i++) {
        str+=CHAR_CHOICESEP_RE+m_parts[i].getString();
      }
    }
    return applyModifiers(CHAR_GROUP_OPEN_RE+str+CHAR_GROUP_CLOSE_RE);
  }
  case UNKNOWN_TYPE: {
    return LimaString();
  }
  }
  return LimaString();
}

// for debug
LimaString AutomatonString::getModifier() const {
  QString s; QTextStream oss(&s);
  if (m_maxOccurrences != 0) {
    oss << CHAR_CARDINALITY_OPEN_RE << m_minOccurrences
        << CHAR_CARDINALITY_UNTIL_RE;
    if (m_maxOccurrences==INFINITE_OCC) {
      oss << STRING_INFINITY_LC;
    }
    else {
      oss << m_maxOccurrences;
    }
    oss << CHAR_CARDINALITY_CLOSE_RE;
  }
  if (m_negative) {
    oss << "_NEGATIVE";
  }
  if (! m_keep) {
    oss << "_NOKEEP";
  }
  return oss.readAll();
}

LimaString AutomatonString::getStringDebug() const {
  switch(m_type) {
  case UNIT: {
    return *OUTPUT_QUOTE+m_unit+*OUTPUT_QUOTE+getModifier();
  }
  case SEQUENCE: {
    LimaString str;
    if (m_parts.size()) {
      str=m_parts[0].getStringDebug();
      for (std::size_t i(1); i<m_parts.size(); i++) {
        str+=CHAR_SEP_RE+m_parts[i].getStringDebug();
      }
    }
    if (isArtificialSequence()) {
      return *STRING_OPEN_DUMMY_SEQUENCE+str+*STRING_CLOSE_DUMMY_SEQUENCE+getModifier();
    }
    else {
      return CHAR_GROUP_OPEN_RE+str+CHAR_GROUP_CLOSE_RE+getModifier();
    }
  }
  case ALTERNATIVE: {
    LimaString str;
    if (m_parts.size()) {
      str=m_parts[0].getStringDebug();
      for (std::size_t i(1); i<m_parts.size(); i++) {
        str+=*OUTPUT_OR+m_parts[i].getStringDebug();
      }
    }
    return CHAR_GROUP_OPEN_RE+str+CHAR_GROUP_CLOSE_RE+getModifier();
  }
  case UNKNOWN_TYPE: {
    return *OUTPUT_NONE;
  }
  default: {
    return LimaString();
  }
  }
  return LimaString();
}

//***********************************************************************
// find an subpart of a group element
//***********************************************************************
AutomatonString& AutomatonString::findSubPart(std::vector <Lima::LinguisticProcessing::Automaton::AutomatonString >::size_type index) {
#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "findSubPart " << index << " in " << *this;
#endif

  switch (m_type) {
  case UNKNOWN_TYPE:
    throw ConstraintSyntaxException("trying to apply a constraint on empty automaton");
  case UNIT:
  case ALTERNATIVE: {
    if (index != 0) {
      throw ConstraintSyntaxException("trying to get a subpart in a unit element");
    }
    return *this;
  }
  case SEQUENCE:
    // is a sequence group
//     if (m_parts.size() == 1) {
//       //only one part -> descend
//       return m_parts[0].findSubPart(index);
//     }
//     else
    if (index > m_parts.size()-1) {
      ostringstream oss;
      oss << "subpart index " << index << " out of group ("
          << m_parts.size() <<" parts identified)";
      throw ConstraintSyntaxException(oss.str());
    }
    else {
      return m_parts[index];
    }
  default:
    throw ConstraintSyntaxException("unsupported subpart type");
  }
}


const AutomatonString& AutomatonString::findSubPart(std::vector <Lima::LinguisticProcessing::Automaton::AutomatonString >::size_type index) const {

  switch (m_type) {
  case UNKNOWN_TYPE:
    throw ConstraintSyntaxException("trying to apply a constraint on empty automaton");
  case UNIT:
  case ALTERNATIVE: {
    if (index != 0) {
      throw ConstraintSyntaxException("trying to get a subpart in a unit element");
    }
    return *this;
  }
  case SEQUENCE:
    // is a sequence group
//     if (m_parts.size() == 1) {
//       //only one part -> descend
//       return m_parts[0].findSubPart(index);
//     }
//     else
    if (index > m_parts.size()-1) {
      ostringstream oss;
      oss << "subpart index " << index << " out of group ("
          << m_parts.size() <<" parts identified)";
      throw ConstraintSyntaxException(oss.str());
    }
    else {
      return m_parts[index];
    }
  default:
    throw ConstraintSyntaxException("unsupported subpart type");
  }
}

//***********************************************************************
// propagate constraints no groups to its elements
//***********************************************************************
void AutomatonString::propagateConstraints() {
  if (isUnit()) {
    return;
  }
  else if (isAlternative() && hasConstraint()) {
    for (vector<AutomatonString>::iterator part=m_parts.begin();
         part!=m_parts.end(); part++) {
      (*part).addConstraints(m_constraints);
      (*part).propagateConstraints();
    }
    m_constraints.clear();
  }
  else if (isSequence()) {
    if (m_parts.size() == 1) {
      // only one element, propagate constraints to it
      if (hasConstraint()) {
        m_parts.front().addConstraints(m_constraints);
        m_parts.front().propagateConstraints();
        m_constraints.clear();
      }
      else {
        m_parts.front().propagateConstraints();
      }
    }
    else {
      for (vector<AutomatonString>::iterator part=m_parts.begin();
           part!=m_parts.end(); part++) {
        (*part).propagateConstraints();
      }
    }
  }
}

void AutomatonString::propagateProperties() {
  if (isUnit()) {
    return;
  }
  else if (isAlternative() || isSequence()) {
    for (vector<AutomatonString>::iterator part=m_parts.begin();
         part!=m_parts.end(); part++) {
      if (! m_keep) {
        (*part).setKeep(false);
      }
      (*part).propagateProperties();
    }
  }
}

//***********************************************************************
// test the existence of a constraint on a part
//***********************************************************************
bool AutomatonString::
existsConstraint(const SubPartIndex* index,
                 const std::string& constraintName,
                 const ConstraintAction& constraintAction,
                 int& c) const {

#ifdef DEBUG_LP
  AUCLOGINIT;
  if (index != 0) {
    LDEBUG << "testing exists constraint "
           << constraintName << " in "
           << *index << " of " << *this;
  }
  else {
    LDEBUG << "testing exists constraint "
           << constraintName << " on " << *this;
  }
#endif

  if (index==0 || index->empty()) {
    // no more subindexes: apply on this
    return existsConstraint(constraintName,constraintAction,c);
  }

  switch (index->getPartIndex().first) {
  case SUB_NONE:
    return findSubPart(index->getPartIndex().second-1).
      existsConstraint(index->getSubPartIndex(),constraintName,
                       constraintAction,c);
  case SUB_FIRST:
    if (isUnit()) {
#ifdef DEBUG_LP
      LDEBUG << "return false";
#endif
      return false; // will be splitted
    }
    else if (m_isSplittedFirst) {
      // already splitted: first is first part
      return findSubPart(0).existsConstraint(index->getSubPartIndex(),
                                             constraintName,constraintAction,c);
    }
    else {
      ostringstream oss;
      oss << "cannot handle FIRST in this context";
      throw ConstraintSyntaxException(oss.str());
    }
    break;
  case SUB_CURRENT:
  case SUB_NEXT:
    if (isUnit()) {
#ifdef DEBUG_LP
      LDEBUG << "return false";
#endif
      return false; // will be splitted
    }
    else if (m_isSplittedFirst) {
      // current part is second
      if (findSubPart(1).isSplittedLast()) {
        return findSubPart(1).
          existsConstraint(index,constraintName,
                           constraintAction,c);
      }
      else {
        return findSubPart(1).
          existsConstraint(index->getSubPartIndex(),
                           constraintName,constraintAction,c);
      }
    }
    else if (m_isSplittedLast) {
      // current part is first
      return findSubPart(0).existsConstraint(index->getSubPartIndex(),
                                             constraintName,constraintAction,c);
    }
    else {
      ostringstream oss;
      oss << "cannot handle CURRENT or NEXT in this context";
      throw ConstraintSyntaxException(oss.str());
    }
    break;
  case SUB_LAST:
    if (isUnit()) {
#ifdef DEBUG_LP
      LDEBUG << "return false";
#endif
      return false;  // will be splitted
    }
    else if (m_isSplittedFirst) {
      // last part is second
      if (findSubPart(1).isSplittedLast()) {
        return findSubPart(1).
          existsConstraint(index,constraintName,
                           constraintAction,c);
      }
      else {
        return findSubPart(1).
          existsConstraint(index->getSubPartIndex(),
                           constraintName,constraintAction,c);
      }
    }
    else if (m_isSplittedLast) {
      // last part is second
      return findSubPart(1).existsConstraint(index->getSubPartIndex(),
                                             constraintName,constraintAction,c);
    }
    else {
      ostringstream oss;
      oss << "cannot handle LAST in this context";
      throw ConstraintSyntaxException(oss.str());
    }
    break;
  }
  return false;
}

bool AutomatonString::
existsConstraint(const std::string& constraintName,
                 const ConstraintAction& constraintAction,
                 int& c) const
{
#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "testing exists constraint "
         << constraintName << " on " << *this;
#endif
  if (m_constraints.empty()) {
    c=0;
#ifdef DEBUG_LP
    LDEBUG << "=> false: no constraint";
#endif
    return false;
  }
  else {
    std::vector<Constraint>::const_iterator
      constraint=m_constraints.begin(),
      constraint_end=m_constraints.end();
    for (; constraint!=constraint_end; constraint++) {
      if ((*constraint).functionName()==constraintName &&
          (*constraint).action()==constraintAction) {
        c=(*constraint).index();
#ifdef DEBUG_LP
        LDEBUG << "=> true: " << c;
#endif
        return true;
      }
    }
#ifdef DEBUG_LP
    LDEBUG << "=> false: last constraint is "<< m_constraints.back();
#endif
    return false;
  }
}

bool AutomatonString::existsConstraint(std::vector <Lima::LinguisticProcessing::Automaton::AutomatonString >::size_type index,
                                       const LimaString& subindex,
                                       int& c) const {

  return findSubPart(index).existsConstraint(subindex,c);
}

bool AutomatonString::existsConstraint(const LimaString& subindex,
                                        int& c) const {

  c=0;

  if (subindex == *STRING_CONSTRAINT_GROUP_FIRST) {
    if (isUnit() || m_parts.size() < 1) {
      return false;
    }
    return m_parts[0].existsConstraint(c);
  }
  else if (subindex == *STRING_CONSTRAINT_GROUP_CURRENT ||
           subindex == *STRING_CONSTRAINT_GROUP_NEXT) {
    if (isUnit() || m_parts.size() < 2) {
      return false;
    }
    return (m_parts[0].existsConstraint(c) ||
            m_parts[1].existsConstraint(c) ||
            m_parts[2].existsConstraint(c));
  }
  else if (subindex == *STRING_CONSTRAINT_GROUP_LAST) {
    if (isUnit() || m_parts.size() < 3) {
      return false;
    }
    return m_parts[2].existsConstraint(c);
  }
  else {
    int i=LimaStringToInt(subindex);
    if (i==0) {
      ostringstream oss;
      oss << "Error on constraint: subindex \""
          << Misc::limastring2utf8stdstring(subindex) << "\" not found";
      throw ConstraintSyntaxException(oss.str());
    }
    return m_parts[i].existsConstraint(c);
  }
}

//***********************************************************************
// insert a constraint in the string
//***********************************************************************
void AutomatonString::insertConstraintInUnit(const Constraint& c) {
  m_constraints.push_back(c);
}

//***********************************************************************
// insert a constraint
//***********************************************************************
void AutomatonString::insertConstraint(const SubPartIndex* index,
                                       Constraint& constraint,
                                       const std::string& depth) {
#ifdef DEBUG_LP
  AUCLOGINIT;
  if (index==0) {
    LDEBUG << depth << "insertConstraint " << constraint
           << " in " << *this;
  }
  else {
    LDEBUG << depth << "insertConstraint " << constraint << " in part "
           << *index << " of " << *this ;
  }
#endif

  if (index==0 || index->empty()) {
    // no more subindexes: apply on this
    insertConstraintInUnit(constraint);
    return;
  }
  else {
    // in case of relative indexes, change store/compare to use stack
#ifdef DEBUG_LP
    LDEBUG << depth << "AutomatonString::insertConstraint index->getPartIndex().first: " << index->getPartIndex().first;
#endif
    switch (index->getPartIndex().first) {
    case SUB_NONE: {
#ifdef DEBUG_LP
      LDEBUG << depth << "SUB_NONE " << m_isSplittedFirst << " " << m_isSplittedLast;
#endif
      // no relative index
      // -1 to match vector indexes

      // GC: bug correction: when using in the same constraint X.first and then x.last, the first
      // search of "first" splits the current part in two parts: the first one and the remaining.
      // Then, the index of the element for "last" is increased by 1. And yes, the code continued
      // to use the same value. Thus, the "last" tried to be applied on the first part of the split
      // result which was no more a repeated element => crash (well, exception)
      //       findSubPart(index->getPartIndex().second - 1).
      uint64_t subpartIndex = m_isSplittedFirst ? index->getPartIndex().second : (index->getPartIndex().second - 1);
      findSubPart(subpartIndex).
        insertConstraint(index->getSubPartIndex(),constraint, depth+"  ");
      break;
    }
    case SUB_FIRST: {
#ifdef DEBUG_LP
      LDEBUG << depth << "SUB_FIRST " << m_isSplittedFirst << " " << m_isSplittedLast;
#endif
      // the element must be composed
      // if it is not, split
      if (isOptional()) {
        // GC: bug correction: if it has already been splitted-last, then we have to work on the
        // first part
        if (m_isSplittedLast)
        {
          m_parts[0].insertConstraint(index,constraint, depth+"  ");
        }
        else
        {
          split(true);
          findSubPart(0).
          insertConstraint(index->getSubPartIndex(),constraint, depth+"  ");
        }
      }
      else if (m_isSplittedFirst) {
        // already splitted: first is first part
        findSubPart(0).
        insertConstraint(index->getSubPartIndex(),constraint, depth+"  ");
      }
      else if (m_isSplittedLast) {
        m_parts[0].insertConstraint(index,constraint, depth+"  ");
      }
      else {
        // cannot be splittedLast (first always before last)
        ostringstream oss;
        oss << depth << "cannot handle FIRST in this context";
        throw ConstraintSyntaxException(oss.str());
      }
      break;
    }
    case SUB_CURRENT:
    case SUB_NEXT: {
#ifdef DEBUG_LP
      LDEBUG << depth << "SUB_CURRENT/SUB_NEXT " << m_isSplittedFirst << " " << m_isSplittedLast << " " << isUnit();
#endif
      if (isUnit()) {
        insertConstraint(index->getSubPartIndex(),constraint, depth+"  ");
      }
      else if (m_isSplittedFirst) {
        // current part is second
        findSubPart(1).insertConstraint(index,constraint, depth+"  ");
      }
      else if (m_isSplittedLast) {
        // current part is first
        findSubPart(1).insertConstraint(index->getSubPartIndex(),constraint, depth+"  ");
      }
      // GC: bug correction: was not able to handle current in constraints not using first or
      // last before
      else if (isSequence()) {
        split(true);
        m_parts[1].insertConstraint(index->getSubPartIndex(),constraint, depth+"  ");
      }
      else {
        ostringstream oss;
        oss << depth << "cannot handle CURRENT or NEXT in this context";
        throw ConstraintSyntaxException(oss.str());
      }
      break;
    }
    case SUB_LAST: {
#ifdef DEBUG_LP
      LDEBUG << depth << "SUB_LAST " << m_isSplittedFirst << " " << m_isSplittedLast;
#endif
      if (isOptional()) {
        split(false);
        findSubPart(1).
        insertConstraint(index->getSubPartIndex(),constraint, depth+"  ");
      }
      else if (m_isSplittedFirst) {
        // already splitted for first
        // insert in second part
        // (may need another split or not)
        findSubPart(1).
        insertConstraint(index,constraint, depth+"  ");
      }
      else if (m_isSplittedLast) {
        // already splitted for last
        // last is second part
        findSubPart(1).
        insertConstraint(index->getSubPartIndex(),constraint, depth+"  ");
      }
      else {
#ifdef DEBUG_LP
        LDEBUG << depth << "insertConstraint " << constraint << " in part "
        << *index << " of " << *this << ": SUB_LAST, " << isOptional() << ", " << m_isSplittedFirst << ", " << m_isSplittedLast << ", cannot handle LAST in this context" ;
#endif
        ostringstream oss;
        oss << "cannot handle LAST in this context";
        throw ConstraintSyntaxException(oss.str());
      }
      break;
    }
    }
  }
}

void AutomatonString::insertConstraint(std::vector <Lima::LinguisticProcessing::Automaton::AutomatonString >::size_type index,
                                       const LimaString& subindex,
                                       const Constraint& constraint) {

  findSubPart(index).insertConstraint(subindex,constraint);
}

void AutomatonString::insertConstraint(const LimaString& subindex,
                                       const Constraint& constraint) {
#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "AutomatonString::insertConstraint '" << subindex << "',  '" << constraint << "'";
#endif
  if (subindex.isEmpty()) {
    insertConstraintInUnit(constraint);
    return;
  }

  // there is a subindex : the element must be composed
  // if it is not, split

  if (isUnit()) {
    split();
  }

  if (subindex == *STRING_CONSTRAINT_GROUP_FIRST) {
    insertConstraintInPart(1,constraint);
  }
  else if (subindex == *STRING_CONSTRAINT_GROUP_CURRENT) {
    insertConstraintInPart(2,constraint);
  }
  else if (subindex == *STRING_CONSTRAINT_GROUP_NEXT) {
    insertConstraintInPart(2,constraint);
  }
  else if (subindex == *STRING_CONSTRAINT_GROUP_LAST) {
    insertConstraintInPart(m_parts.size(),constraint);
  }
  else {
    int i=subindex.toUInt();
    if (i==0) {
      ostringstream oss;
      string str= Common::Misc::limastring2utf8stdstring(subindex);
      oss << "Error on constraint: subindex \""
          << str << "\" not found";
      throw ConstraintSyntaxException(oss.str());
    }
    insertConstraintInPart(i,constraint);
  }
}

//***********************************************************************
// split a repetitive structure into elements to distribute constraints
//***********************************************************************

// utility function to get the cardinality string
template<typename CardMaxType>
LimaString stringCardinality(const int cardmin,
                              const CardMaxType cardmax) {
  QString s; QTextStream oss(&s);
  oss << CHAR_CARDINALITY_OPEN_RE << cardmin
      << CHAR_CARDINALITY_UNTIL_RE;
  if (cardmax==AutomatonString::INFINITE_OCC) {
    oss << STRING_INFINITY_LC;
  }
  else {
    oss << cardmax;
  }
  oss << CHAR_CARDINALITY_CLOSE_RE;
  return oss.readAll();
}

void AutomatonString::split() {
#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "AutomatonString::split() splitting automaton " << *this;
#endif

  if (m_maxOccurrences != -1 && m_maxOccurrences < 3) {
#ifdef DEBUG_LP
    LDEBUG << "m_maxOccurrences: " << m_maxOccurrences;
#endif
    throw ConstraintSyntaxException("cannot split element "+
                                    Misc::limastring2utf8stdstring(getString()));
  }

  AutomatonString part1(*this);
  AutomatonString part2(*this);
  AutomatonString part3(*this);

  int cardmin(m_minOccurrences);
  int cardmax(m_maxOccurrences);

  if (m_minOccurrences == 0) {
    // first is optional
    part1.setOccurrences(0,1);
  }
  else {
    // no modifier on first
    part1.setOccurrences(0,0);
    cardmin--;
  }

  if (cardmax == INFINITE_OCC) {
    part2.setOccurrences(cardmin,INFINITE_OCC);
    part3.setOccurrences(0,1);
  }
  else {
    cardmax-=2;
    part2.setOccurrences(cardmin,cardmax);
    part3.setOccurrences(0,1);
  }

  m_unit=LimaString();
  m_type=SEQUENCE;
  setOccurrences(0,0); // no more modifier on group
  m_parts.push_back(part1);
  m_parts.push_back(part2);
  m_parts.push_back(part3);


#ifdef DEBUG_LP
  LDEBUG << "=> " << *this;
#endif
}

// split(first=true) : split to get first element :
// transform b{i,j} into (b b{i-1,j-1})
// ---
// split(first=false) : split to get last element :
// transform b{i,j} into (b{i-1,j-1} b)
//
// if i==0, add optionality on sequence built

void AutomatonString::split(const bool first)
{
#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "AutomatonString::split(bool) splitting automaton (first=" << first
         << ") " << *this;
#endif

  if (m_maxOccurrences != -1 && m_maxOccurrences < 2) {
#ifdef DEBUG_LP
    LDEBUG << "AutomatonString::split m_maxOccurrences: " << m_maxOccurrences;
#endif
    throw ConstraintSyntaxException("cannot split element "+
                                    Misc::limastring2utf8stdstring(getString()));
  }

  AutomatonString part1(*this);
  AutomatonString part2(*this);

  int cardmin(m_minOccurrences);
  int cardmax(m_maxOccurrences);

  bool optional(false);
  if (m_minOccurrences == 0) {
    // sequence will be optional
    optional=true;
  }
  else {
    cardmin--;
  }

  if (cardmax != INFINITE_OCC) {
    cardmax--;
  }

  if (first) {
    part1.setOccurrences(0,0);
    part2.setOccurrences(cardmin,cardmax);
    m_isSplittedFirst=true;
  }
  else {
    part1.setOccurrences(cardmin,cardmax);
    part2.setOccurrences(0,0);
    m_isSplittedLast=true;
  }

  m_unit=LimaString();
  m_type=SEQUENCE;
  if (optional) {
    setOccurrences(0,1);
  }
  else { // no more occurrences on sequence
    setOccurrences(0,0);
  }
  m_parts.clear(); // if is not unit
  m_parts.push_back(part1);
  m_parts.push_back(part2);

#ifdef DEBUG_LP
  LDEBUG << "=> " << *this;
#endif
}

//***********************************************************************
// insert a constraint in a sub element
//***********************************************************************
// test if the element is composed or not : if it is composed, try to
// find the corresponding sub-element, otherwise DO NOT SPLIT but try
// to find the sub-element inside the string, based on CHAR_SEP_RE
void AutomatonString::insertConstraintInPart(std::vector <Lima::LinguisticProcessing::Automaton::AutomatonString >::size_type index,
                                             const Constraint& c) {

  if (! isUnit()) {

    if (index-1 >= m_parts.size()) {
      ostringstream oss;
      oss << "Error on constraint " << Misc::limastring2utf8stdstring(c.str())
          << ": cannot find subpart " << index << " in part "
          << Misc::limastring2utf8stdstring(getString());
      throw ConstraintSyntaxException(oss.str());
    }
    m_parts[index-1].insertConstraintInUnit(c);
  }
  else {
      ostringstream oss;
      oss << "Error on constraint " << Misc::limastring2utf8stdstring(c.str())
          << ": element has no subparts :" << Misc::limastring2utf8stdstring(getString());
      throw ConstraintSyntaxException(oss.str());
  }
}

//***********************************************************************
// output
//***********************************************************************
ostream& operator << (ostream& os, const AutomatonString& r)
{
  os << Common::Misc::limastring2utf8stdstring(r.getString());
  return os;
}
QDebug& operator << (QDebug& os, const AutomatonString& r)
{
  os << r.getString();
  return os;
}

} // end namespace
} // end namespace
} // end namespace
