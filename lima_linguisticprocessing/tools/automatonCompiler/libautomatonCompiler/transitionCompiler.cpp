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
* File        : transitionCompiler.cpp
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Tue Apr  8 2003
* Copyright   : (c) 2003 by CEA
*
************************************************************************/

#include "transitionCompiler.h"
#include "ruleFormat.h"
#include "gazeteer.h"
#include "tstring.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/TStatus.h"
#include "linguisticProcessing/core/Automaton/numericTransition.h"
#include "linguisticProcessing/core/Automaton/wordTransition.h"
#include "linguisticProcessing/core/Automaton/posTransition.h"
#include "linguisticProcessing/core/Automaton/lemmaTransition.h"
#include "linguisticProcessing/core/Automaton/gazeteerTransition.h"
#include "linguisticProcessing/core/Automaton/starTransition.h"
#include "linguisticProcessing/core/Automaton/entityTransition.h"
#include "linguisticProcessing/core/Automaton/entityGroupTransition.h"
#include "linguisticProcessing/core/Automaton/tstatusTransition.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/PropertyCode/PropertyManager.h"


using namespace std;
using namespace Lima::Common;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {
namespace AutomatonCompiler {

#define AUCLOGINIT LOGINIT("Automaton::Compiler")

//************************************************************************
// declaration of some private functions
//************************************************************************
Tpos createTpos(const std::string& s, MediaId language);
TStatusTransition* createStructuredTStatusTransition(const LimaString& str,
                                                     const int begin);
TStatusTransition* createDefaultTStatusTransition(const LimaString& str,
                                                  const int begin);
                                                  
//************************************************************************
// creates a transition from a entry string, depending on the syntax
// defined in the previous macros
//
// TODO: unify different Factory for Transition ...
TransitionUnit*
  createGazeteerTransition(const LimaString& gazeteerName,
                 MediaId language, const std::string& id,
                 const std::vector<LimaString>& activeEntityGroups,
                 const vector<Gazeteer>& gazeteers,
                 const bool keep,
                 const bool head)
{
  LIMA_UNUSED(language)
  LIMA_UNUSED(activeEntityGroups)
  std::size_t gazeteerIndex;
  for (gazeteerIndex=0; gazeteerIndex<gazeteers.size(); gazeteerIndex++) {
    if (gazeteers[gazeteerIndex].alias() == gazeteerName) {
      break;
    }
  }
  if ( gazeteerIndex >= gazeteers.size() || gazeteers[gazeteerIndex].size() == 0 ) {
    AUCLOGINIT;
    string str=Misc::limastring2utf8stdstring(gazeteerName);
    if (gazeteerIndex<gazeteers.size()) {
      LERROR << "empty class as trigger [" << str << "]";
    }
    else {
     LERROR << "Unrecognized class as trigger [" << str << "]";
    }
    return 0;
  }
  const Gazeteer& gazeteer = gazeteers[gazeteerIndex];
  const std::vector<LimaString>& gazeteerAsVectorOfString = gazeteer;
  // TODO bool negative = automatonString.isNegative()??, Est-ce qu'on autorise un trigger avec une négation?
  bool negative(false);
  TransitionUnit* t = new GazeteerTransition(gazeteerAsVectorOfString, gazeteerName, keep); 
  t->setNegative(negative);
  t->setHead(head);
  t->setId(id);
  return t;
}

TransitionUnit*
createTransition(const AutomatonString& automatonString,
                 MediaId language, const std::string& id,
                 const std::vector<LimaString>& activeEntityGroups)
{
  return createTransition(automatonString.getUnitString(),
                          language, id,
                          activeEntityGroups,
                          automatonString.isKept(),
                          automatonString.isNegative(),
                          automatonString.getConstraints());
}

TransitionUnit* createTransition(const LimaString str,
                                 MediaId language, const std::string& id,
                                 const std::vector<LimaString>& activeEntityGroups,
                                 const bool keep,
                                 const bool neg,
                                 const std::vector<Constraint>& constraints,
                                 const std::vector<LimaString>& gazeteerAsVectorOfString
                                 ) 
{
  LIMA_UNUSED(gazeteerAsVectorOfString)
#ifdef DEBUG_LP
  AUCLOGINIT;
#endif

  TransitionUnit* t(0);
  bool keepIt(keep);
  bool negative(neg);
  bool head(false);
  LimaString s(str);
  const PropertyCode::PropertyAccessor* macroAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MACRO"));
  const PropertyCode::PropertyAccessor* microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));
  FsaStringsPool& sp=Common::MediaticData::MediaticData::changeable().stringsPool(language);

#ifdef DEBUG_LP
  LDEBUG << "createTransition: creating transition from string [" 
         << Common::Misc::limastring2utf8stdstring(str)
         << "] with id" << id;
#endif

  if (str.size()==0) { return t; }

  // check if there are agreement constraints attached to the transition
  /** deprecated
  int firstConstraint(findSpecialCharacter(str,CHAR_BEGIN_CONSTRAINT,0));
  vector<Constraint> builtConstraints(0);
  if (firstConstraint != -1) {
    s = LimaString(str,0,firstConstraint);
    int nextConstraint;
    while (firstConstraint != -1) {
      nextConstraint=findSpecialCharacter(str,CHAR_BEGIN_CONSTRAINT,
                      firstConstraint+1);
      LimaString constraintString;
      if (nextConstraint == -1) {
        constraintString=LimaString(str,firstConstraint+1);
      }
      else {
        constraintString=LimaString(str,firstConstraint+1,
                                     nextConstraint-firstConstraint-1);
      }

      // treats the constraint
      Constraint c(constraintString);
      builtConstraints.push_back(c);

      // to the next constraint
      firstConstraint=nextConstraint;
    }
  }
  else {
    s=str;
  }
  */

  // check if the transition unit is to be kept in recognized expression
  // or not
  if (s[0] == CHAR_NOKEEP_OPEN_TR) {
    if (s[s.length()-1] != CHAR_NOKEEP_CLOSE_TR) {
      AUCLOGINIT;
      LERROR << "createTransition: confused by no_keep format (maybe incomplete) :"
             << Common::Misc::limastring2utf8stdstring(str);
    }
    else {
      keepIt = false;
      s = s.mid(1,s.length()-2);
    }
  }

  // check if the transition is negative
  if (s[0] == CHAR_NEGATIVE_TR) {
    negative=true;
    s = s.mid(1);
  }

  // check if the transition is head of recognized expression
  if (s[0] == CHAR_HEAD_TR) {
    head=true;
    s = s.mid(1);
  }

  // check the nature of the transition unit
  // ----------------------------------------------------------------------
  // numeric transition
  bool isNumeric(false);
  int transitionKeySize(0);
  if (s.indexOf(*STRING_NUM_NAME_TR) == 0 && s != *STRING_NUM_NAME_TR) {
    isNumeric=true;
    transitionKeySize=STRING_NUM_NAME_TR->size();
  }
  else if (s.indexOf(*STRING_NUM_NAME_TR2) == 0 && s != *STRING_NUM_NAME_TR2) {
    isNumeric=true;
    transitionKeySize=STRING_NUM_NAME_TR2->size();
  }
  if (isNumeric) {
    uint64_t val(NumericTransition::noValue);
    uint64_t min(NumericTransition::noValue);
    uint64_t max(NumericTransition::noValue);
    if (s[transitionKeySize] == CHAR_NUM_EQUAL_TR)
    {
      val=s.midRef(transitionKeySize+1).toULong();
    }
    else if (s[transitionKeySize] == CHAR_NUM_LE_TR)
    {
      //int i(s.find(CHAR_NUM_GE_TR,LENGTH_NUM_NAME_TR+1));
      int i(s.indexOf(CHAR_NUM_GE_TR,transitionKeySize+1));
      if (i==-1) {
        max=s.midRef(transitionKeySize+1).toULong();
      }
      else {
        max=s.midRef(transitionKeySize+1, i-transitionKeySize-1).toULong();
        min=s.midRef(i+1).toULong();
      }
    }
    else if (s[transitionKeySize] == CHAR_NUM_GE_TR)
    {
      //int i(findSpecialCharacter(s,CHAR_NUM_LE_TR,LENGTH_NUM_NAME_TR+1));
      int i(findSpecialCharacter(s,CHAR_NUM_LE_TR,transitionKeySize+1));
      if (i==-1) {
        min=s.midRef(transitionKeySize+1).toULong();
      }
      else {
        min=s.midRef(transitionKeySize+1,
                  i-transitionKeySize-1).toULong();
        max=s.midRef(i+1).toULong();
      }
    }

    t = new NumericTransition(val,min,max);
  }
  // ----------------------------------------------------------------------
  // Part-of-speech transition
  else if (s[0] == CHAR_POS_TR) {
    Tpos pos=createTpos(Misc::limastring2utf8stdstring(s.mid(1)),language);
    t = new PosTransition(pos,macroAccessor,microAccessor);
  }
  // ----------------------------------------------------------------------
  // lemma transition
  else if (findSpecialCharacter(s,CHAR_POS_TR,0) != -1 ) {
    int i(findSpecialCharacter(s,CHAR_POS_TR,0));
    Tpos pos=createTpos(Misc::limastring2utf8stdstring(s.mid(i+1)),language);
    LimaString str = s.left(i);
    // remove escape characters from the transition (don't need them anymore)
    removeEscapeCharacters(str);
    StringsPoolIndex strIndex=sp[str];
    t = new LemmaTransition(strIndex,pos,macroAccessor,microAccessor); 
  }
  // ----------------------------------------------------------------------
  // TStatus transition (status returned by the tokenizer)
  else if (s.indexOf(*STRING_TSTATUS_TR,0) == 0) {
    t = createStructuredTStatusTransition(s,LENGTH_TSTATUS_TR);
  }
  else if (s.indexOf(*STRING_TSTATUS_TR_small,0) == 0) {
    t = createDefaultTStatusTransition(s,LENGTH_TSTATUS_TR);
  }
  // ----------------------------------------------------------------------
  // GazeteerTransition: form belongs to gazeteer
  /*
  else if (s.indexOf(CHAR_BEGIN_NAMEGAZ,0) == 0) {
    // name of gazeteer already identified!
    t = new GazeteerTransition(gazeteerAsVectorOfString,alias,keep);
  }
  */
  // ----------------------------------------------------------------------
  // * transition
  else if (s == *STRING_ANY_TR) {
    t = new StarTransition();
  }
  // ----------------------------------------------------------------------
  // entity transition
  else if (s.size()>=2 && s[0]==CHAR_BEGIN_ENTITY && s[s.size()-1]==CHAR_END_ENTITY) {
    LimaString entityName(s.mid(1,s.size()-2));
    Common::MediaticData::EntityType type=resolveEntityName(entityName,activeEntityGroups);
    if (type.isNull()) {
      Common::MediaticData::EntityGroupId groupId = resolveGroupName(entityName,activeEntityGroups);
      if( groupId == 0) {
        AUCLOGINIT;
        LERROR << "createTransition: cannot resolve entity name " 
               << Common::Misc::limastring2utf8stdstring(s);
      }
      else {
        AUCLOGINIT;
        LDEBUG << "createTransition: create EntityGroupTransition(" << groupId << ")";
        t=new EntityGroupTransition(groupId);
      }
    }
    else {
      AUCLOGINIT;
      LDEBUG << "createTransition: create EntityTransition(" << type << ")";
      t=new EntityTransition(type);
    }
  }
  // ----------------------------------------------------------------------
  // simple word transition
  else {
    if (t==0) {
      // remove escape characters from the transition (don't need them anymore)
      removeEscapeCharacters(s);
      StringsPoolIndex sIndex=sp[s];
      t = new WordTransition(sIndex); 
    }
  }
  // set the general properties of the transition
  t->setKeep(keepIt);
  t->setNegative(negative);
  t->setHead(head);
  t->setId(id);
  for (std::size_t i(0); i<constraints.size(); i++) {
    t->addConstraint(constraints[i]);
  }
  /** deprecated
  for (int i(0); i<builtConstraints.size(); i++) {
    t->addConstraint(builtConstraints[i]);
  }
  */
  return t;
}

