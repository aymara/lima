// Copyright 2002-2013 CEA LIST
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

    FsaStringsPool(const FsaStringsPool&) = delete;
    FsaStringsPool& operator=(const FsaStringsPool&) = delete;

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

    FsaStringsPoolPrivate* m_d;
};

} // closing namespace Lima

#endif // LIMA_DATA_STRINGSPOOL_H
