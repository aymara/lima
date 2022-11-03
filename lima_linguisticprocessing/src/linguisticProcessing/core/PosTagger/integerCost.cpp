// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "integerCost.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

IntegerCost::IntegerCost() :
m_cost(0)
{}

IntegerCost::IntegerCost(uint64_t cost) :
m_cost(cost)
{}

IntegerCost::~IntegerCost() {}

bool IntegerCost::operator<(const IntegerCost& fc) const
{
  return m_cost<fc.m_cost;
}

bool IntegerCost::operator==(const IntegerCost& fc) const
{
  return (m_cost==fc.m_cost);
}

std::ostream& operator<<(std::ostream& out,const IntegerCost& cost)
{
  out << "intcost{" << cost.m_cost << "}";
  return out;
}


IntegerCost IntegerCost::operator+(const IntegerCost& fc) const
{
  return IntegerCost(m_cost+fc.m_cost);
}

IntegerCost& IntegerCost::operator+=(const IntegerCost& fc)
{
  m_cost+=fc.m_cost;
  return *this;
}



IntegerCostFunction::IntegerCostFunction()
{}

IntegerCostFunction::IntegerCostFunction(const IntegerCostFunction& cf)
{
  LIMA_UNUSED(cf)
}

IntegerCostFunction::~IntegerCostFunction() {}

IntegerCost IntegerCostFunction::operator()(Lima::LinguisticCode cat1, Lima::LinguisticCode cat2, Lima::LinguisticCode cat3) const
{
  LIMA_UNUSED(cat1)
  LIMA_UNUSED(cat2)
  LIMA_UNUSED(cat3)
  return IntegerCost(0);
}

} // PosTagger
} // LinguisticProcessing
}// Lima
