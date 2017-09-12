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
  * @file        SyntacticAnalyzer-disamb.h
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2003 by CEA
  * @date        Created on Aug, 31 2004
  * @version     $Id$
  *
  */

#ifndef LIMA_SYNTACTICANALYSIS_SYNTACTICANALYZERDISAMB_H
#define LIMA_SYNTACTICANALYSIS_SYNTACTICANALYZERDISAMB_H

#include "SyntacticAnalysisExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{

#define SYNTACTICANALYZERDISAMB_CLASSID "SyntacticAnalyzerDisamb"

/** This class does the syntactic analysis. It works on a pointer to a graph
  * Equivalent of PathIterator
  */
class LIMA_SYNTACTICANALYSIS_EXPORT SyntacticAnalyzerDisamb : public MediaProcessUnit
{
public:
  SyntacticAnalyzerDisamb();

  //  SyntacticAnalyzerDisamb(SyntacticAnalyzerDisamb&);

  virtual ~SyntacticAnalyzerDisamb() {}

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

  MediaId language() const;

  inline void disambiguated(bool disamb) { m_disamb = disamb; }
  inline bool disambiguated() { return m_disamb; }

protected:
  MediaId m_language;
  uint64_t m_depGraphMaxBranchingFactor;

  bool m_disamb;
  
  LinguisticGraphVertex nextChainsDisambBreakFrom(
        const LinguisticGraphVertex& v,
        const LinguisticAnalysisStructure::AnalysisGraph& graph,
        const LinguisticGraphVertex& nextSentenceBreak) const;

};

inline MediaId SyntacticAnalyzerDisamb::language() const
{
  return m_language;
}

} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif
