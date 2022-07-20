// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_POSTAGGER_FREQUENCYCOST_H
#define LIMA_POSTAGGER_FREQUENCYCOST_H

#include "PosTaggerExport.h"
#include "ngramMatrices.h"
#include <iostream>

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

class LIMA_POSTAGGER_EXPORT FrequencyCost
{

  friend LIMA_POSTAGGER_EXPORT std::ostream& operator<<(std::ostream& out,const FrequencyCost& cost);

public:
  FrequencyCost();

  FrequencyCost(
    uint64_t nbtrigrams,
    uint64_t nbbigrams,
    uint64_t nbunigrams,
    float trigramproba,
    float bigramproba);

//  FrequencyCost(const FrequencyCost& fc);

  virtual ~FrequencyCost();

  bool operator<(const FrequencyCost& fc) const;
  bool operator==(const FrequencyCost& fc) const;
  FrequencyCost operator+(const FrequencyCost& fc) const;
  FrequencyCost& operator+=(const FrequencyCost& fc);

  inline bool operator!=(const FrequencyCost& fc) const { return !(operator==(fc));}

private:

  uint64_t m_nbtrigrams;
  uint64_t m_nbbigrams;
  uint64_t m_nbunigrams;

  float m_trigramproba;
  float m_bigramproba;

};


class LIMA_POSTAGGER_EXPORT FrequencyCostFunction
{
public:

  FrequencyCostFunction(
    const TrigramMatrix* trigrams,
    const BigramMatrix* bigrams);

  FrequencyCostFunction(
    const FrequencyCostFunction& cf);

  virtual ~FrequencyCostFunction();

  FrequencyCost operator()(LinguisticCode cat1, LinguisticCode cat2, LinguisticCode cat3) const;

  inline FrequencyCost getMinimumCost() const { return m_minimumCost; }

  inline FrequencyCost getMaximumCost() const { return m_maximumCost; }

private:

  const TrigramMatrix* m_trigrams;
  const BigramMatrix* m_bigrams;

  static FrequencyCost m_minimumCost;
  static FrequencyCost m_maximumCost;

};

} // PosTagger
} // LinguisticProcessing
} // Lima

#endif
