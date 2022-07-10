// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
      WordSenseUnit() = default;
      ~WordSenseUnit() = default;

      WordSenseUnit(uint senseId,
        uint64_t parentLemmaId,
        Mode mode,
        std::string& senseTag,
        std::set<uint64_t>& sensesMembersIds,
        int freq=0);

      WordSenseUnit(const WordSenseUnit& wsu) = default;
      WordSenseUnit& operator=(const WordSenseUnit& wsu) = default;


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
