/*
    Copyright 2002-2020 CEA LIST

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
#include "stringspool.h"
#include "common/Data/strwstrtools.h"

#include <QMutex>

#include <set>

namespace Lima {

class StringsPoolPrivate
{
  friend class StringsPool;

  StringsPoolPrivate();

  StringsPoolPrivate(const StringsPool& /*unused p*/);

  ~StringsPoolPrivate();

  void clear();
  void clear(const uint64_t pos);

#ifdef WIN32
#define MAX_DEPTH 25

  class LimaStringPtrHasher {
  public:
     static const size_t bucket_size = 10; // mean bucket size that the container should try not to exceed
     static const size_t min_buckets = (1 << 10); // minimum number of buckets, power of 2, >0
     LimaStringPtrHasher() {
          // should be default-constructible
     }
     size_t operator()(const Lima::LimaString* s) const {
        const int shift[] = {0,8,16,24};    // 4 shifts to "occupy" 32 bits
        uint64_t key = 0x55555555;        //0101...
        uint64_t oneChar;
        int depth = s->length();
        if (depth > MAX_DEPTH)
            depth = MAX_DEPTH;
        for (int i=0; i<depth; i++) {
            oneChar = ((uint64_t) ((*s)[i].unicode()) )<<shift[i%4];
            key^=oneChar;                    // exclusive or
        }
        return key;
     }

     bool operator()(const Lima::LimaString* left, const Lima::LimaString* right) {
            // this should implement a total ordering on MyClass, that is
            // it should return true if "left" precedes "right" in the ordering
            return *left < *right;
     }
 };
 typedef stdext::hash_map<
     const Lima::LimaString*,
     StringsPoolIndex,
     LimaStringPtrHasher
 > LimaStringPtrHashMap;
#else
    struct HashLimaStringPtr
    {
        size_t operator()(const Lima::LimaString* s) const;
    };

    struct EquaLimaStringPtr
    {
        bool operator()(const Lima::LimaString* s1, const Lima::LimaString* s2) const;
    };

#ifndef NO_STDCPP0X
    typedef std::unordered_map<
      const Lima::LimaString*,
      StringsPoolIndex,
      HashLimaStringPtr,
      EquaLimaStringPtr > LimaStringPtrHashMap;
#else
    typedef __gnu_cxx::hash_map<
      const Lima::LimaString*,
      StringsPoolIndex,
      HashLimaStringPtr,
      EquaLimaStringPtr > LimaStringPtrHashMap;
#endif
#endif
  LimaStringPtrHashMap m_hashPool;
  LimaStringPtrHashMap m_resourcesHashPool;
  std::vector< LimaString* > m_vecPool;

  // index of the position in vecPool for reinitialization (after
  // data initialized by resources)
  uint64_t m_resourcesPoolIndex;

  std::set< void* > m_users;

  QMutex m_mutex;
};

StringsPoolPrivate::StringsPoolPrivate():
    m_hashPool(),
    m_resourcesHashPool(),
    m_vecPool(),
    m_resourcesPoolIndex(0),
    m_users(),
    m_mutex()
{
  StringsPoolIndex newPosition = static_cast<StringsPoolIndex>(m_vecPool.size());
  LimaString* newString = new LimaString();
  const_cast< std::vector< LimaString* >* >(&m_vecPool)->push_back(newString);
  const_cast< StringsPoolPrivate::LimaStringPtrHashMap* >(&m_hashPool)->insert(std::make_pair(newString, newPosition));
  m_resourcesPoolIndex=1;
}


StringsPoolPrivate::~StringsPoolPrivate()
{
  // delete all (including resourcesPool)
  clear(0);
}

StringsPool::StringsPool():
    m_d(new StringsPoolPrivate())
{
}

StringsPool::~StringsPool()
{
  delete m_d;
}

bool StringsPool::empty() const
{
  return m_d->m_vecPool.size()==1;
}

uint64_t StringsPool::size() const
{
  return m_d->m_vecPool.size();
}


void StringsPool::endResourcesPool() {
  m_d->m_resourcesPoolIndex=m_d->m_vecPool.size();
  m_d->m_resourcesHashPool=m_d->m_hashPool;
}

StringsPoolIndex StringsPool::operator[](const LimaString& str) const
{
  //STRPOOLLOGINIT;
  //LTRACE << "StringsPool[" << str << "]";
  if (m_d->m_hashPool.find(&str) == m_d->m_hashPool.end())
  {
    StringsPoolIndex newPosition = static_cast<StringsPoolIndex>(m_d->m_vecPool.size());
    LimaString* newString = new LimaString(str);
    const_cast< std::vector< LimaString* >* >(&m_d->m_vecPool)->push_back(newString);
    const_cast< StringsPoolPrivate::LimaStringPtrHashMap* >(&m_d->m_hashPool)->insert(std::make_pair(newString, newPosition));
    //LTRACE << "   ... not found. Created entry n" << newPosition;
    return newPosition;
  }
  else
  {
    StringsPoolIndex pos = (*(const_cast< StringsPoolPrivate::LimaStringPtrHashMap* >(&m_d->m_hashPool)))[&str];
    //LTRACE << "   ... found at index " << pos;
    return pos;
  }
}

