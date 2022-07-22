// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       entityProperties.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Fri Jan 17 2003
 * copyright   Copyright (C) 2003 by CEA LIST
 * Project     Automaton
 * 
 * @brief      properties of a recognized specific entity (common to all types
 * of results)
 * 
 * 
 ***********************************************************************/

#ifndef ENTITYPROPERTIES_H
#define ENTITYPROPERTIES_H

#include "AutomatonExport.h"
#include <vector>
#include "EntityFeatures.h"
#include "common/Data/LimaString.h"
#include "common/MediaticData/EntityType.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT EntityProperties
{
 public:
  EntityProperties(); 
  EntityProperties(const EntityProperties&);
  ~EntityProperties();
  EntityProperties& operator = (const EntityProperties& p);

  //comparison operator
  bool operator== (const EntityProperties& p);

  LinguisticGraphVertex getHead() const;
  const Common::MediaticData::EntityType& getType() const;
  LinguisticCode getLinguisticProperties() const;
  
  const EntityFeatures& features() const { return m_features; }
  EntityFeatures& features() { return m_features; }

  bool isContextual() const;

  void setHead(const LinguisticGraphVertex& v);
  void setType(const Common::MediaticData::EntityType& t);
  void setLinguisticProperties(const LinguisticCode& lingProp);
  void setContextual(const bool contextual);

  LimaString str() const;

 protected:
  /** indicates which token (in the original text) 
      is the head of the recognized entity */
  LinguisticGraphVertex m_head; 
  Common::MediaticData::EntityType m_type;    /**< the type of the recognized entity */
  LinguisticCode m_linguisticProperties; /**< associated ling prop */
  EntityFeatures m_features; /**< extracted features */
  bool m_contextual;            /**< if true, the expression is contextual
                                   (ambiguous and have to use the context 
                                   to disambiguate)) */

  // copy the content of another entityProperties instance
  void copy(const EntityProperties& p);
  
};
/***********************************************************************/
// inline access functions
/***********************************************************************/
inline LinguisticGraphVertex EntityProperties::getHead() const {
  return m_head;
}
inline const Common::MediaticData::EntityType& EntityProperties::getType() const {
  return m_type;
}
inline LinguisticCode EntityProperties::getLinguisticProperties() const {
  return m_linguisticProperties;
}
inline bool EntityProperties::isContextual() const {
  return m_contextual;
}
/***********************************************************************/
// modifiers
/***********************************************************************/
inline void EntityProperties::
setHead(const LinguisticGraphVertex& v) {
  m_head=v;
} 
inline void EntityProperties::setType(const Common::MediaticData::EntityType& t) {
  m_type=t;
} 
inline void EntityProperties::setLinguisticProperties(const LinguisticCode& lingProp) {
  m_linguisticProperties=lingProp;
}
inline void EntityProperties::setContextual(const bool contextual) {
  m_contextual=contextual;
}

} // end namespace
} // end namespace
} // end namespace

#endif
