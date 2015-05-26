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
// jys 4-JUL-2002
//
// The status of a token found by Tokenizer for the use of
// Morphologic Analysis modules

#ifndef LIMA_LINGUISTICPROCESSING_LINGUISTICANALYSISSTRUCTURE_TSTATUS_H
#define LIMA_LINGUISTICPROCESSING_LINGUISTICANALYSISSTRUCTURE_TSTATUS_H

#include "LinguisticAnalysisStructureExport.h"
#include <iostream>
#include "common/Data/LimaString.h"

namespace Lima {
namespace LinguisticProcessing {
namespace LinguisticAnalysisStructure {


enum AlphaCapitalType
{
  T_NULL_CAPITAL = 0,
  T_CAPITAL,
  T_SMALL,
  T_CAPITAL_1ST,
  T_ACRONYM,
  T_CAPITAL_SMALL,
  T_ABBREV
} ;
enum AlphaRomanType
{
  T_NULL_ROMAN = 0,
  T_CARDINAL_ROMAN,
  T_ORDINAL_ROMAN,
  T_NOT_ROMAN
} ;
enum NumericType
{
    T_NULL_NUM = 0,
    T_INTEGER,
    T_COMMA_NUMBER,
    T_DOT_NUMBER,
    T_FRACTION,
    T_ORDINAL_INTEGER 
};

enum StatusType
{
    T_NULL_STATUS = 0,
    T_ALPHA,
    T_NUMERIC,
    T_ALPHANUMERIC,
    T_PATTERN,
    T_WORD_BRK,
    T_SENTENCE_BRK,
    T_PARAGRAPH_BRK
};

class TStatusPrivate;
class LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT TStatus
{
  friend class TStatusPrivate;

public:
    TStatus();
    TStatus(const TStatus&);
    TStatus(
        AlphaCapitalType capital,
        AlphaRomanType roman,
        bool isHyphen,
        bool isPossessive,
        bool isConcatAbbrev,
        bool isTwitter,
        NumericType numeric,
        StatusType status);
    virtual ~TStatus();

    // Assignment operator. Copies the specified TStatus
    // into self. Returns a reference to self.
    TStatus& operator=(const TStatus&);

    // comparison operator (so that status can be sorted)
    bool operator<(const TStatus& s) const;
    bool operator==(const TStatus& s) const;

    // reset Status values
    void reset();

    void setAlphaCapital(const AlphaCapitalType alphaCapital);
    void setAlphaRoman(const AlphaRomanType alphaRoman);
    void setAlphaHyphen(bool isAlphaHyphen);
    void setAlphaPossessive(bool isAlphaPossessive);
    void setAlphaConcatAbbrev(bool concatAbbrev);
    void setTwitter(bool isTwitter);
    void setNumeric(const NumericType numeric);
    void setStatus(const StatusType other);
    void setDefaultKey(const Lima::LimaString& defaultKey);
    AlphaCapitalType getAlphaCapital() const;
    AlphaRomanType getAlphaRoman() const;
    bool isAlphaHyphen() const;
    bool isAlphaPossessive() const;
    bool isAlphaConcatAbbrev() const;
    bool isTwitter() const;
    NumericType getNumeric() const;
    StatusType getStatus() const;
    const Lima::LimaString& defaultKey() const;
    void outputXML(std::ostream& XmlStream) const;
    std::string toString() const;
    
protected:
  TStatusPrivate* m_d;
  
};


} // closing namespace LinguisticAnalysisStructure
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_LINGUISTICPROCESSING_DATA_TSTATUS_H
