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
 * @file       recognizer.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Tue Oct 15 2002
 * copyright   Copyright (C) 2002 by CEA LIST
 * Project     Automaton
 * 
 * @brief      recognizer for a certain type of expression : contains a
 * list of rules 
 * 
 ***********************************************************************/

#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include "AutomatonExport.h"
#include "rule.h"
#include "recognizerMatch.h"
#include "transitionUnit.h"
#include "transitionSearchStructure.h"
#include "automatonReaderWriter.h"
#include "linguisticProcessing/core/AnalysisDict/AbstractAnalysisDictionary.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include <stdexcept>
#include <map>
#include <set>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

/** 
 * an enumerated type to indicate which kind of strategy adopt to deal
 * with two overlapping entities
 */
enum OverlapResolutionStrategy {
  IGNORE_FIRST, /**< the first entity is ignored => assumes the
                   rightmost trigger is more important */
  IGNORE_SECOND, /**< the second entity is ignored => assumes the
                    leftmost trigger is more important*/
  IGNORE_SMALLEST, /**< the smallest entity is ignored (the one that 
                      covers the smallest number of words 
                      is assumed to be less specific )*/
  KEEP_ALL_OVERLAPS /**< do not try to avoid overlaps, find the largest
                       number of entities possible */
};
#define DEFAULT_OVERLAP_STRATEGY IGNORE_FIRST

// several rules are attached to one trigger
// -> maybe problems if a same unit is trigger of several rules with
//  different transition properties (nokeep, head...) : for the 
// moment, if leads to different triggers, hence problems for the index
// in the dictionary

/** 
 * the SetOfRules type is defined as a vector of pointers on Rule
 * 
 */
typedef std::vector<Rule*> SetOfRules;

#define RECOGNIZER_CLASSID "AutomatonRecognizer"

/** 
 * rief a class for the definition of a complete recognizer
 * 
 * this class is used for the description of a complete recognizer,
 * i.e. a set of rules
 * 
 */
class LIMA_AUTOMATON_EXPORT Recognizer : public AbstractResource
{
friend class AutomatonReader;
friend class AutomatonWriter;

 public:
  Recognizer(); 
  Recognizer(const Recognizer&);
  virtual ~Recognizer();
  Recognizer& operator = (const Recognizer&);
  
  // initialization function
  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;
  
  // access functions
  uint64_t numberOfRules() const;

  /** 
   * add a rule in the storage zone
   * 
   * @param rule the rule to add
   * 
   * @return the index of the rule in the storage zone
   */
  uint64_t addRuleInStorage(Rule* rule);

  /** 
   * add a rule in the recognizer : add the rule in the storage zone,
   * and associate the trigger to the rule
   * find if the trigger already exists in the trigger list : if it is 
   * the case, add the rule in rules associated to this trigger, else
   * add a new trigger (copy of the one in argument) and associate the 
   * rule to it.
   * 
   * @param trigger the trigger of the rule
   * @param rule the rule
   * 
   * @return the index of the rule in the storage zone
   */
  uint64_t addRule(TransitionUnit* trigger, Rule* rule);
  
  /** 
   * associate a known rule to a trigger (the rule is identified by
   * its index in the storage zone)
   *
   * @param trigger the trigger
   * @param index the index of the rule in the storage zone
   */
  void addRule(TransitionUnit* trigger, const uint64_t index);

  /** 
   * apply the recognizer on a graph
   * 
   * @param graph the graph 
   * @param begin the first node to test (is also limit of the left context)
   * @param end the last node to test (is also limit of the right context)
   * @param analysis the content of the analysis (may be modified)
   * @param result the results
   * @param testAllVertices if true, test all vertices, otherwise, 
   * skip recognized expressions
   * @param stopAtFirstSuccess if true, stop testing rules 
   * on the current node after one rule succeeded
   * @param onlyOneSuccessPerType if true, stop testing rules 
   * with same type as a previously successful rule (only used if
   * stopAtFirstSuccess is false)
   * @param returnAtFirstSuccess if true, abort the search as soon
   * a rule is successful (if true, stopAtFirstSuccess will be set to true)
   * @param applySameRuleWhileSuccess if true, when a rule succeeds,
   * retry to apply it on same vertex until the rule does not apply
   * (use with care: setting this argument to true may cause loops if*
   * rules are not well written). Will not apply if stopAtFirstSuccess.
   * 
   * @return number of rules successfuly applied
   */
  uint64_t apply(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                     const LinguisticGraphVertex& begin, 
                     const LinguisticGraphVertex& end,
                     AnalysisContent& analysis,
                     std::vector<RecognizerMatch>& result,
                     bool testAllVertices=false,
                     bool stopAtFirstSuccess=true,
                     bool onlyOneSuccessPerType=false,
                     bool returnAtFirstSuccess=false,
                     bool applySameRuleWhileSuccess=false) const;
  
