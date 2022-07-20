// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
