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
 * @file       SemanticAnnotation.h
 * @author      (romaric.besancon@cea.fr)
 * @date       Thu Sep 13 2007
 * copyright   Copyright (C) 2007 by CEA LIST
 * Project     s2lp
 * 
 * @brief      simple semantic annotation to be inserted in annotation graph, associated to only one vertex
 * 
 * 
 ***********************************************************************/

#ifndef SEMANTICANNOTATION_H
#define SEMANTICANNOTATION_H

#include "linguisticProcessing/common/annotationGraph/GenericAnnotation.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include <iostream>

namespace Lima {
namespace LinguisticProcessing {
namespace SemanticAnalysis {

class SemanticAnnotation
{
 public:
  SemanticAnnotation(const std::string& type):m_type(type) {} 
  ~SemanticAnnotation() {}

  const std::string& getType() const { return m_type; }
  void setType(const std::string& type) { m_type=type; }
  void dump(std::ostream& os) { os << m_type; }
  
  friend std::ostream& operator << (std::ostream&, const SemanticAnnotation&);

 private:
  std::string m_type;
};

class DumpSemanticAnnotation : public Common::AnnotationGraphs::AnnotationData::Dumper
{
  public:
    virtual int dump(std::ostream& os, Common::AnnotationGraphs::GenericAnnotation& ga) const override;
};

} // end namespace
} // end namespace
} // end namespace

#endif
