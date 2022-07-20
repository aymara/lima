// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       subPartIndex.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Wed Jan 19 2005
 * @version    $Id: subPartIndex.h 2223 2005-03-15 16:56:41Z romaric $
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * Project     AutomatonCompiler
 * 
 * @brief      this class contains the structure of the indexing of a subpart of rule (for constraints)
 * 
 * 
 ***********************************************************************/

#ifndef SUBPARTINDEX_H
#define SUBPARTINDEX_H

#include "common/Data/LimaString.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

typedef enum {
  SUB_NONE,
  SUB_FIRST,
  SUB_CURRENT,
  SUB_NEXT,
  SUB_LAST
} SubIndexType;

class SubAutomaton; //needed for named indexes

class SubPartIndex
{
 public:
  SubPartIndex(); 
  SubPartIndex(const LimaString& str,
               const std::vector<SubAutomaton>& subAutomatons); 
  SubPartIndex(const SubPartIndex&);
  ~SubPartIndex();
  SubPartIndex& operator = (const SubPartIndex&);

  void init(const LimaString& str,
            const std::vector<SubAutomaton>& subAutomatons); 
  bool empty() const; 
  bool hasSubPart() const;
  
  const std::pair<SubIndexType,uint64_t>& getPartIndex() const;
  const SubPartIndex* getSubPartIndex() const;

  bool isBefore(const SubPartIndex& i);

  friend std::ostream& operator << (std::ostream&, const SubPartIndex&);
  friend QDebug& operator << (QDebug&, const SubPartIndex&);
  
 private:
  std::pair<SubIndexType,uint64_t> m_partIndex;
  SubPartIndex* m_subPartIndex;
};

//*****************************************************************
// inline functions
inline bool SubPartIndex::hasSubPart() const {
  return m_subPartIndex!=0;
}

inline bool SubPartIndex::empty() const { 
  return (m_partIndex.first==SUB_NONE && m_partIndex.second==0); 
}
inline const std::pair<SubIndexType,uint64_t>& 
SubPartIndex::getPartIndex() const { 
  return m_partIndex; 
}
inline const SubPartIndex* 
SubPartIndex::getSubPartIndex() const { 
  return m_subPartIndex; 
}


} // end namespace
} // end namespace
} // end namespace

#endif
