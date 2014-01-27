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

#ifndef LIMA_LINGUISTICPROCESSING_EVENTS_H
#define LIMA_LINGUISTICPROCESSING_EVENTS_H

#include "EventAnalysisExport.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include "Event.h"
#include "common/Data/readwritetools.h"
#include "common/Data/strwstrtools.h"
#include <vector>

namespace Lima {
namespace LinguisticProcessing{
namespace EventAnalysis {


class LIMA_EVENTANALISYS_EXPORT Events : public AnalysisData, public std::vector<Event *>
{
public:

  Events()  {};
  
  //@{ binary input/output
  virtual void read(std::istream& file);
  virtual void write(std::ostream& file) const;
    //@}
  std::string toString(std::string parentURI) const;
  
private:
};


} //EventAnalysis
} // LinguisticProcessing
} // Lima

#endif
