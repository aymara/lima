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
// C++ Interface: hasheddb
//
// Description: 
//
//
// Author: Claire Mouton, Guillaume Pitel <claire.mouton@cea.fr, guillaume.pitel@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef HASHEDDB_H
#define HASHEDDB_H

#include "WordSenseAnalysisExport.h"
#include <vector>
#include <string>
#include <stdio.h>
#include <string.h>
#include "lshsignature.h"


namespace Lima
{
namespace LinguisticProcessing
{
namespace WordSenseDisambiguation
{

/**
  @author Claire Mouton, Guillaume Pitel <claire.mouton@cea.fr>, <guillaume.pitel@gmail.com>
*/

typedef struct
{
        int index;
        int count;
} vectorCount;

LIMA_WORDSENSEANALYSIS_EXPORT bool vectorCountCompare ( vectorCount i,vectorCount j );

class LIMA_WORDSENSEANALYSIS_EXPORT HashedDB{
  private:
    int signatureLength;
    int nSignatures;
    int *nbZero;
    std::vector<LSHSignature> signatures;
    class comparePermutedSignatures
    {
      std::vector<int>  *myPermutations;
      HashedDB * hashedDb;
      public:
        comparePermutedSignatures ( std::vector <int> &permutations, HashedDB & _hashedDb ) 
        { myPermutations = &permutations ;
        hashedDb = &_hashedDb;}
        bool operator () ( int i, int j )
        {
          return ( hashedDb->signatures[i].comparePermutedBits ( hashedDb->signatures[j],*myPermutations ) );
        }
    };
    
public:
    HashedDB();
    HashedDB(int _signatureLength, int _nSignatures);
    HashedDB(std::string dbFileName);

    ~HashedDB();
    std::vector<int> & fastKNNSearch1 (std::vector<int>&  permutations, int beam);
    void append(LSHSignature & s);
    void removeLast();
    void insert(int ind, LSHSignature & s);
    void writeToFile(std::string fileName, int rank, int nproc);
    LSHSignature & getSignature(int key) { return signatures.at(key); }
    void xorX(LSHSignature & s);
    int getNSignatures() { return nSignatures; }
    void setNSignatures(int n ) { nSignatures = n; }
    void incrNSignatures() { nSignatures++; }
    void decrNSignatures() { nSignatures--; }
    int getSignatureLength() { return signatureLength; }
    void printStats() {
      for (int sig = 0; sig < nSignatures; sig++)
  for (int s = 0; s < signatureLength; s++)
    if (!signatures[sig].getBit(s))
      nbZero[s]++;
      std::ostringstream oss;
      oss << "Sigs zeroes/ones count : " << std::endl;
      for (int s = 0; s < signatureLength; s++)
        oss << nbZero[s] << "/" << nSignatures - nbZero[s] << ",";
      oss << std::endl;
      std::cerr << oss.str();
    }
};

}
}
}
  

#endif
