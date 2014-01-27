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

#ifndef LIMA_LINGUISTICPROCESSING_GEOENTITIESGEOENTITYANNOTATION_H
#define LIMA_LINGUISTICPROCESSING_GEOENTITIESGEOENTITYANNOTATION_H

#include "linguisticProcessing/common/annotationGraph/GenericAnnotation.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"

#include <set>
#include <vector>

namespace Lima
{

namespace LinguisticProcessing
{

namespace GeoEntities
{
typedef enum {
  FIRST,       
  MIDDLE,
  LAST, 
  UNIQ
} PositionType;


/**
* @brief A representation of a geo entity to store in the annotation graph
*/
class GeoEntityAnnotation :  public std::vector< LinguisticGraphVertex>
{
public:

  GeoEntityAnnotation(PositionType,std::set<std::string>);
  virtual ~GeoEntityAnnotation();

  inline PositionType getPosition() const;
  inline void setPosition(const PositionType& p);
  inline std::set<std::string> getGeoClasses() const;
  inline void setGeoClasses(const std::set<std::string>&);

  /** @brief The functions that dumps a GeoEntityAnnotation on an output stream */
  void dump(std::ostream& os);

private:

  PositionType m_pos;
  std::set<std::string> m_classes;


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
class DumpGeoEntityAnnotation : public Common::AnnotationGraphs::AnnotationData::Dumper {
  public:
    virtual ~DumpGeoEntityAnnotation() {}
    virtual int dump(std::ostream& os, Common::AnnotationGraphs::GenericAnnotation& ga) const;
};



inline PositionType GeoEntityAnnotation::getPosition() const
{
  return m_pos;
}

inline void GeoEntityAnnotation::setPosition(const PositionType& pos)
{
  m_pos = pos;
}


inline std::set<std::string> GeoEntityAnnotation::getGeoClasses() const
{
  return m_classes;
}

inline void GeoEntityAnnotation::setGeoClasses(const std::set<std::string>& cl)
{
  m_classes=cl;
}

} // GeoEntities
} // LinguisticProcessing
} // Lima

#endif