//**********************************************************************
//
Common::MediaticData::EntityGroupId
resolveGroupName(const LimaString s,
                  const std::vector<LimaString>& activeEntityGroups)
{
#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "resolveGroupName: try to resolve group name " 
         << Common::Misc::limastring2utf8stdstring(s);
#endif
  Common::MediaticData::EntityGroupId foundGroup;
  try {
    LimaString groupName=s;
#ifdef DEBUG_LP
    LDEBUG << "resolveGroupName: try group name " << Common::Misc::limastring2utf8stdstring(s);
#endif
    foundGroup = Common::MediaticData::MediaticData::single().getEntityGroupId(groupName);
    // group is among active groups
#ifdef DEBUG_LP
    LDEBUG << "resolveGroupName: foundGroup" << foundGroup;
#endif
    for (vector<LimaString>::const_iterator it=activeEntityGroups.begin(),
       it_end=activeEntityGroups.end(); it!=it_end; it++) {
      if( groupName == *it ) {
        return foundGroup;
      }
      AUCLOGINIT;
      LERROR << "resolveGroupName: group " << Common::Misc::limastring2utf8stdstring(s) << " not active";
      return foundGroup;
    }
  }
  catch (LimaException& e) {
      AUCLOGINIT;
      LERROR << "resolveGroupName: cannot resolve group for " 
             << Common::Misc::limastring2utf8stdstring(s);
  }
  return foundGroup;
}

