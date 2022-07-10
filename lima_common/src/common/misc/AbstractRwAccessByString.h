// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                      *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_COMMON_ABSTRACTRWACCESBYSTRING_H
#define LIMA_COMMON_ABSTRACTRWACCESBYSTRING_H

#include "common/misc/AbstractAccessByString.h"


namespace Lima
{
namespace Common
{

/**
* @brief define abstract interface for access method inherits to 
*  AbstractAccesByString with additional addRandomWord() operaton
*/
class AbstractModifierOnAccessByString
{
public:

  /**
   * @brief gives the number of entries
   * @return number of entries
   */
  virtual void addRandomWord( const Lima::LimaString & newWord ) = 0;

};

} // Common
} // Lima

#endif
