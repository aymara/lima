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
 * @file       GraphWap.h
 * @author     Grandjean Alex (alex.grandjean@cea.fr)
 * @date       Wed Nov 26 2014
 * copyright   Copyright (C) 20062014 by CEA LIST
 * Project     Lima
 * 
 * @brief      Transform Lima Graph information to be usable by wapiti Crf library
 * 
 * 
 ***********************************************************************/

#ifndef GRAPHTOWAP_H
#define GRAPHTOWAP_H


#include "model.h"
#include "decoder.h"

#include "SpecificEntitiesCRFExport.h"
#include "common/MediaticData/EntityType.h"
#include "common/misc/fsaStringsPool.h"

namespace Lima {
namespace LinguisticProcessing {

raw_t* graphToRaw(AnalysisContent& analysis, MediaId lg);

raw_t* graphToRawTrain(AnalysisContent& analysis, MediaId lg);


  }
}
#endif
