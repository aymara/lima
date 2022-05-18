// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  *
  * @file        SyntacticAnalyzer-deps.h
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2003 by CEA
  * @date        Created on Aug, 31 2004
  * @version     $Id$
  *
  */

#ifndef LIMA_SYNTACTICANALYSIS_SYNTACTICANALYZERDEPS_H
#define LIMA_SYNTACTICANALYSIS_SYNTACTICANALYZERDEPS_H

#include "SyntacticAnalysisExport.h"
#include "SyntacticData.h"
#include "linguisticProcessing/core/Automaton/recognizer.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{

#define SYNTACTICANALYZERDEPS_CLASSID "SyntacticAnalyzerDeps"

/** 
  * This class does the syntactic analysis. It works on a pointer to a graph.
  *
  * It is a @ref MediaProcessUnit. It accepts the following parameters:
  *   - list:actions : each value is the name of a group in the Resources 
  *     module whose class is AutomatoRecognizer. This group possesses a rules
  *     parameter that points to the compiled rules file that recognizes some 
  *     dependency relations
  */
class LIMA_SYNTACTICANALYSIS_EXPORT SyntacticAnalyzerDeps : public MediaProcessUnit
{
public:
  SyntacticAnalyzerDeps();

  //  SyntacticAnalyzerDeps(SyntacticAnalyzerDeps&);

  virtual ~SyntacticAnalyzerDeps() {}

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const override;

  MediaId language() const { return m_language; }

private:

protected:
  MediaId m_language;

  std::map< std::string, Automaton::Recognizer* > m_recognizers;
  
  std::deque< std::string > m_actions;

  bool m_applySameRuleWhileSuccess;
};


} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif
