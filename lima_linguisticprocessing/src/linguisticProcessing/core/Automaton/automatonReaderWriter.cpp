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
 * @file       automatonReaderWriter.cpp
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Fri Mar 18 2005
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "automatonReaderWriter.h"
#include "automatonCommon.h"
#include "recognizer.h"
#include "rule.h"
#include "automaton.h"
#include "constraintFunction.h"


// transitions
#include "transitionUnit.h"
#include "posTransition.h"
#include "wordTransition.h"
#include "lemmaTransition.h"
#include "numericTransition.h"
#include "epsilonTransition.h"
#include "starTransition.h"
#include "tstatusTransition.h"
#include "andTransition.h"
#include "setTransition.h"
#include "deaccentuatedTransition.h"
#include "entityTransition.h"

#include "common/Data/readwritetools.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/TStatus.h"


using namespace std;
using namespace Lima::Common;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::MediaticData;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

/***********************************************************************/
// read/write utility function for type of transition
/***********************************************************************/
TypeTransition readTypeTransition(std::ifstream& file) {
  return static_cast<TypeTransition>(Common::Misc::readCodedInt(file));
}
void writeTypeTransition(std::ofstream& file, const TypeTransition t) {
  Common::Misc::writeCodedInt(file,t);
}


#define RECOGNIZER_VERSION "1.10"
#define RECOGNIZER_DEBUG_VERSION ".debug"

//----------------------------------------------------------------------
// Reader
//----------------------------------------------------------------------
AutomatonReader::AutomatonReader():
m_constraintFunctionMap(),
m_entityGroupMapping(),
m_entityTypeMapping(),
m_debug(false)
{}
AutomatonReader::~AutomatonReader() {}

void AutomatonReader::
readRecognizer(const std::string& filename,
               Recognizer& reco)
{
  if (! filename.empty() && 
      filename[filename.size()-1] == '/') {
    AULOGINIT;
    ostringstream oss;
    oss << "Cannot open [" << filename 
           << "] ; it is a directory";
    LERROR << oss.str();
    throw OpenFileException(oss.str());
  }
  ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
  if (!(file.is_open() && file.good()) ) {
    AULOGINIT;
    ostringstream oss;
    oss << "Cannot open file [" << filename << "]";
    LERROR << oss.str();
    throw OpenFileException(oss.str());
  }
  try {
    readRecognizer(file,reco);
  }
  catch (std::exception& ) {
    AULOGINIT;
    LERROR << "failed reading file: " << filename;
    throw;
  }
  file.close();
}

void AutomatonReader::
readRecognizer(std::ifstream& file,
               Recognizer& reco)
{
  MediaId language=readHeader(file);
  reco.m_language = language;

  // read all rules in storage
  reco.m_ruleStorage.clear();
  uint64_t nrules=Misc::readCodedInt(file);
  reco.m_ruleStorage.reserve(nrules);
  for (uint64_t i(0); i<nrules; i++)
  {
    Rule* r = readRule(file,reco.m_language);
    reco.m_ruleStorage.push_back(r);
  }

  uint64_t n=Misc::readCodedInt(file);
  reco.m_rules.clear();
  reco.m_rules.reserve(n);
  for (uint64_t i(0); i<n; i++)
  {
    TransitionUnit *t = readTransitionUnit(file,reco.m_language); // read trigger
    uint64_t nRulesTransition=Misc::readCodedInt(file);
    if (nRulesTransition)
    {
      uint64_t index=Misc::readCodedInt(file);
      reco.m_rules.push_back(Recognizer::TriggerRule(t,SetOfRules(1,reco.m_ruleStorage[index])));
      reco.m_rules[i].second.reserve(nRulesTransition);
      for (uint64_t j(1); j<nRulesTransition; j++)
      {
        index=Misc::readCodedInt(file);
        reco.m_rules[i].second.push_back(reco.m_ruleStorage[index]);
      }
    }
  }

  reco.initializeSearchStructure();
}

