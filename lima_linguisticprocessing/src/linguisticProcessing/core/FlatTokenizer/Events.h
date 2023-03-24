// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    explicit Events(std::shared_ptr<CharChart> chart);
    virtual ~Events();
 
    void addEventNamed(const LimaString& eventName);
    
    // Return 1 if supplied event is into the Events class, 0
    // otherwise
    bool isRecognized(const Lima::LimaChar& event) const;

private:
  std::shared_ptr<CharChart> m_chart;
};

} //namespace FlatTokenizer
} // namespace LinguisticProcessing
} // namespace Lima


#endif // LIMA_LINGUISTICPROCESSING_TOKENIZER_EVENTS_H
