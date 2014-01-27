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
// C++ Implementation: lshsignature
//
// Description: 
//
//
// Author: Claire Mouton, Guillaume Pitel <claire.mouton@cea.fr, guillaume.pitel@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "lshsignature.h"

LSHSignature::LSHSignature ( const LSHSignature & s) {  
    precomputedCount = s.precomputedCount;
    bitLength = s.bitLength;
    if (bitLength > 0) {
      data = new base[(bitLength / 8)/sizeof(base)] ;
      std::copy(s.data, s.data + (bitLength / (sizeof(base)*8)), data);
    }
    else data = NULL;  

}

LSHSignature::LSHSignature( int signatureSize)
{
  precomputedCount = -1;
  bitLength = signatureSize;
  if (bitLength > 0)
    data = new base[(bitLength / 8)/sizeof(base)];
  else data = NULL;
}


LSHSignature::~LSHSignature()
{
  if (bitLength > 0)
    delete[] data;
}
