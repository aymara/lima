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
/*************************************************************************
*
* File        : gazeteerTransition.cpp
* Author      : Olivier Mesnard (olivier.mesnard@cea.fr)
* @date       Thu August 04 2015
* copyright   Copyright (C) 2002-2015 by CEA LIST
* Version     : $Id$
*
*************************************************************************/


#include "gazeteerTransition.h"
#include "common/MediaticData/mediaticData.h"
#include <iostream>
#include <boost/tuple/tuple.hpp> // for tie
#include "searchGraph.h"

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::MediaticData;


namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

/***********************************************************************/
// constructors
/***********************************************************************/
GazeteerTransition::GazeteerTransition():
TransitionUnit(),
m_wordSet(),
m_alias()
{
}

GazeteerTransition::GazeteerTransition(const std::vector<LimaString>& wordSet, const LimaString& alias, bool keep):
TransitionUnit(keep),
m_wordSet(wordSet.begin(),wordSet.end()),
m_alias(alias)
{
}

GazeteerTransition::GazeteerTransition(const GazeteerTransition& t):
TransitionUnit(t),
m_wordSet(t.m_wordSet),
m_alias(t.m_alias)
{
// TODO ToBeDeleted ?
  //   copyProperties(t);
}

GazeteerTransition::~GazeteerTransition() {}

GazeteerTransition& GazeteerTransition::operator = (const GazeteerTransition& t) {
  if (this != &t) {
    m_alias = t.alias();
    copyProperties(t);
  }
  return *this;
}


std::string GazeteerTransition::printValue() const {
  ostringstream oss;
  oss << m_alias.data()->unicode();
  return oss.str();
}

/***********************************************************************/
// operators ==
/***********************************************************************/
bool GazeteerTransition::operator== (const TransitionUnit& tright) const {
  if ( (type() == tright.type())
       && (m_alias == static_cast<const GazeteerTransition&>(tright).alias())
       ) {
    return compareProperties(tright);
  }
  else {
    return false;
  }
}

bool GazeteerTransition::
compare(const LinguisticAnalysisStructure::AnalysisGraph& /*graph*/,
        const LinguisticGraphVertex& /*vertex*/,
        AnalysisContent& /*analysis*/,
        const LinguisticAnalysisStructure::Token* token,
        const LinguisticAnalysisStructure::MorphoSyntacticData* /*data*/) const
{
  //AULOGINIT;
//   LDEBUG << "GazeteerTransition compare " << Common::MediaticData::MediaticData::changeable().stringsPool()[token->form()] << " and " << Common::MediaticData::MediaticData::changeable().stringsPool()[m_word];
  QString form(token->stringForm());
  std::set<LimaString>::const_iterator it = m_wordSet.lower_bound(form);
  if( it == m_wordSet.end() ) {
    return false;
  }
  QString element = *it;
  // If element is equal to form
  if( element == form )
  {
    return true;
  }
  // Or element begin with form followed by a space character
  if( element.startsWith(form) )
  {
    if( element.at(form.length()) == ' ')
    {
      return true;
    }
  }
  /*
  QString pattern(form);
  pattern.append("\\b");
  QRegExp rx(pattern);
  int index = qStringList.indexOf(rx);
  */
//    return true;
    return false;
}

/* Gazeteer may contains multi-term elements like  */
/* "managing director","Managing Director","managing editor","managing comitee secretary"... */
/* From wordSet, we build a list of multiple terms, each with parameter firstSimpleTerm as first simple term */
/* [("managing,director");("managing,Director");("managing,editor");("managing,comitee,secretary")] */
/* return false if there is no elements begining with "managing"  */
bool GazeteerTransition::
buildNextTermsList( const LimaString& firstSimpleTerm, std::vector<std::vector<LimaString> >& multiTermList ) const
{
  AULOGINIT;
  LDEBUG << "GazeteerTransition::buildNextTermsList(" << firstSimpleTerm << ")";
  
  // Fill list of list of additional simple terms from list of elements
  std::set<LimaString>::const_iterator it = m_wordSet.lower_bound(firstSimpleTerm);
  if( it == m_wordSet.end() ) {
    LDEBUG << "GazeteerTransition::buildNextTermsList: Error: first term not found";
    return false;
  }
  for( ; it != m_wordSet.end() ; it++ )
  {
    LimaString element = *it;
    LDEBUG << "GazeteerTransition::buildNextTermsList: Examining " << element.toStdString();
    // if element does not start with firstSimpleTerm, there no more possible match 
    if( !element.startsWith(firstSimpleTerm) ) {
      LDEBUG << "GazeteerTransition::buildNextTermsList: stop it!: first term not found";
      break;
    }
    std::vector<LimaString> multiTerm;
    // if element equals the token, we push a vector with a unique element, and go to the next element
    if( element == firstSimpleTerm ) {
      LDEBUG << "GazeteerTransition::buildNextTermsList: push back in multiTermList singleton " << firstSimpleTerm.toStdString();
      multiTerm.push_back(firstSimpleTerm);
      multiTermList.push_back(multiTerm);
      continue;
    }
    // within element, if firstSimpleTerm is not followed by others simple terms separated with space
    // first term is only a prefix and does not match exactly firstSimpleTerm, go to the next element
    int pos(0);
    int index = element.indexOf(' ', pos);
    LDEBUG << "GazeteerTransition::buildNextTermsList: pos = " << pos << ", index=" << index;
    if( index != firstSimpleTerm.length() ) {
      LDEBUG << "GazeteerTransition::buildNextTermsList: no second term for " << element.toStdString();
      continue;
    }
    else {
      LDEBUG << "GazeteerTransition::buildNextTermsList: push back in multiterm " << firstSimpleTerm.toStdString();
      multiTerm.push_back(firstSimpleTerm);
    }
    // build list of elements following firstSimpleTerm
    for( ; ; ) {
      pos = index+1;
      index = element.indexOf(' ', pos);
      LDEBUG << "GazeteerTransition::buildNextTermsList: pos = " << pos << ", index=" << index;
      if( index == -1 ) {
        LDEBUG << "GazeteerTransition::buildNextTermsList: add last term " << element.mid(pos).toStdString();
        multiTerm.push_back(element.mid(pos));
        break;
      }
      else
      {
        LDEBUG << "GazeteerTransition::buildNextTermsList: add term " << element.mid(pos,index-pos).toStdString();
        multiTerm.push_back(element.mid(pos,index-pos));
      }
    }
    LDEBUG << "GazeteerTransition::buildNextTermsList: push back list of " << multiTerm.size() << " elements";
    multiTermList.push_back(multiTerm);
  }
  return( multiTermList.size() > 0 );
}
  
