// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "CompoundsCompatibilityBuilder.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include <iostream>
#include <algorithm>

namespace Lima
{
namespace LinguisticProcessing
{
namespace Compounds
{

CompoundsCompatibilityBuilder::CompoundsCompatibilityBuilder()
{
}

std::list< std::set< uint64_t > > CompoundsCompatibilityBuilder::computeCompatibilities(
  std::map< uint64_t, std::set< uint64_t > >& exclusions,
  std::map< uint64_t, bool >& sizes,
  std::vector< uint64_t >& order)
{
  std::vector< std::set< uint64_t > > searchspace;
  searchspace = buildSearchSpace(exclusions, order);

  std::set< std::set< uint64_t >, SizeSorter > cleanedSearchSpace;
  cleanedSearchSpace = removeVertexEntriesFromSearchSpace(searchspace, sizes);

  std::list< std::set< uint64_t > > result;
  result = removeIncludedVector(cleanedSearchSpace);

//  displayResult(result);
  return result;
}

std::list< std::set< uint64_t > > CompoundsCompatibilityBuilder::computeCompatibilitiesWithChain(
  std::map< uint64_t, std::set< uint64_t > >& exclusions,
  std::map< uint64_t, bool >& sizes,
  std::vector< uint64_t >& order)
{
/*
  Critical Function : comment logging messages
*/
//  COMPOUNDSLOGINIT;
//  LDEBUG << "Exclusions size = " << exclusions.size();
  std::map< uint64_t, std::set< uint64_t > >::const_iterator it, it_end;
  it = exclusions.begin(); it_end = exclusions.end();
  std::set< uint64_t > soleResult;
  for (; it != it_end; it++)
  {
    soleResult.insert((*it).first);
  }
  std::vector< std::set< uint64_t > > searchspace;
//   LDEBUG << "Building search space";
  searchspace = buildSearchSpace(exclusions, order);
  //  searchspace.push_back(soleResult);

//   LDEBUG << "Removing vertices from search space";
  std::set< std::set< uint64_t >, SizeSorter > cleanedSearchSpace;
  cleanedSearchSpace = removeVertexEntriesFromSearchSpace(searchspace, sizes);

//   LDEBUG << "Removing included from search space";
  std::list< std::set< uint64_t > > result;
  result = removeIncludedVector(cleanedSearchSpace);

//     displayResult(result);
  return result;
}

std::vector< std::set< uint64_t > > CompoundsCompatibilityBuilder::buildSearchSpace(
    std::map< uint64_t, std::set< uint64_t > >& exclusions,
    const std::vector< uint64_t >& order)
{
/*
  Critical Function : comment logging messages
*/
//  COMPOUNDSLOGINIT;
  std::vector< std::set< uint64_t > > searchspace;
  std::vector< uint64_t >::const_iterator itOrder, itOrder_end;
  itOrder = order.begin(); itOrder_end = order.end();
  for(; itOrder != itOrder_end; itOrder++)
  {
    uint64_t el = *itOrder;
//    if (logger.isDebugEnabled()) {
//      std::ostringstream oss;
//      oss << "Working on element " << el << " / (" << exclusions[el] << ")";
//      LDEBUG << oss.str();
//    }
    const std::set< uint64_t >& localexclusions = exclusions[el];
    std::vector< std::set< uint64_t > > newsearchspace;

    std::vector< std::set< uint64_t > >::const_iterator sSpaceIt, sSpaceIt_end;
    sSpaceIt = searchspace.begin(); sSpaceIt_end = searchspace.end();
    for (; sSpaceIt != sSpaceIt_end; sSpaceIt++)
    {
      const std::set< uint64_t >& subansw = *sSpaceIt;
      bool toinclude = true;
      std::set< uint64_t >::const_iterator exIt, exIt_end;
      exIt = localexclusions.begin(); exIt_end = localexclusions.end();
      for(; exIt != exIt_end; exIt++)
      {
        uint64_t ex = *exIt;
//        if (logger.isDebugEnabled()) {
//          std::ostringstream oss;
//          oss << "testing " << el << " / " << ex << " on (" << subansw << ")";
//          LDEBUG << oss.str();
//        }
        if (subansw.find(ex) != subansw.end())
        {
//           LDEBUG << "Subanswer cannot include "<<el;
          toinclude = false;
          break;
        }
      }
      if (toinclude)
      {
//         LDEBUG << "Subanswer can include " << el;
        std::set< uint64_t > newsubansw = subansw;
        newsubansw.insert(el);
        newsearchspace.push_back(newsubansw);
      }
      else
      {
        newsearchspace.push_back(subansw);
      }
    }
    if (!newsearchspace.empty())
    {
      searchspace = newsearchspace;
/*      std::vector< std::set< uint64_t > >::const_iterator nssit, nssit_end;
      nssit = newsearchspace.begin(); nssit_end = newsearchspace.end();
      for(; nssit != nssit_end; nssit++)
      {
        searchspace.push_back(*nssit);
      }*/
    }
/*    if (!exclusions[el].empty())
    {*/
      std::set< uint64_t > elarray;
      elarray.insert(el);
      searchspace.push_back(elarray);
/*    }*/
  }
  return searchspace;
}

std::set< std::set< uint64_t >, CompoundsCompatibilityBuilder::SizeSorter > CompoundsCompatibilityBuilder::removeVertexEntriesFromSearchSpace(
    const std::vector< std::set< uint64_t > >& searchspace,
    std::map< uint64_t, bool >& sizes)
{
/*
  Critical Function : comment logging messages
*/
//  COMPOUNDSLOGINIT;
  std::set< std::set< uint64_t >, SizeSorter > newsearchspace;
  std::vector< std::set< uint64_t > >::const_iterator searchSpaceIt, searchSpaceIt_end;
  searchSpaceIt = searchspace.begin(); searchSpaceIt_end = searchspace.end();
//  std::ostringstream oss;
  for (; searchSpaceIt != searchSpaceIt_end; searchSpaceIt++)
  {
    const std::set< uint64_t >& searchspaceelem = *searchSpaceIt;
//    if (logger.isDebugEnabled()) {
//      oss << "Cleaning " << searchspaceelem;
//      LDEBUG << oss.str();
//    }
    std::set< uint64_t > newelem;
    std::set< uint64_t >::const_iterator elemIt, elemIt_end;
    elemIt = searchspaceelem.begin(); elemIt_end = searchspaceelem.end();
    for (; elemIt != elemIt_end; elemIt++)
    {
      if (sizes[*elemIt])
      {
        newelem.insert(*elemIt);
      }
    }

    if (!newelem.empty())
    {
//      if (logger.isDebugEnabled()) {
//        oss.clear();
//        oss << "Adding " << newelem << " to newsearchspace";
//        LDEBUG << oss.str();
//      }
      newsearchspace.insert(newelem);
    }
//    else
//    {
//            LDEBUG << "New elem empty";
//    }
  }
  return newsearchspace;
}


std::list< std::set< uint64_t > >  CompoundsCompatibilityBuilder::removeIncludedVector(
    const std::set< std::set< uint64_t >, CompoundsCompatibilityBuilder::SizeSorter >&  searchSpace)
{
  std::list< std::set< uint64_t > > result;
  std::set< std::set< uint64_t >, SizeSorter >::const_iterator searchSpaceIt, searchSpaceIt_end;
  searchSpaceIt = searchSpace.begin(); searchSpaceIt_end = searchSpace.end();
  for (; searchSpaceIt != searchSpaceIt_end; searchSpaceIt++)
  {
    const std::set< uint64_t >& searchspaceelem = *searchSpaceIt;
    bool found = false;
    std::list< std::set< uint64_t > >::const_iterator resit, resit_end;
    resit = result.begin(); resit_end = result.end();
    for (; resit != resit_end; resit++)
    {
      const std::set< uint64_t >& res = *resit;
      std::set< uint64_t > intersec;
      std::insert_iterator< std::set< uint64_t > > ins(intersec, intersec.end());
      std::set_difference(searchspaceelem.begin(), searchspaceelem.end(),
                        res.begin(), res.end(), ins );

      if (intersec.empty()){ found = true; break; }
    }
    if (!found)
    {
      result.push_back(searchspaceelem);
    }
  }
  return result;
}


void CompoundsCompatibilityBuilder::displayResult(const std::list< std::set< uint64_t > >& searchSpace)
{
  std::cerr << "Displaying " << searchSpace.size() << " results" << std::endl;
  std::list< std::set< uint64_t > >::const_iterator it, it_end;
  it = searchSpace.begin(); it_end = searchSpace.end();

  for (;it != it_end; it++)
  {
    std::cerr << *it << std::endl;
  }
}

std::ostream& operator<<(std::ostream& os, const std::set< uint64_t >& subres)
{
  std::set< uint64_t >::const_iterator it, it_end;
  it= subres.begin(); it_end = subres.end();
  if (it != it_end)
  {
    os << *it++;
  }
  for (; it != it_end; it++)
  {
    os << ", " << *it;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector< uint64_t >& subres)
{
  std::vector< uint64_t >::const_iterator it, it_end;
  it= subres.begin(); it_end = subres.end();
  if (it != it_end)
  {
    os << *it++;
  }
  for (; it != it_end; it++)
  {
    os << ", " << *it;
  }
  return os;
}

} // SyntacticAnalysis
} // LinguisticProcessing
} // Lima

