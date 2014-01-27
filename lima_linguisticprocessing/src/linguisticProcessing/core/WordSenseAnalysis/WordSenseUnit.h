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
#ifndef LIMA_WORDSENSEDISAMBIGUATION_WORDSENSEUNIT_H
#define LIMA_WORDSENSEDISAMBIGUATION_WORDSENSEUNIT_H

#include "WordSenseAnalysisExport.h"
#include <set>
#include <string>
#include "CommonTypedefs.h"



namespace Lima
{
namespace LinguisticProcessing
{
namespace WordSenseDisambiguation
{

class LIMA_WORDSENSEANALYSIS_EXPORT WordSenseUnit
{
  public :
      WordSenseUnit();
      ~WordSenseUnit();
            
      WordSenseUnit(uint senseId,
        uint64_t parentLemmaId, 
        Mode mode, 
        std::string& senseTag, 
        std::set<uint64_t>& sensesMembersIds, 
        int freq=0);
      
      WordSenseUnit(const WordSenseUnit& wsu) : 
      m_parentLemmaId(wsu.m_parentLemmaId),
      m_mode(wsu.m_mode),
      m_senseId(wsu.m_senseId),
      m_senseTag(wsu.m_senseTag),
      m_senseMembersIds(wsu.m_senseMembersIds),
      m_freq(wsu.m_freq)
      {}
      
      
      
      friend LIMA_WORDSENSEANALYSIS_EXPORT std::ostream& operator << (std::ostream& os, const WordSenseUnit& wsu);
     
      uint64_t parentLemmaId();
      uint64_t senseId();
      std::string senseTag();
      std::set<uint64_t>& senseMembersIds();
      Mode mode();
      
      uint64_t parentLemmaId() const ;
      uint64_t senseId() const ;
      const std::string& senseTag() const ;
      const std::set<uint64_t>& senseMembersIds() const ;
      Mode mode() const ;
      
      bool operator<(const WordSenseUnit& wsu) const
      {
  if (m_mode==wsu.mode()) return (m_senseId<wsu.senseId());
  return m_mode<wsu.mode();
      };
      
//         bool operator==(const WordSenseUnit& wsu) const
//       {
//   return (m_mode==wsu.mode() && m_senseId==wsu.senseId());  
//       };
      
      
      
  protected :    
    uint64_t m_parentLemmaId;
    Mode m_mode;
    uint64_t m_senseId;
    std::string m_senseTag;
    std::set<uint64_t> m_senseMembersIds;
    int m_freq; 
};

inline uint64_t WordSenseUnit::parentLemmaId()
{
  return m_parentLemmaId;
}
inline uint64_t WordSenseUnit::senseId()
{
  return m_senseId;
}
inline std::string WordSenseUnit::senseTag()
{
  return m_senseTag;
}
inline std::set<uint64_t>& WordSenseUnit::senseMembersIds()
{
  return m_senseMembersIds;
}
inline Mode WordSenseUnit::mode()
{
  return m_mode;
}
inline uint64_t WordSenseUnit::parentLemmaId() const 
{
  return m_parentLemmaId;
}
inline uint64_t WordSenseUnit::senseId() const
{
  return m_senseId;
}
inline const std::string& WordSenseUnit::senseTag() const 
{
  return m_senseTag;
}
inline const std::set<uint64_t>& WordSenseUnit::senseMembersIds() const 
{
  return m_senseMembersIds;
}
inline Mode WordSenseUnit::mode() const 
{
  return m_mode;
}



}  // namespace WordSenseDisambiguation
}  // namespace LinguisticProcessing
}  // namespace Lima

#endif // LIMA_WORDSENSEDISAMBIGUATION_WORDSENSEUNIT_H
