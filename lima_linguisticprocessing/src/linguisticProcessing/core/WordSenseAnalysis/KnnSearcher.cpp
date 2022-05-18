// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "KnnSearcher.h"
#include "hasheddb.h"
#include "lshsignature.h"
#include "toolnumber.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "common/time/traceUtils.h"
#ifndef WIN32
#include <sys/mman.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <algorithm>
#include <boost/lexical_cast.hpp>


using namespace std;

namespace Lima
{
namespace LinguisticProcessing
{
namespace WordSenseDisambiguation
{

  KnnSearcher::KnnSearcher(const map<string, string>& knnsearchConfig) :
    m_hashedDir(),
    m_data(),
    m_hkMap(),
    m_hkMapSize(0),
    m_nbVectors(0),
    m_nbDims(0),
    m_signatureLength(0),
    m_totalPermutations(0),
    m_beam(0),
    m_k(0)
  {
#ifndef WIN32
    LOGINIT("WordSenseDisambiguator");
    if (knnsearchConfig.find("hashedDir") != knnsearchConfig.end())
    {
      m_hashedDir = knnsearchConfig.find("hashedDir")->second;
    }
    else
    {
      m_hashedDir = "hasheddb";
      LWARN << "No hashedDir in knnsearchConfig. Default is set to 'hasheddb'. ";
    }
    if (knnsearchConfig.find("totalPermutations") != knnsearchConfig.end())
    {
      m_totalPermutations =  boost::lexical_cast< int >(knnsearchConfig.find("totalPermutations")->second);
    }
    else
    {
      m_totalPermutations = 10;
      LWARN << "No totalPermutations in knnsearchConfig. Default is set to '10'. ";
    }
    if (knnsearchConfig.find("beam") != knnsearchConfig.end())
    {
      m_beam =  boost::lexical_cast< int >(knnsearchConfig.find("beam")->second);
    }
    else
    {
      m_beam = 20;
      LWARN << "No beam in knnsearchConfig. Default is set to '5'. ";
    }
    if (knnsearchConfig.find("k") != knnsearchConfig.end())
    {
      m_k = boost::lexical_cast< int >(knnsearchConfig.find("k")->second);
    }
    else
    {
      m_k = 50;
      LWARN << "No k in knnsearchConfig. Default is set to '50'. ";
    }

    struct stat filestatus;
    string convFile = m_hashedDir + "/semmapconversion.global.bin";
    stat(convFile.c_str(), &filestatus );
    uint64_t mmapSize = filestatus.st_size;
    int nbVects = mmapSize/sizeof(uint64_t);
    int ifd = open(convFile.c_str(), O_RDONLY);
    if (ifd == -1)
      cerr << "Error on opening file " << convFile.c_str() << endl;

    uint64_t* codemmap = (uint64_t*)mmap(0, mmapSize, PROT_READ, MAP_PRIVATE, ifd, 0);
    if (codemmap == MAP_FAILED)
    {
      close(ifd);
      perror("Error mmapping the file");
      exit(EXIT_FAILURE);
    }
    else if (codemmap == NULL)
    {
      close(ifd);
      perror("Error mmapping the file - codemmap == NULL");
      exit(EXIT_FAILURE);
    }
    for (int i = 0; i < nbVects; i++)
    {
      m_owner2System[codemmap[i]]=i;
      m_system2Owner[i]=codemmap[i];
    }
    if (munmap(codemmap,  mmapSize) == -1) {
      LWARN << "Error un-mmapping the converter file "<< convFile;
    }
#endif
  }


