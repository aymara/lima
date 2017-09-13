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
#ifndef LIMA_LINGUISTICPROCESSING_MORPHOLOGICANALYSISIDIOMATICEXPRESSIONANNOTATION_H
#define LIMA_LINGUISTICPROCESSING_MORPHOLOGICANALYSISIDIOMATICEXPRESSIONANNOTATION_H

#include "MorphologicAnalysisExport.h"
#include "linguisticProcessing/common/annotationGraph/GenericAnnotation.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"

#include <vector>

namespace Lima
{

namespace LinguisticProcessing
{

namespace MorphologicAnalysis
{

/**
@author Gael de Chalendar
*/
class LIMA_MORPHOLOGICANALYSIS_EXPORT IdiomaticExpressionAnnotation
{
public:
  IdiomaticExpressionAnnotation(const Automaton::RecognizerMatch& entity);

    ~IdiomaticExpressionAnnotation();

    /** @brief The functions that dumps an IdiomaticExpressionAnnotation 
     *  on an output stream 
     */
    void dump(std::ostream& os);

    std::vector< LinguisticGraphVertex > m_vertices;
};

/** @brief Definition of a function suitable to be used as a dumper for idiomatic expressions
  * annotations of an annotation graph 
  * @param @b os <I>std::ostream&amp;</I> the stream on which to dump the point 
  * @param @b ga <I>GenericAnnotation&amp;</I> the specific entity annotation to be 
  *        dumped. 
  * @return <I>int</I> If succesful, SUCCESS_ID is returned. An error is 
  *         displayed in the case where the given annotation is not a specific entity
  *         annotation and UNKNOWN_ERROR is returned .
  */
class DumpIdiomaticExpressionAnnotation : 
    public Common::AnnotationGraphs::AnnotationData::Dumper
{
  public:
    virtual int dump(std::ostream& os, 
                     Common::AnnotationGraphs::GenericAnnotation& ga) const override;
};




} // MorphologicAnalysis

} // LinguisticProcessing

} // Lima

#endif // LIMA_LINGUISTICPROCESSING_MORPHOLOGICANALYSISIDIOMATICEXPRESSIONANNOTATION_H
