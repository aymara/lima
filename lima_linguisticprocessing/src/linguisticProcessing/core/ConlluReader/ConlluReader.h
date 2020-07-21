/*
    Copyright 2002-2020 CEA LIST

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

#ifndef LIMA_LINGUISTICPROCESSING_ConlluReader_H
#define LIMA_LINGUISTICPROCESSING_ConlluReader_H

#include "ConlluReaderExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace ConlluReader
{

#define CONLLUREADER_CLASSID "ConlluReader"

class CharChart;

class ConlluReaderPrivate;

/** @brief This is a @ref MediaProcessUnit that is usually the first element of the pipeline. It cuts the input text into tokens
  *
  * As a ProcessUnit, it has an init and a process function. See @ref ProcessUnit for details.
  *
  * IOPES:
  * - Output:          an AnalysisContent
  * - Preconditions:   the AnalysisContent must contain an AnalysisData of type LimaStringText named "Text"
  * - Effects:         the AnalysisContent will contain an AnalysisData of type AnalysisGraph
  *                    named "AnalysisGraph" which is a linear graph (a string) containing one vertex
  *                    for each detected token.
  */
class LIMA_CONLLUREADER_EXPORT ConlluReader : public MediaProcessUnit
{
public:
  ConlluReader();
  virtual ~ConlluReader();

  void init(Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:
  ConlluReaderPrivate* m_d;
};

} // namespace ConlluReader
} // namespace LinguisticProcessing
} // namespace Lima

#endif
