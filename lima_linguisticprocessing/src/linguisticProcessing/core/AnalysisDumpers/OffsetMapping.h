// Copyright (C) 2016 by CEA - LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef OFFSETMAPPING_H
#define OFFSETMAPPING_H

#include "linguisticProcessing/core/AnalysisDumpers/AnalysisDumpersExport.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include <iostream>
#include <map>

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

class LIMA_ANALYSISDUMPERS_EXPORT OffsetMapping : public AnalysisData {
public:
  OffsetMapping();
  ~OffsetMapping();

  /** @brief add new mapping between old and new offsets. 
   * Warning: this mapping will only be accessed by the new offset (to get the old), unless 
   * a call to createInverseMapping() is explicitely performed
   */
  void addMapping(unsigned int offset, unsigned int newOffset);
  void createInverseMapping();

  void clear();
  
  bool isEmpty() { return m_mappingNewToOld.size()==0; }
  
  OffsetMapping operator+(unsigned int n) const;
  void add(const OffsetMapping& otherMapping);

  unsigned int getOriginalOffset(unsigned int newOffset) const;
  
  void getNewOffsets(unsigned int prevOffset, std::vector<unsigned int>& newOffsets) const;
  
  friend LIMA_ANALYSISDUMPERS_EXPORT std::ostream& operator<<(std::ostream& os, const OffsetMapping& offsetMapping);
  std::string toString() const;
  
private:
  // expanded text duplicates some parts of the original text:
  // hence, new to old mapping is n->1 whereas old to new mapping is 1->n
  std::map<unsigned int, unsigned int> m_mappingNewToOld;
  std::multimap<unsigned int, unsigned int> m_mappingOldToNew;
};

} // end namespace
} // end namespace
} // end namespace

#endif
