// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  std::shared_ptr<Automaton::Recognizer> m_recognizer;
  std::map< Common::MediaticData::SyntacticRelationId, std::shared_ptr<Automaton::Recognizer> > m_subSentRecognizers;
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
