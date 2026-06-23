// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
// -- copying
// -----------------------------------------------------------------------------

LTR_Text::LTR_Text(const LTR_Text& text):
std::vector<boost::shared_ptr<LTR_Token>>()
{
    this->reserve(text.size());
    for (LTR_Text::const_iterator itTok = text.begin();
         itTok != text.end(); ++ itTok) {
        this->push_back( *itTok );
    }
    for (SENTENCE_BOUNDS_T::const_iterator itSentBd = text.m_sentenceBounds.begin();
         itSentBd != text.m_sentenceBounds.end(); ++ itSentBd) {
        m_sentenceBounds.push_back(*itSentBd);
    }
}


// -----------------------------------------------------------------------------
// -- adding
// -----------------------------------------------------------------------------

void LTR_Text::addToken(boost::shared_ptr<LTR_Token>& token) {
  this->push_back(token);
}

void LTR_Text::addSentenceBound(const uint64_t& sb) {
    m_sentenceBounds.push_back(sb);
}

void LTR_Text::appendLTR_Text(const LTR_Text& ltr) {

    LTR_Text::size_type origSize = this->size();
    // copy of tokens
    this->reserve(origSize + ltr.size());
    for (LTR_Text::const_iterator itTok = ltr.begin();
         itTok != ltr.end(); ++ itTok) {
        this->push_back( *itTok );
    }
    // add sentence bounds
    for (LTR_Text::SENTS_CONST_ITER_T itSentBd = ltr.beginSentenceBounds();
         itSentBd != ltr.endSentenceBounds(); ++ itSentBd) {
        m_sentenceBounds.push_back(*itSentBd + origSize);
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
            boost::shared_ptr<LTR_Token> tok( new LTR_Token() );
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
    if (*itTok==nullptr) { os << "NULL" << std::endl; }
    else if ((*itTok)->size()==0) { os << "EMPTY" << std::endl; }
    else { os << **itTok << std::endl; }

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
