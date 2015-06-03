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

/** =======================================================================
    @file       ltrToken.h

    @version    $Id$
    @date       created       jul 4, 2004
    @date       last revised  nov 30, 2010

    @author     Olivier Ferret
    @brief      a token of a linear text representation

    Copyright (C) 2004-2010 by CEA LIST

    ======================================================================= */

#ifndef LIMA_COMMON_BAGOFWORDS__LTR_TOKEN__H
#define LIMA_COMMON_BAGOFWORDS__LTR_TOKEN__H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"

#include <vector>
#include <iostream>

#include <QtCore/QSharedPointer>

namespace Lima {
namespace Common {
namespace BagOfWords {

class LTR_Token;
LIMA_LINEARTEXTREPRESENTATION_EXPORT std::ostream& operator<<(std::ostream& os, const LTR_Token& tok);
LIMA_LINEARTEXTREPRESENTATION_EXPORT QDebug& operator<<(QDebug& os, const LTR_Token& tok);

/**
  * @class  LTR_Token
  * @brief  a token of a linear text representation
            Each LTR_Token represents the set of words that start at a position.
            For each word, a boolean flag states if the corresponding word
            is a plain word
  */

class LIMA_LINEARTEXTREPRESENTATION_EXPORT LTR_Token : public
    std::vector<std::pair< QSharedPointer< Lima::Common::BagOfWords::BoWToken >, bool> > {

public:  // -- methods
    /** @name  constructors */
    //@{
    LTR_Token() {}
    //}@

    /** @name  destructor */
    //@{
    ~LTR_Token();
    //}@

    /** @name  copying */
    //@{
    LTR_Token(const LTR_Token& tok);
    //}@

    /** @name  reading/writing */
    //@{
    void binaryWriteOn(std::ostream& os) const;
    void binaryReadFrom(std::istream& is);
    //}@

    /** @name  printing */
    //@{
      friend LIMA_LINEARTEXTREPRESENTATION_EXPORT std::ostream& operator<<(std::ostream& os, const LTR_Token& tok);
      friend LIMA_LINEARTEXTREPRESENTATION_EXPORT QDebug& operator<<(QDebug& os, const LTR_Token& tok);
      //}@

private:  // -- methods
    /** @name  copying */
    //@{
    // prevent from using affectation
    LTR_Token& operator=(const LTR_Token&);
    //}@
};


}  // BagOfWords
}  // Common
}  // Lima


#endif  // LIMA_COMMON_BAGOFWORDS__LTR_TOKEN__H
