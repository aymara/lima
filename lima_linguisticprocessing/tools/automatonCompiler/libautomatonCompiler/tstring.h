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
/******************************************************************************
*
* File        : tstring.h
* Project     : DETECT - Named Entities
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Wed Nov 20 2002
* Copyright   : (c) 2002 by CEA
* Version     : $Id: tstring.h 2360 2005-06-03 17:16:11Z romaric $
*
* Description : String type used in internal representation
* (can be string or wstring), and related types
*
******************************************************************************/

#ifndef TSTRING_H
#define TSTRING_H

#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cmath> // for pow

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

//***************************************************************************
// Representation in wstring
//***************************************************************************

// LimaString functions
void getlineLimaString(std::istream& in, LimaString& s);

//***************************************************************************
// general functions to find a special character in a string
// (special characters can be escaped not to be taken into account)

// character to escape special characters
#define CHAR_ESCAPE LimaChar('\\')

int
  findSpecialCharacter(const LimaString& , const LimaChar,
                       const int pos=0,
                       const int posEnd=-1);

int
  rfindSpecialCharacter(const LimaString& , const LimaChar,
                        const int pos=-1,
                        const int posEnd=0);

// test if a character (indicated by its position in the string)
// is an escape character
bool isEscapeCharacter(const LimaString& s, const int i);

// remove escape characters '\' in a string
void removeEscapeCharacters(LimaString& s);

} // end namespace
} // end namespace
} // end namespace

#endif
