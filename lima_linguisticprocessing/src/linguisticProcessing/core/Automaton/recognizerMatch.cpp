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
 * @file       recognizerMatch.cpp
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Wed Oct 13 2004
 * copyright   Copyright (C) 2004 by CEA LIST
 * 
 ***********************************************************************/

#include "recognizerMatch.h"

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

//***********************************************************************
// constructors
//***********************************************************************
RecognizerMatch::RecognizerMatch(const LinguisticAnalysisStructure::AnalysisGraph* graph):
std::vector<MatchElement>(),
EntityProperties(),
m_graph(graph)
{
}
 
RecognizerMatch::RecognizerMatch(const LinguisticAnalysisStructure::AnalysisGraph* graph, 
                                 const LinguisticGraphVertex& vertex,
                                 const bool isKept):
std::vector<MatchElement>(),
EntityProperties(),
m_graph(graph)
{
  addBackVertex(vertex,isKept);
}

RecognizerMatch::~RecognizerMatch() {
}

// comparison operator
bool RecognizerMatch::operator == (const RecognizerMatch& m) {
  if (m_graph != m.m_graph) {
    return false;
  }    
  if (size() != m.size()) {
    return false;
  }
  std::vector<MatchElement>::const_iterator
    it1=begin(),
    it1_end=end(),
    it2=m.begin();
  for (; it1!=it1_end; it1++,it2++) {
    if ((*it1).m_elem!=(*it2).m_elem) {
      return false;
    }
  }
  if (! EntityProperties::operator==(m)) {
    return false;
  }
  return true;
}

//***********************************************************************
// member functions
//***********************************************************************
void RecognizerMatch::reinit() {

  // do not reinit graph

  std::vector<MatchElement>::clear();
  // clear entity properties
  m_head=0;
  m_type=Common::MediaticData::EntityType();
  m_linguisticProperties=0;
}

// position of first element of the match
uint64_t RecognizerMatch::positionBegin() const {
  if (empty()) { 
    return 0; 
  }
  return get(vertex_token,*(m_graph->getGraph()),
             front().getVertex())->position();
}

// position after the last element of the match
uint64_t RecognizerMatch::positionEnd() const {
  if (empty()) { 
    return 0; 
  }
  Token *t=get(vertex_token,*(m_graph->getGraph()),
               back().getVertex());
  return t->position()+t->length();
}

uint64_t RecognizerMatch::length() const {
  return (positionEnd() - positionBegin());
}

uint64_t RecognizerMatch::numberOfElements() const {
  uint64_t n(0);
//   AULOGINIT;
//   LDEBUG << "RecognizerMatch:numberOfElements";
//   LDEBUG << "RecognizerMatch:this=" << *this;
  for (RecognizerMatch::const_iterator it=begin(),it_end=end();
       it!=it_end; it++) {
    if ((*it).isKept()) {
      n++;
    }
  }
  return n;
}

bool RecognizerMatch::isContiguous() const {
  for (RecognizerMatch::const_iterator it=begin(),it_end=end();
       it!=it_end; it++) {
    if (! (*it).isKept()) {
      return false;
    }
  }
  return true;
}

LimaString RecognizerMatch::getString() const {
  LimaString str;
  uint64_t currentPosition(0);
  if (empty()) { 
    return str;
  }
  RecognizerMatch::const_iterator i(begin());
  const LinguisticGraphVertex& v=(*i).getVertex();
  bool firstHyphenPassed = false;
  if (v != m_graph->firstVertex() &&
      v != m_graph->lastVertex()) {
    if ((*i).isKept()) {
      Token *t = get(vertex_token,*(m_graph->getGraph()),v);
      if (t->status().isAlphaHyphen()) {
        firstHyphenPassed = true;
      }
      if (t!=0) {
        str += t->stringForm();
      }
      currentPosition=t->position()+t->length();
    }
  }
  i++;
  while (i!=end()) {
    const LinguisticGraphVertex& v=(*i).getVertex();
    if (v != m_graph->firstVertex() &&
        v != m_graph->lastVertex()) {
      if ((*i).isKept()) {
        Token *t = get(vertex_token,*(m_graph->getGraph()),v);
        // hack to deal with missing information of what is bewteen
        // the tokens : rely on positions
        if (t->position() > currentPosition) {
          if (t->status().isAlphaHyphen()) {
            if (firstHyphenPassed) {
              str += LimaChar('-');
            }
            else {
              str += LimaChar(' ');
              firstHyphenPassed = true;
            }
          } 
          else {
            str += LimaChar(' ');
            if (firstHyphenPassed) {
              firstHyphenPassed = false;
            }
          }
        }
        str += t->stringForm();
        currentPosition=t->position()+t->length();
      }
    }
    i++;
  }
  return str;
}

