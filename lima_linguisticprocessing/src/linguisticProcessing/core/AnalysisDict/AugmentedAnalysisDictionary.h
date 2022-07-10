// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDICTSIMPLEANALYSISDICTIONARY_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDICTSIMPLEANALYSISDICTIONARY_H

#include "linguisticProcessing/core/AnalysisDict/AbstractAnalysisDictionary.h"
#include "common/misc/AbstractAccessByString.h"

#include "SimpleAnalysisDictionary.h"


namespace Lima {

namespace LinguisticProcessing {

namespace AnalysisDict {


#define SIMPLEANALYSISDICTIONARY_CLASSID "AugmentedAnalysisDictionary"
/**
@author Olivier Mesnard
*/
class AugmentedAnalysisDictionary : public SimpleAnalysisDictionary
{
friend class SimpleAnalysisDictionarySubWordIterator;

public:
    AugmentedAnalysisDictionary();
    AugmentedAnalysisDictionary(
      Lima::Common::AbstractAccessByString* m_access,
      Lima::Common::AbstractModifierOnAccessByString* m_RwAccess,
      const std::string& dataFileName);

    ~AugmentedAnalysisDictionary();

    virtual void init(
      Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
      Manager* manager);
    
    void addEntry(Dictionary::DictionaryEntry);    
private:

  Lima::Common::AbstractAccessByString* m_access;
    
};

}

}

}

#endif
