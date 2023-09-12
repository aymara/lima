// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
#include <boost/shared_ptr.hpp>


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

class LIMA_LINEARTEXTREPRESENTATION_EXPORT LTR_Text : public std::vector< boost::shared_ptr<LTR_Token> > {

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
    virtual ~LTR_Text() {this->clear();};
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
//    LTR_Text& operator=(const LTR_Text&);
    //}@

    /** @brief clear */
    // redefines the clear from base vector class to clear also sentence boundaries
    void clear() {
      // free tokens
//      for (LTR_Text::const_iterator itTok = this->begin();
//           itTok != this->end(); ++ itTok) {
//          if(*itTok) delete *itTok;
//      }
      std::vector< boost::shared_ptr<LTR_Token> >::clear();
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
    void addToken(boost::shared_ptr<LTR_Token>& token);
    void addSentenceBound(const uint64_t& sb);
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
    LTR_Text& operator=(const LTR_Text&) = delete;
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