StringsPoolIndex StringsPool::operator[](const LimaString& str)
{
  //STRPOOLLOGINIT;
  //LTRACE << "StringsPool[" << str << "]";
  if (m_d->m_hashPool.find(&str) == m_d->m_hashPool.end())
  {
    QMutexLocker locker(&m_d->m_mutex);
    StringsPoolIndex newPosition = static_cast<StringsPoolIndex>(m_d->m_vecPool.size());
    LimaString* newString = new LimaString(str);
    m_d->m_vecPool.push_back(newString);
    m_d->m_hashPool.insert(std::make_pair(newString, newPosition));
    //LTRACE << "   ... not found. Created entry n" << newPosition;
    return newPosition;
  }
  else
  {
    StringsPoolIndex pos = m_d->m_hashPool[&str];
    //LTRACE << "   ... found at index " << pos;
    return pos;
  }
}

const LimaString& StringsPool::operator[](const StringsPoolIndex ind) const
{
  //STRPOOLLOGINIT;
  //LTRACE << "const StringsPool[" << ind << "]";
  if (ind >= static_cast<StringsPoolIndex>(m_d->m_vecPool.size()))
  {
      std::ostringstream oss;
      oss << "stringspool(size = " << m_d->m_vecPool.size() << "): Out of bounds (" << ind << ")";
      throw std::runtime_error(oss.str());
  }
  //LTRACE << "StringsPool[" << ind << "] = " << *(m_d->m_vecPool[ind]);
  return *(m_d->m_vecPool[ind]);
}

LimaString& StringsPool::operator[](const StringsPoolIndex ind)
{
  //STRPOOLLOGINIT;
  //LTRACE << "StringsPool[" << ind << "]";
  if (ind >= static_cast<StringsPoolIndex>(m_d->m_vecPool.size()))
  {
      std::ostringstream oss;
      oss << "stringspool(size = " << m_d->m_vecPool.size() << "): Out of bounds (" << ind << ")";
      throw std::runtime_error(oss.str());
  }
  //LTRACE << "StringsPool[" << ind << "] = " << *(m_d->m_vecPool[ind]);
  return *(m_d->m_vecPool[ind]);
}

void StringsPool::registerUser(void* p)
{
//    STRPOOLLOGINIT;
//    LDEBUG << "Stringspool registering user " << p;
    m_d->m_users.insert(p);
}

void StringsPool::unregisterUser(void* p)
{
//    STRPOOLLOGINIT;
//    LDEBUG << "Stringspool unregistering user " << p;
    std::set< void* >::iterator it = m_d->m_users.find(p);
    if ( it != m_d->m_users.end() )
        m_d->m_users.erase(it);

    if (m_d->m_users.empty())
        m_d->clear();
}

void StringsPoolPrivate::clear()
{
  clear(m_resourcesPoolIndex);
}

// reinit from pos to the end
void StringsPoolPrivate::clear(const uint64_t pos)
{
    // reinitialize hashPool
	// WARNING: The m_hashPool hash table contains the same pointer as the m_vecPool
	// vector. So, override its content BEFORE free memory to avoid crash (on Windows)
    m_hashPool=m_resourcesHashPool;

//    STRPOOLLOGINIT;
//    LDEBUG << "clearing StringsPool";
    uint64_t i(pos),size(m_vecPool.size());
    for (; i != size; i++)
    {
        delete (m_vecPool[i]);
        m_vecPool[i] = 0;
    }
    m_vecPool.resize(pos);
}

#ifndef WIN32
#define MAX_DEPTH 25
size_t StringsPoolPrivate::HashLimaStringPtr::operator()(const Lima::LimaString* s) const
{
    const int shift[] = {0,8,16,24};    // 4 shifts to "occupy" 32 bits
    uint64_t key = 0x55555555;        //0101...
    uint64_t oneChar;
    int depth = s->length();
    if (depth > MAX_DEPTH)
        depth = MAX_DEPTH;
    for (int i=0; i<depth; i++) {
        oneChar = ((uint64_t) ((*s)[i].unicode()) )<<shift[i%4];
        key^=oneChar;                    // exclusive or
    }
    return key;
}

bool StringsPoolPrivate::EquaLimaStringPtr::operator()(
        const Lima::LimaString* s1,
        const Lima::LimaString* s2) const
{
    return ((*s1) == (*s2));
}
#endif
} // closing namespace Lima
