/************************************************************************
 *
 * @file       OffsetMapping.cpp
 * @author     Romaric Besançon (romaric.besancon@cea.fr)
 * @date       Thu Nov 17 2016
 * copyright   Copyright (C) 2016 by CEA - LIST
 * 
 ***********************************************************************/

#include "OffsetMapping.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"

using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

//***********************************************************************
OffsetMapping::OffsetMapping():
m_mappingNewToOld(),
m_mappingOldToNew()
{
}
OffsetMapping::~OffsetMapping()
{
}

void OffsetMapping::clear()
{
  m_mappingNewToOld.clear();
  m_mappingOldToNew.clear();

}

void OffsetMapping::addMapping(unsigned int offset, unsigned int newOffset)
{
  m_mappingNewToOld[newOffset]=offset;
}

OffsetMapping OffsetMapping::operator+(unsigned int n) const
{
  // add n to all key values (new offsets)
  OffsetMapping newMap;
  for (const auto& m: m_mappingNewToOld) {
    //cout << "OffsetMapping::operator+" << n << " " << m.first << "," << m.second+n << endl;
    // m.first is the new offset, m.second the old
    newMap.addMapping(m.second,m.first+n);
  }
  return newMap;
}

void OffsetMapping::add(const OffsetMapping& otherMapping)
{
  for (const auto& m: otherMapping.m_mappingNewToOld) {
    //cout << "OffsetMapping::add " << m.first << "-" << m.second << endl;
    m_mappingNewToOld[m.first]=m.second;
  }
}


void OffsetMapping::createInverseMapping() 
{
  //LOGINIT("LP::TextPreprocessing");
  for (const auto& m: m_mappingNewToOld) {
    m_mappingOldToNew.insert(make_pair(m.second,m.first));
  }
  //LDEBUG << "OffsetMapping::createInverseMapping: map size=" << m_mappingOldToNew.size();
}

unsigned int OffsetMapping::getOriginalOffset(unsigned int newOffset) const
{
  // mapping is incomplete: not all offset are stored, only the ones where a change occurs
  // find closest change
  auto it=m_mappingNewToOld.lower_bound(newOffset);
  // lower_bound is the first iterator that is greater or equal to the newOffset: 
  // need to take previous one (if it exists) to have highest smaller value
  
  unsigned int prevOffset(0);
  unsigned int prevOffsetMapping(0);
  if (it!=m_mappingNewToOld.begin()) {
    if (it==m_mappingNewToOld.end()) {
      prevOffset=m_mappingNewToOld.rbegin()->first;
      prevOffsetMapping=m_mappingNewToOld.rbegin()->second;
    }
    else {
      if ((*it).first != newOffset) { 
        // take previous one only if strictly superior: if equal, keep this iterator
        it--;
      }
      prevOffset=(*it).first;
      prevOffsetMapping=(*it).second;
    }
  }
  // otherwise, before first offset mapping: keep zeros
  
  // return old value translated by the difference between the target offset and the closest smaller one that was found in the map
  DUMPERLOGINIT;
  LDEBUG << "OffsetMapping::getOriginalOffset" << newOffset << "->" << prevOffsetMapping << "+" << newOffset << "-" << prevOffset;
  return (prevOffsetMapping + newOffset-prevOffset);
}

void OffsetMapping::getNewOffsets(unsigned int prevOffset, std::vector<unsigned int>& newOffsets) const
{
  newOffsets.clear();
  //LOGINIT("LP::TextPreprocessing");
  if (m_mappingOldToNew.size()==0) {
    //LERROR << "OffsetMapping::getNewOffsets: missing offset mapping";
    // no mapping: keep offset
    newOffsets.push_back(prevOffset);
    return;
  }
  auto it=m_mappingOldToNew.upper_bound(prevOffset);
  // uppper_bound is the first iterator that is greater to the newOffset: 
  // need to take previous ones (if they exists) to have highest smaller value
  // oldToNew is a multimap : several entries can have same key value
  if (it!=m_mappingNewToOld.begin()) {
    it--;
    unsigned int newOffset=(*it).first;
    while (it!=m_mappingOldToNew.begin() && (*it).first==newOffset) {
      // to keep the same order, push in front 
      newOffsets.insert(newOffsets.begin(),(*it).second+prevOffset-newOffset);
      it--;
    }
  }
  else {
    // otherwise, before first offset mapping: keep same value
    newOffsets.push_back(prevOffset);
  }
}


//***********************************************************************
string OffsetMapping::toString() const
{
  ostringstream oss;
  //os << "mapping (new->old)=" << endl;
  for (const auto& m: m_mappingNewToOld) {
    oss << m.first << "->" << m.second << endl;
  }
  return oss.str();
}
std::ostream& operator<<(std::ostream& os, const OffsetMapping& offsetMapping)
{
  os << offsetMapping.toString();
  return os;
}


} // end namespace
} // end namespace
} // end namespace
