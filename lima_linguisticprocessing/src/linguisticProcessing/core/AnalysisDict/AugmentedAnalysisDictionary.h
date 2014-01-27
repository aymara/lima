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
