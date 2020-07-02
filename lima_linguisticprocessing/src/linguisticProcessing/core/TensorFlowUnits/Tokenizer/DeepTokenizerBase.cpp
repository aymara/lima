/*
    Copyright 2002-2020 CEA LIST

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

#include "DeepTokenizerBase.h"

#include <QtCore/QRegularExpression>

#include "common/Data/strwstrtools.h"

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::Misc;

namespace Lima
{
namespace LinguisticProcessing
{
namespace TensorFlowUnits
{
namespace Tokenizer
{

// set default key in status according to other elements in status
void DeepTokenizerBase::computeDefaultStatus(TStatus& curSettings)
{
  string defaultKey;
  switch (curSettings.getStatus()) {
    case T_ALPHA : {
      switch (curSettings.getAlphaCapital()) {
        case T_CAPITAL       : defaultKey = "t_capital"      ; break;
        case T_SMALL         : defaultKey = "t_small"        ; break;
        case T_CAPITAL_1ST   : defaultKey = "t_capital_1st"  ; break;
        case T_ACRONYM       : defaultKey = "t_acronym"      ; break;
        case T_CAPITAL_SMALL : defaultKey = "t_capital_small"; break;
        case T_ABBREV        : defaultKey = "t_abbrev"       ; break;
        default : break;
      }
      switch (curSettings.getAlphaRoman()) { // Roman supersedes Cardinal
        case T_CARDINAL_ROMAN : defaultKey = "t_cardinal_roman"; break;
        case T_ORDINAL_ROMAN  : defaultKey = "t_ordinal_roman" ; break;
        case T_NOT_ROMAN      : defaultKey = "t_not_roman"     ; break;
        default : break;
      }
      if (curSettings.isAlphaHyphen()) {
        //no change
        //defaultKey = "t_alpha_hyphen";
      }
      if (curSettings.isAlphaPossessive()) {
        defaultKey = "t_alpha_possessive";
      }
      break;
    } // end T_ALPHA
    case T_NUMERIC : {
      switch (curSettings.getNumeric()) {
        case T_INTEGER         : defaultKey = "t_integer"       ; break;
        case T_COMMA_NUMBER    : defaultKey = "t_comma_number"  ; break;
        case T_DOT_NUMBER      : defaultKey = "t_dot_number"    ; break;
        case T_FRACTION        : defaultKey = "t_fraction"      ; break;
        case T_ORDINAL_INTEGER : defaultKey = "t_ordinal_integer"; break;
        default: break;
      }
      break;
    }
    case T_ALPHANUMERIC : defaultKey = "t_alphanumeric" ; break;
    case T_PATTERN      : defaultKey = "t_pattern"      ; break;
    case T_WORD_BRK     : defaultKey = "t_word_brk"     ; break;
    case T_SENTENCE_BRK : defaultKey = "t_sentence_brk" ; break;
    case T_PARAGRAPH_BRK: defaultKey = "t_paragraph_brk" ; break;
    default: defaultKey = "t_fallback";
  }
// #ifdef DEBUG_LP
//   TOKENIZERLOGINIT;
//   LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus" << defaultKey;
// #endif
  curSettings.setDefaultKey(utf8stdstring2limastring(defaultKey));
}

// set default key in status according to other elements in status
void DeepTokenizerBase::computeDefaultStatus(Token& token)
{
// #ifdef DEBUG_LP
//   TOKENIZERLOGINIT;
//   LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus"
//           << token.stringForm();
// #endif
  static QRegularExpression reCapital("^[[:upper:]]+$", QRegularExpression::UseUnicodePropertiesOption);
  static QRegularExpression reSmall("^[[:lower:]]+$", QRegularExpression::UseUnicodePropertiesOption);
  static QRegularExpression reCapital1st("^[[:upper:]]\\w+$", QRegularExpression::UseUnicodePropertiesOption);
  static QRegularExpression reAcronym("^([[:upper:]]\\.)+$", QRegularExpression::UseUnicodePropertiesOption);
  static QRegularExpression reCapitalSmall("^([[:upper:][:lower:]])+$", QRegularExpression::UseUnicodePropertiesOption);
  static QRegularExpression reAbbrev("^\\w+\\.$", QRegularExpression::UseUnicodePropertiesOption);
  static QRegularExpression reTwitter("^[@#]\\w+$", QRegularExpression::UseUnicodePropertiesOption);
  static QRegularExpression reAlphaHyphen("^\\w+[\\-]\\w+$", QRegularExpression::UseUnicodePropertiesOption);

  // t_cardinal_roman
  static QRegularExpression reCardinalRoman("^(?=[MDCLXVI])M*(C[MD]|D?C{0,3})(X[CL]|L?X{0,3})(I[XV]|V?I{0,3})$");
  // t_ordinal_roman
  static QRegularExpression reOrdinalRoman("^(?=[MDCLXVI])M*(C[MD]|D?C{0,3})(X[CL]|L?X{0,3})(I[XV]|V?I{0,3})(st|nd|d|th|er|ème)$");
  // t_integer
  static QRegularExpression reInteger("^\\d+$");
  // t_comma_number
  static QRegularExpression reCommaNumber("^\\d+,\\d$");
  // t_dot_number
  static QRegularExpression reDotNumber("^\\d\\.\\d$");
  // t_fraction
  static QRegularExpression reFraction("^\\d([.,]\\d+)?/\\d([.,]\\d+)?$");
  // t_ordinal_integer
  static QRegularExpression reOrdinalInteger("^\\d+(st|nd|d|th|er|ème)$");
  // t_alphanumeric
  static QRegularExpression reAlphanumeric("^[\\d[:lower:][:upper:]]+$", QRegularExpression::UseUnicodePropertiesOption);
  static QRegularExpression reSentenceBreak("^[;.!?]$");

  TStatus curSettings;
  if (reCapital.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_capital";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_capital"));
  }
  else if (reSmall.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_small";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_small"));
  }
  else if (reCapital1st.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_capital_1st";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_capital_1st"));
  }
  else if (reAcronym.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_acronym";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_acronym"));
  }
  else if (reCapitalSmall.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_capital_small";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_capital_small"));
  }
  else if (reAbbrev.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_abbrev";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_abbrev"));
  }
  else if (reTwitter.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_twitter";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_twitter"));
  }
  else if (reCardinalRoman.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_cardinal_roman";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_cardinal_roman"));
  }
  else if (reOrdinalRoman.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_ordinal_roman";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_ordinal_roman"));
  }
  else if (reInteger.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_integer";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_integer"));
  }
  else if (reCommaNumber.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_comma_number";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_comma_number"));
  }
  else if (reDotNumber.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_dot_number";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_dot_number"));
  }
  else if (reFraction.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_fraction";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_fraction"));
  }
  else if (reOrdinalInteger.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_ordinal_integer";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_ordinal_integer"));
  }
  else if (reAlphanumeric.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_alphanumeric";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_alphanumeric"));
  }
  else if (reAlphaHyphen.match(token.stringForm()).hasMatch())
  {
    curSettings.setDefaultKey(QString::fromUtf8("t_alpha_hyphen"));
    curSettings.setAlphaHyphen(true);
  }
  else if (reSentenceBreak.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_sentence_brk";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_sentence_brk"));
  }
  else // if (reSmall.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppUppsalaTokenizerPrivate::computeDefaultStatus t_word_brk (default)";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_word_brk"));
  }

  /*
  // t_not_roman
  static QRegularExpression reNotRoman("^$");
  // t_alpha_concat_abbrev
  static QRegularExpression reAlphConcatAbbrev("^$");
  // t_pattern
  static QRegularExpression rePattern("^$");
  // t_word_brk
  static QRegularExpression reWordBreak("^$");
  // t_sentence_brk
  static QRegularExpression reSentenceBreak("^$");
  //  t_paragraph_brk
  static QRegularExpression reParagraphBreak("^$");
  */

  token.setStatus(curSettings);
}

} // namespace Tokenizer
} // namespace TensorFlowUnits
} // namespace LinguisticProcessing
} // namespace Lima
