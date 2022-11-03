// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
