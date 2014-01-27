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
/**
  *
  * @file        annotationGraphTestProcessUnit.h
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2004 by CEA
  * @date        Created on Nov, 8 2004
  * @version     $Id$
  *
  */

#ifndef LIMA_ANNOTATIONGRAPHS_TESTPROCESSUNIT_H
#define LIMA_ANNOTATIONGRAPHS_TESTPROCESSUNIT_H

#include "linguisticProcessing/common/annotationGraph/GenericAnnotation.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

#include <string>


namespace Lima
{
namespace LinguisticProcessing
{
namespace AnnotationGraphs
{

#define ANNOTATIONGRAPHTESTPO_CLASSID "AnnotationGraphTestProcessUnit"

/** @brief A simple process unit to test the annotation graphs */
class AnnotationGraphTestProcessUnit : public MediaProcessUnit
{
public:
  AnnotationGraphTestProcessUnit();

  virtual ~AnnotationGraphTestProcessUnit() {}

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const;


    MediaId language() const;

private:


protected:

  MediaId m_language;

};

inline MediaId AnnotationGraphTestProcessUnit::language() const
{
  return m_language;
}

/** @brief This simple structure is used to demonstrate how to use annotation 
  *        graphs */
struct Point
{
  int x;
  int y;
  
  /** @brief The functions that dumps a Point on an output stream */
  void dump(std::ostream& os)
  {
    os << "(";
    os << x;
    os << ",";
    os << y;
    os << ")";
  }
};

/** @brief Definition of a function suitable to be used as a dumper for Point
  * annotations of an annotation graph 
  * @param @b os <I>std::ostream&amp;</I> the stream on which to dump the point 
  * @param @b ga <I>GenericAnnotation&amp;</I> the point annotation to be 
  *        dumped. 
  * @return <I>int</I> If succesful, SUCCESS_ID is returned. An error is 
  *         displayed in the case where the given annotation is not a Point 
  *         annotation and UNKNOWN_ERROR is returned .
  */
class DumpPoint : public Common::AnnotationGraphs::AnnotationData::Dumper
{
  public:
    virtual int dump(std::ostream& os, Common::AnnotationGraphs::GenericAnnotation& ga) const;
};

} // closing namespace AnnotationGraphs
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_ANNOTATIONGRAPHS_TESTPROCESSUNIT_H
