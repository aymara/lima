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
BOOST_STRONG_TYPEDEF(uint64_t, StringsPoolIndex);
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

private:
  StringsPool(const StringsPool& /*unused p*/);
  StringsPool& operator=(const StringsPool& /*unused p*/);

  StringsPoolPrivate* m_d;
};

} // closing namespace Lima

#endif // LIMA_DATA_STRINGSPOOL_H
