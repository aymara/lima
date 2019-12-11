/*
    Copyright 2002-2014 CEA LIST

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
