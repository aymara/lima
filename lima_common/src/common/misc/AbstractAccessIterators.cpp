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
 *   Copyright (C) 2004-2012 by CEA LIST                       *
 *                                                                         *
 ***************************************************************************/

#include "AbstractAccessIterators.h"

namespace Lima
{
namespace Common
{

AccessSubWordIterator::AccessSubWordIterator(ClonableSubWordIterator* delegate) :
    AbstractSubWordIterator(),
    m_delegate(delegate)
{
#ifdef DEBUG_CD
    STRINGMAPLOGINIT;
    LDEBUG << this << ": AccessSubWordIterator::AccessSubWordIterator(ClonableSubWordIterator* delegate) "
          ;
#endif
}

AccessSubWordIterator::AccessSubWordIterator(const AccessSubWordIterator& original) :
    AbstractSubWordIterator()
{
#ifdef DEBUG_CD
    STRINGMAPLOGINIT;
    LDEBUG << this << ": AccessSubWordIterator::AccessSubWordIterator(const AccessSubWordIterator& original) "
          ;
#endif
  m_delegate=original.m_delegate->clone();
}

AccessSubWordIterator& AccessSubWordIterator::operator=(const AccessSubWordIterator& original)
{
#ifdef DEBUG_CD
    STRINGMAPLOGINIT;
    LDEBUG <<  this << ": AccessSubWordIterator& AccessSubWordIterator::operator=(const AccessSubWordIterator& original) "
          ;
#endif
  delete m_delegate;
  m_delegate=original.m_delegate->clone();
  return *this;
}

AccessSubWordIterator::~AccessSubWordIterator()
{
  delete m_delegate;
}

AccessSuperWordIterator::AccessSuperWordIterator(ClonableSuperWordIterator* delegate) :
  m_delegate(delegate)
{
#ifdef DEBUG_CD
    STRINGMAPLOGINIT;
    LDEBUG <<  this << ": AccessSuperWordIterator::AccessSuperWordIterator(ClonableSuperWordIterator* delegate) "
          ;
#endif
}

AccessSuperWordIterator::AccessSuperWordIterator(const AccessSuperWordIterator& original) :
    AbstractSuperWordIterator()
{
#ifdef DEBUG_CD
    STRINGMAPLOGINIT;
    LDEBUG <<  this << ": AccessSuperWordIterator::AccessSuperWordIterator(const AccessSuperWordIterator& original) "
          ;
#endif
  m_delegate=original.m_delegate->clone();
}

AccessSuperWordIterator& AccessSuperWordIterator::operator=(const AccessSuperWordIterator& original)
{
#ifdef DEBUG_CD
    STRINGMAPLOGINIT;
    LDEBUG <<  this << ": AccessSuperWordIterator& AccessSuperWordIterator::operator=(const AccessSuperWordIterator& original) "
          ;
#endif
  delete m_delegate;
  m_delegate=original.m_delegate->clone();
  return *this;
}

AccessSuperWordIterator::~AccessSuperWordIterator() {
  delete m_delegate;
}

} // Common
} // Lima
