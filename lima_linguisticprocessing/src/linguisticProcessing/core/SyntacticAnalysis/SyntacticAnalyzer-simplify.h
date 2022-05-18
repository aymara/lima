// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  *
  * @file        SyntacticAnalyzer-simplify.h
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2005 by CEA
  * @date        Created on Mar, 15 2005
  * @version     $Id$
  *
  */

#ifndef LIMA_SYNTACTICANALYSIS_SYNTACTICANALYZERSIMPLIFY_H
#define LIMA_SYNTACTICANALYSIS_SYNTACTICANALYZERSIMPLIFY_H

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

#define SYNTACTICANALYZERSIMPLIFY_CLASSID "SyntacticAnalyzerSimplify"

/** Simplifies the sentence by removing (while storing) recursively the subsentences like
  * relatives of subordinates, thus simplifying later heterosyntagmatic relations search
  */
class LIMA_SYNTACTICANALYSIS_EXPORT SyntacticAnalyzerSimplify : public MediaProcessUnit
{
public:
  SyntacticAnalyzerSimplify();

  virtual ~SyntacticAnalyzerSimplify() {}

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const override;

  inline MediaId language() const { return m_language; }

private:

protected:
  MediaId m_language;

  Automaton::Recognizer* m_recognizer;

};

} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_SYNTACTICANALYSIS_SYNTACTICANALYZERSIMPLIFY_H
