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
/************************************************************************
 *
 * @file       applyRecognizer.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Fri Jan 14 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * Project     s2lp
 * 
 * @brief this class contains a generic process unit for the
 * application of automaton rules on a text (used for named entities,
 * syntactic analysis...)
 * 
 * 
 ***********************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_APPLYRECOGNIZER_H
#define LIMA_LINGUISTICPROCESSING_APPLYRECOGNIZER_H

#include "ApplyRecognizerExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/Automaton/recognizerData.h"
#include "linguisticProcessing/core/Automaton/recognizer.h"

namespace Lima {
namespace LinguisticProcessing {
namespace ApplyRecognizer {

#define APPLYRECOGNIZER_CLASSID "ApplyRecognizer"

class LIMA_APPLYRECOGNIZER_EXPORT ApplyRecognizer : public MediaProcessUnit
{
public:
  ApplyRecognizer();

  virtual ~ApplyRecognizer();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager)
    ;

  LimaStatusCode process(AnalysisContent& analysis) const;
    
private:
  // automaton to apply
  std::vector<Automaton::Recognizer*> m_recognizers;

  // parameters of the application
  bool m_useSentenceBounds;
  bool m_updateGraph;
  bool m_resolveOverlappingEntities;
  Automaton::OverlapResolutionStrategy m_overlappingEntitiesStrategy;
  bool m_testAllVertices;
  bool m_stopAtFirstSuccess;
  bool m_onlyOneSuccessPerType;
  std::string m_graphId;
  std::string m_dataForStorage;

  // private member functions
  // (this function should be in XMLConfigurationFiles)
  bool getBooleanParameter(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                           const std::string& param) const;

  LimaStatusCode processOnEachSentence(AnalysisContent& analysis,
                                     Automaton::Recognizer* reco,
                                     RecognizerData* recoData) const;
  LimaStatusCode processOnWholeText(AnalysisContent& analysis,
                                  Automaton::Recognizer* reco,
                                  RecognizerData* recoData) const;
};

} // end namespace
} // end namespace
} // end namespace

#endif
