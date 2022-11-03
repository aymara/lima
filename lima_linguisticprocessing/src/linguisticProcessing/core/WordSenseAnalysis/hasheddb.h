// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  @author Claire Mouton <claire.mouton@cea.fr>, 
  @author Guillaume Pitel <guillaume.pitel@cea.fr>
*/

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


typedef struct
{
        int index;
        int count;
} vectorCount;

LIMA_WORDSENSEANALYSIS_EXPORT bool vectorCountCompare ( vectorCount i,
                                                        vectorCount j );

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
        comparePermutedSignatures ( std::vector <int> &permutations, 
                                    HashedDB & _hashedDb ) 
        {
          myPermutations = &permutations ;
          hashedDb = &_hashedDb;
        }
        bool operator () ( int i, int j )
        {
          return ( hashedDb->signatures[i].comparePermutedBits ( 
                                hashedDb->signatures[j],*myPermutations ) );
        }
    };

public:
    HashedDB();
    HashedDB(int _signatureLength, int _nSignatures);
    HashedDB(std::string dbFileName);

    ~HashedDB();
    std::vector<int> & fastKNNSearch1 (std::vector<int>&  permutations, 
                                       int beam);
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
    void printStats();
};

}
}
}

#endif
