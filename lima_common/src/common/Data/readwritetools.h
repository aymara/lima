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
/************************************************************************
 *
 * @file       readwritetools.h
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Fri Nov  7 2003
 * @version    $Id$
 * copyright   Copyright (C) 2003-2012 by CEA LIST
 * Project     misc
 * 
 * @brief      contains helper functions for binary read/write
 * 
 * 
 ***********************************************************************/

#ifndef LIMA_COMMONS_MISC_READWRITETOOLS_H
#define LIMA_COMMONS_MISC_READWRITETOOLS_H

#include "common/LimaCommon.h"

#include <iostream>

namespace Lima {
namespace Common {
namespace Misc {

//@{ functions for variable-byte read/write of integers
//(one byte for numbers in [0-127], two bytes for numbers in [128-16383] etc.

/** 
 * read a integer coded in variable-byte format in a file
 * 
 * @param file the file from which to read
 * 
 * @return the integer read
 */
LIMA_DATA_EXPORT uint64_t readCodedInt(std::istream& file);

/** 
 *  write a integer coded in variable-byte format in a file
 * 
 * @param file the file to write in
 * @param number the integer to write
 */
LIMA_DATA_EXPORT void writeCodedInt(std::ostream& file, const uint64_t number);

//@}


//@{ functions for reading/writing integers on one byte
//(knowning that the integer is less than 256, useful for enum values)

/** 
 * read a integer coded on one byte in a file
 * 
 * @param file the file from which to read
 * 
 * @return the integer read
 */
LIMA_DATA_EXPORT uint64_t readOneByteInt(std::istream& file);

/** 
 *  write a integer coded on one byte format in a file
 * 
 * @param file the file to write in
 * @param number the integer to write
 */
LIMA_DATA_EXPORT void writeOneByteInt(std::ostream& file, const uint64_t number);

//@}

} // end namespace
} // end namespace
} // end namespace

#endif
