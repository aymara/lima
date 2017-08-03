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
// C++ Interface: lshsignature
//
// Description:
//
//
// Author: Claire Mouton, Guillaume Pitel <claire.mouton@cea.fr, guillaume.pitel@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef LSHSIGNATURE_H
#define LSHSIGNATURE_H

#include "WordSenseAnalysisExport.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h>

/**
  @author Claire Mouton <claire.mouton@cea.fr>
  @author Guillaume Pitel <guillaume.pitel@gmail.com>
  @author Gael de Chalendar <gael.de-chalendar@cea.fr>
*/
class LIMA_WORDSENSEANALYSIS_EXPORT LSHSignature
{
public:
  typedef uint64_t base;

  LSHSignature ( int bitLength );
  LSHSignature ( const LSHSignature & s);
  LSHSignature& operator=( const LSHSignature & s);

  ~LSHSignature();

  base* getData();

  int getBitLength();

  void setBit(uint b);

  void unsetBit(uint b);
  bool getBit(uint b);


  void merge(std::vector<LSHSignature>& sigs);

  void print(double rank, int limit);

  void xorX ( LSHSignature & signature );

  int bitCount();

  int bitCount64(int where);

  bool comparePermutedBits(LSHSignature & s, std::vector<int>& permutations);

private:
  int precomputedCount;
  base *data;
  int bitLength;
};

#endif
