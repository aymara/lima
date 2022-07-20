// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LINGUISTICPROCESSING_LIMASTRINGTEXT_H
#define LIMA_LINGUISTICPROCESSING_LIMASTRINGTEXT_H

#include "common/Data/LimaString.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"

namespace Lima {
namespace LinguisticProcessing {

/**
* define the limaStringText resource which is the text in limaString formal
*/
class LimaStringText : public LimaString, public AnalysisData
{

public:
  LimaStringText(const LimaString& ls) : LimaString(ls) {}
  
  LimaStringText& operator=(const LimaString& src);

};

inline LimaStringText& LimaStringText::operator=(const LimaString& src)
{
  LimaString::operator=(src);
  return *this;
}

} // LinguisticProcessing
} // Lima

#endif
