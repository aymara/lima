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
