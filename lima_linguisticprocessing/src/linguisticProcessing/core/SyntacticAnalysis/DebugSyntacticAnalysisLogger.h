// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** @brief      debug only logger for syntactic analysis.
  *
  * @file       DebugSyntacticAnalysisLogger.h
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>
  *             Copyright (c) 2004 by CEA
  * @version    $Id$
  */

#ifndef LIMA_LINGUISTICPROCESSING_SYNTACTICANALYSIS_DEBUGSYNTACTICANALYSISLOGGER_H
#define LIMA_LINGUISTICPROCESSING_SYNTACTICANALYSIS_DEBUGSYNTACTICANALYSISLOGGER_H

#include "SyntacticAnalysisExport.h"
#include "linguisticProcessing/common/misc/AbstractLinguisticLogger.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{

#define DEBUGSYNTACTICANALYSISLOGGER_CLASSID "DebugSyntacticAnalysisLogger"

/** @brief A process unit used as a logger to debug syntactic analysis.*/
class LIMA_SYNTACTICANALYSIS_EXPORT DebugSyntacticAnalysisLogger : public AbstractLinguisticLogger
{

public:

  DebugSyntacticAnalysisLogger();
  virtual ~DebugSyntacticAnalysisLogger();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

protected:

private:

  MediaId m_language;
  bool m_displayChains;
  bool m_displayDependencies;
  bool m_displayRelationsOrderOfArguments;
  bool m_displayRelationsDistanceOfArguments;
  bool m_displayLemmas;
};


} // SyntacticAnalysis
} // LinguisticProcessing
} // Lima

#endif // LIMA_LINGUISTICPROCESSING_SYNTACTICANALYSIS_DEBUGSYNTACTICANALYSISLOGGER_H
