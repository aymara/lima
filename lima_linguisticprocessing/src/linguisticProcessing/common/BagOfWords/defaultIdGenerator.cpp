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
 * @file       defaultIdGenerator.cpp
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Tue Feb  7 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "defaultIdGenerator.h"

namespace Lima {
namespace Common {
namespace BagOfWords {

class LIMA_BOW_EXPORT DefaultIdGeneratorPrivate
{
  friend class DefaultIdGenerator;
  DefaultIdGeneratorPrivate(const uint64_t lastId);
  ~DefaultIdGeneratorPrivate();

  uint64_t m_lastId;
  uint64_t m_simpleTermCount;
  uint64_t m_compoundTermCount;
};

DefaultIdGeneratorPrivate::DefaultIdGeneratorPrivate(const uint64_t lastId) :
  m_lastId(lastId),
  m_simpleTermCount(0),
  m_compoundTermCount(0)
{
}

DefaultIdGeneratorPrivate::~DefaultIdGeneratorPrivate()
{
}

DefaultIdGenerator::DefaultIdGenerator(AbstractLexiconIdGeneratorInformer* informer, 
                                       const uint64_t lastId) : 
  AbstractLexiconIdGenerator(informer),
  m_d(new DefaultIdGeneratorPrivate(lastId))
{
}

DefaultIdGenerator::~DefaultIdGenerator() 
{
  delete m_d;
}

uint64_t DefaultIdGenerator::getId(const LimaString& /*unused word*/)
{
  const_cast<DefaultIdGenerator*>(this)->m_d->m_simpleTermCount++;
  return const_cast<DefaultIdGenerator*>(this)->m_d->m_lastId++; 
}

uint64_t DefaultIdGenerator::getId(  const std::vector<uint64_t>& /*unused structure*/ ) 
{
  const_cast<DefaultIdGenerator*>(this)->m_d->m_compoundTermCount++;
  return const_cast<DefaultIdGenerator*>(this)->m_d->m_lastId++;
}

uint64_t DefaultIdGenerator::getSize() const 
{
  return m_d->m_simpleTermCount + m_d->m_compoundTermCount;
}

uint64_t DefaultIdGenerator::getSimpleTermSize() const 
{
  return m_d->m_simpleTermCount;
}

} // end namespace
} // end namespace
} // end namespace