//**********************************************************************
//
Common::MediaticData::EntityType
resolveEntityName(const LimaString s,
                  const std::vector<LimaString>& activeEntityGroups)
{
#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "resolveEntityName: try to resolve entity name " 
         << Common::Misc::limastring2utf8stdstring(s);
#endif
  
  // test if word is a known entity name => in this case, entity transition
  if (s.indexOf(Common::MediaticData::MediaticData::single().getEntityTypeNameSeparator())!=-1) {
#ifdef DEBUG_LP
    LDEBUG << "resolveEntityName: entity name is complete";
#endif
    try {
      return Common::MediaticData::MediaticData::single().getEntityType(s);
    }
    catch (LimaException& e) {
      AUCLOGINIT;
      LERROR << "resolveEntityName: unknown entity " << s;
    }
  }
  else { // try to find this entity in active groups
    Common::MediaticData::EntityType type;
    for (vector<LimaString>::const_iterator it=activeEntityGroups.begin(),
           it_end=activeEntityGroups.end(); it!=it_end; it++) {
      try {
        LimaString entityName=(*it)+Common::MediaticData::MediaticData::single().getEntityTypeNameSeparator()+s;
#ifdef DEBUG_LP
        LDEBUG << "resolveEntityName: try entity name " << Common::Misc::limastring2utf8stdstring(entityName);
#endif
        Common::MediaticData::EntityType findType=
          Common::MediaticData::MediaticData::single().getEntityType(entityName);
        if (!type.isNull()) {
          // there is ambiguity
          AUCLOGINIT;
          LERROR << "resolveEntityName: cannot resolve entity group for entity " 
                 << Common::Misc::limastring2utf8stdstring(s)
                 << " (at least two groups contain this entity)";
        }
        else {
          type=findType;
        }
      }
      catch (LimaException& e) { 
        // not in this group: do nothing (continue search)
#ifdef DEBUG_LP
        LDEBUG << "resolveEntityName: entity " << Common::Misc::limastring2utf8stdstring(s)
               << " not in group " << Common::Misc::limastring2utf8stdstring(*it);
#endif
      }
    }
    if (type.isNull()) { // try to interpret s as group
      AUCLOGINIT;
      LERROR << "resolveEntityName: cannot resolve entity group for entity " 
             << Common::Misc::limastring2utf8stdstring(s)
             << " (no active group contains this entity)";
    }
    else {
     return type;
    }
  }
  return Common::MediaticData::EntityType();
}

