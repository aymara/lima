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
#include "linguisticProcessing/core/LinguisticProcessors/AbstractLinguisticLogger.h"

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
    Manager* manager)
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const;

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
