// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

//
// C++ Implementation: hasheddb
//
// Description:
//
//
// Author: Claire Mouton, Guillaume Pitel <claire.mouton@cea.fr, guillaume.pitel@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include "hasheddb.h"
#include "lshsignature.h"
#ifdef DMALLOC
#include "dmalloc.h"
#endif

using namespace std;

namespace Lima
{
namespace LinguisticProcessing
{
namespace WordSenseDisambiguation
{

bool vectorCountCompare ( vectorCount i,vectorCount j )
{
  if ( i.count == j.count)
    return i.index < j.index;
  return ( i.count < j.count );
}

HashedDB::HashedDB()
{

}

HashedDB::~HashedDB()
{
    delete nbZero;
}

HashedDB::HashedDB ( int _signatureLength, int _nSignatures )
{
        signatureLength = _signatureLength;
        nSignatures = _nSignatures;
  nbZero = new int[signatureLength];
  for (int s= 0; s<signatureLength; s++)
    nbZero[s] = 0;
}

void HashedDB::append ( LSHSignature & s )
{
  signatures.push_back ( s );
}

void HashedDB::removeLast (  )
{
  signatures.pop_back (  );
}

void HashedDB::insert ( int ind, LSHSignature & s )
{
  cerr << "Insert @ " << ind <<  endl;
  s.print(0,10);
  signatures.at(ind)= s ;
}



HashedDB::HashedDB ( string dbFileName )
{
  nbZero = new int[signatureLength];
  int totalSignatureLength=0;
  ifstream is;
  is.open(dbFileName.c_str(), std::ifstream::binary);
  if (!is.is_open())
  {
    cerr << "Warning: Error opening file " << dbFileName << endl;
  }
  is.seekg(0,ios_base::beg);
  is.read((char*)&totalSignatureLength, sizeof(totalSignatureLength));
  is.read((char*)&nSignatures, sizeof(nSignatures));
  cerr << "Sig Length : " << totalSignatureLength << endl;
  cerr << "Nb signatures : " << nSignatures << endl;

  signatureLength = totalSignatureLength;

  for (int sigIndex = 0; sigIndex < nSignatures; sigIndex++) {
    LSHSignature * newLshSig = new LSHSignature(signatureLength);
    append(*newLshSig);
    // Read from disk
    is.read((char*)signatures[sigIndex].getData(), totalSignatureLength/8);
  }
  LSHSignature target1 = getSignature ( 0 );
  LSHSignature target2 = getSignature ( 2 );
  LSHSignature target3 = getSignature ( 3 );
  cerr << "Test loadHAshed db : ";
  target1.print(1,20);
  target2.print(2,20);
target3.print(3,20);
  is.close();
}

vector< int > & HashedDB::fastKNNSearch1 ( vector<int>& permutations, int beam )
{
  vector<int> sortBuffer ( nSignatures );
  vector<int> *result = new vector<int> ( beam );
  for ( int i = 0; i < nSignatures; i++ )
    sortBuffer[i] = i;
  if (signatureLength > 0)
    cerr << "before sort " << *sortBuffer.begin() << " " ;
  getSignature(*(sortBuffer.begin()+1)).print(getSignature(*(sortBuffer.begin()+1)).bitCount(), 20 );
  getSignature(60996).print(60996, 20);
  nth_element ( sortBuffer.begin(),
      sortBuffer.begin() + beam,
      sortBuffer.end(),
      (comparePermutedSignatures ( permutations, *this ) ) );

  cerr << "after sort " << *sortBuffer.begin() << " " ;
  getSignature(*(sortBuffer.begin()+1)).print(getSignature(*(sortBuffer.begin()+1)).bitCount(), 20 );

  copy ( sortBuffer.begin(), sortBuffer.begin() + beam, (*result).begin() );
  return *result;
}

void HashedDB::xorX(LSHSignature & s)
{
  if (signatureLength > 0)
    for (int i = 0; i < nSignatures; i++) {
      signatures[i].xorX(s);
    }
}

void HashedDB::printStats()
{
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

}
}
}