MediaId AutomatonReader::
readHeader(std::ifstream& file)
{
  // read version number
  string version;
  Misc::readStringField(file,version);
  string::size_type deb=version.find(RECOGNIZER_DEBUG_VERSION);
  if (deb!=string::npos) {
    m_debug=true;
    version=string(version,0,deb);
  }
  if (version != RECOGNIZER_VERSION)
  {
    AULOGINIT;
    ostringstream oss;
    oss << "incompatible version of binary rules format: " 
           << version << ", current version is " 
           << RECOGNIZER_VERSION;
    LERROR << oss.str();
    throw runtime_error(oss.str());
  }

  // read language
  std::string language;
  Misc::readStringField(file,language);

  MediaId lang=static_cast<MediaId>(0);
  // if language is specified
  if (!language.empty())
  {
    // see if it is already initialized (if not, do it)
    if (Common::MediaticData::MediaticData::single().getMediasIds().find(language)
        ==Common::MediaticData::MediaticData::single().getMediasIds().end())
    {
      AULOGINIT;
      ostringstream oss;
      oss << "language " << language 
          << " is not initialized";
      LERROR << oss.str();
      throw runtime_error(oss.str());
    }
    lang=Common::MediaticData::MediaticData::single().media(language);
  }

  // readEntityType and save mapping
  Common::MediaticData::MediaticData::changeable().readEntityTypes(file,m_entityGroupMapping,m_entityTypeMapping);
  
  // initialize constraint functions
  readRegisteredConstraints(file,lang);

  return lang;
}

Rule* AutomatonReader::
readRule(std::ifstream& file,MediaId language)
{
  Rule* rule=new Rule;

  //read rule id if debug
  if (m_debug) {
    std::string id;
    Misc::readStringField(file,id);
    rule->setRuleId(id);
  }

  TransitionUnit *t=readTransitionUnit(file,language);
  rule->m_trigger=t;
  readAutomaton(file,rule->m_left,language);
  readAutomaton(file,rule->m_right,language);

  // read type of expression
  EntityGroupId groupId=static_cast<EntityGroupId>(Misc::readCodedInt(file));
  EntityTypeId typeId=static_cast<EntityTypeId>(Misc::readCodedInt(file));
  // use EntityType mapping
  rule->setType(m_entityTypeMapping[EntityType(typeId,groupId)]);
  uint64_t lingProp = 0;
  file.read((char *) &lingProp,sizeof(uint64_t));
  rule->setLinguisticProperties(static_cast<LinguisticCode>(lingProp));

  // read list attribute values of normalized form
  Misc::readUTF8StringField(file,rule->m_normalizedForm);
  
  rule->m_numberOfConstraints=Misc::readCodedInt(file);
  file.read((char *) &(rule->m_contextual), sizeof(bool));
  file.read((char *) &(rule->m_negative), sizeof(bool));
  file.read((char *) &(rule->m_hasLeftRightConstraint), sizeof(bool));

  // read actions attached to the rule
  uint64_t nbActions=Misc::readCodedInt(file);
  rule->m_actions.reserve(nbActions);
  for (uint64_t i(0); i<nbActions; i++) {
    Constraint c;
    readConstraint(file,c);
    rule->m_actions.push_back(c);
  }
  uint64_t nbActionsWithOneArgument=Misc::readCodedInt(file);
  for (uint64_t i(0); i<nbActionsWithOneArgument; i++) {
    Constraint c;
    readConstraint(file,c);
    LimaString id;
    Misc::readUTF8StringField(file,id);
    rule->m_actionsWithOneArgument.push_back(std::pair<LimaString,Constraint>(id,c));
  }
  file.read((char*) &(rule->m_weight),sizeof(double));
  return rule;
}

