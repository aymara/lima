// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

class SpecificEntityAnnotationPrivate;
/**
* @brief A representation of a specific entity to store in the annotation graph
*/
class LIMA_AUTOMATON_EXPORT SpecificEntityAnnotation
{
  friend class SpecificEntityAnnotationPrivate;
public:

  SpecificEntityAnnotation(const Automaton::RecognizerMatch& entity,
                           FsaStringsPool& sp);
  SpecificEntityAnnotation( const std::deque<LinguisticGraphVertex>& vertices,
                            Common::MediaticData::EntityType entityType,
                            const LimaString& form,
                            const LimaString& normalizedForm,
                            int nbError,
                            uint64_t startPos,
                            uint64_t length,
                            FsaStringsPool& sp);
  virtual ~SpecificEntityAnnotation();
  SpecificEntityAnnotation(const SpecificEntityAnnotation& annotation);
  SpecificEntityAnnotation& operator=(const SpecificEntityAnnotation& annotation);

  LinguisticGraphVertex getHead() const;
  void setHead(const LinguisticGraphVertex& head);
  Common::MediaticData::EntityType getType() const;
  void setType(const Common::MediaticData::EntityType& type);
  const Automaton::EntityFeatures& getFeatures() const;
  void setFeatures(const Automaton::EntityFeatures& features);
  StringsPoolIndex getString() const;
  StringsPoolIndex getNormalizedString() const;
  StringsPoolIndex getNormalizedForm() const;
  uint64_t getPosition() const;
  uint64_t getLength() const;
  const std::vector< LinguisticGraphVertex>& vertices() const;

  /** @brief The functions that dumps a SpecificEntityAnnotation on an output stream */
  void dump(std::ostream& os) const;


private:
  SpecificEntityAnnotationPrivate* m_d;
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



} // SpecificEntities
} // LinguisticProcessing
} // Lima

#endif
