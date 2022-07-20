// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_WORDSENSEDISAMBIGUATION_KNNSEARCHER_H
#define LIMA_WORDSENSEDISAMBIGUATION_KNNSEARCHER_H

#include "WordSenseAnalysisExport.h"
#include "hasheddb.h"
#include "lshsignature.h"
#include "CommonTypedefs.h"
#include <string.h>

#include <map>
#include <set>
#include <string>
#include <vector>


namespace Lima
{
namespace LinguisticProcessing
{
namespace WordSenseDisambiguation
{
  
typedef std::map<uint64_t, float> NNList;
typedef std::map<std::string, HashedDB*>  HashedData;




class LIMA_WORDSENSEANALYSIS_EXPORT KnnSearcher
{
  public :
    KnnSearcher(const std::map<std::string, std::string>& knnsearchConfig); 
    
    ~KnnSearcher();
    
    int getKNN(uint64_t lemmaId,
         SemanticContext::const_iterator itContext,
         NNList& knns);
    uint64_t owner2Sys(uint64_t ownerId);
    uint64_t sys2Owner(uint64_t systemId);
    
  protected : 
    std::string m_hashedDir ;
    HashedData m_data;
    char* m_hkMap;
    uint64_t m_hkMapSize;
    //uint64_t m_hashedDbMapSize;
    int m_nbVectors;
    int m_nbDims;
    int m_signatureLength;
    int m_totalPermutations;
    int m_beam;
    int m_k;
    
    std::map<uint64_t,uint64_t> m_owner2System;
    std::map<uint64_t, uint64_t> m_system2Owner;
    
    void mapHashKey(std::string relation);
    //void mapHashedDB(std::string relation);
    LSHSignature* buildSignature(SemanticContext::const_iterator itContext);    
    int fastKNNSort(HashedDB &xoredDB, vectorCount* finalResult);
    
};



}
}
}
#endif // LIMA_WORDSENSEDISAMBIGUATION_KNNSEARCHER_H
