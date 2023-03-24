// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

    virtual std::shared_ptr<Common::AbstractAccessByString> getAccessByString() const override;
    virtual std::shared_ptr<Common::AbstractModifierOnAccessByString> getRwAccessByString() const;

private:
  std::shared_ptr<Common::AbstractModifierOnAccessByString> m_fsaRwAccess;
//  Common::AbstractAccessByString* m_fsaAccess;
    
};

}

}

}

#endif // LIMA_LINGUISTICPROCESSING_ANALYSISDICTFSARWACCESSRESOURCE_H
