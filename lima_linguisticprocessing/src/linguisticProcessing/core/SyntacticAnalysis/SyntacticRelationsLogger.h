// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef SYNTACTICRELATIONSLOGGER_H
#define SYNTACTICRELATIONSLOGGER_H

// #include "linguisticProcessing/common/misc/AbstractLinguisticLogger.h"
#include "SyntacticAnalysisExport.h"
#include "linguisticProcessing/core/LinguisticProcessors/AbstractTextualAnalysisDumper.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{

#define SYNTACTICRELATIONSLOGGER_CLASSID "SyntacticRelationsLogger"

/** @brief A process unit used as a logger to debug syntactic analysis.*/
class LIMA_SYNTACTICANALYSIS_EXPORT SyntacticRelationsLogger : public AbstractTextualAnalysisDumper
{

public:

  SyntacticRelationsLogger();
  virtual ~SyntacticRelationsLogger();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const override;

protected:

private:

  MediaId m_language;
};


} // SyntacticAnalysis
} // LinguisticProcessing
} // Lima

#endif // SYNTACTICRELATIONSLOGGER_H
