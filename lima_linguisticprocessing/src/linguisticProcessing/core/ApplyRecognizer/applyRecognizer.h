// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
            Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;
    
private:
  // automaton to apply
  std::vector<std::shared_ptr<Automaton::Recognizer>> m_recognizers;

  // parameters of the application
  bool m_useSentenceBounds;
  std::string m_sentenceBoundsData; // name of data containing the boundaries if m_useSentenceBounds
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