  /** 
   * apply the recognizer on a graph between two nodes and search between two others. 
   * precondition: [begin, end] included in [upstreamBound,downstreamBound]
   * 
   * @param graph the graph 
   * @param begin the first node to test (is not the limit of the left context)
   * @param end the last node to test (is not the limit of the right context)
   * @param upstreamBound the limit of the left context)
   * @param downstreamBound the limit of the right context)
   * @param analysis the content of the analysis (may be modified)
   * @param result the results
   * @param testAllVertices if true, test all vertices, otherwise, 
   * skip recognized expressions
   * @param stopAtFirstSuccess if true, stop testing rules 
   * after one rule succeeded
   * @param onlyOneSuccessPerType if true, stop testing rules 
   * with same type as a previously successful rule (only used if
   * stopAtFirstSuccess is false)
   * @param returnAtFirstSuccess if true, abort the search as soon
   * a rule is successful (if true, stopAtFirstSuccess will be set to true)
   * @param applySameRuleWhileSuccess if true, when a rule succeeds,
   * retry to apply it on same vertex until the rule does not apply
   * (use with care: setting this argument to true may cause loops if*
   * rules are not well written). Will not apply if stopAtFirstSuccess.
   * 
   * @return number of rules successfuly applied
   */
  uint64_t apply(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                     const LinguisticGraphVertex& begin, 
                     const LinguisticGraphVertex& end,
                     const LinguisticGraphVertex& upstreamBound, 
                     const LinguisticGraphVertex& downstreamBound,
                     AnalysisContent& analysis,
                     std::vector<RecognizerMatch>& result,
                     bool testAllVertices,
                     bool stopAtFirstSuccess,
                     bool onlyOneSuccessPerType,
                     bool returnAtFirstSuccess,
                     bool applySameRuleWhileSuccess) const;
  
  /** 
   * test the recognizer on a given vertex : check if this vertex is a trigger
   * and if a rule applies, add the result to the vector given as parameter
   * (takes the first rule which gives a match)
   * 
   * @param graph the graph
   * @param current the current vertex to test
   * @param begin the limit on the left of the current vertex
   * @param end the limit on the right of the current vertex
   * @param result the vector in which the result is stored
   * 
   * @return true if a match was found, false otherwise
   */
  uint64_t testOnVertex(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                            LinguisticGraphVertex& current,
                            const LinguisticGraphVertex& begin,
                            const LinguisticGraphVertex& end,
                            AnalysisContent& analysis,
                            std::vector<RecognizerMatch>& result,
                            bool stopAtFirstSuccess=true,
                            bool onlyOneSuccessPerType=false,
                            bool applySameRuleWhileSuccess=false) const;
  
  /** 
   * resolve the problem of overlapping entities in the list of entities :
   * when two entities are overlaping, only one is kept
   * @param strategy the strategy to use : 
   * IGNORE_FIRST will simply ignore the first entity
   * IGNORE_SECOND will simply ignore the second entity
   * IGNORE_SMALLEST will ignore the entity that covers the less words
   *
   * @return the number of overlapping entities removed
   */
    uint64_t 
      resolveOverlappingEntities(std::vector<RecognizerMatch>& listEntities,
                                 const OverlapResolutionStrategy& 
                                 strategy
                                 = DEFAULT_OVERLAP_STRATEGY) const;
  
  /** 
   * TODO: toBeDeleted
   * Parse tokens paths from the trigger point
   * 
   * @param triggerToken : the token that triggered the rules
   * @param graph        : the graph of the analysis
   * @param current      : the vertex in the graph corresponding to the trigger
   * @param offset       : the offset of the set of rules to test
   * @param results      : the output
   * 
   * @return true if match, false otherwise
  bool parse(const LinguisticAnalysisStructure::Token& triggerToken,
             const LinguisticAnalysisStructure::AnalysisGraph& graph,
             const LinguisticGraphVertex& current,
             uint64_t offset,
             AnalysisContent& analysis,
             RecognizerMatch& result) const;
   */  
  