  KnnSearcher::~KnnSearcher()
  {
#ifndef WIN32
    LOGINIT("WordSenseDisambiguator");
    if (munmap(m_hkMap,  m_hkMapSize) == -1) {
      LWARN << "Error un-mmapping the hk file ";
    }
#endif
   }

int KnnSearcher::getKNN(uint64_t lemmaId,
      SemanticContext::const_iterator itContext,
      NNList& knns)
{
  LOGINIT("WordSenseDisambiguator");

  HashedDB* hashedDb;
  if (m_data.find(itContext->first) == m_data.end()
    || m_data.find(itContext->first)->second == NULL)
  {
    string hashedDbFile = m_hashedDir + "/SemMap." + itContext->first + ".16384.hashed";
    hashedDb = new HashedDB(hashedDbFile);
    m_nbVectors = hashedDb->getNSignatures();
    m_data[itContext->first] = hashedDb;
  }
  else
  {
    hashedDb = m_data.find(itContext->first)->second;
  }
  if (m_hkMap == NULL)
  {
    mapHashKey(itContext->first);
  }
  // hash context :D
  TimeUtils::updateCurrentTime();
  uint64_t start = time(NULL);
  //LSHSignature * instanceSignature = buildSignature(itContext);
  LSHSignature * instanceSignature = new LSHSignature(m_signatureLength);
  uint64_t end = time(NULL);
  LDEBUG << "Hash context duration " << end-start << " s ";

  // knn lookup
  // Extract target vector signature (partial)
  LSHSignature targetSignature = hashedDb->getSignature ( owner2Sys(lemmaId ) );

  // change with instance Signature
  hashedDb->insert (owner2Sys(lemmaId), *instanceSignature);

  // XOR the hashed DB with the target signature (partial)
  LDEBUG << "Xoring hashed db" ;
  LSHSignature target1 = hashedDb->getSignature ( 0 );
  LSHSignature target2 = hashedDb->getSignature ( 1 );
  cerr << owner2Sys(lemmaId) << endl;
  LSHSignature trueTarget = hashedDb->getSignature ( owner2Sys(lemmaId) );
  target1.print(0,20);
  target2.print(0,20);
  trueTarget.print(0,20);
  hashedDb->xorX ( *instanceSignature );
  target1 = hashedDb->getSignature ( 0 );
  target2 = hashedDb->getSignature ( 1 );
  trueTarget = hashedDb->getSignature ( owner2Sys(lemmaId) );

  LDEBUG << "Sorting hashed db";
  // Fast Search
  /*
  vectorCount* finalResult = NULL;
  int resultsCount = fastKNNSort ( *hashedDb, finalResult );
  */
  vectorCount* finalResult = (vectorCount*) malloc (50*sizeof(vectorCount));
  int resultsCount = 50;

  cerr << "test2 : " ;
  cerr << finalResult[0].index << endl;
  // Presenting results
  LDEBUG << "ResultsCount : " << resultsCount;
  for (int i = 0; i < resultsCount; i++)
  {
    cerr << "Reading at : " << i << endl;
    LDEBUG << sys2Owner(finalResult[i].index) << " : " << finalResult[i].count;
    knns[sys2Owner(finalResult[i].index)] = finalResult[i].count;
  }

  // back to original hasheddb
  hashedDb->insert (owner2Sys(lemmaId), targetSignature);
  free(finalResult);
  delete instanceSignature;
  //delete hashedDb;
  //return knns[sys2Owner(finalResult[resultsCount].index)];
  return knns.size();
}


int KnnSearcher::fastKNNSort(HashedDB& xoredDb, vectorCount* semiFinalResult)
{
  LOGINIT("WordSenseDisambiguator");
  // m_totalPermutations : the number of permutations performed by the whole thing
  // result will contain beam * nPermutations candidates
  int nbLongsInLSH = m_signatureLength  / ( 8*sizeof ( LSHSignature::base ) );
  vector<int> result;
  for ( int permutationI = 0; permutationI < m_totalPermutations; permutationI ++ )
    {
      // count permutation's bits for this candidate
      // permutations : actually, a randomization from {0,1,2,...,X}
      vector<int> & permutation = ToolNumber::makePermutation ( nbLongsInLSH );

      int beam = m_beam > m_nbVectors
        ? m_nbVectors
        :m_beam ;
      cerr << "beam "<< m_beam << "|" << m_nbVectors << " -> " << beam << endl;
      vector<int> & tmpResult = xoredDb.fastKNNSearch1 ( permutation ,beam ) ;
      delete &permutation;
      for ( vector<int>::iterator candidateItr = tmpResult.begin(); candidateItr != tmpResult.end(); candidateItr ++ )
      {
        LDEBUG << "tmpResult systemId : "<< *candidateItr << " -> "<< sys2Owner(*candidateItr)  << " : " << xoredDb.getSignature ( *candidateItr ).bitCount() ;
        xoredDb.getSignature ( *candidateItr ).print(0,30);
      }
      result.insert ( result.end(), tmpResult.begin(), tmpResult.end() );
      delete &tmpResult;
    }
  //  cerr << "results.size : " << result.size() << endl;
  // count local bits for each candidate
  set<int> doneSet;
  // semiFinalResult : (index, semiFinalCount)[maxPermutations*beam]
  const int maxResults = m_totalPermutations * m_beam;
  semiFinalResult=(vectorCount*) malloc (maxResults*sizeof(vectorCount));
  LDEBUG << "Max results : " << maxResults;
  int resultsCount;
  int resultIndex = 0;
  // merge local candidates
  for ( vector<int>::iterator candidateItr = result.begin(); candidateItr != result.end(); candidateItr ++ )
    if ( doneSet.find ( *candidateItr ) == doneSet.end() )
      {
        semiFinalResult[resultIndex].index = *candidateItr;
        LDEBUG << "ownerId : "<< *candidateItr;
        semiFinalResult[resultIndex].count = xoredDb.getSignature ( *candidateItr ).bitCount();
        LDEBUG << "count : " << semiFinalResult[resultIndex].count;
        doneSet.insert ( *candidateItr );
        resultIndex++;
        cerr << "resultindex : " << resultIndex << endl;
      }
  resultsCount = resultIndex;

  for (; resultIndex < maxResults; resultIndex++)
  {
    semiFinalResult[resultIndex].index = -1;
    semiFinalResult[resultIndex].count = m_signatureLength + 1;
  }

  // Now, every process has a bunch of candidates and their full cosine approximation : we must sort / merge and keep only neighbours best values
  // Select the n-best candidates in the current (local) list / The n-best are sorted

  if ( m_k > resultsCount)
  {
    LDEBUG << "m_k ("
           <<  m_k
           << ")> resultsCount ("
           << resultsCount
           << "). Process will potentially fail.";
  }
  //LDEBUG << "nbTerms  + 1 : " << m_nbVectors  + 1;
  int limitSort =  m_k > resultsCount
    ? resultsCount
    : m_k ;
  LDEBUG << "limit sort : " << limitSort;
  partial_sort ( semiFinalResult, semiFinalResult + limitSort, semiFinalResult + resultsCount, vectorCountCompare );

  cerr << "maxResults : " << maxResults << endl;
  cerr << "resultsCount : " << resultsCount << endl;
  cerr << "test : " << semiFinalResult[0].index << endl;

  return resultsCount;
}


LSHSignature* KnnSearcher::buildSignature(SemanticContext::const_iterator itContext)
{
  LOGINIT("WordSenseDisambiguator");
  LSHSignature* instanceSignature = new LSHSignature(m_signatureLength);
  int  nPairs = itContext->second.size();
  char * data;

  data = (char*) malloc (nPairs*(sizeof(uint)+sizeof(float)));
  int cntCtx = 0;
  for (set<uint64_t>::const_iterator itValues = itContext->second.begin();
          itValues != itContext->second.end();
          itValues++)
  {
    uint tmpI = owner2Sys(*itValues);
    memcpy(&data[cntCtx], &tmpI, sizeof(uint));
    float tmpF = (float)1./(float)nPairs;
    memcpy(&data[cntCtx]+sizeof(int), &tmpF, sizeof(float));
    cntCtx++;
  }
  assert(nPairs==cntCtx);

  LDEBUG << "magiccosine at " << itContext->first << " - m_nbDims = " << m_nbDims << " - nPairs = " << nPairs  ;
  uint64_t start = time(NULL);
  assert(m_data.find(itContext->first)!=m_data.end());
  assert(m_hkMap!=NULL);

  for ( int key=0; key < m_signatureLength; key++ )
  {
    if ( ToolNumber::magiccosine ( data ,
                       ((float*) (m_hkMap + 2*sizeof(int) +key* m_nbDims*sizeof(float))),
                       m_nbDims,
                       nPairs) > 0 )
    {
      instanceSignature->setBit ( key );
    }
    else
    {
      instanceSignature->unsetBit ( key );
    }
  }

  free(data);
  uint64_t end = time(NULL);
  LDEBUG << "cosine duration : " << end-start<<  " s" ;
  TimeUtils::logElapsedTime("Hash context");
  return instanceSignature;
}

/*
void KnnSearcher::mapHashedDB(string relation)
{
  LOGINIT("WordSenseDisambiguator");
  //--------initialize mmap ----------
  string hashedDbFile = m_hashedDir + "SemMap."+ relation + ".16384.A.hasheddb";

  int fd = open(hashedDbFile.c_str(), O_RDONLY);
  if (fd == -1) {
    perror("Error opening file for reading");
    exit(EXIT_FAILURE);
  }
  int *premap;  // mmapped array of int's
  premap = (int*)mmap(0, 2*sizeof(int), PROT_READ, MAP_PRIVATE, fd, 0);
  if (premap == MAP_FAILED) {
    close(fd);
    perror("Error mmapping the file");
    exit(EXIT_FAILURE);
  }

  // Read the file int-by-int from the mmap

  m_signatureLength = premap[0];
  m_nbVectors = premap[1];
  //LDEBUG << lshSignatureLength << "*" << nVectors << "=" <<  lshSignatureLength * nVectors ;
  if (munmap(premap, 2*sizeof(int)) == -1) {
    perror("Error un-mmapping the file");
  }
  close(fd);
  open(hashedDbFile.c_str(), O_RDONLY);
  //      LDEBUG << "lshSignatureLength : " << lshSignatureLength ;
  //      LDEBUG << "nVectors : " << nVectors ;
  if (m_hashedDbMapSize=0)
  {
    m_hashedDbMapSize = (m_signatureLength/8)*m_nbVectors+ 2*sizeof(int);
  }
  LDEBUG << m_hashedDbMapSize;
  LDEBUG << "hashed map n vectors " << m_nbVectors ;
  //        LDEBUG << "HASHEDMAP SIZE : " << hashedMapSize ;
  //              LDEBUG << "getPageSize "<< getpagesize() ;
  m_data[relation].second = (char*)mmap(0, m_hashedDbMapSize, PROT_READ, MAP_PRIVATE, fd, 0);
  if (m_data[relation].second == MAP_FAILED) {
    close(fd);
    perror("Error mmapping the file");
    exit(EXIT_FAILURE);
  }

  close(fd);
  //-----------------------------


}
*/


void KnnSearcher::mapHashKey(string relation)
{
#ifndef WIN32
  LOGINIT("WordSenseDisambiguator");
  map<uint64_t, int> dists;
  string hashkeyFile = m_hashedDir + "/SemMap.all.16384.hashkey";
  int hkfd = open(hashkeyFile.c_str(), O_RDONLY);
  if (hkfd == -1) {
    LERROR << "Error opening file "<< hashkeyFile <<" for reading";
    exit(EXIT_FAILURE);
  }
  int *hkpremap;  // mmapped array of int's
  hkpremap = (int*)mmap(0, 2*sizeof(int), PROT_READ, MAP_PRIVATE, hkfd, 0);
  if (hkpremap == MAP_FAILED) {
    close(hkfd);
    LDEBUG << "Error mmapping the file " << hashkeyFile ;
    exit(EXIT_FAILURE);
  }

  // Read the file int-by-int from the mmap
  m_signatureLength = hkpremap[1];
  m_nbDims = hkpremap[0];
  //LDEBUG << lshSignatureLength << "*" << nVectors << "=" <<  lshSignatureLength * nVectors ;
  if (munmap(hkpremap, 2*sizeof(int)) == -1)
  {
    LDEBUG << "Error un-mmapping the file " << hashkeyFile ;
  }
  if (m_hkMapSize == 0 )
  {
    m_hkMapSize = m_signatureLength*m_nbDims*sizeof(float)+ 2*sizeof(int);
  }
  m_hkMap = (char*)mmap(0, m_hkMapSize, PROT_READ, MAP_PRIVATE, hkfd, 0);
  if (m_hkMap == MAP_FAILED)
  {
    close(hkfd);
    LDEBUG << "Error mmapping the file " << hashkeyFile << " at the second time ." ;
    exit(EXIT_FAILURE);
  }

  close(hkfd);

  LDEBUG << "Hashkey mapped for " << relation ;
#endif
}



uint64_t KnnSearcher::owner2Sys(uint64_t ownerId)
{
  if (m_owner2System.find(ownerId)!=m_owner2System.end())
  {
    return m_owner2System[ownerId];
  }
  return 2000000;
}


uint64_t KnnSearcher::sys2Owner(uint64_t systemId)
{
  if (m_system2Owner.find(systemId)!=m_system2Owner.end())
  {
    return m_system2Owner[systemId];
  }
  return 0;
}

}
}
}
