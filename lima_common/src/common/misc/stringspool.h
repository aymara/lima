// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

//
// C++ Interface: stringspool
//
// Description:
//
//
// Author: CEA LIST <Gael.de-Chalendar@cea.fr>
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef LIMA_DATA_STRINGSPOOL_H
#define LIMA_DATA_STRINGSPOOL_H

#include "common/LimaCommon.h"
#include "common/Data/LimaString.h"

#ifdef WIN32
#include <hash_map>
#else
#ifndef NO_STDCPP0X
#include <unordered_map>
#else
#include <ext/hash_map>
#endif
#endif
#include <boost/serialization/strong_typedef.hpp>

namespace Lima {

class StringsPoolPrivate;

#define STRINGS_POOL_INDEX_TYPE uint64_t
BOOST_STRONG_TYPEDEF(STRINGS_POOL_INDEX_TYPE, StringsPoolIndex);
#define STRINGS_POOL_INDEX_MAX_VALUE std::numeric_limits<STRINGS_POOL_INDEX_TYPE>::max()

/**
@author CEA LIST
*/
class LIMA_COMMONMISC_EXPORT StringsPool
{
public:
    StringsPool();

    ~StringsPool();

    StringsPoolIndex operator[](const LimaString& str) const;
    StringsPoolIndex operator[](const LimaString& str);

    const LimaString& operator[](const StringsPoolIndex ind) const;
    LimaString& operator[](const StringsPoolIndex ind);

    void registerUser(void* p);
    void unregisterUser(void* p);

    void endResourcesPool();

    bool empty() const;
    uint64_t size() const;

  StringsPool(const StringsPool& /*unused p*/) = delete;
  StringsPool& operator=(const StringsPool& /*unused p*/)  = delete;

private:

  StringsPoolPrivate* m_d;
};

} // closing namespace Lima

namespace std {

template<> struct hash<Lima::StringsPoolIndex> {
  std::size_t operator()(const Lima::StringsPoolIndex s) const noexcept {
    return (size_t) qHash(s);
  }
};

}

#endif // LIMA_DATA_STRINGSPOOL_H