  /** 
   * test a given set of rules of the recognizer on a text,
   * at a position given by a LinguisticAnalysisStructure::AnalysisGraph 
   * 
   * @param trigger   the TransitionUnit corresponding to the trigger
   * @param rules     the set of rules to test
   * @param position  the position in the text from with to test
   * @param match     the resulting match (if success)
   * @param forbiddenTypes if not 0, contains the types currently
   *        not permitted for the current trigger (in case of negative
   *        rules); updated when leaving the function
   * @param stopAtFirstSuccess if true, stop testing rules 
   * after one rule succeeded
   * @param applySameRuleWhileSuccess if true, when a rule succeeds,
   * retry to apply it on same vertex until the rule does not apply
   * (use with care: setting this argument to true may cause loops if*
   * rules are not well written). Will not apply if stopAtFirstSuccess.
   * 
   * @return true if there is a match : the reference arguments
   * match and rule contain in this case the resulting match 
   * and the rule for wich the test has succeeded
   * the match can be a MultipleMatch or a PIMultipleMatch
   */
  uint64_t testSetOfRules(const TransitionUnit& trigger,
                              const SetOfRules& rules,
                              const LinguisticAnalysisStructure::AnalysisGraph& graph,
                              const LinguisticGraphVertex& position,
                              const LinguisticGraphVertex& begin,
                              const LinguisticGraphVertex& end,
                              AnalysisContent& analysis,
                              std::vector<RecognizerMatch>& match,
                              std::set<Common::MediaticData::EntityType>* forbiddenTypes=0,
                              bool stopAtFirstSuccess=true,
                              bool onlyOneSuccessPerType=false,
                              bool applySameRuleWhileSuccess=false) const ;

  //read recognizer from a text file
  //void readFromTextFile(std::string filename);

  // output the list of triggers with their associated index
  void listTriggers() const;

  // text output
  friend LIMA_AUTOMATON_EXPORT std::ostream& operator << (std::ostream&, const Recognizer&);

  void initializeSearchStructure();

 //***************************************************************************
 protected:
  // set of transitions associated with a set of pointers
  // to rules
  class TriggerRule : public std::pair<TransitionUnit*, SetOfRules> {
  public:
    TriggerRule():
    std::pair<TransitionUnit*, SetOfRules>((TransitionUnit*)0,SetOfRules(0)) {}
    TriggerRule(TransitionUnit* t,const SetOfRules& s):
      std::pair<TransitionUnit*, SetOfRules>(t,s) {}
    ~TriggerRule() {}
    TransitionUnit* transitionUnit() const { return first; }
    void setTransitionUnit(TransitionUnit* t) { first=t; }
    const SetOfRules& setOfRules() const { return second; }
    SetOfRules& setOfRules() { return second; }
  };
  class CompareTriggerRule; // comparison function object on TriggerRule

  std::vector<TriggerRule> m_rules; 
  SetOfRules m_ruleStorage; // the place where the rules are actually 
                            //  stored (dont really need it, but make 
                            //  it simpler to free the memory)
  MediaId m_language;

  /** parameters to control the search of the automaton */
  AutomatonControlParams m_automatonControlParams; 

  // for debug
  std::string m_filename;

  //************************************************************
  // a superstructure on top of m_rules to make the search faster
  TransitionSearchStructure<TriggerRule> m_searchStructure;

  void clearSearchStructure();

  // using TransitionSearchStructure
  void findNextSetOfRules(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                          LinguisticGraphVertex& vertex,
                          AnalysisContent& analysis,
                          const LinguisticAnalysisStructure::Token* token,
                          const LinguisticAnalysisStructure::MorphoSyntacticData* data, 
                          std::vector<TriggerRule*>& matchingSetOfRules) const;
  
  //**********************************************************************
  // helper functions for constructors and destructors
  void init();
  void copy(const Recognizer& a);
  void freeMem();

  void setNormalizedForm(const LimaString& norm,
                         RecognizerMatch& match) const ;

  // for the binary input/output
  uint64_t findRuleIndex(Rule*) const;
 
};

//**********************************************************************
// inline access functions
//**********************************************************************
inline uint64_t Recognizer::numberOfRules() const { 
  return m_rules.size();
}

} // namespace end
} // namespace end
} // namespace end

#endif
