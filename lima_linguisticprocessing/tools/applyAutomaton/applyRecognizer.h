// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       applyRecognizer.h
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Wed Dec 17 2003
 * @version    $Id$
 * copyright   Copyright (C) 2003 by CEA LIST
 * Project     
 * 
 * @brief      application of a recognizer, depending of the preceding
 *             analysis of the text (no analysis, tokenization, complete 
 *             analysis)
 * 
 ***********************************************************************/

#ifndef APPLYRECOGNIZER_H
#define APPLYRECOGNIZER_H

#include "linguisticProcessing/core/Automaton/recognizer.h"
//#include "AutomatonText/recognizerText.h"
#include "linguisticProcessing/core/Tokenizer/Tokenizer.h"
#include "linguisticProcessing/core/Tokenizer/Automaton.h"
#include "linguisticProcessing/core/Tokenizer/CharChart.h"
#include <iostream>
#include <string>

namespace Lima {
namespace LinguisticProcessing {

// two outputs possible : one MUC-like style where the tags are placed in
// the text (inline), and one Detect-like style, where the identified 
// entities are placed in a separate file (offline)
typedef enum { INLINE, OFFLINE } StyleOutput;

enum OutputNormalizationType {
  NO_NORMALIZATION,        /**< no normalization is indicated */
  DEFAULT_NORMALIZATION,    /**< when there is no normalization available,
                   uses a default normalization (which is simply
                   the string of the entity in the text */
  KNOWN_NORMALIZATION_ONLY    /**< output normalization only if there is
                     a true normaization available for the 
                     entity */
};

//**********************************************************************
// generic virtual class
//**********************************************************************
class RecognizerToApply {
public:
  RecognizerToApply(Automaton::Recognizer* reco,
                    MediaId language):
    m_language(language),
    m_recognizer(reco),
    m_listEntities(false),
    m_encoding("latin1"),
    m_outputStyle(INLINE),
    m_normalizationStyle(NO_NORMALIZATION),
    m_doPosTagging(false),
    m_testOnFullToken(true)
    {}
  virtual ~RecognizerToApply() {}
  virtual uint64_t applyToText(LimaString contentText, 
                                   std::ostream& output) =0;

  void setParameters(const bool listEntities, 
                     const std::string& encoding,
                     const StyleOutput& outputStyle,
                     const OutputNormalizationType& normalization,
                     const bool doPosTagging=false,
                     const bool testOnFullToken=true);

  template<typename ResultType> 
    static LimaString 
    outputString(const ResultType& result,
                 const LimaString& text,
                 const uint64_t offsetLastSuccess,
                 const Common::MediaticData::LanguageData::EntityNames& entityNames,
                 const OutputNormalizationType normType=NO_NORMALIZATION,
                 const uint64_t firstOffset=1);

  template<typename ResultType>
    void printResults(const ResultType& result, 
                      const LimaString& text,
                      std::ostream& output);

  static bool findType(const std::string& type,
                       uint64_t& i);
  static const std::string& getOpeningTag(const uint64_t& i);
  static const std::string& getClosingTag(const uint64_t& i);

  static const std::vector<std::string>& knownTypes();
  static const std::vector<std::string>& openingTags();
  static const std::vector<std::string>& closingTags();

  unsigned char getLanguage() { return m_language; }
  
protected:
  MediaId m_language;
  Automaton::Recognizer* m_recognizer;

