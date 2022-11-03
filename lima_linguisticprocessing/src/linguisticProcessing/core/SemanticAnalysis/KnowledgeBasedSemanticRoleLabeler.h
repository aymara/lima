// Copyright 2016 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
