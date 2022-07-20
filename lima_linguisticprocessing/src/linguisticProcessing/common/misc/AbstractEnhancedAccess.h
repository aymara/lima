// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_COMMON_ABSTRACTENHANCEDACCESS_H
#define LIMA_COMMON_ABSTRACTENHANCEDACCESS_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include <common/misc/AbstractAccessByString.h>

namespace Lima
{
namespace Common
{

class LIMA_LPMISC_EXPORT AbstractEnhancedAccess : public AbstractAccessByString {
public:
    
    /**
     * @brief get string from index
     * @param index 
     * @return string corresponding to index or empty string if index does not exist.
     */
    virtual LimaString getSpelling(const uint64_t index ) const = 0;
};


} // Common
} // Lima

#endif