  // parameters
  bool m_listEntities;
  std::string m_encoding;
  StyleOutput m_outputStyle;
  OutputNormalizationType m_normalizationStyle;
  bool m_doPosTagging;
  bool m_testOnFullToken;
};

//**********************************************************************
// apply recognizer after complete analysis
//**********************************************************************
class RecognizerOnAnalyzedText : public RecognizerToApply {
public:
  RecognizerOnAnalyzedText(Automaton::Recognizer* reco, 
                           MediaId language,
                           const std::string& resourcesPath,
                           const bool dumpXML=false,
                           const bool reorganizeRules=false);
  ~RecognizerOnAnalyzedText() {
  }
  uint64_t applyToText(LimaString contentText, std::ostream& output);
protected:
};

//**********************************************************************
// apply recognizer with no analysis, based on tokenization
//**********************************************************************
/*
class RecognizerOnTokenizedText : public RecognizerToApply {
public:
  RecognizerOnTokenizedText(Automaton::Recognizer* reco,
                            MediaId language,
                            const std::string& resourcesPath);

  ~RecognizerOnTokenizedText() { 
    delete m_tokenizer; 
  }

  uint64_t applyToText(LimaString contentText, std::ostream& output);
protected:
  Tokenizer::Tokenizer* m_tokenizer;
  MediaId m_language;
};
*/

//**********************************************************************
// apply recognizer with no analysis, on simple text, based on 
// basic tokenization on spaces
//**********************************************************************
/*
class RecognizerOnSimpleText : public RecognizerToApply {
public:
  RecognizerOnSimpleText(Automaton::Recognizer* reco);
  uint64_t applyToText(LimaString contentText, std::ostream& output);
protected:
  Automaton::RecognizerText* m_recognizerText;
};
*/

//**********************************************************************
// template function for output
/** 
 * text string with tags inserted around matches
 * 
 * @param result the result of the named entity search
 * @param text the reference text in which the tag are inserted
 * @param offsetLastSuccess the position where we start in the text
 * @param openingTag the opening tag to insert
 * @param closingTag the closing tag to insert
 * @param firstOffset the offset of the first element in the text
 * (to deal with both FullTokens that start at 1 and usual strings
 * that start at 0 -- default value is 1)
 *
 * @return a LimaString
 */
// the template function can be defined in the .cpp because 
// it is only instanciated in this file
template<typename ResultType>
LimaString RecognizerToApply::
outputString(const ResultType& result,
             const LimaString& text,
             const uint64_t offsetLastSuccess,
             const Common::MediaticData::LanguageData::EntityNames& entityNames,
             const OutputNormalizationType normType,
             const uint64_t firstOffset) 
{
  // have to take into account the offset of the first character of
  // the string because FullToken positions begin at 1, not 0
  if (result.size() == 0) {
    return text;
  }

  LimaString output;
  uint64_t lastPosition(offsetLastSuccess);
  LimaString openingTag, closingTag;
  uint64_t i;
  Common::MediaticData::LanguageData::EntityNames::const_iterator
    type=entityNames.find(result.getType());
  if (type==entityNames.end()) {
    AULOGINIT;
    LERROR << "undefined type " << result.getType();
    openingTag="UNDEFINED_TYPE";
    closingTag="/UNDEFINED_TYPE";
  }
  else {
    const std::string& typeName=(*type).second;
    if (findType(typeName,i)) {
      openingTag = getOpeningTag(i);
      closingTag = getClosingTag(i);
    }
    else {
      openingTag=typeName;
      closingTag="/"+typeName;
    }
  }
  
  switch (normType) {
  case DEFAULT_NORMALIZATION:
    {
      if (result.getNormalizedForm().size() != 0) {
        openingTag += LimaChar(' ')+result.getNormalizedForm().str();
      }
      else {
        std::ostringstream oss;
        oss << DEFAULT_ATTRIBUTE << "=\""
            << result.concatString() << "\"" ;
        LimaString defaultNormalizedForm(oss.str());
        openingTag += LimaChar(' ')+defaultNormalizedForm;
      }
      break;
    }
  case KNOWN_NORMALIZATION_ONLY:
    {
      if (result.getNormalizedForm().size() != 0) {
        openingTag += LimaChar(' ')+result.getNormalizedForm().str();
      }
      break;

    }
  case NO_NORMALIZATION:
    {
      break;
    }
  }

/*   for (uint64_t i(0); i<result.numberOfMatches(); i++) { */
/*     output += LimaString(text,lastPosition, */
/*                           result[i].position()-firstOffset-lastPosition) */
/*       +LimaString("<")+openingTag+LimaString(">") */
/*       +LimaString(text,result[i].position()-firstOffset,result[i].length()) */
/*       +LimaString("<")+closingTag+LimaString(">"); */
/*     lastPosition=result[i].positionEnd()-firstOffset; */
/*   } */

  if (! result.empty()) {
    // keep notKept element inside tags
    output += LimaString(text,lastPosition,
                          result.positionBegin()-firstOffset-lastPosition)
      +Common::Misc::utf8stdstring2limastring("<")+openingTag+Common::Misc::utf8stdstring2limastring(">")
      +Common::Misc::utf8stdstring2limastring(LimaString(text,result.positionBegin()-firstOffset,result.length()))
      +Common::Misc::utf8stdstring2limastring("<")+closingTag+Common::Misc::utf8stdstring2limastring(">");
  }
  
  return output;
}

} // end namespace
} // end namespace

#endif
