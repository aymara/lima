// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  * @brief        A structure to identify a syntagmatic chain in a graph
  *
  * @file         chainIdStruct.h
  * @author       Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *               Copyright   : (c) 2003 by CEA
  * @date         Created on  : Oct, 29 2003
  * @version      $Id$
  *
  */

#ifndef LIMA_GRAPH_CHAINIDSTRUCT_H
#define LIMA_GRAPH_CHAINIDSTRUCT_H

#include "linguisticProcessing/common/linguisticData/languageData.h"

namespace Lima {
namespace LinguisticProcessing {
namespace LinguisticAnalysisStructure {

typedef enum {UNKNOWN,BEGIN,END,UNIGRAM,PART} ChainElemType ;

/** Property to identify the chains in the graph. Works like the PathIdProperty
  * but each edge can belong to several chains in various paths, so the property
  * is a set of objects composed of a path number, a chain number in the path
  * and a chain type */
class ChainIdStruct
{
public:
    
    static const uint64_t noid=UINT_MAX;

    ChainIdStruct(
      Common::MediaticData::ChainsType chainType = Common::MediaticData::NO_CHAIN_TYPE, //uint64_t pathId = 0,
      uint64_t chainId = noid,
      ChainElemType type = PART) :
        m_chainType(chainType), m_chainId(chainId), m_elemType(type) {}
    virtual ~ChainIdStruct() {}

    ChainIdStruct(const ChainIdStruct& cis) :
        m_chainType(cis.m_chainType),
//        m_pathId(cis.m_pathId),
        m_chainId(cis.m_chainId),
        m_elemType(cis.m_elemType)
    {
    }

    ChainIdStruct& operator=(const ChainIdStruct& cis)
    {
        m_chainType = cis.m_chainType;
//        m_pathId = cis.m_pathId;
        m_chainId = cis.m_chainId;
        m_elemType = cis.m_elemType;
        return (*this);
    }

    inline bool operator==(const ChainIdStruct& cis) const;

    bool operator<(const ChainIdStruct& cis) const
    {
//      if (m_chainType==cis.m_chainType) return (m_chainType < cis.m_chainType);
      return (m_chainId < cis.m_chainId);
    }

    inline Common::MediaticData::ChainsType chainType() const { return m_chainType; }
    inline void chainType(Common::MediaticData::ChainsType t) { m_chainType = t; }
    inline uint64_t chainId() const { return m_chainId; }
    inline void chainId(uint64_t chainId) { m_chainId = chainId; }
    inline ChainElemType elemType() const { return m_elemType; }
    inline void elemType(ChainElemType type) { m_elemType = type; }


private:
    /** Chain type */
    Common::MediaticData::ChainsType m_chainType;
    uint64_t m_chainId;
    ChainElemType m_elemType;
};


inline std::ostream& operator<<(std::ostream& os, const ChainIdStruct& ids)
{
    os << "[";
    if (ids.chainType() == Common::MediaticData::NO_CHAIN_TYPE)
        os << "0";
    else if (ids.chainType() == Common::MediaticData::NOMINAL)
        os << "N";
    else
        os << "V";
    os << ";"; 
    os << ids.chainId() << ";";
    switch (ids.elemType())
    {
      case BEGIN: os << "b"; break;
      case END: os << "e"; break;
      case UNIGRAM: os << "u"; break;
      case PART: os << "p"; break;
      default: os << "u";
    }
    os << "]";
    return os;
}

inline QDebug& operator<<(QDebug& os, const ChainIdStruct& ids)
{
  os << "[";
  if (ids.chainType() == Common::MediaticData::NO_CHAIN_TYPE)
    os << "0";
  else if (ids.chainType() == Common::MediaticData::NOMINAL)
    os << "N";
  else
    os << "V";
  os << ";";
  os << ids.chainId() << ";";
  switch (ids.elemType())
  {
    case BEGIN: os << "b"; break;
    case END: os << "e"; break;
    case UNIGRAM: os << "u"; break;
    case PART: os << "p"; break;
    default: os << "u";
  }
  os << "]";
  return os;
}

inline bool ChainIdStruct::operator==(const ChainIdStruct& cis) const
{
    return ( (m_chainType == cis.m_chainType) && (m_chainId == cis.m_chainId) );
}

} // closing namespace LinguisticAnalysisStructure
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_GRAPH_CHAINIDSTRUCT_H
