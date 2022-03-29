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

/** =======================================================================
    @file       ltrText.cpp

    @version    $Id$
    @date       created       jul 6, 2004
    @date       last revised  nov 25, 2010

    @author     Olivier Ferret
    @brief      a linear text representation

    Copyright (C) 2004-2010 by CEA LIST

    ======================================================================= */

#include "ltrText.h"

#include "common/Data/readwritetools.h"


using namespace Lima::Common::Misc;


namespace Lima {
namespace Common {
namespace BagOfWords {


/**
  * @class  LTR_Text
  */

// -----------------------------------------------------------------------------
// -- destructor
// -----------------------------------------------------------------------------

LTR_Text::~LTR_Text() {

    // free tokens
    for (LTR_Text::const_iterator itTok = this->begin();
         itTok != this->end(); ++ itTok) {
        delete *itTok;
    }
}


// -----------------------------------------------------------------------------
// -- copying
// -----------------------------------------------------------------------------

LTR_Text::LTR_Text(const LTR_Text& text):
std::vector<LTR_Token*>()
{

    this->reserve(text.size());
    for (LTR_Text::const_iterator itTok = text.begin();
         itTok != text.end(); ++ itTok) {
        this->push_back(new LTR_Token(**itTok));
    }
    for (SENTENCE_BOUNDS_T::const_iterator itSentBd = text.m_sentenceBounds.begin();
         itSentBd != text.m_sentenceBounds.end(); ++ itSentBd) {
        m_sentenceBounds.push_back(*itSentBd);
    }
}


// -----------------------------------------------------------------------------
// -- adding
// -----------------------------------------------------------------------------

void LTR_Text::appendLTR_Text(const LTR_Text& ltr) {

    LTR_Text::size_type thiSize = this->size();
    // copy of tokens
    this->reserve(thiSize + ltr.size());
    for (LTR_Text::const_iterator itTok = ltr.begin();
         itTok != ltr.end(); ++ itTok) {
        LTR_Token* tok = new LTR_Token(**itTok);
        this->push_back(tok);
    }
    // add sentence bounds
    for (LTR_Text::SENTS_CONST_ITER_T itSentBd = ltr.beginSentenceBounds();
         itSentBd != ltr.endSentenceBounds(); ++ itSentBd) {
        m_sentenceBounds.push_back(*itSentBd + thiSize);
    }
}


// -----------------------------------------------------------------------------
// -- reading/writing
// -----------------------------------------------------------------------------

void LTR_Text::binaryWriteOn(std::ostream& os) const {

    uint64_t tokenCounter = 0;
    writeCodedInt(os, this->size());
    if (this->size()==0) {
      return;
    }
    SENTENCE_BOUNDS_T:: const_iterator itSb = m_sentenceBounds.begin();
    writeCodedInt(os, *itSb);
    for (LTR_Text::const_iterator itTok = this->begin();
         itTok != this->end(); itTok ++) {
        (*itTok)->binaryWriteOn(os);
        tokenCounter ++;
        if (tokenCounter == *itSb) {
            itSb ++;
            if (itSb != m_sentenceBounds.end()) {
                writeCodedInt(os, *itSb);
            }
        }
    }
    // dump named entities
//std::cerr << "-- number of NE: " << m_namedEntities.size() << std::endl;
/*    writeCodedInt(os, m_namedEntities.size());
    for (NAMED_ENTITIES_T::const_iterator itNe = m_namedEntities.begin();
         itNe != m_namedEntities.end(); itNe ++) {
        itNe->first->write(os);
        writeCodedInt(os, itNe->second);
    }*/    
}

void LTR_Text::binaryReadFrom(std::istream& is) {

    uint64_t tokenNb = readCodedInt(is);
    uint64_t tokenCounter = 1;
    while (tokenCounter <= tokenNb) {
        uint64_t sentenceBound = readCodedInt(is);
        m_sentenceBounds.push_back(sentenceBound);
        for (; tokenCounter <= sentenceBound; tokenCounter ++) {
            LTR_Token* tok = new LTR_Token();
            tok->binaryReadFrom(is);
            this->push_back(tok);
        }
    }
}


// -----------------------------------------------------------------------------
// -- printing
// -----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const LTR_Text& text) {
  
  uint64_t tokenCounter = 1;
  LTR_Text::SENTENCE_BOUNDS_T::const_iterator itSb = text.m_sentenceBounds.begin();
  for (LTR_Text::const_iterator itTok = text.begin();
       itTok != text.end(); itTok ++) {
    LTR_Token* tok=*itTok;
    if (tok==nullptr) { os << "NULL" << std::endl; } 
    else if (tok->size()==0) { os << "EMPTY" << std::endl; }
    else { os << *tok << std::endl; }

    if (itSb != text.m_sentenceBounds.end()) {
      if (tokenCounter == *itSb) {
        os << "==SB==========" << std::endl;
        itSb ++;
      }
    }
    tokenCounter ++;
  }
  return os;
}

QDebug& operator<<(QDebug& os, const LTR_Text& text) {
  
  uint64_t tokenCounter = 1;
  LTR_Text::SENTENCE_BOUNDS_T::const_iterator itSb = text.m_sentenceBounds.begin();
  for (LTR_Text::const_iterator itTok = text.begin();
       itTok != text.end(); itTok ++) {
    if (*itTok==nullptr) { os << "NULL"; } 
    else if ((*itTok)->size()==0) { os << "EMPTY"; }
    else { os << **itTok; }
    
    if (itSb != text.m_sentenceBounds.end()) {
      if (tokenCounter == *itSb) {
        os << "==SB==========";
        itSb ++;
      }
    }
    tokenCounter ++;
  }
  return os;
}

}  // BagOfWords
}  // Common
}  // Lima
