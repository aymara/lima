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

// NAUTITIA
//
// jys 11-JUL-2002
//
// Events is a string of events. Class of characters and other
// events are in the form of printable bytes. Class Events 
// holds events which are of the same effect.

#ifndef LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_EVENTS_H
#define LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_EVENTS_H

#include "FlatTokenizerExport.h"
#include "CharClass.h"
#include "common/Data/LimaString.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{

class CharChart;

class LIMA_FLATTOKENIZER_EXPORT Events : public std::vector<const CharClass*>
{

public:
    explicit Events(const CharChart* chart);
    virtual ~Events();
 
    void addEventNamed(const LimaString& eventName);
    
    // Return 1 if supplied event is into the Events class, 0
    // otherwise
    bool isRecognized(const Lima::LimaChar& event) const;

private:
  const CharChart* m_chart;
};

} //namespace FlatTokenizer
} // namespace LinguisticProcessing
} // namespace Lima


#endif // LIMA_LINGUISTICPROCESSING_TOKENIZER_EVENTS_H
