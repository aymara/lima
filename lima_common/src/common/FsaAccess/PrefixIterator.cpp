// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "common/FsaAccess/PrefixIterator.h"
#include "common/Data/strwstrtools.h"
namespace Lima {
namespace Common {
namespace FsaAccess {

PrefixIterator::PrefixIterator(const LimaString & word,
                               const uint64_t wordPos)
// TODO: test with word.str()
  : m_word_content(word.data()), m_word_length(word.length()), m_wordPos(wordPos) {
}

PrefixIterator::PrefixIterator( const LimaChar*  word_content, const int32_t word_length, const int32_t wordPos)
  : m_word_content(word_content), m_word_length(word_length), m_wordPos(wordPos) {
}
  
std::ostream& operator<<(std::ostream& os, const PrefixIterator& ctx) {
  ctx.print(os);
  return os;
}

void PrefixIterator::print(std::ostream& os) const {
  os << "SpWCtx = { " << Lima::Common::Misc::limastring2utf8stdstring(
                          LimaString(m_word_content))
     << ", word_length=" << m_word_length
     << ", wordPos=" << m_wordPos << "}" << std::endl;
}

bool PrefixIterator::operator==(const PrefixIterator& it) const {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG << "PrefixIterator::operator==():"
         << "m_word_content=" << m_word_content << ", m_word_length = " << m_word_length
         << "it.m_word_content=" << it.m_word_content << ", it.m_word_length = " << it.m_word_length;
#endif
  return( (m_word_content == it.m_word_content)
   &&  (m_wordPos == it.m_wordPos ) );
}

bool PrefixIterator::operator!=(const PrefixIterator& it) const {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG << "PrefixIterator::operator!=():"
         << "m_word_content=" << m_word_content << ", m_word_length = " << m_word_length
         << "it.m_word_content=" << it.m_word_content << ", it.m_word_length = " << it.m_word_length;
#endif
  return( (m_word_content != it.m_word_content)
   ||  (m_wordPos != it.m_wordPos ) );
}

ForwardPrefixIterator::ForwardPrefixIterator(const LimaString & word,
                                             const uint64_t wordPos)
  : PrefixIterator(word,wordPos) {
}

ForwardPrefixIterator::ForwardPrefixIterator( const LimaChar*  word_content, const int32_t word_length, const int32_t wordPos)
  : PrefixIterator( word_content, word_length, wordPos ) {
}


ReversePrefixIterator::ReversePrefixIterator(const LimaString & word,
                                             const uint64_t wordPos)
  : PrefixIterator(word,word.length()-wordPos) {
}

ReversePrefixIterator::ReversePrefixIterator( const LimaChar*  word_content, const int32_t word_length, const int32_t wordPos)
  : PrefixIterator( word_content, word_length, wordPos) {
}

  
bool ForwardPrefixIterator::hasNextLetter() const {
// #ifdef DEBUG_CD
//   FSAALOGINIT;
//   LDEBUG <<  "ForwardPrefixIterator::hasNextLetter: m_wordPos = "
//             << m_wordPos << ", m_word_length = " << m_word_length;
// #endif

  return( m_wordPos < m_word_length );
}

bool ReversePrefixIterator::hasNextLetter() const {
  return( m_wordPos > 0 );
}

char32_t ForwardPrefixIterator::getNextLetter(int32_t& wordOffset) const {
    
  char32_t result;
  int32_t wordPos0 = m_wordPos;

  U16_NEXT(m_word_content, wordPos0, m_word_length, result);
  wordOffset = wordPos0 - m_wordPos;
  return result;
}

void ForwardPrefixIterator::next( const int32_t wordOffset ) {
  m_wordPos = m_wordPos + wordOffset;
}

char32_t ReversePrefixIterator::getNextLetter(int32_t& wordOffset) const {
    
  char32_t result;
  int32_t wordPos0 = m_wordPos;

  U16_PREV(m_word_content, 0, wordPos0, result);
  wordOffset = m_wordPos - wordPos0;
  return result;
}

void ReversePrefixIterator::next( const int32_t wordOffset ) {
  m_wordPos = m_wordPos - wordOffset;
}

int32_t ForwardPrefixIterator::getExternalWordPos() const {
 return( m_wordPos );
}


int32_t ReversePrefixIterator::getExternalWordPos() const {
  return( m_word_length - m_wordPos );
}

void ForwardPrefixIterator::terminate() {
  m_wordPos = m_word_length;
}

void ReversePrefixIterator::terminate() {
  m_wordPos = 0;
}

PrefixIterator* ForwardPrefixIterator::clone( const PrefixIterator* const orig ) const {
  PrefixIterator* result = new ForwardPrefixIterator( orig->m_word_content, orig->m_word_length, orig->m_wordPos );
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG << "ForwardPrefixIterator::clone: "
         << "m_word_content=" << result->m_word_content << ", m_word_length=" << result->m_word_length << ", wordPos=" << result->m_wordPos << "\n"
         << "                              "
         << "orig.m_word_content=" << orig->m_word_content << ", it.m_word_length = " << orig->m_word_length << ", wordPos=" << orig->m_wordPos;
#endif
  return( result );
//  return( new ForwardPrefixIterator( orig->m_word_content, orig->m_word_length, orig->m_wordPos ) );
}

PrefixIterator* ReversePrefixIterator::clone( const PrefixIterator* const orig ) const {
  return( new ReversePrefixIterator( orig->m_word_content, orig->m_word_length, orig->m_wordPos ) );
}

const LimaString ForwardPrefixIterator::getTotalPrefix() const {
  return LimaString(m_word_content).left(m_word_length);
}

const LimaString ForwardPrefixIterator::getPastPrefix() const {
 return LimaString(m_word_content).left(m_wordPos );
}

const LimaString ForwardPrefixIterator::getCurrentPrefix() const {
 return LimaString(m_word_content).mid(m_wordPos, m_word_length);
}

const LimaString ReversePrefixIterator::getTotalPrefix() const {
  return LimaString(m_word_content).left(m_word_length);
}

const LimaString ReversePrefixIterator::getPastPrefix() const {
 return LimaString(m_word_content).mid(m_wordPos, m_word_length);
}

const LimaString ReversePrefixIterator::getCurrentPrefix() const {
 return LimaString(m_word_content).left(m_wordPos );
}

const Lima::LimaChar* ForwardPrefixIterator::getCurrentContent() const {
  return( m_word_content + m_wordPos );
}

const Lima::LimaChar* ReversePrefixIterator::getCurrentContent() const {
  return( m_word_content + m_word_length - m_wordPos );
}

/*
 * to find the right offset in the vector of out_edge:
 * search for the character currentChar in the string text
 * which contains label of all out_edges using dichotomy search
 * (assume characters are ordered in textString)
 */
int32_t ForwardPrefixIterator::findEdge(
  const char32_t searchChar, int32_t wordOffset, int32_t highCharTextPos ) const {

  // letter searchChar is defined with 1 16bits word
  if( wordOffset > 1 ) {
    int32_t min = 0;
    int range = highCharTextPos;
#ifdef DEBUG_CD
  FSAALOGINIT;
  LTRACE <<  "ForwardPrefixIterator::findEdge(" << searchChar << "): min = " << min
            << " range = " << range;
#endif
    char32_t edgeLabel;
    int32_t offset = min;
    while( range > 0 ) {
      offset = min + range/2;
      int32_t offset0 = offset;
#ifdef DEBUG_CD
      LTRACE <<  "ForwardPrefixIterator::findEdge(1): min = " << min
             << " range = " << range << " try " << offset;
#endif
      U16_NEXT(m_word_content, offset, m_word_length, edgeLabel);
      if( edgeLabel == searchChar ) {
#ifdef DEBUG_CD
      LTRACE <<  "ForwardPrefixIterator::findEdge(1): match " << edgeLabel << " return " << offset0;
#endif
        return offset0;
      }
      else if( searchChar > edgeLabel ){
        range = min + range - offset0 - 1;
        min = offset0 + 1;
      }
      else {
        range = offset0 - min;
      }
    }
#ifdef DEBUG_CD
    LTRACE <<  "ForwardPrefixIterator::findEdge(1): min = " << min
           << " range = " << range << " try " << min;
#endif
    int32_t offset0 = min;
    U16_NEXT(m_word_content, min, m_word_length, edgeLabel);
    if( edgeLabel == searchChar ) {
#ifdef DEBUG_CD
      LTRACE <<  "ForwardPrefixIterator::findEdge(1): match " << edgeLabel << " return " << offset0;
#endif
      return offset0;
    }
    else {
#ifdef DEBUG_CD
      LTRACE <<  "ForwardPrefixIterator::findEdge(1): unmatch " << edgeLabel << " and " << searchChar;
#endif
      return -1;
    }
  }
  // letter searchChar is defined with 2 16bits words
  else {
    int32_t min = highCharTextPos;
    int range = (m_word_length - min)/2;
#ifdef DEBUG_CD
  FSAALOGINIT;
  LTRACE <<  "ForwardPrefixIterator::findEdge(" << searchChar << "): min = " << min
            << " range = " << range;
#endif
    char32_t edgeLabel;
    int32_t offset = min;
    while( range > 0 ) {
      offset = 2*(min + range/2);
      int32_t offset0 = offset;
#ifdef DEBUG_CD
      LTRACE <<  "ForwardPrefixIterator::findEdge(2): min = " << min
             << " range = " << range << " try " << offset;
#endif
      U16_NEXT(m_word_content, offset, m_word_length, edgeLabel);
      if( edgeLabel == searchChar ) {
#ifdef DEBUG_CD
      LTRACE <<  "ForwardPrefixIterator::findEdge(2): match " << edgeLabel << " return " << offset0/2;
#endif
        return offset0/2;
      }
      else if( searchChar > edgeLabel ){
        range = min + range - offset0 - 2;
        min = 2*(offset0 + 1);
      }
      else {
        range = offset0 - min;
      }
    }
#ifdef DEBUG_CD
    LTRACE <<  "ForwardPrefixIterator::findEdge(2): min = " << min
           << " range = " << range << " try " << min;
#endif
    int32_t offset0 = min;
    U16_NEXT(m_word_content, min, m_word_length, edgeLabel);
    if( edgeLabel == searchChar ) {
#ifdef DEBUG_CD
      LTRACE <<  "ForwardPrefixIterator::findEdge(2): match " << edgeLabel << " return " << offset0/2;
#endif
      return (offset0/2);
    }
    else {
#ifdef DEBUG_CD
      LTRACE <<  "ForwardPrefixIterator::findEdge(2): unmatch " << edgeLabel << " and " << searchChar;
#endif
      return -1;
    }
  }
}



} // namespace FsaAccess
} // namespace Common
} // namespace Lima
