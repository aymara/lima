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
    @file       ltrText.h

    @version    $Id$
    @date       created       jul 6, 2004
    @date       last revised  nov 30, 2010

    @author     Olivier Ferret
    @brief      a linear text representation

    Copyright (C) 2004-2010 by CEA LIST

    ======================================================================= */

#ifndef LIMA_COMMON_BAGOFWORDS__LTR_TEXT__H
#define LIMA_COMMON_BAGOFWORDS__LTR_TEXT__H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "ltrToken.h"
#include "linguisticProcessing/common/BagOfWords/bowNamedEntity.h"

#include <vector>
#include <iostream>


namespace Lima {
namespace Common {
namespace BagOfWords {

class LTR_Text;
LIMA_LINEARTEXTREPRESENTATION_EXPORT std::ostream& operator<<(std::ostream& os, const LTR_Text& text);
LIMA_LINEARTEXTREPRESENTATION_EXPORT QDebug& operator<<(QDebug& os, const LTR_Text& text);

/**
  * @class  LTR_Text
  * @brief  a linear text representation
  */

class LIMA_LINEARTEXTREPRESENTATION_EXPORT LTR_Text : public std::vector<LTR_Token*> {

private:  // -- types
    typedef std::vector<uint64_t> SENTENCE_BOUNDS_T;
    typedef std::deque<std::pair<
        Lima::Common::BagOfWords::BoWNamedEntity*, uint64_t> > NAMED_ENTITIES_T;

public:  // -- types
    typedef SENTENCE_BOUNDS_T::const_iterator SENTS_CONST_ITER_T;

public:  // -- methods
    /** @name  constructors/destructor */
    //@{
    LTR_Text() {}
    ~LTR_Text();
    //}@

    /** @name  accessors */
    //@{
    std::string getId() const {
        return m_id;
    }
    void setId(const std::string& id) {
        m_id = id;
    }
    //}@

    /** @name  copying */
    //@{
    LTR_Text(const LTR_Text& text);
    //}@

    /** @brief clear */
    // redefines the clear from base vector class to clear also sentence boundaries
    void clear() {
      std::vector<LTR_Token*>::clear();
      m_sentenceBounds.clear();
      m_namedEntities.clear();
    }
    
    /** @name  accessing */
    //@{
    SENTS_CONST_ITER_T beginSentenceBounds() const {
        return m_sentenceBounds.begin();
    }
    SENTS_CONST_ITER_T endSentenceBounds() const {
        return m_sentenceBounds.end();
    }
    //}@

    /** @name  adding */
    //@{
    void addToken(LTR_Token* token) {
        this->push_back(token);
    }
    void addSentenceBound(const uint64_t& sb) {
        m_sentenceBounds.push_back(sb);
    }
    void appendLTR_Text(const LTR_Text& ltr);
    //}@

    /** @name  reading/writing */
    //@{
    void binaryWriteOn(std::ostream& os) const;
    void binaryReadFrom(std::istream& is);
    //}@

    /** @name  printing */
    //@{
      friend LIMA_LINEARTEXTREPRESENTATION_EXPORT std::ostream& operator<<(std::ostream& os, const LTR_Text& text);
      friend LIMA_LINEARTEXTREPRESENTATION_EXPORT QDebug& operator<<(QDebug& os, const LTR_Text& text);
      //}@


private:  // -- methods
    /** @name  copying */
    //@{
    // prevent from using affectation
    LTR_Text& operator=(const LTR_Text&);
    //}@

private:  // -- data
    SENTENCE_BOUNDS_T m_sentenceBounds;
    /// @todo  such an identifier should certainly be present in a wrapping class;
    ///        not taken into account for reading/writing
    std::string m_id;
    NAMED_ENTITIES_T m_namedEntities;
};


}  // BagOfWords
}  // Common
}  // Lima


#endif  // LIMA_COMMON_BAGOFWORDS__LTR_TEXT__H
