// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
