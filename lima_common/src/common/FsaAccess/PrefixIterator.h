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
#ifndef FSA_PREFIX_ITERATOR_HPP
#define FSA_PREFIX_ITERATOR_HPP

#include <string>

#include "common/Data/LimaString.h"
#include "common/LimaCommon.h"

namespace Lima {
namespace Common {
namespace FsaAccess {

class ForwardPrefixIterator;
class ReversePrefixIterator;

class LIMA_FSAACCESS_EXPORT PrefixIterator {
  friend LIMA_FSAACCESS_EXPORT std::ostream& operator<<(std::ostream& os, const PrefixIterator& t);
public:
  friend class ForwardPrefixIterator;
  friend class ReversePrefixIterator;
  PrefixIterator(const LimaString & word,
                 const uint64_t offset);
  PrefixIterator( const LimaChar*  word_content,
                                  const int32_t word_length, const int32_t wordPos);
  virtual ~PrefixIterator() {}
  
  void print(std::ostream& os) const;
  
  virtual PrefixIterator* clone( const PrefixIterator* const orig ) const = 0;
  virtual char32_t getNextLetter(int32_t& wordOffset) const = 0;
  virtual void next(const int32_t wordOffset) = 0;
  virtual bool hasNextLetter() const = 0;
  int32_t getWordPos() const;
  bool operator==(const PrefixIterator& it) const;
  bool operator!=(const PrefixIterator& it) const;
  virtual int32_t getExternalWordPos() const = 0;
  virtual void terminate() = 0;
  virtual const LimaString getTotalPrefix() const = 0;
  virtual const LimaString getPastPrefix() const = 0;
  virtual const LimaString getCurrentPrefix() const = 0;
  virtual const Lima::LimaChar* getCurrentContent() const = 0;

protected:  
	PrefixIterator& operator=(const PrefixIterator&) {return *this;}
  const Lima::LimaChar* const m_word_content;
  int32_t m_word_length;
  int32_t m_wordPos;
};

class LIMA_FSAACCESS_EXPORT ForwardPrefixIterator : public PrefixIterator {
public:
  int32_t getExternalWordPos() const;
  ForwardPrefixIterator(const LimaString & word,
                        const uint64_t offset=0);
  ForwardPrefixIterator( const LimaChar*  word_content, const int32_t word_length, const int32_t wordPos);
  PrefixIterator* clone( const PrefixIterator* const orig ) const;
  char32_t getNextLetter(int32_t& wordOffset) const;
  bool hasNextLetter() const;
  void next(const int32_t wordOffset);
  void terminate();
  const LimaString getTotalPrefix() const;
  const LimaString getPastPrefix() const;
  const LimaString getCurrentPrefix() const;
  const Lima::LimaChar* getCurrentContent() const;
  int32_t findEdge( const char32_t searchChar, int32_t wordOffset, int32_t highCharTextPos ) const;
private:
	ForwardPrefixIterator& operator=(const ForwardPrefixIterator&) {return *this;}
};

class LIMA_FSAACCESS_EXPORT ReversePrefixIterator : public PrefixIterator{
public:
  int32_t getExternalWordPos() const;
  ReversePrefixIterator(const LimaString & word,
                        const uint64_t offset=0);
  ReversePrefixIterator( const LimaChar*  word_content, const int32_t word_length, const int32_t wordPos);
  PrefixIterator* clone( const PrefixIterator* const orig ) const;
  char32_t getNextLetter(int32_t& wordOffset) const;
  bool hasNextLetter() const;
  void next(const int32_t wordOffset);
  void terminate();
  const LimaString getTotalPrefix() const;
  const LimaString getPastPrefix() const;
  const LimaString getCurrentPrefix() const;
  const Lima::LimaChar* getCurrentContent() const;
private:
  ReversePrefixIterator& operator=(const ReversePrefixIterator&) {return *this;}

};
  
inline int32_t PrefixIterator::getWordPos() const {
 return m_wordPos;
}

} // namespace FsaAccess
} // namespace Common
} // namespace Lima

#endif   //FSA_PREFIX_ITERATOR_HPP
