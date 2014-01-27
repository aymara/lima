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
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDICO_ANALYSISDICOITERATORS_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDICO_ANALYSISDICOITERATORS_H

#include "AnalysisDictExport.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include "AbstractDictionaryEntry.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace AnalysisDict
{

/**
* @brief iterator on subwords
*/
class LIMA_ANALYSISDICT_EXPORT AbstractDictionarySubWordIterator
{
public:

  /**
  * @brief return subword data
  * SubWord data are the ending offset of the subword, \
  * and the corresponding dictionary entry.
  * @return ending offset and dictionary entry.
  */
  virtual const std::pair<uint64_t, DictionaryEntry> operator*() const = 0;

  virtual AbstractDictionarySubWordIterator& operator++(int i) = 0;
  virtual bool operator==(const AbstractDictionarySubWordIterator& it) const = 0;
  virtual bool operator!=(const AbstractDictionarySubWordIterator& it) const = 0;
  
  virtual AbstractDictionarySubWordIterator* clone() const = 0;
  virtual ~AbstractDictionarySubWordIterator() {};
};

/**
* @brief iterator on superwords
*/
class LIMA_ANALYSISDICT_EXPORT AbstractDictionarySuperWordIterator
{
public:
  /**
  * @brief return superword
  * @return superword string
  */
  virtual const LimaString operator*() const = 0;
 
  virtual AbstractDictionarySuperWordIterator &operator++(int) = 0;
  virtual bool operator==(const AbstractDictionarySuperWordIterator& it) const = 0;
  virtual bool operator!=(const AbstractDictionarySuperWordIterator& it) const = 0;
  
  virtual AbstractDictionarySuperWordIterator* clone() const = 0;
  virtual ~AbstractDictionarySuperWordIterator() {};
};

class LIMA_ANALYSISDICT_EXPORT DictionarySubWordIterator
{
public:
  DictionarySubWordIterator(AbstractDictionarySubWordIterator* delegate);
  DictionarySubWordIterator(const DictionarySubWordIterator& source);
  virtual ~DictionarySubWordIterator();
  DictionarySubWordIterator& operator=(const DictionarySubWordIterator& source);

  inline const std::pair<uint64_t,DictionaryEntry> operator*() const;
  inline AbstractDictionarySubWordIterator& operator++(int i);
  inline bool operator==(const DictionarySubWordIterator& it) const;
  inline bool operator!=(const DictionarySubWordIterator& it) const;
private:
  AbstractDictionarySubWordIterator* m_delegate;
};

inline const std::pair<uint64_t,DictionaryEntry>
    DictionarySubWordIterator::operator*() const 
{
  return m_delegate->operator*();
}

inline AbstractDictionarySubWordIterator& DictionarySubWordIterator::operator++(int i)
{
  return m_delegate->operator++(i);
}
  
inline bool DictionarySubWordIterator::operator==(const DictionarySubWordIterator& abit) const
{
  return m_delegate->operator==(*(abit.m_delegate));
}

inline bool DictionarySubWordIterator::operator!=(const DictionarySubWordIterator& abit) const
{
  return m_delegate->operator!=(*(abit.m_delegate));
}

class LIMA_ANALYSISDICT_EXPORT DictionarySuperWordIterator
{
public:
  DictionarySuperWordIterator(AbstractDictionarySuperWordIterator* delegate);
  DictionarySuperWordIterator(const DictionarySuperWordIterator& source);
  virtual ~DictionarySuperWordIterator();
  DictionarySuperWordIterator& operator=(const DictionarySuperWordIterator& source);

  inline const LimaString operator*() const;
  inline AbstractDictionarySuperWordIterator &operator++(int);
  inline bool operator==(const DictionarySuperWordIterator& it) const;
  inline bool operator!=(const DictionarySuperWordIterator& it) const;
private:
  AbstractDictionarySuperWordIterator* m_delegate;
};

inline const LimaString DictionarySuperWordIterator::operator*() const
{
  return m_delegate->operator*();
}

inline AbstractDictionarySuperWordIterator& DictionarySuperWordIterator::operator++(int i)
{
  return m_delegate->operator++(i);
}
  
inline bool DictionarySuperWordIterator::operator==(const DictionarySuperWordIterator& abit) const
{
  return m_delegate->operator==(*(abit.m_delegate));
}

inline bool DictionarySuperWordIterator::operator!=(const DictionarySuperWordIterator& abit) const
{
  return m_delegate->operator==(*(abit.m_delegate));
}

} // namespace AnalysisDict
} // namespace LinguisticProcessing
} // namespace Lima

#endif
