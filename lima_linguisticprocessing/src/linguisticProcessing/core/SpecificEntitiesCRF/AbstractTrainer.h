
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

 *************************************************************************
 * @file       AbstractTrainer.h
 * @author     Grandjean Alex (alex.grandjean@cea.fr)
 * @date       Wed Mar 26 2014
 * copyright   Copyright (C) 20062014 by CEA LIST
 * Project     Lima
 * 
 * @brief      TODO
 * 
 * 
 ***********************************************************************/


#ifndef ABSTRACTTRAINER_H
#define ABSTRACTTRAINER_H

#include "SpecificEntitiesCRFExport.h"

#include <map>
#include <string>
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima {

class AnalysisContent;

class AbstractTrainer 
{
public:

  virtual ~AbstractTrainer()  {};

  virtual void initOptions(const std::map<std::string, std::string>& options) = 0;

  virtual void training() = 0;
  
  virtual void learning(AnalysisContent& ac, MediaId lg)=0;



};


}

#endif

