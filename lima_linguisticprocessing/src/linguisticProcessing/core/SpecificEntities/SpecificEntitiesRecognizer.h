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
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_SPECIFICENTITIESSPECIFICENTITIESRECOGNIZER_H
#define LIMA_LINGUISTICPROCESSING_SPECIFICENTITIESSPECIFICENTITIESRECOGNIZER_H

#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/Automaton/recognizer.h"


namespace Lima
{
class AnalysisContent;
namespace LinguisticProcessing
{
namespace LinguisticAnalysisStructure
{
  class AnalysisGraph;
}
namespace Automaton
{
  class RecognizerMatch;
}
namespace SpecificEntities
{

#define SPECIFICENTITIESRECOGNIZER_CLASSID "SpecificEntitiesRecognizer"

/**
* @brief specific entities recognizer
*/
class SpecificEntitiesRecognizer : public MediaProcessUnit
{
public:
  SpecificEntitiesRecognizer();

  virtual ~SpecificEntitiesRecognizer();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const;

private:

  Automaton::Recognizer* m_recognizer;
  bool m_useSentenceBounds;
  std::string m_sentenceBoundsData; // name of data containing the considered boundaries if m_useSentenceBounds
  bool m_useDicoWords;
  std::string m_graph;

  // private member functions
  LimaStatusCode processOnEachSentence(AnalysisContent& analysis) const;
  LimaStatusCode processOnWholeText(AnalysisContent& analysis) const;

  bool findSEFromRecognizer(LinguisticGraphVertex& currentVertex,
                            LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                            AnalysisContent& analysis,
                            Automaton::RecognizerMatch& se) const;
};

} // SpecificEntities

} // LinguisticProcessing

} // Lima

#endif
