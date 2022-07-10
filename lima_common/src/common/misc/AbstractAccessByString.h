// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                      *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_COMMON_ABSTRACTACCESBYSTRING_H
#define LIMA_COMMON_ABSTRACTACCESBYSTRING_H

#include "common/Data/LimaString.h"
#include "common/misc/AbstractAccessIterators.h"


namespace Lima
{
namespace Common
{

/**
* @brief define abstract interface for access method
*/
class AbstractAccessByString
{
public:

  virtual ~AbstractAccessByString() {}
  
  /**
   * @brief gives the number of entries
   * @return number of entries
   */
  virtual uint64_t getSize() const = 0;

  /**
   * @brief get key index
   * @param word 
   * @return key index or 0 if key does not exist.
   */
  virtual uint64_t getIndex(const Lima::LimaString & word ) const = 0;


  /**
   * @brief get subwords from a string
   * A subword is a word in the key string that begin at the given offset.
   * @param offset 
   * @param word 
   * @return subword iterators.
   */
  virtual std::pair<AccessSubWordIterator,AccessSubWordIterator>
  getSubWords(const uint64_t offset,
              const Lima::LimaString & word ) const = 0;

  /**
   * @brief get superWords from a string
   * A superword entry is an entry that begins with the given key
   * @param offset 
   * @param word 
   * @return superword iterators.
   */
  virtual std::pair<AccessSuperWordIterator,AccessSuperWordIterator>
  getSuperWords(const Lima::LimaString & word ) const = 0;

  /**
   * @brief get string from index
   * @param index 
   * @return string corresponding to index or empty string if index does not exist.
   */
  virtual Lima::LimaString getSpelling(const uint64_t index ) const = 0;
  
};

} // Common
} // Lima

#endif
