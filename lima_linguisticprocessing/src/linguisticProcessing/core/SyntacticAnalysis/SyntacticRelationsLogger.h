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
    Manager* manager)
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const;

protected:

private:

  MediaId m_language;
};


} // SyntacticAnalysis
} // LinguisticProcessing
} // Lima

#endif // SYNTACTICRELATIONSLOGGER_H