//**********************************************************************
// to parse a text form of a part of speech
// either 23-34 or NC_GEN
//**********************************************************************
Tpos createTpos(const std::string& s, MediaId language) {
  Tpos p(0);
  AUCLOGINIT;
  if (s.empty()) {
    LERROR << "empty Part-Of-Speech transition: use NONE_1";
    p=NONE_1;
  }

  const Common::PropertyCode::PropertyManager& macroManager=
    static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyManager("MACRO");
  const Common::PropertyCode::PropertyManager& microManager=
    static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyManager("MICRO");


  if (isdigit(s[0])) {
    LWARN << "numeric part-of-speech are deprecated: "
          << "please use symbolic names";
    //int sep(findSpecialCharacter(Common::Misc::utf8stdstring2limastring(s),CHAR_SEP_MACROMICRO,0));
    int sep(findSpecialCharacter(Common::Misc::utf8stdstring2limastring(s),CHAR_SEP_MACROMICRO,0));
    if (sep==-1) {
      p=static_cast<LinguisticCode>(atoi(s.c_str()));
    }
  }
  else {
    bool hasMicro(false);
    // search for separator '-' (separator macro/micro)
    //int sep1(findSpecialCharacter(Common::Misc::utf8stdstring2limastring(s),CHAR_SEP_MACROMICRO,0));
    int sep1(findSpecialCharacter(Common::Misc::utf8stdstring2limastring(s),CHAR_SEP_MACROMICRO,0));
    if (sep1 != -1) {
//      string macro=string(s,0,sep1);
      string micro=string(s,sep1+1);
      hasMicro=true;
//      if (macro.find("L_")!=0) { macro=string("L_")+ macro; }
      LinguisticCode microCode = microManager.getPropertyValue(micro);
//       if (microCode == 0 &&  micro.find("L_")!=0) 
//       { 
//         micro=string("L_")+ micro; 
//         microCode= microManager.getPropertyValue(micro);
//       }
      p = microCode;
#ifdef DEBUG_LP
      LDEBUG << "TransitionCompiler: micro="<< micro << "->" << p;
#endif
    }
    else {
      // only macro
      string macro=s;
//       if (macro.find("L_")!=0) { macro=string("L_")+ macro; }
      p= macroManager.getPropertyValue(macro);
#ifdef DEBUG_LP
      LDEBUG << "TransitionCompiler: macro="<< macro << "->" << p;
#endif
      /*
      //search for separator '_'
      int sep(findSpecialCharacter(Common::Misc::utf8stdstring2limastring(s),CHAR_SEP_MACROMICRO_STRING,0));
      if (sep != -1 && string(s,0,sep) == "L") {
        // '_' found after L (NC)
        sep=findSpecialCharacter(Common::Misc::utf8stdstring2limastring(s),CHAR_SEP_MACROMICRO_STRING,sep+1);
      }
      if (sep == -1) { // only macro
        string macro=s;
        if (macro.find("L_")!=0) { macro=string("L_")+ macro; }
        p.first= macroManager.getPropertyValue(macro);
        p.second=NONE_1;
        LDEBUG << "TransitionCompiler: macro="<< macro << "->" << p.first
               << ", no micro" << "->" << p.second;
      }
      else { // micro -> try to guess macro
        string macro=string(s,0,sep);
        string micro(s);
        hasMicro=true;
        if (macro.find("L_")!=0) { macro=string("L_")+ macro; }
        if (micro.find("L_")!=0) { micro=string("L_")+ micro; }
        p.first= macroManager.getPropertyValue(macro);
        p.second= microManager.getPropertyValue(micro);
        LDEBUG << "TransitionCompiler: macro="<< macro << "->" << p.first
               << ", micro="<< micro << "->" << p.second;
      }
      */
    }
    if (macroManager.getPropertyAccessor().empty(p)) {
      LERROR << "transitionCompiler: failed identifying macro category \""
             << s << "\": use NONE_1 instead";
    }
    if (hasMicro && microManager.getPropertyAccessor().empty(p)) {
      LERROR << "transitionCompiler: failed identifying micro category in \""
             << s << "\": use NONE_1 instead";
    }
  }
  return p;
}