void AutomatonReader::
readAutomaton(std::ifstream& file,
              Automaton& a,
              MediaId language)
{
  a.reinit();
  Tstate nbStates;
  // first read the number of states
  nbStates=readTstate(file);

  // read deterministic status
  file.read((char*) &(a.m_deterministic), sizeof(bool));

  // read about the final states
  a.m_finalStates = vector<bool>(nbStates,false);
  Tstate nbFinals(0);
  nbFinals=readTstate(file);
  for (uint64_t i(0); i<nbFinals; i++) {
    Tstate s=readTstate(file);
    a.m_finalStates[s] = true;
  }
  // read the informations about each state
  for (uint64_t i(0); i<nbStates; i++) {
    Tstate lastStateAdded=a.addState();
    // the number of transitions from this state
    Tstate nbTrans=Misc::readCodedInt(file);
    // the transitions
    a.m_transitions[lastStateAdded].reserve(nbTrans);
    for (uint64_t j(0); j<nbTrans; j++) {
      Transition c;
      c.setTransitionUnit(readTransitionUnit(file,language));
      Tstate state=readTstate(file);
      c.setNextState(state);
      a.m_transitions[lastStateAdded].push_back(c);
    }
  }
  //a.initializeSearchStructures(language);
}

void AutomatonReader::
readConstraint(std::ifstream& file,
               Constraint& c)
{
  c.reinit();
  // read id of constraint function
  uint64_t id=Misc::readCodedInt(file);
  c.m_functionAddr= m_constraintFunctionMap[id];
  
  c.m_index=Misc::readCodedInt(file);
  c.m_action=static_cast<ConstraintAction>(Misc::readOneByteInt(file));
  file.read((char *) &(c.m_negative), sizeof(bool));
}

void AutomatonReader::
readRegisteredConstraints(std::ifstream& file,
                          MediaId language) {
  m_constraintFunctionMap.clear();
  uint64_t nbFunctions=Misc::readCodedInt(file);
  for (uint64_t i(0); i!=nbFunctions; i++) {
    // write constraint id for binary file
    uint64_t n=Misc::readCodedInt(file);
    string functionName("");
    LimaString complement;
    // write function name
    Misc::readStringField(file,functionName);
    // write function complement
    Misc::readUTF8StringField(file,complement);

    // store id <-> function in map
    ConstraintFunction* functionPtr=
      ConstraintFunctionManager::changeable().
      getConstraintFunction(functionName,language,complement);
    m_constraintFunctionMap[n]=functionPtr;
  }
}

