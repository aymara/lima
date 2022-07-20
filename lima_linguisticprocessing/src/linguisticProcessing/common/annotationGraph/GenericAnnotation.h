// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
