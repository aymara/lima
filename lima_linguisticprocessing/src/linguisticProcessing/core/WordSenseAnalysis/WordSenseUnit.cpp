// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "WordSenseUnit.h"
#include "CommonTypedefs.h"

#include <iostream>

namespace Lima
{
namespace LinguisticProcessing
{
namespace WordSenseDisambiguation
{
  WordSenseUnit::WordSenseUnit(uint senseId,
                               uint64_t parentLemmaId,
                               Mode mode,
                               std::string& senseTag,
                               std::set<uint64_t>& senseMembersIds,
                               int freq):
     m_parentLemmaId(parentLemmaId),
     m_mode(mode),
     m_senseId(senseId),
     m_senseTag(senseTag),
     m_senseMembersIds(senseMembersIds),
     m_freq(freq)
     {}

  std::ostream& operator << (std::ostream& os, const WordSenseUnit& wsu)
  {
    os <<  wsu.parentLemmaId() << "#"<<wsu.senseId() << "(" << wsu.mode() << "):" << wsu.senseTag() ;
    return os;
  }




}  // namespace WordSenseDisambiguation
}  // namespace LinguisticProcessing
}  // namespace Lima
