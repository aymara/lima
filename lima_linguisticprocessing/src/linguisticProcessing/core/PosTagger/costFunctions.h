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
