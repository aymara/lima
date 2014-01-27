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

#ifndef LIMA_POSTAGGER_INTEGERCOST_H
#define LIMA_POSTAGGER_INTEGERCOST_H

#include "PosTaggerExport.h"
#include <iostream>
#include <limits>
#include <common/LimaCommon.h>

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

class LIMA_POSTAGGER_EXPORT IntegerCost
{

  friend LIMA_POSTAGGER_EXPORT std::ostream& operator<<(std::ostream& out,const IntegerCost& cost);

public:
  IntegerCost();
  
  IntegerCost(uint64_t cost);

//  IntegerCost(const IntegerCost& fc);

  virtual ~IntegerCost();

  bool operator<(const IntegerCost& fc) const;
  bool operator==(const IntegerCost& fc) const;
  IntegerCost operator+(const IntegerCost& fc) const;
  IntegerCost& operator+=(const IntegerCost& fc);

  inline bool operator!=(const IntegerCost& fc) const { return !(operator==(fc));}

private:

  uint64_t m_cost;
};

#ifdef WIN32
#undef max
#endif

class LIMA_POSTAGGER_EXPORT IntegerCostFunction
{
public:

  IntegerCostFunction();

  IntegerCostFunction(
    const IntegerCostFunction& cf);

  virtual ~IntegerCostFunction();

  IntegerCost operator()(LinguisticCode cat1, LinguisticCode cat2, LinguisticCode cat3) const;

  inline IntegerCost getMinimumCost() const { return 0; }

  inline IntegerCost getMaximumCost() const { return std::numeric_limits<uint64_t>::max(); }

private:


};

} // PosTagger
} // LinguisticProcessing
} // Lima

#endif
