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
