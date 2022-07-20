// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
