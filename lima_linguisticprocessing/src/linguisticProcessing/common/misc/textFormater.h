// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
                          textFormater.h  -  description
                             -------------------
    begin                : mar d� 17 2002
    copyright            : (C) 2002 by CEA
    email                : de-chalendarg@zoe.cea.fr
 ***************************************************************************/


#ifndef LIMA_COMMONS_MISC_TEXTFORMATER_H
#define LIMA_COMMONS_MISC_TEXTFORMATER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include <iostream>
#include <stdexcept>

namespace Lima {
namespace Common {
namespace Misc {


/**
  * This class is dedicated to the various adaptations that have to be done on texts :
  * - un-HTML-isation;
  * - maybe XML validation;
  * - ...
  *
  * @author Gael de Chalendar
  */
class LIMA_LPMISC_EXPORT TextFormater
{
public: 
    TextFormater();
    virtual ~TextFormater();
    
    /**
      * This method replaces all the tags in the UTF-8 encoded input stream by 
      * spaces or underscores in the output stream. 
      * Underscores are written when the tag is adjacent to a word (directly or by 
      * another) tag. Spaces are written otherwise.  
      * If an unclosed tag finishes the input stream, the result is written but an 
      * error message is written in <I>status</I> and a non zero-value is returned.
      * If an opening '<' is found inside a tag, it is ignored (replaced) but an 
      * error message is written in <I>status</I> and a non zero-value will be returned.
      * If a closing '>' is found outside a tag, it is ignored (replaced) but an 
      * error message is written in <I>status</I> and a non zero-value will be returned.
      * Currently, this method is stateless. 2 subsequent calls consider working 
      * on two different streams.
      * By the way, it has not to store persistent information about its state and
      * thus it does not modify its receiver. Consequently, it is constant.
      *
      * @param std::ostream& status the stream where errors and warning will be 
      *        written
      * @param std::istream& input the input stream. Should contain a valid SGML 
      *        file
      * @param std::ostream& output the output stream. The result stream where
               un-tagged results are written
      * @param std::string& endSentenceTag if set to non-empty, this will be used 
      *        as a sentence separator and the first character of this tag (the <)
      *        will be replaced by a dot in the output
      * @return int 0 if there is no error nor warning ; non-zero otherwise.
    */
    int untaggingWithSpaces(std::ostream& status, std::istream& input, 
            std::ostream& output, bool wide = false, 
            const std::string& endSentenceTag = "") const;

private:
    void putTag(std::ostream& status, std::ostream& output, bool wide,
        const std::string& endSentenceTag, std::ostringstream& tag, 
        std::ostringstream& tagValue) const;

    void putWhites(std::ostream& status, std::ostream& output, bool wide,
    const std::string& endSentenceTag, std::ostringstream& tagValue, 
    std::ostream& carLuTarget, char carLu, char fillChar,
    uint64_t nb, uint64_t nbNewLines) const;

};

} // closing namespace Misc
} // closing namespace Common
} // closing namespace Lima


#endif
