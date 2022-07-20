// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
