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
  if (bitLength > 0 && data)
    delete[] data;
}

LSHSignature::LSHSignature ( const LSHSignature & s) :
  precomputedCount(s.precomputedCount),
  data(nullptr),
  bitLength(s.bitLength)
{
  if (bitLength > 0) {
    data = new base[(bitLength / 8)/sizeof(base)] ;
    std::copy(s.data, s.data + (bitLength / (sizeof(base)*8)), data);
  }
}

LSHSignature& LSHSignature::operator=( const LSHSignature & s)
{
  precomputedCount = s.precomputedCount;
  bitLength = s.bitLength;
  if (data)
  {
    delete[] data;
  }
  if (bitLength > 0) 
  {
    data = new base[(bitLength / 8)/sizeof(base)] ;
    std::copy(s.data, s.data + (bitLength / (sizeof(base)*8)), data);
  }
  else data = nullptr;  
  return *this;
}

LSHSignature::base* LSHSignature::getData() 
{ 
  return data; 
}

int LSHSignature::getBitLength() 
{
  return bitLength;
}

void LSHSignature::setBit(uint b) 
{
  (data[b/(sizeof(base)*8)] = data[b/(sizeof(base)*8)] | ((base)1 << (b & ((sizeof(base)*8)-1)))) ;
}

void LSHSignature::unsetBit(uint b) 
{
  (data[b/(sizeof(base)*8)] = data[b/(sizeof(base)*8)] & (~((base)1 << (b & ((sizeof(base)*8)-1))))) ;
}

bool LSHSignature::getBit(uint b) 
{
  return  (data[b/(sizeof(base)*8)] & ((base)1 << (b & ((sizeof(base)*8)-1)))); 
}


void LSHSignature::merge(std::vector<LSHSignature>& sigs) 
{
  for(int i = 0; i<bitLength; i++) {
    int cnt0 = 0;
    int cnt1 = 1;
    for (std::vector<LSHSignature>::iterator it = sigs.begin(); it!=sigs.end(); it++) {
      if(it->getBit(i)==1) {
  cnt1++;
      } else {
  cnt0++;
      }
    }
    if (cnt1>cnt0) {
      setBit(i);
    } else if (cnt0>cnt1) {
      unsetBit(i);
    } else {
      // on garde le bit du noyau
    }
  }
}

void LSHSignature::print(double rank, int limit)
{
  std::ostringstream os;
  os << rank << ":sig: ";
  if (data!=NULL) {
    for (int i = 0; i < limit; i++) {
      os << getBit(i) ;
    }
  }
  os << std::endl;
  std::cerr << os.str();
}
              
void LSHSignature::xorX ( LSHSignature & signature )
{
  if (bitLength > 0 ) 
  {
    if ( signature.bitLength != bitLength )
      throw 0;
    long l = bitLength / ( 8 * sizeof ( base ) );
    base *xdata = signature.getData();
    while ( l-- ) 
      data[l] ^= xdata[l];
    precomputedCount = -1;
  }
}

int LSHSignature::bitCount()
{
  if (bitLength > 0) {
    unsigned char byteCount[256] =
        { 0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
          1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
          1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
          2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
          1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
          2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
          2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
          3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
          1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
          2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
          2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
          3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
          2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
          3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
          3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
          4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
        };
    if ( precomputedCount >= 0 )
            return precomputedCount;
    long l = bitLength / ( 8 * sizeof ( base ) );
    int sum = 0;
    unsigned char i;
    base v;
    while ( l-- )
    {
      v = data[l];
      i = v & 0xff;
      sum += byteCount[i];
      i = ( v >> 8 ) & 0xff;
      sum += byteCount[i];
      i = ( v >> 16 ) & 0xff;
      sum += byteCount[i];
      i = ( v >> 24 ) & 0xff;
      sum += byteCount[i];
      i = ( v >> 32 ) & 0xff;
      sum += byteCount[i];
      i = ( v >> 40 ) & 0xff;
      sum += byteCount[i];
      i = ( v >> 48 ) & 0xff;
      sum += byteCount[i];
      i = ( v >> 56 ) & 0xff;
      sum += byteCount[i];
    }
    precomputedCount = sum;
    return sum;
  } else return 0;
}
            
int LSHSignature::bitCount64(int where)
{
  if (bitLength > 0) 
  {
    unsigned char byteCount[256] =
    { 0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
    };
    
    int sum = 0;
    unsigned char i;
    base v;
    v = data[where];
    i = v & 0xff;
    sum += byteCount[i];
    i = ( v >> 8 ) & 0xff;
    sum += byteCount[i];
    i = ( v >> 16 ) & 0xff;
    sum += byteCount[i];
    i = ( v >> 24 ) & 0xff;
    sum += byteCount[i];
    i = ( v >> 32 ) & 0xff;
    sum += byteCount[i];
    i = ( v >> 40 ) & 0xff;
    sum += byteCount[i];
    i = ( v >> 48 ) & 0xff;
    sum += byteCount[i];
    i = ( v >> 56 ) & 0xff;
    sum += byteCount[i];
    return sum;
  } 
  else return 0;
}

bool LSHSignature::comparePermutedBits(LSHSignature & s, 
                                       std::vector<int>& permutations) 
{
  int bca, bcb;

  for (uint p = 0; p < bitLength / ( 8 * sizeof ( base ) ); p++) 
  {
    bca = bitCount64(permutations[p]);
    bcb = s.bitCount64(permutations[p]);
    if (bca != bcb)
      return bca < bcb;
  }
  return false;
}
