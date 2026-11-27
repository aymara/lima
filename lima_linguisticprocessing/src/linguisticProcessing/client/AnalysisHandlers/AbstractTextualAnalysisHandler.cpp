// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 *   Copyright (C) 2007-2012 by CEA LIST                               
 *                                             
 * @file        .h
 * @author      Claire Mouton (Claire.Mouton@cea.fr) 

 *              Copyright (c) 2007 by CEA
 * @date        Created on Apr, 24 2007
 */
#include "AbstractTextualAnalysisHandler.h"

namespace Lima{

AbstractTextualAnalysisHandler::~AbstractTextualAnalysisHandler()
{
}

void AbstractTextualAnalysisHandler::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                    Manager* manager)
{
    LIMA_UNUSED(manager);
    // Optional (a handler declared without parameters in the Resources module
    // of the configuration, see PipelineHandlers.h)
    try {
        m_suffix = unitConfiguration.getParamsValueAtKey("suffix");
    }
    catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    }
}


} // Lima