TransitionUnit* AutomatonReader::
readTransitionUnit(std::ifstream& file,MediaId language)
{
  TransitionUnit *t=0;
  TypeTransition codeTrans=
    static_cast<TypeTransition>(Misc::readOneByteInt(file));
  switch(codeTrans) {
  case T_WORD: {
    FsaStringsPool& sp=Common::MediaticData::MediaticData::changeable().stringsPool(language);
    Tword s;
    readTword(file,s,sp);
    t=new WordTransition(s);
    break; }
  case T_NUM: {
    uint64_t val=Misc::readCodedInt(file);
    uint64_t min=Misc::readCodedInt(file);
    uint64_t max=Misc::readCodedInt(file);
    t=new NumericTransition(val,min,max);
    break; }
  case T_POS: {
    const Common::PropertyCode::PropertyAccessor* macroAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MACRO"));
    const Common::PropertyCode::PropertyAccessor* microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));
    Tpos p;
    readTpos(file,p);
    t=new PosTransition(p,macroAccessor,microAccessor);
    break; }
  case T_LEMMA: {
    const Common::PropertyCode::PropertyAccessor* macroAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MACRO"));
    const Common::PropertyCode::PropertyAccessor* microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));
    FsaStringsPool& sp=Common::MediaticData::MediaticData::changeable().stringsPool(language);
    Tpos p;
    Tword s;
    readTword(file,s,sp);
    readTpos(file,p);
    t=new LemmaTransition(s,p,macroAccessor,microAccessor);
    break; }
  case T_TSTATUS: {
     //enum types coded in file as uint64_t
    StatusType s=static_cast<StatusType>(Misc::readOneByteInt(file));
    AlphaCapitalType c=static_cast<AlphaCapitalType>(Misc::readOneByteInt(file));
    AlphaRomanType r=static_cast<AlphaRomanType>(Misc::readOneByteInt(file));
    NumericType n=static_cast<NumericType>(Misc::readOneByteInt(file));
    unsigned char h;
    unsigned char p;
    file.read((char *) &h, sizeof(unsigned char));
    file.read((char *) &p, sizeof(unsigned char));

    LimaString defaultKey;
    Misc::readUTF8StringField(file,defaultKey);

    TStatus status(c,r,h,p,false,false,n,s);
    status.setDefaultKey(defaultKey);

    t=new TStatusTransition(status);
    break;
  }
  case T_AND: {
    uint64_t size=Misc::readCodedInt(file);
    vector<TransitionUnit*> tmp(size);
    for (uint64_t i(0); i<size; i++) {
      tmp[i]=readTransitionUnit(file,language);
    }
    t=new AndTransition(tmp);
    //!! do not delete pointers in tmp vector because they are kept in t !!
    break;
  }
  case T_SET: {
    FsaStringsPool& sp=Common::MediaticData::MediaticData::changeable().stringsPool(language);
    set<Tword> tmp;
    uint64_t size=Misc::readCodedInt(file);
    for (uint64_t i(0); i<size; i++) {
      LimaString elt;
      Misc::readUTF8StringField(file,elt);
      tmp.insert(sp[elt]);
    }
    t=new SetTransition(tmp);
    break;
  }
  case T_DEACCENTUATED: {
    LimaString deaccentuatedForm;
    MediaId language;
    Misc::readUTF8StringField(file,deaccentuatedForm);
    file.read((char*)&language,sizeof(unsigned char));
    t=new DeaccentuatedTransition(deaccentuatedForm,language);
    break;
  }
  case T_EPSILON: {
    t=new EpsilonTransition();
    break; 
  }
  case T_STAR: {
    t=new StarTransition();
    break; 
  }
  case T_ENTITY: {
    EntityGroupId groupId=static_cast<EntityGroupId>(Misc::readCodedInt(file));
    EntityTypeId typeId=static_cast<EntityTypeId>(Misc::readCodedInt(file));
    // use entity type mapping
    t=new EntityTransition(m_entityTypeMapping[EntityType(typeId,groupId)]);
    break; 
  }
  default: {
    AULOGINIT;
    LERROR << "Undefined type of transition: " << codeTrans;
  }
  }
  // read properties
  if (t!=0) {
    bool keep, neg, head;
    int len;
    file.read((char*)&keep, sizeof(bool));
    t->setKeep(keep);
    file.read((char*)&neg, sizeof(bool));
    t->setNegative(neg);
    file.read((char*)&head, sizeof(bool));
    t->setHead(head);
    file.read((char*)&len, sizeof(len));
    char *buf = new char [len];
    file.read(buf, len);
    t->setId(std::string(buf,len));
    uint64_t n=Misc::readCodedInt(file);
    Constraint c;
    for (uint64_t i(0); i<n; i++) {
      readConstraint(file,c);
      t->addConstraint(c);
    }
  }
  return t;
}

//----------------------------------------------------------------------
// Writer
//----------------------------------------------------------------------
AutomatonWriter::AutomatonWriter():
m_constraintFunctionMap(),
m_debug(false)
{}

AutomatonWriter::~AutomatonWriter() {}

