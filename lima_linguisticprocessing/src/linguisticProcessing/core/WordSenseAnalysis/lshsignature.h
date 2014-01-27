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
        @author Claire Mouton, Guillaume Pitel <claire.mouton@cea.fr, guillaume.pitel@gmail.com>
*/
class LIMA_WORDSENSEANALYSIS_EXPORT LSHSignature
{
        public:
                typedef uint64_t base;
        private:
                int precomputedCount;
                base *data;
                int bitLength;
        public:
                LSHSignature ( int bitLength );
                LSHSignature ( const LSHSignature & s);

                ~LSHSignature();

                base *getData() { return data; }
    
    int getBitLength() {return bitLength;}

                inline void  setBit(uint b) {
                  (data[b/(sizeof(base)*8)] = data[b/(sizeof(base)*8)] | ((base)1 << (b & ((sizeof(base)*8)-1)))) ;
                }

                inline void unsetBit(uint b) {
                  (data[b/(sizeof(base)*8)] = data[b/(sizeof(base)*8)] & (~((base)1 << (b & ((sizeof(base)*8)-1))))) ;
                }

                inline bool getBit(uint b) {
                  return  (data[b/(sizeof(base)*8)] & ((base)1 << (b & ((sizeof(base)*8)-1)))); 
                }


    void merge(std::vector<LSHSignature>& sigs) {
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

    void print(double rank, int limit)
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
                
                void xorX ( LSHSignature & signature )
                {
      if (bitLength > 0 ) {
        if ( signature.bitLength != bitLength )
          throw 0;
        long l = bitLength / ( 8 * sizeof ( base ) );
        base *xdata = signature.getData();
        while ( l-- ) 
          data[l] ^= xdata[l];
        precomputedCount = -1;
      }
                }

                int bitCount()
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
                        register long l = bitLength / ( 8 * sizeof ( base ) );
                        register int sum = 0;
                        register unsigned char i;
                        register base v;
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
                
                int bitCount64(int where)
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
                  
                  register int sum = 0;
                  register unsigned char i;
                  register base v;
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
      } else return 0;
                }

                bool comparePermutedBits(LSHSignature & s, std::vector<int>& permutations) {
                  int bca, bcb;
  
                  for (uint p = 0; p < bitLength / ( 8 * sizeof ( base ) ); p++) {
                    bca = bitCount64(permutations[p]);
                    bcb = s.bitCount64(permutations[p]);
                    if (bca != bcb)
                      return bca < bcb;
                  }
                  return false;
                }
};

#endif
