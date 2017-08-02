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
 *   Copyright (C) 2004-2012 by CEA LIST                      *
 *                                                                         *
 ***************************************************************************/

 
#ifndef LIMA_COMMON_ABSTRACTACCESSITERATORS_H
#define LIMA_COMMON_ABSTRACTACCESSITERATORS_H

#include "common/LimaCommon.h"
#include "common/Data/LimaString.h"

namespace Lima
{
namespace Common
{

/**
* @brief defines abstract interface for iterators on subwords
*/
class AbstractSubWordIterator
{
public:
  /**
   * gives subword data. First, the offset of subword end in the given string.
   * Second, the index of the subword.
   * @see AbstractAccessByString#getSubWords
   * @return pair of <end offset, index>
   */
  virtual const std::pair<uint64_t, uint64_t> operator*() const = 0;
  virtual AbstractSubWordIterator &operator++(int) = 0;
  virtual bool operator==(const AbstractSubWordIterator& it) const = 0;
  virtual bool operator!=(const AbstractSubWordIterator& it) const = 0;
  virtual ~AbstractSubWordIterator() {};
};

/**
* @brief defines abstract interface for iterators on superwords
*/
class AbstractSuperWordIterator
{
public:
  virtual const Lima::LimaString operator*() const = 0;
  virtual AbstractSuperWordIterator &operator++(int) = 0;
  virtual bool operator==(const AbstractSuperWordIterator& it) const = 0;
  virtual bool operator!=(const AbstractSuperWordIterator& it) const = 0;
  virtual ~AbstractSuperWordIterator() {};
};

/**
* @brief clonable subworditerator
* specific subworditerator for a specific access method should inherits from this class
*/
class ClonableSubWordIterator : public AbstractSubWordIterator 
{
public:
  virtual ClonableSubWordIterator* clone() const = 0;
};

/**
* @brief clonable superworditerator
* specific superworditerator for a specific access method should inherits from this class
*/
class ClonableSuperWordIterator : public AbstractSuperWordIterator 
{
public:
  virtual ClonableSuperWordIterator* clone() const = 0;
};

class LIMA_COMMONMISC_EXPORT AccessSubWordIterator : public AbstractSubWordIterator
{
public:
  AccessSubWordIterator(ClonableSubWordIterator* delegate);
  AccessSubWordIterator(const AccessSubWordIterator& original);
  AccessSubWordIterator& operator=(const AccessSubWordIterator& original);
  virtual ~AccessSubWordIterator();
  
  virtual const std::pair<uint64_t, uint64_t> operator*() const;
  virtual AbstractSubWordIterator &operator++(int);
  virtual bool operator==(const AbstractSubWordIterator& it) const;
  virtual bool operator!=(const AbstractSubWordIterator& it) const;
  
private:
  ClonableSubWordIterator* m_delegate;
};


inline const std::pair<uint64_t, uint64_t> AccessSubWordIterator::operator*() const {
  return m_delegate->operator*(); // clazy:exclude=rule-of-two-soft
}

inline AbstractSubWordIterator& AccessSubWordIterator::operator++(int i)
{
  return m_delegate->operator++(i);
}

inline bool AccessSubWordIterator::operator==(const AbstractSubWordIterator& it) const 
{
  const AccessSubWordIterator& asi=static_cast<const AccessSubWordIterator&>(it);
  return m_delegate->operator==(*(asi.m_delegate));
}

inline bool AccessSubWordIterator::operator!=(const AbstractSubWordIterator& it) const
{
  const AccessSubWordIterator& asi=static_cast<const AccessSubWordIterator&>(it);
  return m_delegate->operator!=(*(asi.m_delegate));
}

class LIMA_COMMONMISC_EXPORT AccessSuperWordIterator : public AbstractSuperWordIterator
{
public:
  AccessSuperWordIterator(ClonableSuperWordIterator* delegate);
  AccessSuperWordIterator(const AccessSuperWordIterator& original);
  AccessSuperWordIterator& operator=(const AccessSuperWordIterator& original);
  virtual ~AccessSuperWordIterator();

  virtual const Lima::LimaString operator*() const;
  virtual AbstractSuperWordIterator &operator++(int);
  virtual bool operator==(const AbstractSuperWordIterator& it) const;
  virtual bool operator!=(const AbstractSuperWordIterator& it) const;
    
private:
  ClonableSuperWordIterator* m_delegate;
};

inline const Lima::LimaString AccessSuperWordIterator::operator*() const
{
  return m_delegate->operator*();
}

inline AbstractSuperWordIterator& AccessSuperWordIterator::operator++(int i)
{
  return m_delegate->operator++(i);
}

inline bool AccessSuperWordIterator::operator==(const AbstractSuperWordIterator& it) const
{
  const AccessSuperWordIterator& asi=static_cast<const AccessSuperWordIterator&>(it);
  return m_delegate->operator==(*(asi.m_delegate));
}

inline bool AccessSuperWordIterator::operator!=(const AbstractSuperWordIterator& it) const
{
  const AccessSuperWordIterator& asi=static_cast<const AccessSuperWordIterator&>(it);
  return m_delegate->operator!=(*(asi.m_delegate));
}

} // Common
} // Lima

#endif
