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
* File        : tstring.cpp
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Thu Nov 21 2002
* Copyright   : (c) 2002 by CEA
* Version     : $Id: tstring.cpp 2611 2005-10-20 20:21:45Z gael $
*
******************************************************************************/

#include "tstring.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstdlib>

using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {


//***************************************************************************
// functions to deal with escape characters in a string
//***************************************************************************
void getlineLimaString(std::istream& in, LimaString& s) {
  // first get a string and convert it to wstring
  std::string tmp = Lima::Common::Misc::readLine(in);
  s=Common::Misc::utf8stdstring2limastring(tmp);
}

//***************************************************************************
// functions to deal with escape characters in a string
//***************************************************************************
// tests if the character in indicated position is an effective escape
// character
bool isEscapeCharacter(const LimaString& s, const int i) {

  if (i>s.size() || s[i] != CHAR_ESCAPE) {
    return false;
  }
  else if (i != 0) { 
    // have to check the number of escape characters before this one : 
    // the escape character is effective is the number of escape characters
    // before it is 0 or an even number
    int j(i);
    uint64_t numberOfEscapeBefore(0);
    while (j!=0) {
      j--;
      if (s[j] == CHAR_ESCAPE) {
        numberOfEscapeBefore++;
      }
      else { 
        break; 
      }
    }
    if (numberOfEscapeBefore%2 == 0) { return true; }
    else { return false; }
  }
  else {
    return true;
  }
}

int findSpecialCharacter(const LimaString& s, const LimaChar c,
                      const int posBegin,
                      const int posEnd) {
  int i(posBegin);

  do {
    i=s.indexOf(c,i);
    if (posEnd>=0 && i>posEnd) {
      return -1; // found but out of the limits
    }
    if ( i == -1 || i==0 ) {
      return i;
    }
    if (! isEscapeCharacter(s,i-1)) {
      return i;
    }
    i++;
  } while (i!=-1);

  return i;
}

int rfindSpecialCharacter(const LimaString& s, const LimaChar c,
                    const int posBegin,
                    const int posEnd) {

  int i(posBegin);

  do {
    i=s.lastIndexOf(c,i);
    if (posEnd>=0 && i<posEnd) {
      return -1; // found but out of the limits
    }
    if ( i == -1 || i==posEnd ) {
      return i;
    }
    if (! isEscapeCharacter(s,i-1)) {
      return i;
    }
    i--;
  } while (i>0);

  return i;
}

void removeEscapeCharacters(LimaString& s) {
  for (LimaString::iterator i(s.begin()); i!= s.end(); i++) {
    if (*i == CHAR_ESCAPE) {
      s.remove(*i);
      // no changing i cause next character to be skipped, 
      // but that's what we want : don't remove second '\' in '\'
    }
  }
}

} // end namespace
} // end namespace
} // end namespace
