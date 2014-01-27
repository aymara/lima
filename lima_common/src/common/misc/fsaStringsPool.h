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
#ifndef LIMA_DATA_FSASTRINGSPOOL_H
#define LIMA_DATA_FSASTRINGSPOOL_H

#include "common/LimaCommon.h"
#include "common/Data/LimaString.h"
#include "stringspool.h"
#include "AbstractAccessByString.h"

namespace Lima {

class FsaStringsPoolPrivate;
/**
@author CEA LIST
*/
class LIMA_COMMONMISC_EXPORT FsaStringsPool
{
public:
    FsaStringsPool();

    virtual ~FsaStringsPool();

    StringsPoolIndex operator[](const LimaString& str);
    const LimaString& operator[](const StringsPoolIndex ind) const;
    
    /*
    * @TODO create a distinct FsaStringsPool
    * A simple FsaStringsPool should be kept, and a FsaStringsPool
    * using an automaton as source. It would allow to not merge both
    */
   
    void registerMainKeys(Common::AbstractAccessByString* mainKeys);
    
    void registerUser(void* p);
    void unregisterUser(void* p);
    
    void endResourcesPool();

private:
    FsaStringsPool(const FsaStringsPool& /*unused p*/);
    FsaStringsPool& operator=(const FsaStringsPool& /*unused p*/);

    FsaStringsPoolPrivate* m_d;
};

} // closing namespace Lima

#endif // LIMA_DATA_STRINGSPOOL_H
