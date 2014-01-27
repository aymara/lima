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
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_LINGUISTICPROCESSING_POSTAGGING_VITERBIUTILS_H
#define LIMA_LINGUISTICPROCESSING_POSTAGGING_VITERBIUTILS_H

#include "common/MediaticData/mediaticData.h"
#include "common/Data/strwstrtools.h"

#include "frequencyCost.h"

#include <boost/regex.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace Lima
{

namespace LinguisticProcessing
{

namespace PosTagger
{


template<typename Cost, typename CostFactory>
class ViterbiCostFunction
{
public:

  ViterbiCostFunction(
    MediaId language,
    const Common::PropertyCode::PropertyManager* microManager,
    const CostFactory& costFactory,
    const std::string& trigramsFile,
    const std::string& bigramsFile);

  virtual ~ViterbiCostFunction() {}

  template<typename PredData>
  void apply(LinguisticCode micro,PredData& predData,const std::vector<PredData>& predPredData) const;

  inline Cost getMinimumCost() const { return m_minimumCost; }

  inline Cost getMaximumCost() const { return m_maximumCost; }

private:

  void readTrigramMatrixFile(const std::string& fileName,const CostFactory& costFactory);
  void readBigramMatrixFile(const std::string& fileName,const CostFactory& costFactory);
  void sortData();

  MediaId m_language;
  const Common::PropertyCode::PropertyManager* m_microManager;
  Cost m_minimumCost;
  Cost m_maximumCost;
  Cost m_unigramCost;

  struct CategCostPair
  {
    LinguisticCode m_cat;
    Cost m_cost;
    // lets us insert the struct in ordered containers
    inline bool operator<(const CategCostPair& ccp) const { return m_cat<ccp.m_cat;}
  };

  struct GramsData
  {
    Cost m_default; // cost of the bigram, or unigram cost if not
    std::vector<CategCostPair> m_grams; // this is the last part of the n-gram.
  };

  std::map<LinguisticCode, std::map<LinguisticCode, GramsData> > m_data;

  // this is used to store the bigram frequencies. code + code -> bigram
  std::map<LinguisticCode, std::map<LinguisticCode, float> > m_biFreq;

};

class FrequencyCostFactory
{
public:
  FrequencyCost getCost(bool isTrigram,bool isBigram,bool,float triFreq) const
  {
    if (isTrigram) { return FrequencyCost(1,0,0,triFreq,0); }
    if (isBigram) { return FrequencyCost(0,1,0,0,triFreq); }
    else { return FrequencyCost(0,0,1,0,0);}
  }

  FrequencyCost getMinimumCost() const
    { return FrequencyCost(0,0,0,0,0);}

  FrequencyCost getMaximumCost() const
    { return FrequencyCost(0,0,999999,0,0); }

};

template <class CostType>
class SimpleIntegerCostFactory
{
public:

  SimpleIntegerCostFactory(
    uint64_t trigramCost,
    uint64_t bigramCost,
    uint64_t unigramCost) :
      m_trigramcost(trigramCost),
      m_bigramcost(bigramCost),
      m_unigramcost(unigramCost)
  {}

  CostType getCost(bool isTrigram,bool isBigram,bool,float) const
  {
    if (isTrigram) { return m_trigramcost; }
    if (isBigram) { return m_bigramcost; }
    else { return m_unigramcost;}
  }

  CostType getMinimumCost() const
  { return CostType(0);}

  CostType getMaximumCost() const
  { return CostType(std::numeric_limits<uint64_t>::max()); }

private:
  CostType m_trigramcost;
  CostType m_bigramcost;
  CostType m_unigramcost;

};

  
} // PosTagger
} // LinguisticProcessing
} // FrCeaLic2M

#include "viterbiUtils.tcc"

#endif
