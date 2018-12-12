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
//
// C++ Implementation: stringspool
//
// Description:
//
//
// Author: CEA LIST <Gael.de-Chalendar@cea.fr>
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "fsaStringsPool.h"
#include "common/Data/strwstrtools.h"

#include <QMutex>

using namespace Lima::Common;
using namespace Lima::Common::Misc;

namespace Lima
{

class FsaStringsPoolPrivate
{
  friend class FsaStringsPool;
  
  FsaStringsPoolPrivate();
  virtual ~FsaStringsPoolPrivate();

  FsaStringsPoolPrivate(const FsaStringsPool& /*unused p*/);

  void clear();
  void clear(const uint32_t pos);

  Common::AbstractAccessByString* m_mainKeys;
  StringsPoolIndex m_mainKeySize;
  StringsPool m_additionalPool;
  QMutex m_mutex;
};

FsaStringsPoolPrivate::FsaStringsPoolPrivate():
    m_mainKeys(0),
    m_mainKeySize(0),
    m_additionalPool(),
    m_mutex()
{}


FsaStringsPoolPrivate::FsaStringsPoolPrivate(const FsaStringsPool& /*unused p*/)
{
}

FsaStringsPoolPrivate::~FsaStringsPoolPrivate()
{

}

FsaStringsPool::FsaStringsPool():
    m_d(new FsaStringsPoolPrivate())
{}


FsaStringsPool::FsaStringsPool(const FsaStringsPool& /*unused p*/) :
    m_d(new FsaStringsPoolPrivate())
{
}

FsaStringsPool::~FsaStringsPool()
{
  delete m_d;
}


FsaStringsPool& FsaStringsPool::operator=(const FsaStringsPool& /*unused p*/)
{
  return (*this);
}

void FsaStringsPool::registerMainKeys(AbstractAccessByString* mainKeys)
{
  STRPOOLLOGINIT;
  LINFO << "register main keys" ;

  if (!m_d->m_additionalPool.empty())
  {
    LERROR << "Can't register main keys if stringspool not empty!" ;
    LERROR << "contains " << m_d->m_additionalPool[static_cast<StringsPoolIndex>(1)] ;
    throw LimaException("Can't register main keys if stringspool not empty!");
  }
  if (mainKeys == nullptr)
  {
    LERROR << "Can't register null main keys!" ;
    throw LimaException("Can't register null main keys!");
  }

  m_d->m_mainKeys=mainKeys;
  m_d->m_mainKeySize=m_d->m_mainKeys->getSize();
}


StringsPoolIndex FsaStringsPool::operator[](const LimaString& str)
{
  if (m_d->m_mainKeys)
  {
    StringsPoolIndex res= static_cast<StringsPoolIndex>(m_d->m_mainKeys->getIndex(str));
    if (res!=static_cast<StringsPoolIndex>(0))
    {
      return res;
    }
  }
  return static_cast<StringsPoolIndex>(m_d->m_mainKeySize + m_d->m_additionalPool[str]);
}

LimaString FsaStringsPool::operator[](const StringsPoolIndex ind) const
{
  //    STRPOOLLOGINIT;
  //    LDEBUG << "const FsaStringsPool[" << ind << "]" ;
  if (ind < m_d->m_mainKeySize)
  {
    QMutexLocker lock(&m_d->m_mutex);
    if (ind == 0)
      return LimaString();
    return m_d->m_mainKeys->getSpelling(ind);
  }
  return m_d->m_additionalPool[static_cast<StringsPoolIndex>(ind - m_d->m_mainKeySize)];
}

void FsaStringsPool::registerUser(void* p)
{
  m_d->m_additionalPool.registerUser(p);
}

void FsaStringsPool::unregisterUser(void* p)
{
  m_d->m_additionalPool.unregisterUser(p);
}

void FsaStringsPool::endResourcesPool()
{
  m_d->m_additionalPool.endResourcesPool();
}


} // closing namespace Lima
