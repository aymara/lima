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
/** @brief       A Dummy
  *              primarily nodes of a text anlaysis
  *
  * @file        annotationGraph.h
  * @author      Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *              Copyright (c) 2004 by CEA
  * @date        Created on Nov, 8 2004
  * @version     $Id$
  *
  */

#ifndef LIMA_ANNOTATIONGRAPHS_GENERICANNOTATION_H
#define LIMA_ANNOTATIONGRAPHS_GENERICANNOTATION_H

#include <iostream>
#include <boost/any.hpp>

namespace Lima {
namespace Common {
namespace AnnotationGraphs {

/** @brief This class allows to convert any object into an annotation by inheritance. */
class GenericAnnotation
{
public:
  GenericAnnotation() {}
  GenericAnnotation(const GenericAnnotation& ga) : m_annot(ga.m_annot) {}
  template<typename ValueType> GenericAnnotation(const ValueType & v) : m_annot(v) {}
  GenericAnnotation& operator=(const GenericAnnotation& ga) 
  { 
    m_annot = ga.m_annot;
    return  *this; 
  }
  template<typename ValueType> GenericAnnotation& operator=(const ValueType &v) 
  { 
    m_annot = v;
    return *this; 
  }
  virtual ~GenericAnnotation() {}

  template<typename ValueType> ValueType * pointerValue()
  {
    return boost::any_cast<ValueType>(&m_annot);
  }

  template<typename ValueType> const ValueType * pointerValue() const
  {
    return boost::any_cast<ValueType>(&m_annot);
  }

  template<typename ValueType> ValueType  value() const
  {
    return boost::any_cast<ValueType>(m_annot);
  }

private:
  boost::any m_annot;
};

template<typename ValueType> std::ostream& operator<<(std::ostream& os, const GenericAnnotation& ga)
{
  os << ga.value<ValueType>();
  return os;
}


} // closing namespace AnnotationGraphs
} // closing namespace Common
} // closing namespace Lima

#endif // LIMA_ANNOTATIONGRAPHS_GENERICANNOTATION_H
