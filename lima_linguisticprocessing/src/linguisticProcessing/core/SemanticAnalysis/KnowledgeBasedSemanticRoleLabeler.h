/*
    Copyright 2016 CEA LIST

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

#ifndef LIMA_LINGUISTICPROCESSING_SEMANTICANALYSIS_KNOWLEDGEBASEDSEMANTICROLELABELER_H
#define LIMA_LINGUISTICPROCESSING_SEMANTICANALYSIS_KNOWLEDGEBASEDSEMANTICROLELABELER_H

#include "SemanticAnalysisExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace SemanticAnalysis
{

#define KNOWLEDGEBASEDSEMANTICROLELABELER_CLASSID "KnowledgeBasedSemanticRoleLabeler"

class KnowledgeBasedSemanticRoleLabelerPrivate;

/** @brief This is a @ref MediaProcessUnit which do semantic role labeling using the knowledge-based
 * SRL in python made by Quentin Pradet during his PhD thesis
  *
  * As a ProcessUnit, it has an init and a process function. See @ref ProcessUnit for details.
  * 
  * IOPES:
  * - Input: an AnalysisContent and the following parameters in the configuration file:
  *   - debug: whether the debug option of the python module should be activated or not
  *   - path: the path to the knowledgesrl python package
  *   - mode: the semantic model to use to annotate. Either VerbNet (default) or FrameNet.
  * - Output: an AnalysisContent
  * - Preconditions: the AnalysisContent must the result of the syntactic analysis
  * - Effects: the annotation graph will be updated with SRL annotations.
  */
class LIMA_SEMANTICANALYSIS_EXPORT KnowledgeBasedSemanticRoleLabeler : public MediaProcessUnit
{

public:
  KnowledgeBasedSemanticRoleLabeler();
  virtual ~KnowledgeBasedSemanticRoleLabeler();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

  private:

    KnowledgeBasedSemanticRoleLabelerPrivate* m_d;
};

} // namespace SemanticAnalysis
} // namespace LinguisticProcessing
} // namespace Lima

#endif
