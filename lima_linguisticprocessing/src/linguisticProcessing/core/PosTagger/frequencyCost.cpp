// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "frequencyCost.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

FrequencyCost::FrequencyCost() :
    m_nbtrigrams(0),
    m_nbbigrams(0),
    m_nbunigrams(0),
    m_trigramproba(0),
    m_bigramproba(0)
{}

FrequencyCost::FrequencyCost(
  uint64_t nbtrigrams,
  uint64_t nbbigrams,
  uint64_t nbunigrams,
  float trigramproba,
  float bigramproba) :
    m_nbtrigrams(nbtrigrams),
    m_nbbigrams(nbbigrams),
    m_nbunigrams(nbunigrams),
    m_trigramproba(trigramproba),
    m_bigramproba(bigramproba)
{}

/*
FrequencyCost::FrequencyCost(const FrequencyCost& fc) :
    m_nbtrigrams(fc.m_nbtrigrams),
    m_nbbigrams(fc.m_nbbigrams),
    m_nbunigrams(fc.m_nbunigrams),
    m_trigramproba(fc.m_trigramproba),
    m_bigramproba(fc.m_bigramproba)
{}
*/

FrequencyCost::~FrequencyCost() {}

bool FrequencyCost::operator<(const FrequencyCost& fc) const
{
  if (m_nbunigrams!=fc.m_nbunigrams) return m_nbunigrams<fc.m_nbunigrams;
  if (m_nbbigrams!=fc.m_nbbigrams) return m_nbbigrams<fc.m_nbbigrams;
  if (m_bigramproba!=fc.m_bigramproba) return m_bigramproba>fc.m_bigramproba;
  if (fc.m_nbtrigrams==0) return false;
  if (m_nbtrigrams==0) return true;
  return ((m_trigramproba/m_nbtrigrams)>(fc.m_trigramproba/fc.m_nbtrigrams));
}

bool FrequencyCost::operator==(const FrequencyCost& fc) const
{
  return ((m_trigramproba==fc.m_trigramproba) &&
          (m_bigramproba==fc.m_bigramproba) &&
          (m_nbtrigrams==fc.m_nbtrigrams) &&
          (m_nbbigrams==fc.m_nbbigrams) &&
          (m_nbunigrams==fc.m_nbunigrams));
}

std::ostream& operator<<(std::ostream& out,const FrequencyCost& cost)
{
  out << "freqcost{" << cost.m_nbtrigrams << "," << cost.m_nbbigrams << "," << cost.m_nbunigrams << ";" << cost.m_trigramproba << ";" << cost.m_bigramproba << "}";
  return out;
}


FrequencyCost FrequencyCostFunction::m_minimumCost(0,0,0,0,0);
FrequencyCost FrequencyCostFunction::m_maximumCost(0,0,999999,0,0);


FrequencyCost FrequencyCost::operator+(const FrequencyCost& fc) const
{
  return FrequencyCost(
           m_nbtrigrams+fc.m_nbtrigrams,
           m_nbbigrams+fc.m_nbbigrams,
           m_nbunigrams+fc.m_nbunigrams,
           m_trigramproba+fc.m_trigramproba,
           m_bigramproba+fc.m_bigramproba);
}

FrequencyCost& FrequencyCost::operator+=(const FrequencyCost& fc)
{
  m_nbtrigrams+=fc.m_nbtrigrams;
  m_nbbigrams+=fc.m_nbbigrams;
  m_nbunigrams+=fc.m_nbunigrams;
  m_trigramproba+=fc.m_trigramproba;
  m_bigramproba+=fc.m_bigramproba;
  return *this;
}



FrequencyCostFunction::FrequencyCostFunction(
  const TrigramMatrix* trigrams,
  const BigramMatrix* bigrams) :
    m_trigrams(trigrams),
    m_bigrams(bigrams)
{}

FrequencyCostFunction::FrequencyCostFunction(
  const FrequencyCostFunction& cf) :
    m_trigrams(cf.m_trigrams),
    m_bigrams(cf.m_bigrams)
{}

FrequencyCostFunction::~FrequencyCostFunction() {}

FrequencyCost FrequencyCostFunction::operator()(Lima::LinguisticCode cat1, Lima::LinguisticCode cat2, Lima::LinguisticCode cat3) const
{
  float f=m_trigrams->freq(cat1,cat2,cat3);
  if (f>0)
  {
    float b=m_bigrams->freq(cat1,cat2);
    if (b>0)
    {
      return FrequencyCost(1,0,0,f/b,0);
    }
    else
    {
      // should not happen
      return FrequencyCost(1,0,0,f,0);
    }
  }
  else
  {
    f=m_bigrams->freq(cat2,cat3);
    if (f>0)
    {
      return FrequencyCost(0,1,0,0,f);
    }
    else
    {
      return FrequencyCost(0,0,1,0,0);
    }
  }
}

} // PosTagger
} // LinguisticProcessing
}// Lima
