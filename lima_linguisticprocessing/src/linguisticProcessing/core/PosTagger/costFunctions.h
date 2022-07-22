// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_POSTAGGER_COSTFUNCTION_H
#define LIMA_POSTAGGER_COSTFUNCTION_H

#include "ngramMatrices.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{


template<typename Cost>
class SimpleCostFunction
{
    public:

        SimpleCostFunction(
            const TrigramMatrix* trigrams,
            const BigramMatrix* bigrams,
            Cost trigramCost,
            Cost bigramCost,
            Cost unigramCost,
            Cost minimumCost,
            Cost maximumCost) :
                m_trigrams(trigrams),
                m_bigrams(bigrams),
                m_trigramCost(trigramCost),
                m_bigramCost(bigramCost),
                m_unigramCost(unigramCost),
                m_minimumCost(minimumCost),
                m_maximumCost(maximumCost)
        {}

        virtual ~SimpleCostFunction() {}

        Cost operator()(uint64_t cat1,uint64_t cat2,uint64_t cat3) const;

        inline Cost getMinimumCost() const { return m_minimumCost; }

        inline Cost getMaximumCost() const { return m_maximumCost; }

    private:

        const TrigramMatrix* m_trigrams;
        const BigramMatrix* m_bigrams;

        Cost m_trigramCost;
        Cost m_bigramCost;
        Cost m_unigramCost;
        Cost m_minimumCost;
        Cost m_maximumCost;

};

template<typename Cost>
Cost SimpleCostFunction<Cost>::operator()(uint64_t cat1,uint64_t cat2,uint64_t cat3) const
{
    if (m_trigrams->exists(cat1,cat2,cat3))
    {
        return m_trigramCost;
    }
    if (m_bigrams->exists(cat2,cat3))
    {
        return m_bigramCost;
    }
    return m_unigramCost;
}

template<typename Cost>
class ThresholdCostFunction
{
    public:

        ThresholdCostFunction(
            const TrigramMatrix* trigrams,
            const BigramMatrix* bigrams,
            float trigramThreshold,
            Cost goodTrigramCost,
            Cost badTrigramCost,
            Cost bigramCost,
            Cost unigramCost,
            Cost minimumCost,
            Cost maximumCost) :
                m_trigrams(trigrams),
                m_bigrams(bigrams),
                m_trigramThreshold(trigramThreshold),
                m_goodTrigramCost(goodTrigramCost),
                m_badTrigramCost(badTrigramCost),
                m_bigramCost(bigramCost),
                m_unigramCost(unigramCost),
                m_minimumCost(minimumCost),
                m_maximumCost(maximumCost)
        {}

        virtual ~ThresholdCostFunction() {}

        Cost operator()(uint64_t cat1,uint64_t cat2,uint64_t cat3) const;

        inline Cost getMinimumCost() const { return m_minimumCost; }

        inline Cost getMaximumCost() const { return m_maximumCost; }

    private:

        const TrigramMatrix* m_trigrams;
        const BigramMatrix* m_bigrams;

        float m_trigramThreshold;
        Cost m_goodTrigramCost;
        Cost m_badTrigramCost;
        Cost m_bigramCost;
        Cost m_unigramCost;
        Cost m_minimumCost;
        Cost m_maximumCost;

};

template<typename Cost>
Cost ThresholdCostFunction<Cost>::operator()(uint64_t cat1,uint64_t cat2,uint64_t cat3) const
{
    float f=m_trigrams->freq(cat1,cat2,cat3);
    if (f!=0)
    {
  float b=m_bigrams->freq(cat1,cat2);
        if ( (b>0) && ((f/b)>m_trigramThreshold))
        {
            return m_goodTrigramCost;
        }
        else
        {
            return m_badTrigramCost;
        }
    }
    if (m_bigrams->exists(cat2,cat3))
    {
  return m_bigramCost;
    }
    return m_unigramCost;
}


}
}
}

#endif
