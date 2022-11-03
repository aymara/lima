// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  int32_t getExternalWordPos() const override;
  ForwardPrefixIterator(const LimaString & word,
                        const uint64_t offset=0);
  ForwardPrefixIterator( const LimaChar*  word_content, const int32_t word_length, const int32_t wordPos);
  PrefixIterator* clone( const PrefixIterator* const orig ) const override;
  char32_t getNextLetter(int32_t& wordOffset) const override;
  bool hasNextLetter() const override;
  void next(const int32_t wordOffset) override;
  void terminate() override;
  const LimaString getTotalPrefix() const override;
  const LimaString getPastPrefix() const override;
  const LimaString getCurrentPrefix() const override;
  const Lima::LimaChar* getCurrentContent() const override;
  int32_t findEdge( const char32_t searchChar, int32_t wordOffset, int32_t highCharTextPos ) const;
private:
	ForwardPrefixIterator& operator=(const ForwardPrefixIterator&) {return *this;}
};

class LIMA_FSAACCESS_EXPORT ReversePrefixIterator : public PrefixIterator{
public:
  int32_t getExternalWordPos() const override;
  ReversePrefixIterator(const LimaString & word,
                        const uint64_t offset=0);
  ReversePrefixIterator( const LimaChar*  word_content, const int32_t word_length, const int32_t wordPos);
  PrefixIterator* clone( const PrefixIterator* const orig ) const override;
  char32_t getNextLetter(int32_t& wordOffset) const override;
  bool hasNextLetter() const override;
  void next(const int32_t wordOffset) override;
  void terminate() override;
  const LimaString getTotalPrefix() const override;
  const LimaString getPastPrefix() const override;
  const LimaString getCurrentPrefix() const override;
  const Lima::LimaChar* getCurrentContent() const override;
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