void AutomatonWriter::
writeRecognizer(const Recognizer& reco,
                const std::string& filename,
                MediaId language,
                bool debug)
{
  ofstream file(filename.c_str(), std::ios::out | std::ios::binary);
  if (!file) {
    AULOGINIT;
    ostringstream oss;
    oss << "Cannot open file [" << filename << "]";
    LERROR << oss.str();
    throw OpenFileException(oss.str());
  }
  writeRecognizer(file,reco,language,debug);
  file.close();
}

void AutomatonWriter::
writeRecognizer(std::ofstream& file,
                const Recognizer& reco,
                MediaId language,
                bool debug)
{
  m_debug=debug;
  writeHeader(file,language);

  //write all rules
  uint64_t nRule(reco.m_ruleStorage.size());
  Common::Misc::writeCodedInt(file,nRule);
  for (uint64_t i(0); i<nRule; i++)
  {
    writeRule(file,*(reco.m_ruleStorage[i]),language);
  }

  // then write the structure with only indexes of rules (instead of pointers)
  uint64_t n(reco.m_rules.size());
  Misc::writeCodedInt(file,n);
  for (uint64_t i(0); i<n; i++)
  {
    writeTransitionUnit(file,reco.m_rules[i].first,language); // write transition
    uint64_t nRulesTransition(reco.m_rules[i].second.size());
    Misc::writeCodedInt(file,nRulesTransition);
    for (uint64_t j(0); j<nRulesTransition; j++)
    {
      Misc::writeCodedInt(file,reco.findRuleIndex(reco.m_rules[i].second[j]));
    }
  }
}

void AutomatonWriter::
writeHeader(std::ofstream& file,
            MediaId language)
{  
  std::string version(RECOGNIZER_VERSION);
  if (m_debug) {
    version.append(RECOGNIZER_DEBUG_VERSION);
  }
  
  // write version number
  Misc::writeStringField(file,version);
  
  // write language
  Misc::writeStringField(file,Lima::Common::MediaticData::MediaticData::single().media(language));

  // write entity types
  Lima::Common::MediaticData::MediaticData::single().writeEntityTypes(file);

  // write registered constraints (after language, because 
  // language is needed when reinitializing constraints)
  writeRegisteredConstraints(file);
}

void AutomatonWriter::
writeRule(std::ofstream& file,
          const Rule& rule,
          MediaId language) 
{
  // write id if debug
  if (m_debug) {
    //LOGINIT("LP::Automaton");
    //LDEBUG<< "write rule id '"<< rule.m_ruleId<< "'";
    Misc::writeStringField(file,rule.m_ruleId);
  }

  writeTransitionUnit(file,rule.m_trigger,language);
  writeAutomaton(file,rule.m_left,language);
  writeAutomaton(file,rule.m_right,language);

  // write type of expression
  Misc::writeCodedInt(file,rule.getType().getGroupId());
  Misc::writeCodedInt(file,rule.getType().getTypeId());

  uint64_t lingProp=rule.getLinguisticProperties();
  file.write((char *) &lingProp,sizeof(uint64_t));
  
  // write normalized form (LimaString)
  Misc::writeUTF8StringField(file,rule.m_normalizedForm);

  Common::Misc::writeCodedInt(file,rule.m_numberOfConstraints);
  file.write((char *) &(rule.m_contextual), sizeof(bool));
  file.write((char *) &(rule.m_negative), sizeof(bool));
  file.write((char *) &(rule.m_hasLeftRightConstraint), sizeof(bool));

  // write actions attached to the rule
  uint64_t nbActions=rule.m_actions.size();
  Common::Misc::writeCodedInt(file,nbActions);
  for (uint64_t i(0); i<nbActions; i++) {
    writeConstraint(file,rule.m_actions[i]);
  }

  // write actions (with one argument) attached to the rule
  uint64_t nbActionsWithOneArgument=rule.m_actionsWithOneArgument.size();
  Common::Misc::writeCodedInt(file,nbActionsWithOneArgument);
  for (std::vector<std::pair<LimaString,Constraint>>::const_iterator it = rule.m_actionsWithOneArgument.begin() ; 
       it != rule.m_actionsWithOneArgument.end() ; it++ ) {
    writeConstraint(file,it->second);
    Misc::writeUTF8StringField(file,it->first);
  }
  file.write((char*) &(rule.m_weight),sizeof(double));
}

