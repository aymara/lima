// Copyright 2002-2014 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSTEXTDUMPER_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSTEXTDUMPER_H

#include "SemanticAnalysisExport.h"
#include "linguisticProcessing/core/LinguisticProcessors/AbstractTextualAnalysisDumper.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace AnalysisDumpers
{

#define CONLLDUMPER_CLASSID "ConllDumper"

class ConllDumperPrivate;
class OutputFormatter;

/**
 * This dumper outputs analysis result in various CoNLL formats. Default is
 * CoNLL-U but CoNLL-2003 is also supported.
 * @author Gael de Chalendar
*/
class LIMA_SEMANTICANALYSIS_EXPORT ConllDumper : public AbstractTextualAnalysisDumper
{
public:
  ConllDumper();

  virtual ~ConllDumper();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:
  ConllDumperPrivate* m_d;

};

} // AnalysisDumpers
} // LinguisticProcessing
} // Lima

#endif
