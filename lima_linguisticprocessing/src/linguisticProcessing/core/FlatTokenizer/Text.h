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
// jys 24-JUL-2002
//
// Text is the class which reads original text and does its
// 1st transformation into characters classes string.

#ifndef LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_TEXT_H
#define LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_TEXT_H


#include "FlatTokenizerExport.h"
#include "common/Data/LimaString.h"
#include "common/misc/Exceptions.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/TStatus.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include <wchar.h>

#include "CharChart.h"


namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{

class LIMA_FLATTOKENIZER_EXPORT Text
{

public:
  Text(MediaId lang, const CharChart* charChart);
  virtual ~Text();

  void setText(const Lima::LimaString& text);
  
  // Clear the entirely class and structure to accept new text
  void clear();

  // set graph in which insert token
  void setGraph(LinguisticGraphVertex position,LinguisticGraph* graph);
  void finalizeAndUnsetGraph();

  // gives the current character class
  const CharClass* currentClass() const;

  // gives the current character
  inline Lima::LimaChar currentChar() const;

  //
  // returns the character class with signed offset i
  Lima::LimaChar operator[] (int i) const;

  // increments text pointer
  Lima::LimaChar advance();

  // flushes current token
  void flush();

  // takes a token and add it to the result graph
  Lima::LimaString token();

  // performs a trace
  void trace();

  // sets token status
  void setAlphaCapital(const LinguisticAnalysisStructure::AlphaCapitalType alphaCapital);
  void setAlphaRoman(const LinguisticAnalysisStructure::AlphaRomanType alphaRoman);
  void setAlphaHyphen(const unsigned char isAlphaHyphen);
  void setAlphaPossessive(const unsigned char isAlphaPossessive);
  void setAlphaConcatAbbrev(const unsigned char isConcatAbbreviation);
  void setTwitter(const unsigned char isTwitter);
  void setNumeric(const LinguisticAnalysisStructure::NumericType numeric);
  void setStatus(const LinguisticAnalysisStructure::StatusType status);
  void setDefaultKey(const Lima::LimaString& defaultKey);

  inline int position() const {return _curPtr;}
  inline int size() const {return m_text.size();}

  inline void setStatus(const LinguisticAnalysisStructure::TStatus& status) {_curSettings = status;}

 void computeDefaultStatus();

private:
  Lima::LimaString m_text;
  int _curPtr;
  int _debPtr;
  LinguisticAnalysisStructure::TStatus _curSettings;
  LinguisticGraph* _tTokenGraph;
  LinguisticGraphVertex _currentVx;
  LinguisticGraphVertex _lastVx;
  bool _isDone;
  bool _thereIsUnknown;

  FsaStringsPool* _stringsPool;
  const CharChart* m_charChart;
};

inline Lima::LimaChar Text::currentChar() const
{
  if (_curPtr >= m_text.size())
  {
    return 0;
  }
  return m_text[_curPtr];
}

} //namespace FlatTokenizer
} // namespace LinguisticProcessing
} // namespace Lima

#endif