void AutomatonWriter::
writeAutomaton(std::ofstream& file,
               const Automaton& a,
               MediaId language) 
{
  // first write the number of states
  writeTstate(file,a.m_numberStates);

  // read deterministic status
  file.write((char*) &(a.m_deterministic), sizeof(bool));

  // write the final states
  // (list of final states instead of full boolean vector)
  vector<Tstate> finals(a.finalStates());
  Tstate nbFinals(finals.size());
  writeTstate(file,nbFinals);
  for (uint64_t i(0); i<nbFinals; i++) {
    writeTstate(file,finals[i]);
  }

  // then write the informations about each state
  for (uint64_t i(0); i<a.m_numberStates; i++) {
    // the number of transitions from this state
    Tstate nbTrans(a.m_transitions[i].size());
//     std::cerr << "State: " << i << ": " << nbTrans << " transitions" << endl;
    Common::Misc::writeCodedInt(file,nbTrans);
    // the transitions
    for (uint64_t j(0); j<nbTrans; j++) {
//       std::cerr << "Transition: " << *(m_transitions[i][j].transitionUnit()) << endl;
      writeTransitionUnit(file,a.m_transitions[i][j].transitionUnit(),language);
      Tstate state(a.m_transitions[i][j].nextState());
      writeTstate(file,state);
    }
  }
}

