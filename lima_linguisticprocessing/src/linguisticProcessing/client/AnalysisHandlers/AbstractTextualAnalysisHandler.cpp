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
    try {
        m_suffix = unitConfiguration.getParamsValueAtKey("suffix");
    }
    catch (Common::XMLConfigurationFiles::NoSuchParam& e) {
        throw InvalidConfiguration(std::string("No such param 'suffix': ") + e.what());
    }
}


} // Lima
