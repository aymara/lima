/*
    Copyright 2002-2020 CEA LIST

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
#ifndef LIMA_LINGUISTICPROCESSING_MORPHOLOGICANALYISIDIOMATICALTERNATIVESCONSTRAINTS_H
#define LIMA_LINGUISTICPROCESSING_MORPHOLOGICANALYISIDIOMATICALTERNATIVESCONSTRAINTS_H

#include "MorphologicAnalysisExport.h"
#include "linguisticProcessing/core/ApplyRecognizer/applyRecognizerActions.h"

namespace Lima {

namespace LinguisticProcessing {

namespace MorphologicAnalysis {

// ids for constraints in this file
#define CreateIdiomaticAlternativeId "CreateIdiomaticAlternative"

class LIMA_MORPHOLOGICANALYSIS_EXPORT CreateIdiomaticAlternative : public ApplyRecognizer::CreateAlternative
{
public:
  CreateIdiomaticAlternative(MediaId language,
                       const LimaString& complement=LimaString());
  virtual ~CreateIdiomaticAlternative() {}
  virtual bool operator()(Automaton::RecognizerMatch& match,
                          AnalysisContent& analysis) const override;

private:

  /** @brief removes edges that are no more necessary in case of absolute IE
   * Principle: except the edges member of the match, all edges pointing to
   * or from one of the match vertices must be removed. By extension, all edges
   * that become pending should also be removed.
   * Algorithm:
   * go through all match nodes.
   *   for each (in/out) edge not member of the match, store it as to be
   *   removed. put the (source/target) not as to be checked
   * remove the stored edges
   * for node stored as to be checked, call "clearUnreachableVertices
   **/
  void removeEdges(LinguisticGraph& graph,
                   const Automaton::RecognizerMatch& match,
                   AnalysisContent& analysis) const;

};


} // MorphologicAnalysis

} // LinguisticProcessing

} // Lima

#endif // LIMA_LINGUISTICPROCESSING_MORPHOLOGICANALYISIDIOMATICALTERNATIVESCONSTRAINTS_H
