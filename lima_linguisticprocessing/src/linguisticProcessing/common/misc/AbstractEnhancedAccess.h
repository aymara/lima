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

#ifndef LIMA_COMMON_ABSTRACTENHANCEDACCESS_H
#define LIMA_COMMON_ABSTRACTENHANCEDACCESS_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "AbstractAccessByString.h"

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
