// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSSTOPLIST_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSSTOPLIST_H

#include "AnalysisDumpersExport.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "common/Data/LimaString.h"

#include <set>

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDumpers
{

#define STOPLIST_CLASSID "StopList"
/**
@author Benoit Mathieu
*/
class LIMA_ANALYSISDUMPERS_EXPORT StopList : public AbstractResource, public std::set<LimaString>
{
  Q_OBJECT
public:
    StopList();

    virtual ~StopList();

    void init(
      Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
      Manager* manager) override
    ;

  };

} // AnalysisDumper

} // LinguisticProcessing

} // Lima

#endif
