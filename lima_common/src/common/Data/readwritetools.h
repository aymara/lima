// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
LIMA_DATA_EXPORT uint8_t readOneByteInt(std::istream& file);

/** 
 *  write a integer coded on one byte format in a file
 * 
 * @param file the file to write in
 * @param number the integer to write
 */
LIMA_DATA_EXPORT void writeOneByteInt(std::ostream& file, const uint8_t number);

//@}

} // end namespace
} // end namespace
} // end namespace

#endif
