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
#ifndef BASICHANDLER_H
#define BASICHANDLER_H

#include "linguisticProcessing/core/AnalysisDict/AbstractDictionaryEntryHandler.h"
#include <ostream>

namespace Lima
{

/**
@author Benoit Mathieu
*/
class BasicHandler : public LinguisticProcessing::AnalysisDict::AbstractDictionaryEntryHandler
{
public:
  BasicHandler(
    std::ostream* out);

  virtual ~BasicHandler();

  virtual void foundLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm);
  virtual void deleteLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm);
  virtual void endLingInfos();

  virtual void foundConcatenated();
  virtual void deleteConcatenated();
  virtual void foundComponent(uint64_t position, uint64_t length,StringsPoolIndex form);
  virtual void endComponent();
  virtual void endConcatenated();

  virtual void foundAccentedForm(StringsPoolIndex form);
  virtual void deleteAccentedForm(StringsPoolIndex form);
  virtual void endAccentedForm();

  virtual void foundProperties(LinguisticCode lings);
  
private:

  std::ostream* m_out;

};

}

#endif
