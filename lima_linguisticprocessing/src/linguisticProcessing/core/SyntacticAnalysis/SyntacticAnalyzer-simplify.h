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
    Manager* manager)
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const;

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