LimaString RecognizerMatch::getNormalizedString(const FsaStringsPool& sp) const {
  LimaString str;
  uint64_t currentPosition(0);
  if (empty()) { 
    return str;
  }
  bool firstHyphenPassed = false;
  RecognizerMatch::const_iterator i(begin());
  const LinguisticGraphVertex& v=(*i).getVertex();
  if (v != m_graph->firstVertex() &&
      v != m_graph->lastVertex()) {
    if ((*i).isKept()) {
      Token* t = get(vertex_token,*(m_graph->getGraph()),v);
#ifdef DEBUG_LP
      AULOGINIT;
      LDEBUG << "RecognizerMatch::getNormalizedString(...) token.form(): " << t->form();
      LDEBUG << "RecognizerMatch::getNormalizedString(...) token.stringForm(): " << t->stringForm();
#endif

      if (t->status().isAlphaHyphen()) {
        firstHyphenPassed = true;
      }
      MorphoSyntacticData* data = get(vertex_data,*(m_graph->getGraph()),v);
      
      if (data==0 || data->empty()) {
        str += t->stringForm();
      }
      else {
        // take first norm
#ifdef DEBUG_LP
        AULOGINIT;
        LDEBUG << "RecognizerMatch::getNormalizedString(...) data->front().normalizedForm: " << data->front().normalizedForm;
#endif
        str += sp[data->front().normalizedForm];
      }
      currentPosition=t->position()+t->length();
    }
  }
  i++;
  while (i!=end()) {
    const LinguisticGraphVertex& v=(*i).getVertex();
    if (v != m_graph->firstVertex() &&
        v != m_graph->lastVertex()) {
      if ((*i).isKept()) {
        Token *t = get(vertex_token,*(m_graph->getGraph()),v);
        MorphoSyntacticData* data = get(vertex_data,*(m_graph->getGraph()),v);

        // hack to deal with missing information of what is bewteen
        // the tokens : rely on positions
        if (t->position() > currentPosition) {
          if (t->status().isAlphaHyphen()) {
            if (firstHyphenPassed) {
              str += LimaChar('-');
            }
            else {
              str += LimaChar(' ');
              firstHyphenPassed = true;
            }
          }
          else {
            str += LimaChar(' ');
            if (firstHyphenPassed) {
              firstHyphenPassed = false;
            }
          }
        }

        if (data == 0 || data->empty()) {
          str += t->stringForm();
        }
        else {
          // take first norm
          str += sp[data->front().normalizedForm];
        }

        currentPosition=t->position()+t->length();
      }
    }
    i++;
  }
  return str;
}

bool RecognizerMatch::
isOverlapping(const RecognizerMatch& otherMatch) const {
  if (positionBegin() <= otherMatch.positionBegin()) {
    if (positionEnd() <= otherMatch.positionBegin()) {
      return false;
    }
  }
  else {
    if (positionBegin() >= otherMatch.positionEnd()) {
      return false;
    }
  }
  return true;
}
//**********************************************************************
// construction functions
//**********************************************************************
void RecognizerMatch::addBackVertex(const LinguisticGraphVertex& v,
                                    bool isKept, const LimaString& ruleElementId ) {
#ifdef DEBUG_LP
  AULOGINIT;
  LDEBUG << "RecognizerMatch:addBackVertex(v:" << v << ", isKept:" << isKept << ", ruleElmtId:" << ruleElementId << ")";
#endif
  push_back(MatchElement(v,isKept, ruleElementId));
}
  
void RecognizerMatch::popBackVertex() {
  if (empty()) {
    return;
  }
  pop_back();
}

void RecognizerMatch::addFrontVertex(const LinguisticGraphVertex& v,
                                     bool isKept, const LimaString& ruleElementId) {
#ifdef DEBUG_LP
  AULOGINIT;
  LDEBUG << "RecognizerMatch:addFrontVertex(v:" << v << ", isKept:" << isKept << ", ruleElmtId:" << ruleElementId << ")";
#endif
  insert(begin(),MatchElement(v,isKept,ruleElementId));
}

void RecognizerMatch::popFrontVertex() {
  if (empty()) {
    return;
  }
  erase(begin());
}

void RecognizerMatch::addBack(const RecognizerMatch& l) {
  if( l.getHead() != 0 ){
    setHead(l.getHead());
  }
  insert(end(),l.begin(),l.end());
}
  
void RecognizerMatch::addFront(const RecognizerMatch& l) {
  if( l.getHead() != 0 ){
    setHead(l.getHead());
  }
  insert(begin(),l.begin(),l.end());
}

void RecognizerMatch::removeUnkeptAtExtremity() {
  // remove unkept at beginning
  RecognizerMatch::iterator it=begin();
  while (it != end() && ! (*it).isKept() ) {
    it=erase(it);
  }
  if (it == end()) {
    return;
  }
  // remove unkept at end
  // cannot erase reverse_iterator => use forward iterators
  // go to last element
  RecognizerMatch::iterator next=it;
  next++;
  while (next != end()) {
    next++; it++;
  }
  while (! (*it).isKept() ) {
    next=it;
    it--;
    erase(next);
  }
}


//***********************************************************************
// output
//***********************************************************************
LIMA_AUTOMATON_EXPORT std::ostream& operator << (std::ostream& os, const RecognizerMatch& m) {
  os << "/[-";
  for (RecognizerMatch::const_iterator i(m.begin()); i != m.end(); i++) {
    if ((*i).isKept()) {
      os << (*i).getVertex() << "-";
    }
    else {
      os << "(" << (*i).getVertex() << ")" << "-";
    }
  }
  os << "]";
  os.flush();
  return os;
}

LIMA_AUTOMATON_EXPORT QDebug& operator << (QDebug& os, const RecognizerMatch& m) {
  os << "/[-";
  for (RecognizerMatch::const_iterator i(m.begin()); i != m.end(); i++) {
    if ((*i).isKept()) {
      os << (*i).getVertex() << "-";
    }
    else {
      os << "(" << (*i).getVertex() << ")" << "-";
    }
  }
  os << "]";
  return os;
}

} // end namespace
} // end namespace
} // end namespace
