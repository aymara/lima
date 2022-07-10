// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
*
* File        : recognizerCompiler.cpp
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Mon Apr  7 2003
* Copyright   : (c) 2003 by CEA
* Version     : $Id: recognizerCompiler.cpp 9081 2008-02-25 18:34:51Z de-chalendarg $
*
************************************************************************/

#include "recognizerCompiler.h"
#include "transitionCompiler.h"
#include "ruleCompiler.h"
#include "compilerExceptions.h"
#include "tstring.h"
#include "common/LimaCommon.h"
#include "common/Data/strwstrtools.h"
#include "common/tools/FileUtils.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/time/timeUtilsController.h"

#include "linguisticProcessing/core/Automaton/automatonCommon.h" // for exceptions
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h" // to check rules
#include "linguisticProcessing/core/Automaton/entityTransition.h" // for exceptions

using namespace Lima::LinguisticProcessing::Automaton::AutomatonCompiler;
using namespace Lima::Common;
using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

#define MAX_RULE_WEIGHT 1000.0

//************************************************************
// initialazation of static members
std::string RecognizerCompiler::m_recognizerEncoding="latin1";

//***********************************************************************
// general formatting of the file
//***********************************************************************
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_DEFINE_ENCODING, (QLatin1String("set encoding=")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_DEFINE_DEFAULTACTION, (QLatin1String("set defaultAction=")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_UNSET_DEFAULTACTION, (QLatin1String("unset defaultAction=")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_DEFINE_ENTITYTYPES, (QLatin1String("set entityTypes=")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_USING_MODEX, (QLatin1String("using modex ")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_USING_ENTITYGROUPS, (QLatin1String("using groups ")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_USING_LIBS, (QLatin1String("using libs ")));

//***********************************************************************
// syntax for the type definition
//***********************************************************************
// possible types are defined in the rules file by lines like
// define type=LOCATION;ENAMEX TYPE="LOCATION";/ENAMEX;
// define type=PERSON;ENAMEX TYPE="PERSON";/ENAMEX;prenom,nom

Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_DEFINE_TYPE, (QLatin1String("define type=")));

//***********************************************************************
// syntax for subautomaton definition
//***********************************************************************

Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_DEFINE_SUBAUTOMATON, (QLatin1String("define subautomaton ")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_SUBAUTOMATON_PATTERN, (QLatin1String("pattern")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_SUBAUTOMATON_INDEX, (QLatin1String("index ")));



//************************************************************
// constructors and destructors
RecognizerCompiler::RecognizerCompiler(const std::string& filename):
m_defaultAction(),
m_activeEntityGroups(),
m_lineNumber(0),
m_filename(filename),
m_currentLine(),
m_stream(0),
m_nbRule(0)
{
  AUCLOGINIT;
  LDEBUG << "Opening recognizer compiler with file " << filename;
  m_stream=new ifstream(filename.c_str(), std::ifstream::binary);
  if (! m_stream || !m_stream->good()) {
    LERROR << "Cannot open file [" << filename << "]";
    throw AutomatonCompilerException("no file "+filename);
  }
}

RecognizerCompiler::~RecognizerCompiler() {
  delete m_stream;
}

//**********************************************************************
// build a recognizer from a text file
//**********************************************************************
void RecognizerCompiler::buildRecognizer(Recognizer& reco,
                                         MediaId language,
                                         const std::string& filename) {
  AUCLOGINIT;
  // Lima::TimeUtilsController("RecognizerCompiler::buildRecognizer", true);
  // Lima::TimeUtilsController* ctrl1  = new Lima::TimeUtilsController("RecognizerCompiler::buildRecognizer", true);
  // ugly hack to keep tracks of the files
  // (should be rewritten more properly)
  std::ifstream* tmpStream(0);
  std::string tmpFilename=m_filename;
  uint64_t tmpLineNumber=m_lineNumber;

  if (! filename.empty()) {
    tmpStream=m_stream;
    m_stream=new ifstream(filename.c_str(), std::ifstream::binary);
    if (! m_stream || !m_stream->good()) {
      LERROR << "Cannot open file [" << filename << "]";
      throw AutomatonCompilerException("no file "+filename);
    }
    m_filename=filename;
    m_lineNumber=0;
  }

  LimaString s;
  vector <Gazeteer> gazeteers;
  vector <SubAutomaton> subAutomatons;
  while (! m_stream->eof()) {
    readline(s);
    //LDEBUG << "[" << Misc::limastring2utf8stdstring(s) << "]";
    if (s.length() == 0) { continue; } // skip blank lines
    if (s[0] == CHAR_COMMENT) { continue; } // skip comments

    // check if it is a type definition
    if (s.indexOf(*STRING_DEFINE_ENCODING)==0) {
      m_recognizerEncoding = Misc::limastring2utf8stdstring(s.mid(STRING_DEFINE_ENCODING->size()));
#ifdef DEBUG_LP
      LDEBUG << "Got encoding definition: '"
             << m_recognizerEncoding << "'";
#endif
      continue;
    }
    // check if it is a type definition
    if (s.indexOf(*STRING_DEFINE_TYPE)==0) {
      // ignore it : deprecated
      continue;
    }

    if (s.indexOf(*STRING_USING_LIBS)==0) {
      int
      begin=STRING_USING_ENTITYGROUPS->length(),
      next=0;
      do {
        next=findSpecialCharacter(s,CHAR_SEP_LIST,begin);
        LimaString str = s.mid(begin,next-begin);
#ifdef DEBUG_LP
        LDEBUG << "RecognizerCompiler: use lib " << Common::Misc::limastring2utf8stdstring(str);
#endif

        begin=next+1;
      } while (next != -1);
      continue;
    }


    if (s.indexOf(*STRING_USING_ENTITYGROUPS)==0) {
      int
        begin=STRING_USING_ENTITYGROUPS->length(),
        next=0;
      do {
        next=findSpecialCharacter(s,CHAR_SEP_LIST,begin);
        LimaString str = s.mid(begin,next-begin);
#ifdef DEBUG_LP
        LDEBUG << "RecognizerCompiler: use group " << Common::Misc::limastring2utf8stdstring(str);
#endif
        m_activeEntityGroups.push_back(str);
        begin=next+1;
      } while (next != -1);
      continue;
    }

    if (s.indexOf(*STRING_DEFINE_ENTITYTYPES)==0) {
      //default action to be associated to all rules
      LWARN << "this syntax ("
            << Common::Misc::limastring2utf8stdstring(*STRING_DEFINE_ENTITYTYPES)
            << ") is deprecated: should be 'using modex XXX,YYY'";
      continue;
    }

    if (s.indexOf(*STRING_USING_MODEX)==0) {
      int
        begin=STRING_USING_MODEX->length(),
        next=0;
      do {
        next=findSpecialCharacter(s,CHAR_SEP_LIST,begin);
        LimaString str = s.mid(begin,(next==-1)?next:next-begin);
        // initialize entities

        QString filename = Common::Misc::findFileInPaths(Common::MediaticData::MediaticData::single().getConfigPath().c_str(),str);
        XMLConfigurationFiles::XMLConfigurationFileParser parser(filename.toUtf8().constData());
        MediaticData::MediaticData::changeable().initEntityTypes(parser);
        begin=next+1;
      } while (next != -1);
      continue;
    }

    if (s.indexOf(*STRING_DEFINE_DEFAULTACTION)==0) {
      //default action to be associated to all rules
      m_defaultAction+=
        CHAR_BEGIN_ACTION+
        s.mid(STRING_DEFINE_DEFAULTACTION->length());
      continue;
    }

    if (s.indexOf(*STRING_UNSET_DEFAULTACTION)==0) {
      //remove a default action in current default actions
      LimaString action=
        CHAR_BEGIN_ACTION+
        s.mid(STRING_UNSET_DEFAULTACTION->size());
      int i=m_defaultAction.indexOf(action);
      if (i==-1) {
        printWarning("cannot unset default action (not defined)",s);
        continue;
      }
      m_defaultAction.remove(i,action.size());
      continue;
    }

    // check if it is a sub-automaton definition
    if (s.indexOf(*STRING_DEFINE_SUBAUTOMATON)==0) {
      readSubAutomaton(s.mid(STRING_DEFINE_SUBAUTOMATON->size()),gazeteers,subAutomatons);
      continue;
    }

    // check if must include external rules files
    if (s.indexOf(*STRING_INCLUDE_GAZ) == 0) {
      int
        beginFilename=LENGTH_INCLUDE_GAZ,
        nextFilename=0;
      do {
        nextFilename=findSpecialCharacter(s,CHAR_SEP_LISTFILENAME,beginFilename);
        string filename=Common::Misc::limastring2utf8stdstring(
          s.mid(beginFilename,(nextFilename==-1)?nextFilename:nextFilename-beginFilename));
        buildRecognizer(reco, language, filename);
        beginFilename=nextFilename+1;
      } while (nextFilename != -1);

      continue;
    }

    // check if must load gazeteers from external files
    if (s.indexOf(*STRING_USE_GAZ) == 0) {
      int
        beginFilename=LENGTH_USE_GAZ,
        nextFilename=0;
      do {
        nextFilename=findSpecialCharacter(s,CHAR_SEP_LISTFILENAME,beginFilename);
        string filename=Common::Misc::limastring2utf8stdstring(
          s.mid(beginFilename,(nextFilename==-1)?nextFilename:nextFilename-beginFilename));
        readGazeteers(filename,gazeteers,subAutomatons);
        beginFilename=nextFilename+1;
      } while (nextFilename != -1);

      continue;
    }

    m_currentLine=s;

    // check if there are internal gazeteers definitions
    if (s[0] == CHAR_BEGIN_NAMEGAZ) {
      int offsetEqual(findSpecialCharacter(s,CHAR_EQUAL_GAZ,0));
  // test if it is a gazeteer definition
      if (offsetEqual != -1
          && (offsetEqual == s.size()-1 ||
            (s[offsetEqual+1] != CHAR_BEGIN_ACTION_IF_SUCCESS &&
             s[offsetEqual+1] != CHAR_BEGIN_ACTION_IF_FAILURE)
            )
         ) {
        Gazeteer g;
        LimaString alias = s.mid(1,offsetEqual-1);
        g.setAlias(alias);
        int offsetParOpen(findSpecialCharacter(s,CHAR_OPEN_GAZ,
                                                        offsetEqual));
        if (offsetParOpen != -1) {
          g.readValues(*this,gazeteers,s.mid(offsetParOpen+1));
        }
        else {
          g.readValues(*this,gazeteers);
        }
        //std::cerr << g << endl;
        g.buildAutomatonString(gazeteers,subAutomatons);
        // check if another gazeteer exist with same name
        vector<Gazeteer>::const_iterator
          git=gazeteers.begin(),
          git_end=gazeteers.end();
        bool gazeteerExist(false);
        for (;git!=git_end;git++) {
          if ((*git).alias() == alias) {
            LWARN << "class with name " << alias
                  << " already exists: second definition is ignored";
            gazeteerExist=true;
            break;
          }
        }
        if (! gazeteerExist) {
          LINFO << "Adding gazeteer:" << g.alias() << "->" << g.size();
          gazeteers.push_back(g);
        }
      }

      else { // it's a rule with a class as a trigger -> multiply the rules
        // identify class alias
        int endTrigger(findSpecialCharacter(s,CHAR_SEP_RULE,1));
        LimaString classAlias = s.mid(1,(endTrigger==-1)?endTrigger:endTrigger-1);
        s=s.mid(endTrigger+1);
        addRuleWithGazeteerTrigger(classAlias,s,gazeteers,subAutomatons,
                                   m_defaultAction,reco,language,"trigger", true);
      }
      continue;
    }
    if (s.size()>=2 && s[1] == CHAR_BEGIN_NAMEGAZ) {
      if (s[0] == CHAR_NOKEEP_OPEN_RE) {
        // gazeteer trigger not kept [@class]:
        int endTrigger(findSpecialCharacter(s,CHAR_SEP_RULE,2));
        int endNoKeep(findSpecialCharacter(s,CHAR_NOKEEP_CLOSE_RE,2));
        LimaString classAlias = s.mid(2,(endNoKeep==-1)?endNoKeep:endNoKeep-2);
        s=s.mid(endTrigger+1);
        // add default actions
        addRuleWithGazeteerTrigger(classAlias,s,gazeteers,subAutomatons,
                                   m_defaultAction,reco,language,"trigger",false);
        continue;
      }
      if (s[0] == CHAR_HEAD_TR) {
        // head gazeteer trigger &@class:
        int endTrigger(findSpecialCharacter(s,CHAR_SEP_RULE,2));
        LimaString classAlias = s.mid(2,(endTrigger==-1)?endTrigger:endTrigger-2);
        s=s.mid(endTrigger+1);
        // add default actions
        addRuleWithGazeteerTrigger(classAlias,s,gazeteers,subAutomatons,
                                   m_defaultAction,reco,language,"trigger",true,true);
        continue;
      }
    }

    // check if there are agreement constraints on following lines
    // and add them at end of the rule if there are
    s=s+peekConstraints(*m_stream);

    // it's just a rule
    //expandGazeteersInRule(s,gazeteers);
    //expandSubAutomatonsInRule(s,subAutomatons);

    //syntax check: no spaces or alternatives in trigger
    int firstSep=findSpecialCharacter(s,CHAR_SEP_RULE);
    int firstSpace=findSpecialCharacter(s,CHAR_SEP_RE);
    int firstAlt=findSpecialCharacter(s,CHAR_CHOICESEP_RE);
    if (firstSpace != -1 && (firstSep==-1 || firstSpace < firstSep)) {
      printWarning("spaces in triggers are not allowed",s);
      continue;
    }
    if (firstAlt != -1 && (firstSep == -1 || firstAlt < firstSep)) {
      printWarning("alternatives in triggers are not allowed (use a class)",s);
      continue;
    }

    // add default actions
    s+=m_defaultAction;

    Rule* r=new Rule;
//     s=RuleCompiler::treatConstraints(*r,s); // this can modify the trigger
    try {
      RuleCompiler::initRule(*r,s,language,
                             gazeteers,subAutomatons,
                             m_activeEntityGroups,
                             m_filename,
                             m_lineNumber);
    }
    catch (AutomatonCompilerException& e) {
      throwError(e.what(),m_currentLine);
    }
    catch (UnknownTypeException& e) {
      throwError(e.what(),m_currentLine);
    }
    if (r->getTrigger()==0) {
      printWarning("Null trigger",m_currentLine);
      continue;
    }
    TransitionUnit* trigger = r->getTrigger()->clone();
    if (trigger==0) {
      printWarning("Null trigger",m_currentLine);
      continue;
    }
    // set weight
    r->setWeight(currentRuleWeight());
    /*
    ostringstream message;
    if (!checkRule(*r,trigger,language,message)) {
      LERROR << message.str();
    }
    */
    LDEBUG << "Adding rule no " << m_nbRule << "(" << r->getRuleId() << ")"
          << ": trigger=" << *trigger;
    reco.addRule(trigger,r);
#ifdef DEBUG_LP
    LDEBUG << "rule[" << m_nbRule << "]=" << *r;
#endif

    m_nbRule++;
    delete trigger;
  }
  m_stream->close();
  delete m_stream; m_stream = 0;
  if (!filename.empty()) {
    m_stream=tmpStream;
    m_filename=tmpFilename;
    m_lineNumber=tmpLineNumber;
  }

  // Lima::TimeUtilsController* ctrl3  = new Lima::TimeUtilsController("initializeSearchStructure", true);
  reco.initializeSearchStructure();
  // delete ctrl3;
  // delete ctrl1;
}

//**********************************************************************
// read a subautomaton
//**********************************************************************
void RecognizerCompiler::
readSubAutomaton(const LimaString& line,
                 const std::vector<Gazeteer>& gazeteers,
                 std::vector<SubAutomaton>& subAutomatons)
{
  SubAutomaton sub;
  //line contains "define subautomaton "
  int bracket=line.indexOf(CHAR_SUBAUTOMATON_BEGIN);
  if (bracket==-1) {
    ostringstream oss;
    oss << "definition of sub-automaton must have opening character "
        <<CHAR_SUBAUTOMATON_BEGIN.unicode()<<" on first line";
    throwError(oss.str(),line);
  }
  sub.setAutomatonName(line.left(bracket).trimmed());

  // read values of sub-automaton
  bool endOfSubAutomaton(false);
  LimaString s;
  while (!endOfSubAutomaton) {
    readline(s);
    int i=s.indexOf(CHAR_SUBAUTOMATON_END);
    // ending sign "}" should be on beginning of line: easier
    // to distinguish from cardinality signs
    if (i==0) {
      endOfSubAutomaton=true;
      break;
    }
    // cut spaces at beginning
    s=s.trimmed();
    int j=s.indexOf(CHAR_SUBAUTOMATON_NAMEVALUESEP);
    if (j==-1) {
      throwError("definition of sub-automaton must name=value elements",
                 line);
    }
    LimaString name = s.left(j);
    LimaString value = s.mid(j+1);
    if (name==*STRING_SUBAUTOMATON_PATTERN) {
      // add parenthesis around the value (safer)
      sub.setPattern(Common::Misc::utf8stdstring2limastring("(")+value+Common::Misc::utf8stdstring2limastring(")"));
    }
    else if (name.indexOf(*STRING_SUBAUTOMATON_INDEX)==0) {
      name.remove(0,STRING_SUBAUTOMATON_INDEX->size());
      sub.addAttribute(name,value);
    }
    else {
      sub.addAttribute(name,value);
    }
  }
  if (sub.getPattern().isEmpty()) {
    ostringstream oss;
    oss << "definition of sub-automaton must have at least an attribute "
        << Common::Misc::limastring2utf8stdstring(*STRING_SUBAUTOMATON_PATTERN);
    throwError(oss.str(),line);
  }

#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "read sub automaton " << sub;
#endif

  sub.buildAutomatonString(gazeteers,subAutomatons);
  subAutomatons.push_back(sub);
}

//**********************************************************************
// add a rule with a gazeteer trigger ->
//   1) create a rule and multiply the reference to this rule in the index
//      of recognizer (transition with 1 entry,rule)
//   2) create a a gazeteerTransition and create only one entry in the index
//      of recognizer (gazeteerTransition,rule)
//**********************************************************************
void RecognizerCompiler::
addRuleWithGazeteerTrigger(const LimaString& gazeteerName,
                           LimaString& ruleString,
                           const vector<Gazeteer>& gazeteers,
                           const std::vector<SubAutomaton>& subAutomatons,
                           const LimaString& defaultAction,
                           Recognizer& reco,
                           MediaId language,const std::string& currentId,
                           const bool keepTrigger,
                           const bool headTrigger) {

  AUCLOGINIT;
  // find gazeteer
  std::size_t gazeteerIndex;
  for (gazeteerIndex=0; gazeteerIndex<gazeteers.size(); gazeteerIndex++) {
    if (gazeteers[gazeteerIndex].alias() == gazeteerName) {
      break;
    }
  }

  // gazeteer not found
  if ( gazeteerIndex >= gazeteers.size() || gazeteers[gazeteerIndex].size() == 0 ) {
    string str=Misc::limastring2utf8stdstring(gazeteerName);
    if (gazeteerIndex<gazeteers.size()) {
      printWarning("empty class as trigger ["+str+"]",ruleString);
    }
    else {
      printWarning("Unrecognized class as trigger ["+str+"]",ruleString);
    }
    return;
  }
  const Gazeteer& gazeteer = gazeteers[gazeteerIndex];
  Rule* r=new Rule;

  // check if there are agreement constraints on following lines
  // and add them at end of the rule if there are
  ruleString=ruleString+peekConstraints(*m_stream);
  ruleString=ruleString+defaultAction;

  // add the trigger to deal with agreement constraints
  LimaString triggerString=gazeteer[0];
  // triggerString must not contain space (its real value is not important, the important is the
  // trigger used to get to the rule in the recognizer)
  triggerString.remove(QChar(' '));
  if (! keepTrigger) {
    triggerString=CHAR_NOKEEP_OPEN_RE+triggerString+CHAR_NOKEEP_CLOSE_RE;
  }
  if (headTrigger) {
    triggerString=CHAR_HEAD_TR+triggerString;
  }

  ruleString=triggerString+CHAR_SEP_RULE+ruleString;
  try {
    RuleCompiler::initRule(*r,ruleString,language,
                           gazeteers,subAutomatons,
                           m_activeEntityGroups,
                           m_filename,
                           m_lineNumber);
  }
  catch (AutomatonCompilerException& e) {
    throwError(e.what(),m_currentLine);
  }

  r->setWeight(currentRuleWeight());
  LDEBUG << "Adding rule no " << m_nbRule << "(" << r->getRuleId() << ")"
        << ": multiple trigger (first is "<<Common::Misc::limastring2utf8stdstring(gazeteer[0])<<")";
  int indexRule=reco.addRuleInStorage(r);

  if( (!gazeteer.hasOnlyWords()) && gazeteer.hasMultiTermWord() ) {
    throwError("use of gazetteer with multi-term words and with category or t_status forbidden in trigger: ",m_currentLine);
  }
  if( gazeteer.hasOnlyWords() )
  {
    // const std::vector<LimaString>& gazeteerAsVectorOfString = gazeteer;
    // TransitionUnit* trigger = new GazeteerTransition(gazeteerAsVectorOfString,gazeteerName,keepTrigger); */
    TransitionUnit* trigger = createGazeteerTransition(gazeteerName,
                 language, currentId, m_activeEntityGroups,
                 gazeteers,keepTrigger,headTrigger);
    if (trigger != 0)
    {
      //copy the properties of the trigger of the rule
      trigger->copyProperties(*(r->getTrigger()));
      reco.addRule(trigger,indexRule);
      //LINFO << nbRule << ": trigger=" << *trigger;
      delete trigger; // it has been copied
    }
  }
  else
  {
    for (std::size_t j(0); j<gazeteer.size(); j++) {
      triggerString=gazeteer[j];
      if (headTrigger) {
        triggerString=CHAR_HEAD_TR+triggerString;
      }
      TransitionUnit* trigger = createTransition(triggerString,
                                                 language,currentId,
                                                 m_activeEntityGroups,
                                                 keepTrigger);
      if (trigger != 0)
      {
        //copy the properties of the trigger of the rule
        trigger->copyProperties(*(r->getTrigger()));
        reco.addRule(trigger,indexRule);
        //LINFO << nbRule << ": trigger=" << *trigger;
        delete trigger; // it has been copied
      }
    }
  }
  m_nbRule++;
}

//**********************************************************************
// useful functions
//**********************************************************************
double RecognizerCompiler::currentRuleWeight() {
  // decreasing function of the rule number
  // (first rules are prioritary)
#ifdef DEBUG_LP
  AUCLOGINIT;
#endif
  double w=MAX_RULE_WEIGHT/(1+log((double)m_nbRule+1));
#ifdef DEBUG_LP
  LDEBUG << "weight of rule " << m_nbRule << "=" << w;
#endif
  return w;
}

// get a line and interpret it with respect to its encoding
void RecognizerCompiler::readline(LimaString& line) {
  string str = Lima::Common::Misc::readLine(*m_stream);
  m_lineNumber++;
  if (str.empty()) {
    line.clear();
    return;
  }
  try {
    if (m_recognizerEncoding=="latin1") {
      line  = Misc::latin15stdstring2limastring(str);
    }
    else if (m_recognizerEncoding=="utf8") {
      line = Misc::utf8stdstring2limastring(str);
    }
    else {
      throw std::runtime_error(std::string("Unsupported rules file encoding: ") + m_recognizerEncoding);
    }
  }
  catch (exception& e) {
    ostringstream oss;
    oss <<  "file " << m_filename << ",line "
        << m_lineNumber << ": " << e.what();
    AUCLOGINIT;
    LERROR << "Recognizer compiler Error: " << oss.str();
    throw RecognizerCompilerException(oss.str());
  }
}

bool isRuleSeparator(LimaChar c) {
  if (c == CHAR_SEP_RULE || c == CHAR_SEP_RE || c == CHAR_CHOICESEP_RE
    || c == CHAR_GROUP_CLOSE_RE || c == CHAR_NOKEEP_CLOSE_RE || c == CHAR_OPTIONAL_RE || c == CHAR_CARDINALITY_OPEN_RE || c == CHAR_BEGIN_CONSTRAINT || c == CHAR_BEGIN_ACTION)
  {
    return true;
  }
  else
    return false;
}

void RecognizerCompiler::readGazeteers(const std::string& filename,
                                       vector<Gazeteer>& gazeteers,
                                       const vector<SubAutomaton>& subAutomatons) {
  AUCLOGINIT;
#ifdef DEBUG_LP
  LDEBUG << "reading gazeteer file: " << filename;
#endif
  RecognizerCompiler reco(filename);
  while (! reco.endOfFile()) {
    Gazeteer g;
    g.read(reco);
    if (g.numberOfWords()!=0) {
      LINFO << "Adding gazeteer:"
            << g.alias()
            << "->" << g.size();
      g.buildAutomatonString(gazeteers,subAutomatons);
      gazeteers.push_back(g);
    }
#ifdef DEBUG_LP
    else { LDEBUG << "gazeteer is empty: " << g; }
    if (reco.endOfFile()) { LDEBUG << "reached end of file"; }
#endif
  }
}

void RecognizerCompiler::
expandGazeteersInRule(LimaString& s,
                      const vector<Gazeteer>& gazeteers) {
  int beginGazeteer(findSpecialCharacter(s,CHAR_BEGIN_NAMEGAZ,0));
  int next(0);
  while (beginGazeteer != -1) {
    // find the corresponding gazeteer
    std::size_t i(0);
    for (i=0; i<gazeteers.size(); i++) {
      if (s.indexOf(gazeteers[i].alias(),beginGazeteer+1)==beginGazeteer+1) {
        // check if next char is a separator
        next=beginGazeteer + 1 + gazeteers[i].alias().length();
        if (isRuleSeparator(s[next])) {
          break;
        }
      }
    }
    if (i<gazeteers.size()) {
      // replace by the automaton string
      s.replace(beginGazeteer, gazeteers[i].alias().length()+1,
                gazeteers[i].stringAutomaton());
    }
    else {
      printWarning("unrecognized class name",m_currentLine);
    }
    // next gazeteer to replace
    beginGazeteer = findSpecialCharacter(s,CHAR_BEGIN_NAMEGAZ,
                                         beginGazeteer+1);
  }
}

void RecognizerCompiler::
expandSubAutomatonsInRule(LimaString& s,
                          const std::vector<SubAutomaton>& subAutomatons) {
#ifdef DEBUG_LP
  AUCLOGINIT;
  LDEBUG << "expand rule with sub-automatons";
  LDEBUG << "rule before expansion :" << Common::Misc::limastring2utf8stdstring(s);
#endif
  int beginSub(findSpecialCharacter(s,CHAR_BEGIN_NAMESUB,0));
  int next(0);
  while (beginSub != -1) {
    // find the corresponding subAutomaton
    std::size_t i(0);
    for (i=0; i<subAutomatons.size(); i++) {
      if (s.indexOf(subAutomatons[i].getName(),beginSub+1)==beginSub+1) {
        // check if next char is a separator
        next=beginSub + 1 + subAutomatons[i].getName().size();
        if (isRuleSeparator(s[next])) {
          break;
        }
      }
    }
    if (i<subAutomatons.size()) {
      s.replace(beginSub, subAutomatons[i].getName().size()+1,
                subAutomatons[i].getPattern());
    }
    else {
      printWarning("Unrecognized sub-automaton name",m_currentLine);
    }
    // next subAutomaton to replace
    beginSub = findSpecialCharacter(s,CHAR_BEGIN_NAMESUB,
                                    beginSub+1);
  }
#ifdef DEBUG_LP
  LDEBUG << "rule after expansion :" << s;
#endif
}

// bool RecognizerCompiler::
// checkRule(const Rule& rule,
//           const TransitionUnit* trigger,
//           MediaId language,
//           std::ostringstream& message) const
// {
//   // check if rule may produce an element that might be trigger
//   // of same rule => should cause infinite loop
//
//   // happen only if both left and right contexts can be empty
//   // and an action is attached to the rule
//   if (rule.leftAutomaton().isFinalState(0) &&
//       rule.rightAutomaton().isFinalState(0) &&
//       !rule.getActions().empty()) {
//
//     switch (trigger->type()) {
//     case T_STAR: {
//       message << "line " << m_lineNumber
//               << ": rule may cause infinite loops (star trigger on a rule that may recognize only one token)";
//       return false;
//     }
//     case T_WORD: {
//       LimaString str;
//       if (rule.getNormalizedForm().isEmpty()) {
//         message << "line " << m_lineNumber
//                 << ": rule may cause infinite loops (word trigger on a rule that may recognize only one token and has no normalized form)";
//         return false;
//       }
//       else {
//         FsaStringsPool& sp=Common::MediaticData::MediaticData::changeable().stringsPool(language);
//         StringsPoolIndex index=sp[rule.getNormalizedForm()];
//         if (index == static_cast<const WordTransition*>(trigger)->word()) {
//           message << "line " << m_lineNumber
//                   << ": rule may cause infinite loops (word trigger on a rule that may recognize only one token and whose normalized form is same as the trigger)";
//           return false;
//         }
//       }
//       return true;
//     }
//     case T_POS: {
//       LinguisticCode pos=rule.getLinguisticProperties();
//       if (pos!=L_NONE) {
//         if (static_cast<const PosTransition*>(trigger)->comparePos(pos)) {
//           message << "line " << m_lineNumber
//                   << ": rule may cause infinite loops "
//                   << "(pos trigger on a rule that may recognize only one token, whose result POS is same as trigger)";
//           return false;
//         }
//         return true;
//       }
//       return true;
//     }
//     case T_LEMMA: {
//       message << "line " << m_lineNumber
//               << ": rule may cause infinite loops "
//               << ": (lemma trigger on a rule that may recognize only one token, whose result may match the trigger)";
//       return true;
//     }
//     case T_GAZETEER: {
//       return true;
//     }
//     case T_NUM: {
//       return true;
//     }
//     case T_TSTATUS: {
//       message << "line " << m_lineNumber
//               << ": rule may cause infinite loops "
//               << ": tstatus trigger on a rule that may recognize only one token";
//       return false;
//     }
//     case T_ENTITY: {
//       if (static_cast<const EntityTransition*>(trigger)->entityType() == rule.getType()) {
//         message << "line " << m_lineNumber
//                 << ": rule may cause infinite loops "
//                 << ": (entity trigger on a rule of same entity type that may recognize only one token)";
//         return false;
//       }
//       return true;
//     }
//     case T_ENTITY GROUP: {
//       if (static_cast<const EntityTransition*>(trigger)->entityType().getGroupId() == rule.getType().getGroupId()) {
//         message << "line " << m_lineNumber
//                 << ": rule may cause infinite loops "
//                 << ": (entity trigger on a rule of same entity group type that may recognize only one token)";
//         return false;
//       }
//       return true;
//     }
//     case T_AND:
//     case T_SET:
//     case T_DEACCENTUATED:
//     case T_EPSILON:
//       // these transitions are not used
//       return true;
//     }
//   }
//   return true;
// }

LimaString RecognizerCompiler::peekConstraints(std::ifstream& file) {
  LimaString s;
  LimaString line;
  LimaChar c = QChar::fromLatin1(file.peek());
  while ((c==CHAR_BEGIN_CONSTRAINT || c==CHAR_BEGIN_ACTION)
         && !file.eof())
  {
    readline(line);
    s += line;
    c = QChar::fromLatin1(file.peek());
  }
  return s;
}

string RecognizerCompiler::
nextFieldTypeDefinition(const LimaString& str, int& offset) {
  int nextSep(findSpecialCharacter(str,CHAR_SEP_TYPE,offset));
  if (nextSep==-1) {
    string s=Misc::limastring2utf8stdstring(str);
    throw TypeDefSyntaxException("not enough fields in type definition : "+s);
  }
  string result=Misc::limastring2utf8stdstring(str.mid(offset,nextSep-offset));
  offset=nextSep+1;
  return result;
}

void RecognizerCompiler::
parseTypeDefinition(const LimaString& str,
                    int& offset,
                    std::string& typeName,
                    std::string& openingTag,
                    std::string& closingTag,
                    vector<std::string>& attributes) {

  // first field is name
  typeName=nextFieldTypeDefinition(str,offset);
  // second field is opening tag
  openingTag=nextFieldTypeDefinition(str,offset);
  // third is closing tag
  closingTag=nextFieldTypeDefinition(str,offset);
  //fourth and last is attributes
  if (offset >= str.length()) { // no attributes
    return;
  }
  int nextSep;
  nextSep=findSpecialCharacter(str,CHAR_SEP_TYPE_ATTRIBUTES,offset);
  while (nextSep != -1) {
    attributes.push_back(Misc::limastring2utf8stdstring(str.mid(offset,nextSep-offset)));
    offset=nextSep+1;
    nextSep=findSpecialCharacter(str,CHAR_SEP_TYPE_ATTRIBUTES,offset);
  }
  // last one
  attributes.push_back(Misc::limastring2utf8stdstring(str.mid(offset)));
}

void RecognizerCompiler::
throwError(const std::string& error,
           const LimaString& ruleString) const {
  AUCLOGINIT;
  LERROR << "Error file " << m_filename
         << ", line " << m_lineNumber
         << ": " << error;
  if (! ruleString.isEmpty()) {
    LERROR << "in rule: " << ruleString;
  }
  throw AutomatonCompilerException(error);
}

void RecognizerCompiler::
printWarning(const std::string& error,
             const LimaString& ruleString) const {
  AUCLOGINIT;
  // print warnings in LERROR, because they should not be missed !
  LERROR << "Warning file " << m_filename
        << ", line " << m_lineNumber
        << ": " << error;
  if (! ruleString.isEmpty()) {
    LERROR << "in rule: " << ruleString;
  }
}


} // end namespace
} // end namespace
} // end namespace
