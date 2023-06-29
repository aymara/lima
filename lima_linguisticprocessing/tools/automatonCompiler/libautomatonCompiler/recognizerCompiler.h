// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
*
* File        : recognizerCompiler.h
* Project     : Named Entities
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Mon Apr  7 2003
* Copyright   : (c) 2003 by CEA
* Version     : $Id: recognizerCompiler.h 9081 2008-02-25 18:34:51Z de-chalendarg $
*
* Description : a namespace for the compilation of a recognizer based on a text file
*
************************************************************************/

#ifndef RECOGNIZERCOMPILER_H
#define RECOGNIZERCOMPILER_H

#include "AutomatonCompilerExport.h"
#include "linguisticProcessing/core/Automaton/recognizer.h"
#include "gazeteer.h"
#include "subAutomaton.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATONCOMPILER_EXPORT RecognizerCompiler
{
 public:
  RecognizerCompiler(const std::string& filename); 
  ~RecognizerCompiler();
  
  /** 
   * encoding of the recognizer compiler (encoding of the file 
   * containing the rules)
   * 
   * @param encoding the encoding to set (if not set, is "latin1",
   * can be set to "utf8", other encodings are ignored)
   */
  static void setRecognizerEncoding(const std::string& encoding) {
    m_recognizerEncoding = encoding;
  }
 
  /** 
   * read a line from a text file into a LimaString, according
   * to the encoding set by setRecognizerEncoding() function
   * 
   * @param file the file to read from
   * @param line the line read
   */
  void readline(LimaString& line);

  int getLineNumber() const { return m_lineNumber; }
  const std::string& getFilename() const { return m_filename; }
  
  /** 
   * build a recognizer from a file
   * 
   * @param reco the recognizer build
   * @param filename the name of the text file containing the rules
   */
  void buildRecognizer(Recognizer& reco, 
                       MediaId language,
                       const std::string& filename="");

  bool endOfFile() { return m_stream->eof(); }

  /** 
   * function to log error properly (filename, line number...)
   * and throw exception
   * 
   * @param error 
   * @param ruleString 
   */
  void throwError(const std::string& error,
                  const LimaString& line=LimaString()) const;

  void printWarning(const std::string& error,
                    const LimaString& line=LimaString()) const;


 private:

  static std::string m_recognizerEncoding;
  LimaString m_defaultAction;
  std::vector<LimaString> m_activeEntityGroups;
  int m_lineNumber; // just kept for better error identification
  std::string m_filename;    // just kept for better error identification
  LimaString m_currentLine; // just kept for better error identification
  std::ifstream* m_stream;
  uint64_t m_nbRule;
  
  // private functions
  void expandGazeteersInRule(LimaString& s, 
                             const std::vector<Gazeteer>& gazeteers);
  void expandSubAutomatonsInRule(LimaString& s,
                                 const std::vector<SubAutomaton>& subAutomatons);
  LimaString peekConstraints(std::ifstream& file);
  void parseTypeDefinition(const LimaString& str, 
                           int& offset,
                           std::string& typeName,
                           std::string& openingTag,
                           std::string& closingTag,
                           std::vector<std::string>& attributes);
  std::string nextFieldTypeDefinition(const LimaString& str, 
                                      int& offset);

  void readSubAutomaton(const LimaString& line,
                        const std::vector<Gazeteer>& gazeteers,
                        std::vector<SubAutomaton>& subAutomatons);
  void readGazeteers(const std::string& filename,
                     std::vector<Gazeteer>& gazeteers,
                     const std::vector<SubAutomaton>& subAutomatons);

  void searchFile(std::string& filename);

  void addRuleWithGazeteerTrigger(const LimaString& gazeteerName,
                                  LimaString& ruleString,
                                  const std::vector<Gazeteer>& gazeteers,
                                  const std::vector<SubAutomaton>& subAutomatons,
                                  const LimaString& defaultAction,
                                  Recognizer& reco,
                                  MediaId language,const std::string& currentId,
                                  const bool keepTrigger=true,
                                  const bool headTrigger=false);

  double currentRuleWeight();

//   bool checkRule(const Rule& rule,
//                  const TransitionUnit* trigger,
//                  MediaId language,
//                  std::ostringstream& message) const;
  
};

} // end namespace
} // end namespace
} // end namespace

#endif
