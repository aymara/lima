// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

  void clear();
  void clear(const uint64_t pos);

  Common::AbstractAccessByString* m_mainKeys;
  StringsPoolIndex m_mainKeySize;
  StringsPool m_additionalPool;
  std::vector<LimaString>* m_cache;
  QMutex m_mutex;
};

FsaStringsPoolPrivate::FsaStringsPoolPrivate():
    m_mainKeys(nullptr),
    m_mainKeySize(0),
    m_additionalPool(),
    m_cache(nullptr),
    m_mutex()
{}


FsaStringsPoolPrivate::~FsaStringsPoolPrivate()
{
  delete m_cache;
}

FsaStringsPool::FsaStringsPool():
    m_d(new FsaStringsPoolPrivate())
{}


FsaStringsPool::~FsaStringsPool()
{
  delete m_d;
}

void FsaStringsPool::registerMainKeys(AbstractAccessByString* mainKeys)
{
  STRPOOLLOGINIT;
  LINFO << "register main keys" ;

  if (!m_d->m_additionalPool.empty())
  {
    LIMA_EXCEPTION( "Can't register main keys if stringspool not empty!" << endl
                  << "contains " << m_d->m_additionalPool[static_cast<StringsPoolIndex>(1)] );
  }
  if (mainKeys == nullptr)
  {
    LIMA_EXCEPTION( "Can't register null main keys!" );
  }

  m_d->m_mainKeys=mainKeys;
  m_d->m_mainKeySize=m_d->m_mainKeys->getSize();
  m_d->m_cache=new std::vector<LimaString>(m_d->m_mainKeySize,LimaString());
}


StringsPoolIndex FsaStringsPool::operator[](const LimaString& str)
{
  if (m_d->m_mainKeys)
  {
    StringsPoolIndex res= static_cast<StringsPoolIndex>(m_d->m_mainKeys->getIndex(str));
    if (res!=static_cast<StringsPoolIndex>(0))
    {
      QMutexLocker lock(&m_d->m_mutex);
      LimaString& cachedStr=(*m_d->m_cache)[res];
      if (cachedStr.size()==0)
      {
        cachedStr=str;
      }
      return res;
    }
  }
  return static_cast<StringsPoolIndex>(m_d->m_mainKeySize + m_d->m_additionalPool[str]);
}

const LimaString& FsaStringsPool::operator[](const StringsPoolIndex ind) const
{
  //    STRPOOLLOGINIT;
  //    LDEBUG << "const FsaStringsPool[" << ind << "]" ;
  if (ind < m_d->m_mainKeySize)
  {
    QMutexLocker lock(&m_d->m_mutex);
    LimaString& str=(*m_d->m_cache)[ind];
    if (ind == 0)
    {
      return str;
    }
    if (str.size()==0)
    {
      str= LimaString(m_d->m_mainKeys->getSpelling(ind));
    }
    return str;
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
