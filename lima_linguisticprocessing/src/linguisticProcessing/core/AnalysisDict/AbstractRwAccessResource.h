// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDICTABSTRACTRWACCESSRESOURCE_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDICTABSTRACTRWACCESSRESOURCE_H

#include "linguisticProcessing/core/LinguisticResources/AbstractAccessResource.h"
#include "common/misc/AbstractRwAccessByString.h"
#include "common/misc/AbstractAccessByString.h"

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDict {

/**
@author Olivier Mesnard
*/
class AbstractRwAccessResource : public AbstractAccessResource
{
  Q_OBJECT
public:

    AbstractRwAccessResource(QObject* parent = 0);
    virtual ~AbstractRwAccessResource();

    virtual void init(
      Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
      Manager* manager) override = 0;

    virtual std::shared_ptr<Common::AbstractAccessByString> getAccessByString() const override = 0;
 
    virtual std::shared_ptr<Common::AbstractModifierOnAccessByString> getRwAccessByString() const = 0;

};

}

}

}

#endif
