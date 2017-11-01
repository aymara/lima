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
/***************************************************************************
 *   Copyright (C) 2005 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_LINGUISTICPROCESSING_SPECIFICENTITIESSPECIFICENTITYANNOTATION_H
#define LIMA_LINGUISTICPROCESSING_SPECIFICENTITIESSPECIFICENTITYANNOTATION_H

#include "AutomatonExport.h"
#include "linguisticProcessing/common/annotationGraph/GenericAnnotation.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "common/MediaticData/EntityType.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"

#include <vector>

namespace Lima
{

namespace LinguisticProcessing
{

namespace SpecificEntities
{

/**
* @brief A representation of a specific entity to store in the annotation graph
*/
class LIMA_AUTOMATON_EXPORT SpecificEntityAnnotation
{
public:

  SpecificEntityAnnotation(const Automaton::RecognizerMatch& entity,
                           FsaStringsPool& sp);
  SpecificEntityAnnotation( const std::deque<LinguisticGraphVertex>& vertices,
                            Common::MediaticData::EntityType entityType,
                            const LimaString& form,
                            const LimaString& normalizedForm,
                            uint64_t startPos,
                            uint64_t length,
                            FsaStringsPool& sp);
  virtual ~SpecificEntityAnnotation();

  inline LinguisticGraphVertex getHead() const;
  inline void setHead(const LinguisticGraphVertex& head);
  inline Common::MediaticData::EntityType getType() const;
  inline void setType(const Common::MediaticData::EntityType& type);
  inline const Automaton::EntityFeatures& getFeatures() const;
  inline void setFeatures(const Automaton::EntityFeatures& features);
  inline StringsPoolIndex getString() const;
  inline StringsPoolIndex getNormalizedString() const;
  inline StringsPoolIndex getNormalizedForm() const;
  inline uint64_t getPosition() const;
  inline uint64_t getLength() const;

  /** @brief The functions that dumps a SpecificEntityAnnotation on an output stream */
  void dump(std::ostream& os) const;

  std::vector< LinguisticGraphVertex> m_vertices;
private:

  LinguisticGraphVertex m_head;
  Common::MediaticData::EntityType m_type;    /**< the type of the entity */
  Automaton::EntityFeatures m_features;
  StringsPoolIndex m_string;
  StringsPoolIndex m_normalizedString;
  StringsPoolIndex m_normalizedForm;
  uint64_t m_position;
  uint64_t m_length;

  //  Linguistic properties and normalized form are given by the normalized
  //  form of the vertex in the morphological graph
  //
  //  LinguisticCode m_linguisticProperties; /**< associated ling prop */
  //  StringsPoolIndex m_normalizedForm; /**< the normalized form of the
  //                                        recognized entity*/

};


/** @brief Definition of a function suitable to be used as a dumper for specific entities
  * annotations of an annotation graph
  * @param @b os <I>std::ostream&amp;</I> the stream on which to dump the point
  * @param @b ga <I>GenericAnnotation&amp;</I> the specific entity annotation to be
  *        dumped.
  * @return <I>int</I> If succesful, SUCCESS_ID is returned. An error is
  *         displayed in the case where the given annotation is not a specific entity
  *         annotation and UNKNOWN_ERROR is returned .
  */
class LIMA_AUTOMATON_EXPORT DumpSpecificEntityAnnotation : 
    public Common::AnnotationGraphs::AnnotationData::Dumper 
{
public:
  virtual ~DumpSpecificEntityAnnotation() {}
  virtual int dump(std::ostream& os, 
                    Common::AnnotationGraphs::GenericAnnotation& ga) const override;
};



inline LinguisticGraphVertex SpecificEntityAnnotation::getHead() const
{
  return m_head;
}

inline void SpecificEntityAnnotation::setHead(const LinguisticGraphVertex& head)
{
  m_head = head;
}

inline Common::MediaticData::EntityType SpecificEntityAnnotation::getType() const
{
  return m_type;
}

inline void SpecificEntityAnnotation::setType(const Common::MediaticData::EntityType& type)
{
  m_type = type;
}

inline const Automaton::EntityFeatures& SpecificEntityAnnotation::getFeatures() const
{
  return m_features;
}

inline void SpecificEntityAnnotation::setFeatures(const Automaton::EntityFeatures& features)
{
  m_features = features;
}

inline StringsPoolIndex SpecificEntityAnnotation::getString() const
{
  return m_string;
}

inline StringsPoolIndex SpecificEntityAnnotation::getNormalizedString() const
{
  return m_normalizedString;
}

inline StringsPoolIndex SpecificEntityAnnotation::getNormalizedForm() const
{
  return m_normalizedForm;
}

inline uint64_t SpecificEntityAnnotation::getPosition() const
{
  return m_position;
}

inline uint64_t SpecificEntityAnnotation::getLength() const
{
  return m_length;
}

} // SpecificEntities
} // LinguisticProcessing
} // Lima

#endif
