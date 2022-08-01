// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef COMPOUNDS_COMPATIBILITY_BUILDER_H
#define COMPOUNDS_COMPATIBILITY_BUILDER_H

#include "CompoundsExport.h"
#include<set>
#include<vector>
#include<list>
#include<map>
#include<iostream>

namespace Lima
{
namespace LinguisticProcessing
{
namespace Compounds
{

LIMA_COMPOUNDS_EXPORT std::ostream& operator<<(std::ostream& os, const std::set< uint64_t >& subres);
LIMA_COMPOUNDS_EXPORT std::ostream& operator<<(std::ostream& os, const std::vector< uint64_t >& subres);

class LIMA_COMPOUNDS_EXPORT CompoundsCompatibilityBuilder
{
public:
  CompoundsCompatibilityBuilder();
  virtual ~CompoundsCompatibilityBuilder() {}

  std::list< std::set< uint64_t > > computeCompatibilities(
    std::map< uint64_t, std::set< uint64_t > >& exclusions,
    std::map< uint64_t, bool >& sizes,
    std::vector< uint64_t >& order);
  std::list< std::set< uint64_t > > computeCompatibilitiesWithChain(
    std::map< uint64_t, std::set< uint64_t > >& exclusions,
    std::map< uint64_t, bool >& sizes,
    std::vector< uint64_t >& order);

  void displayResult(const std::list< std::set< uint64_t > >& searchSpace);

  friend std::ostream& operator<<(std::ostream& os, const std::set< uint64_t >& subres);
  friend std::ostream& operator<<(std::ostream& os, const std::vector< uint64_t >& subres);

private:

  std::vector< std::set< uint64_t > > buildSearchSpace(
      std::map< uint64_t, std::set< uint64_t > >& exclusions,
      const std::vector< uint64_t >& order);

  struct SizeSorter
  {
    bool operator()(const std::set< uint64_t >& a, const std::set< uint64_t >& b) const
    {
      if (a.size() > b.size())
      {
        return true;
      }
      else if (a.size() < b.size())
      {
        return false;
      }
      else if (a.empty() && b.empty())
      {
        return false;
      }
      else
      {
        std::set< uint64_t >::const_reverse_iterator ita, ita_end;
        std::set< uint64_t >::const_reverse_iterator itb, itb_end;
        ita = a.rbegin(); ita_end = a.rend();
        itb = b.rbegin(); itb_end = b.rend();
        for (; ita != ita_end; ita++,itb++)
        {
          if (*ita > *itb)
          return true;
        }
      }
      return false;
    }
  };

  std::set< std::set< uint64_t >, SizeSorter >  removeVertexEntriesFromSearchSpace(
      const std::vector< std::set< uint64_t > >& searchSpace,
      std::map< uint64_t, bool >& sizes);


  std::list< std::set< uint64_t > >  removeIncludedVector(const std::set< std::set< uint64_t >, SizeSorter >&  searchSpace);

};

} // SyntacticAnalysis
} // LinguisticProcessing
} // Lima

#endif // COMPOUNDS_COMPATIBILITY_BUILDER_H
