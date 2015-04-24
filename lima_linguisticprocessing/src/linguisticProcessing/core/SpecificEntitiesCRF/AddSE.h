/*
    Copyright 2002-2014 CEA LIST

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

 ************************************************************************
 *
 * @file       AddSE.h
 * @author     Grandjean Alex (alex.grandjean@cea.fr)
 * @date       Mon Nov 24 2014
 * copyright   Copyright (C) 2006-2014 by CEA LIST
 * Project     Lima
 * 
 * @brief      Add Specific Entity into the graph
 * 
 * 
 ***********************************************************************/
#ifndef ADDSE_H
#define ADDSE_H


#include "SpecificEntitiesCRFExport.h"

#include "common/misc/fsaStringsPool.h"
namespace Lima {

  namespace LinguisticProcessing {


    /**
     * @brief Add an entity in the graph
     */
void addSpecificEntities(AnalysisContent& analysis, MediaId lg,  std::string str, std::string type, uint64_t pos, uint64_t length) ;

  }
}

#endif