bool GazeteerTransition::
checkMultiTerms( const LinguisticAnalysisStructure::AnalysisGraph& aGraph,
             const LinguisticGraphVertex& position,
             const LinguisticGraphVertex& begin,
             const LinguisticGraphVertex& end,
             AnalysisContent& analysis,
             const std::vector<std::vector<LimaString> >& multiTermListCandidates,
             std::stack<std::stack<LinguisticGraphVertex,std::vector<LinguisticGraphVertex> >,
                        std::vector<std::stack<LinguisticGraphVertex> > >& matches
    ) const {
  
               
  AULOGINIT;
  LDEBUG << "GazeteerTransition::checkMultiTerms( from " << position << ")";
  // Iteration on multi-terms from gazeteer whose first term matches current token
  std::vector<std::vector<LimaString> >::const_iterator multiTermsIt = multiTermListCandidates.begin();
  const LinguisticGraph* lGraph = aGraph.getGraph();
  for( ; multiTermsIt != multiTermListCandidates.end() ; multiTermsIt++ ) {
    // iterator for simpleterms
    std::vector<LimaString>::const_iterator termsIt = (*multiTermsIt).begin();
    std::vector<LimaString>::const_iterator termsIt_end = (*multiTermsIt).end();
    LDEBUG << "GazeteerTransition::checkMultiTerms: check multi-term ("
           << *termsIt << " and " << (*multiTermsIt).size()-1 << " more...)";
    // For each list of simple Terms, we make a deep first search in the graph
    // searchPos stores a stack of position in the graph to perform the deep first search
    ForwardSearch searchPos;
    // the completed path is stored in a stack of vertex (initialized with position)
    std::stack<LinguisticGraphVertex> triggerMatch;
    triggerMatch.push(position);
    termsIt++;
    // init search from position
    searchPos.findNextVertices(lGraph, position);
    // init current position
    LinguisticGraphVertex nextVertex = position;
    // if list is not exhausted
    
    // case of empty list of simple term
    if(termsIt == termsIt_end ) {
      // Error! 
      LDEBUG << "GazeteerTransition::checkMultiTerms: list of simple terms is a singleton!";
      matches.push(triggerMatch);
      //matches.push(triggerMatch);
    }
    else {
      // go one step ahead from curentPosition if possible
      while ( searchPos.getNextVertex(lGraph, nextVertex )) {
        LDEBUG << "GazeteerTransition::checkMultiTerms: progress one step forward, nextVertex=" << nextVertex;
        LDEBUG << "GazeteerTransition::checkMultiTerms: test " << *termsIt;
        // test currentVertex
        Token* token = get(vertex_token, *lGraph, nextVertex);
        LimaString form(token->stringForm());
        if( form == *termsIt ) {
          LDEBUG << "GazeteerTransition::checkMultiTerms: match with " << *termsIt;
          //  If match, push vertex in triggerMatch and initialize next step
          // Push out_edge is a better if we have to follow the path from the begining ???
          triggerMatch.push(nextVertex);
          // stack next step to continue the search
          searchPos.findNextVertices(lGraph, nextVertex);
          termsIt++;
          if(termsIt == termsIt_end ) {
            LDEBUG << "GazeteerTransition::checkMultiTerms: list of simple terms exhausted!";
            // list of Simple term exhausted: success
            // we push the path in the aGraph as a solution of triggerMatch
            // Only if size of solution is greater than previous one !!
            if( matches.empty() || (triggerMatch.size() > matches.top().size()) ) {
              LDEBUG << "GazeteerTransition::checkMultiTerms: push stack of size " << triggerMatch.size();
              matches.push(triggerMatch);
            }
            // no need to go forward
            break;
          }
          // else we do not stack next steps, we obtain a cut
        }
      }
    }
  }
  if( matches.empty() )
    return false;
  return true;
}
     
} // namespace end
} // namespace end
} // namespace end
