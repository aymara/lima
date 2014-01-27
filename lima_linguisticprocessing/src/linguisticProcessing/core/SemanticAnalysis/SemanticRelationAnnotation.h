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
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#ifndef PATTERNPROCESSING_SEMANTICANALYSIS_H
#define PATTERNPROCESSING_SEMANTICANALYSIS_H

#include "SemanticAnalysisExport.h"
#include "linguisticProcessing/common/annotationGraph/GenericAnnotation.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace SemanticAnalysis
{

class LIMA_SEMANTICANALYSIS_EXPORT SemanticRelationAnnotation
{
public:
  SemanticRelationAnnotation(const std::string& type) : m_type(type) {}

  virtual ~SemanticRelationAnnotation() {}

  inline std::string& type();
  inline const std::string& type() const;
  inline void type(const std::string& newType);
  inline void dump(std::ostream& os);

private:

protected:
  std::string m_type;
};

inline std::string& SemanticRelationAnnotation::type()
{
  return m_type;
}
inline const std::string& SemanticRelationAnnotation::type() const
{
  return m_type;
}
inline void SemanticRelationAnnotation::type(const std::string& newType)
{
  m_type = newType;
}
inline void SemanticRelationAnnotation::dump(std::ostream& os)
{
  os << m_type;
}


class LIMA_SEMANTICANALYSIS_EXPORT DumpSemanticRelation : public Common::AnnotationGraphs::AnnotationData::Dumper
{
  public:
    virtual int dump(std::ostream& os, Common::AnnotationGraphs::GenericAnnotation& ga) const;
};


} // closing namespace SemanticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // PATTERNPROCESSING_SEMANTICANALYSIS_H
