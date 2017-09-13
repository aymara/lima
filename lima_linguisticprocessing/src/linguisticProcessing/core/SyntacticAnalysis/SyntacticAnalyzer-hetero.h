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
 * @brief      process unit to extract heterosyntagmatic dependency relations
 *
 * @file       SyntacticAnalyzer-hetero.h
 * @author     besancon (besanconr@zoe.cea.fr) 

 *             Copyright (C) 2004 by CEA LIST
 * @date       Tue Oct  5 2004
 * @version    $Id$
 */

#ifndef LIMA_SYNTACTICANALYSIS_SYNTACTICANALYZERDEPS_HETERO_H
#define LIMA_SYNTACTICANALYSIS_SYNTACTICANALYZERDEPS_HETERO_H

#include "SyntacticAnalysisExport.h"
#include "SyntacticData.h"
#include "linguisticProcessing/core/Automaton/recognizer.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima {
namespace LinguisticProcessing {
namespace SyntacticAnalysis {

#define SYNTACTICANALYZERDEPSHETERO_CLASSID "SyntacticAnalyzerDepsHetero"

class LIMA_SYNTACTICANALYSIS_EXPORT SyntacticAnalyzerDepsHetero : public MediaProcessUnit
{
public:
  SyntacticAnalyzerDepsHetero();
  SyntacticAnalyzerDepsHetero(const SyntacticAnalyzerDepsHetero&);
  virtual ~SyntacticAnalyzerDepsHetero();
  SyntacticAnalyzerDepsHetero& operator=(const SyntacticAnalyzerDepsHetero&);

  void init(
    Common::XMLConfigurationFiles::
    GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;
  
  LimaStatusCode process(AnalysisContent& analysis) const override;
  
  unsigned char getLanguage() const;

private:

protected:
  MediaId m_language;
  Automaton::Recognizer* m_recognizer;
  std::map< Common::MediaticData::SyntacticRelationId, Automaton::Recognizer* > m_subSentRecognizers;
  bool m_unfold;
  bool m_linkSubSentences;
  bool m_applySameRuleWhileSuccess;
private:
  LimaStatusCode recopyCoordinationStrings(Lima::AnalysisContent& analysis) const;
};

//**********************************************************************
// inline functions 
inline unsigned char SyntacticAnalyzerDepsHetero::getLanguage() const {
  return m_language;
}

} // end namespace
} // end namespace
} // end namespace

#endif
