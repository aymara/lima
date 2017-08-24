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
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDICTFSARWACCESSRESOURCE_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDICTFSARWACCESSRESOURCE_H

#include "AnalysisDictExport.h"
#include "FsaAccessResource.h"
#include "AbstractRwAccessResource.h"
// #include "common/FsaAccess/FsaAccessBuilderRandom16.h"

namespace Lima {

namespace LinguisticProcessing {

namespace AnalysisDict {

#define FSARWACCESSRESSOURCE_CLASSID "FsaRwAccess"

/**
@author Benoit Mathieu
*/
//class FsaRwAccessResource : public AbstractRwAccessResource
class LIMA_ANALYSISDICT_EXPORT FsaRwAccessResource : public FsaAccessResource
{
  Q_OBJECT
public:
    FsaRwAccessResource();

    virtual ~FsaRwAccessResource();
    
    void init(
     Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
     Manager* manager) override;

    virtual Common::AbstractModifierOnAccessByString* getRwAccessByString() const;
    virtual Common::AbstractAccessByString* getAccessByString() const override;

private:
  Common::AbstractModifierOnAccessByString* m_fsaRwAccess;
//  Common::AbstractAccessByString* m_fsaAccess;
    
};

}

}

}

#endif // LIMA_LINGUISTICPROCESSING_ANALYSISDICTFSARWACCESSRESOURCE_H