void AutomatonWriter::
writeTransitionUnit(std::ofstream& file,
                    TransitionUnit* transition,
                    MediaId language)
{
  // write specific informations
  TypeTransition type=transition->type();
  Misc::writeOneByteInt(file,type);
  switch (type) {
  case T_WORD: {
    FsaStringsPool& sp=Common::MediaticData::MediaticData::changeable().stringsPool(language);
    WordTransition* t=static_cast<WordTransition*>(transition);
    writeTword(file,t->word(),sp);
    break;
  }
  case T_POS: {
    PosTransition* t=static_cast<PosTransition*>(transition);
    writeTpos(file,t->pos());
    break;
  }
  case T_LEMMA: {
    FsaStringsPool& sp=Common::MediaticData::MediaticData::changeable().stringsPool(language);
    LemmaTransition* t=static_cast<LemmaTransition*>(transition);
    writeTword(file,t->lemma(),sp);
    writeTpos(file,t->partOfSpeech());
    break;
  }
  case T_NUM: {
    NumericTransition* t=static_cast<NumericTransition*>(transition);
    Misc::writeCodedInt(file,t->value());
    Misc::writeCodedInt(file,t->min());
    Misc::writeCodedInt(file,t->max());
    break;
  }
  case T_TSTATUS: {
    TStatusTransition* t=static_cast<TStatusTransition*>(transition);
    
    // write the Tstatus
    const TStatus& status=t->status();
    StatusType s(status.getStatus());
    AlphaCapitalType a(status.getAlphaCapital());
    AlphaRomanType r(status.getAlphaRoman());
    NumericType n(status.getNumeric());
    unsigned char h(status.isAlphaHyphen());
    unsigned char p(status.isAlphaPossessive());
    
    Misc::writeOneByteInt(file,s); // enum type StatusType as int
    Misc::writeOneByteInt(file,a); // enum type AlphaCapitalType as int
    Misc::writeOneByteInt(file,r); // enum type AlphaRomanType as int
    Misc::writeOneByteInt(file,n); // enum type NumericType as int
    file.write((char *) &h, sizeof(unsigned char));
    file.write((char *) &p, sizeof(unsigned char));
    Common::Misc::writeUTF8StringField(file,status.defaultKey());
    break;
  }
  case T_AND: {
    AndTransition* t=static_cast<AndTransition*>(transition);
    const vector<TransitionUnit*>& transitions=t->getTransitions();
    uint64_t size(transitions.size());
    Misc::writeCodedInt(file,size);
    vector<TransitionUnit*>::const_iterator
      it=transitions.begin(),
      it_end=transitions.end();
    for (; it!=it_end; it++) {
      writeTransitionUnit(file,*it,language);
    }
    break;
  }
  case T_SET: {
    SetTransition* t=static_cast<SetTransition*>(transition);
    const std::set<Tword>& words=t->getWords();
    Misc::writeCodedInt(file,words.size());
    set<Tword>::const_iterator 
      w=words.begin(),
      w_end=words.end();
    for (;w!=w_end; w++) {
      Misc::writeUTF8StringField(file,Common::MediaticData::MediaticData::single().stringsPool(language)[*w]);
    }
    break;
  }
  case T_DEACCENTUATED: {
    DeaccentuatedTransition* t=static_cast<DeaccentuatedTransition*>(transition);
    Misc::writeUTF8StringField(file,t->getDeaccentuatedForm());
    MediaId lang=t->getLanguage();
    file.write((char*) &lang,sizeof(unsigned char));
    break;
  }
  case T_ENTITY: {
    EntityTransition* t=static_cast<EntityTransition*>(transition);
    EntityType entityType=t->entityType();
    Misc::writeCodedInt(file,entityType.getGroupId());
    Misc::writeCodedInt(file,entityType.getTypeId());
    break;
  }
  case T_EPSILON: 
  case T_STAR: 
    // nothing to do
    break;
  default: {
    AULOGINIT;
    LERROR << "Undefined type of transition: " << type;
  }
  }

  // write common transition unit properties
  bool keep=transition->keep();
  bool negative=transition->negative();
  bool head=transition->head();
  std::string id = transition->getId();
  file.write((char *) &keep, sizeof(bool));
  file.write((char *) &negative, sizeof(bool));
  file.write((char *) &head, sizeof(bool));
  int len = id.size();
  LOGINIT("Automaton::Compiler");
  std::string(id.c_str(),len);
  LDEBUG << "Transition Writer: write id = " << id;
  file.write((char *) &len, sizeof(len));
  file.write((char *) id.c_str(), len);
  uint64_t n=transition->numberOfConstraints();
  Misc::writeCodedInt(file,n);
  for (uint64_t i(0); i<n; i++) {
    writeConstraint(file,transition->constraint(i));
  }
}

void AutomatonWriter::
writeConstraint(std::ofstream& file,
                const Constraint& c)
{
  // write id of constraint function
  Misc::writeCodedInt(file,m_constraintFunctionMap[c.m_functionAddr]);
  
  Misc::writeCodedInt(file,c.m_index);
  Misc::writeOneByteInt(file,c.m_action);
  file.write((char *) &(c.m_negative), sizeof(bool));
}

void AutomatonWriter::
writeRegisteredConstraints(std::ofstream& file)
{
  const std::multimap<std::string,ConstraintFunction*>& 
    registered=
    ConstraintFunctionManager::single().
    getRegisteredFunctions();

  std::multimap<std::string,ConstraintFunction*>::
    const_iterator
    c=registered.begin(),
    c_end=registered.end();

  uint64_t nbFunctions=registered.size();
  Misc::writeCodedInt(file,nbFunctions);
  uint64_t n(1);
  for (; c!=c_end; c++) {
    // write constraint id for binary file
    Misc::writeCodedInt(file,n);
    // write function name
    Misc::writeStringField(file,(*c).first);
    // write function complement
    Misc::writeUTF8StringField(file,(*c).second->getComplementString());
    // store function <-> id in map
    m_constraintFunctionMap[(*c).second]=n;
    n++;
  }
}

} // end namespace
} // end namespace
} // end namespace