//**********************************************************************
// to parse a text form of a TstatusTransition
//**********************************************************************
// TStatus in structured form (T_A1, T_Ni etc.)
TStatusTransition* createStructuredTStatusTransition(const LimaString& s,
                                                     const int begin) {
#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "Create TStatus transition: parse structured format";
#endif
  TStatusTransition* t = new TStatusTransition();

  int current=begin;
  t->setStatus(TStatus()); // initializes TStatus

  if (s.indexOf(*TSTATUS_ALPHANUM,current) == current) {
    t->status().setStatus(T_ALPHANUMERIC);
  }
  else if (s.indexOf(*TSTATUS_ALPHA,current) == current) {
    t->status().setStatus(T_ALPHA);
    current+=LENGTH_TSTATUS_ELT;
    // capital status
    if (s.indexOf(*TSTATUS_ALPHACAP_CAPITAL,current) == current) {
      t->status().setAlphaCapital(T_CAPITAL);
      current+=LENGTH_TSTATUS_ELT;
    }
    else if (s.indexOf(*TSTATUS_ALPHACAP_SMALL,current) == current) {
      t->status().setAlphaCapital(T_SMALL);
      current+=LENGTH_TSTATUS_ELT;
    }
    else if (s.indexOf(*TSTATUS_ALPHACAP_CAPITAL_1ST,current) == current) {
      t->status().setAlphaCapital(T_CAPITAL_1ST);
      current+=LENGTH_TSTATUS_ELT;
    }
    else if (s.indexOf(*TSTATUS_ALPHACAP_ACRONYM,current) == current) {
      t->status().setAlphaCapital(T_ACRONYM);
      current+=LENGTH_TSTATUS_ELT;
    }
    else if (s.indexOf(*TSTATUS_ALPHACAP_CAPITAL_SMALL,current) == current) {
      t->status().setAlphaCapital(T_CAPITAL_SMALL);
      current+=LENGTH_TSTATUS_ELT;
    }
    // roman status
    if (s.indexOf(*TSTATUS_ALPHAROMAN_CARDINAL,current) == current) {
      t->status().setAlphaRoman(T_CARDINAL_ROMAN);
      current+=LENGTH_TSTATUS_ELT;
    }
    else if (s.indexOf(*TSTATUS_ALPHAROMAN_ORDINAL,current) == current) {
      t->status().setAlphaRoman(T_ORDINAL_ROMAN);
      current+=LENGTH_TSTATUS_ELT;
    }
    else if (s.indexOf(*TSTATUS_ALPHAROMAN_NOT,current) == current) {
      t->status().setAlphaRoman(T_NOT_ROMAN);
      current+=LENGTH_TSTATUS_ELT;
    }
    else { // default is not roman
      t->status().setAlphaRoman(T_NOT_ROMAN);
    }

    // hyphen or possessive
    if (s.indexOf(*TSTATUS_ISHYPHEN,current) == current) {
      t->status().setAlphaHyphen(true);
    }
    if (s.indexOf(*TSTATUS_ISPOSSESSIVE,current) == current) {
      t->status().setAlphaPossessive(true);
    }
  }
  else if (s.indexOf(*TSTATUS_NUMERIC,current) == current) {
    t->status().setStatus(T_NUMERIC);
    current+=LENGTH_TSTATUS_ELT;
    // numeric status
    if (s.indexOf(*TSTATUS_NUMERIC_INTEGER,current) == current) {
      t->status().setNumeric(T_INTEGER);
      current+=LENGTH_TSTATUS_ELT;
    }
    else if (s.indexOf(*TSTATUS_NUMERIC_COMMA_NUMBER,current) == current) {
      t->status().setNumeric(T_COMMA_NUMBER);
      current+=LENGTH_TSTATUS_ELT;
    }
    else if (s.indexOf(*TSTATUS_NUMERIC_DOT_NUMBER,current) == current) {
      t->status().setNumeric(T_DOT_NUMBER);
      current+=LENGTH_TSTATUS_ELT;
    }
    else if (s.indexOf(*TSTATUS_NUMERIC_FRACTION,current) == current) {
      t->status().setNumeric(T_FRACTION);
      current+=LENGTH_TSTATUS_ELT;
    }
    else if (s.indexOf(*TSTATUS_NUMERIC_ORDINAL_INTEGER,current) == current) {
      t->status().setNumeric(T_ORDINAL_INTEGER);
      current+=LENGTH_TSTATUS_ELT;
    }
  }
  else if (s.indexOf(*TSTATUS_PATTERN,current) == current) {
    t->status().setStatus(T_PATTERN);
    current+=LENGTH_TSTATUS_ELT;
  }
  else if (s.indexOf(*TSTATUS_WRD_BRK,current) == current) {
    t->status().setStatus(T_WORD_BRK);
    current+=LENGTH_TSTATUS_ELT;
  }
  else if (s.indexOf(*TSTATUS_SENTENCE_BRK,current) == current) {
    t->status().setStatus(T_SENTENCE_BRK);
    current+=LENGTH_TSTATUS_ELT;
  }
  else if (s.size()!=0) {
  }
  else {
   // default is already t_fallback
  }

  return t;
}

// TStatus in default status format (t_integer,t_capital_1st etc)
TStatusTransition* createDefaultTStatusTransition(const LimaString& s,
                                                  const int /*begin*/) {
#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "Create TStatus transition: default status";
#endif
  
  TStatusTransition* t = new TStatusTransition();
  
  t->setStatus(TStatus()); // initializes TStatus
  t->status().setDefaultKey(s);
  return t;
}

} // end namespace
} // end namespace
} // end namespace
} // end namespace
