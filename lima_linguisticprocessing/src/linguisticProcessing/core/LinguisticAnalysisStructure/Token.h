// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_LINGUISTICANALYSISSTRUCTURETOKEN_H
#define LIMA_LINGUISTICPROCESSING_LINGUISTICANALYSISSTRUCTURETOKEN_H

#include "LinguisticAnalysisStructureExport.h"
#include "common/LimaCommon.h"
#include "common/Data/LimaString.h"
#include "common/misc/fsaStringsPool.h"
#include "common/MediaticData/mediaticData.h"
#include "TStatus.h"
// #include "linguisticProcessing/core/Tokenizer/CharChart.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"

namespace Lima {

namespace LinguisticProcessing {

namespace LinguisticAnalysisStructure {

/**
  * @brief holds surface data of a token
  * @author Benoit Mathieu
  */
class LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT Token{
public:

    Token(StringsPoolIndex form,const LimaString& stringForm,uint64_t position,uint64_t length);
    Token(StringsPoolIndex form,const LimaString& stringForm,uint64_t position,uint64_t length, const TStatus& status);
    Token(const Token& tok);

    virtual ~Token();

    inline StringsPoolIndex form() const;
    inline const LimaString& stringForm() const;
    inline uint64_t position() const;
    inline uint64_t length() const;
    inline const TStatus& status() const;
    inline TStatus& status();
    inline void setPosition(uint64_t pos);
    inline void setLength(uint64_t length);
    
    /**
     * Set the TStatus of a token.
     * <b>Warning : Token is the owner of status object</b>. Destructor
     * of Token deletes status. Be aware of deleting old status before
     * setting a new one.
     * @param status 
     */
    inline void setStatus(const TStatus& status);
    
    virtual void outputXml(std::ostream& xmlStream,
        const Common::PropertyCode::PropertyCodeManager& pcm,
        const FsaStringsPool& sp) const;
    
    inline const std::vector<StringsPoolIndex>& orthographicAlternatives() const;
    inline void addOrthographicAlternatives(StringsPoolIndex alt);
    

private:
    StringsPoolIndex m_form;
    LimaString m_stringForm;
    uint64_t m_position;
    uint64_t m_length;
    TStatus m_status;
    std::vector<StringsPoolIndex> m_alternatives;
};

inline StringsPoolIndex Token::form() const {
  return m_form;
}

inline const LimaString& Token::stringForm() const {
  return m_stringForm;
}

inline uint64_t Token::position() const {
  return m_position;
}

inline uint64_t Token::length() const {
  return m_length;
}

inline const TStatus& Token::status() const {
  return m_status;
}

inline TStatus& Token::status() {
  return m_status;
}

inline void Token::setStatus(const TStatus& status) {
  m_status=status;
}

inline void Token::setPosition(uint64_t pos) {
  m_position=pos;
}

inline void Token::setLength(uint64_t length) {
  m_length=length;
}

inline const std::vector<StringsPoolIndex>& Token::orthographicAlternatives() const
{
  return m_alternatives;
}

inline void Token::addOrthographicAlternatives(StringsPoolIndex alt)
{
  m_alternatives.push_back(alt);
}

} // namespace LinguisticAnalysisStructure

} // namespace Lima

} // namespace LinguisticProcessing

#endif
