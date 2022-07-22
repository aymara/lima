// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file     bowText.h
 * @author   Besancon Romaric
 * @date     Tue Oct  7 17:13:26 2003
 * copyright Copyright (C) 2003 by CEA LIST
 * 
 * @brief  
 * 
 * 
 ***********************************************************************/

#ifndef BOWTEXT_H
#define BOWTEXT_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include <boost/shared_ptr.hpp>

#include <vector>
#include <iostream>

namespace Lima {
namespace Common {
namespace BagOfWords {

class AbstractBoWElement;

/**
 * This class represents a list of elements, that are pointers on
 * polymmorphic tokens that can be simple tokens or complex tokens
 * such as terms or named entities. Complex tokens have a list of
 * components that can be references to other tokens in the list or to
 * tokens specific to themselves. 
 *
 * @attention: the references in the component list of the complex
 * tokens cannot be forward references: they can only be references to
 * token present in the list before the complex token (forward
 * references will cause problems in read/write functions)
 * 
 **/
class LIMA_BOW_EXPORT BoWText : public std::vector< boost::shared_ptr< AbstractBoWElement > >
{
public:
  BoWText();
  BoWText(const BoWText&);
  BoWText& operator = (const BoWText&);
  virtual ~BoWText();
  virtual void clear();

  friend LIMA_BOW_EXPORT std::ostream& operator << (std::ostream&, const BoWText&);
  friend LIMA_BOW_EXPORT QDebug& operator << (QDebug&, const BoWText&);
  
  void writeBoWText(std::ostream& stream);
  
  std::string lang;
};

} // namespace BagOfWords
} // namespace Common
} // namespace Lima

#endif 
