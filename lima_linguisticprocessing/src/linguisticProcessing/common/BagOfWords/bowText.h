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
class LIMA_BOW_EXPORT BoWText : public std::vector< AbstractBoWElement* >
{
public:
  BoWText();
  BoWText(const BoWText&);
  BoWText& operator = (const BoWText&);
  virtual ~BoWText();
  void clear();

  friend LIMA_BOW_EXPORT std::ostream& operator << (std::ostream&, const BoWText&);
  friend LIMA_BOW_EXPORT QDebug& operator << (QDebug&, const BoWText&);
  
  void writeBoWText(std::ostream& stream);
  
  std::string lang;
};

} // namespace BagOfWords
} // namespace Common
} // namespace Lima

#endif 
