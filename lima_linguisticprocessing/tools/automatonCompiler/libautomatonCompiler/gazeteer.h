// Copyright 2002-2018 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/******************************************************************************
*
* File        : gazeteer.h
* Project     : DETECT - NamedEntities
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Thu Oct 24 2002
* Copyright   : (c) 2002 by CEA
* Version     : $Id: gazeteer.h 2799 2006-03-07 16:22:12Z romaric $
*
* Description : a class for the definition of gazeteers
*
******************************************************************************/

#ifndef GAZETEER_H
#define GAZETEER_H

#include "automatonString.h"
#include <iostream>
#include <string>
#include <vector>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class RecognizerCompiler;

class Gazeteer : public std::vector<LimaString>
{
 public:
  Gazeteer();
  Gazeteer(const Gazeteer&);
  ~Gazeteer();
  Gazeteer& operator = (const Gazeteer&);

  friend std::ostream& operator << (std::ostream&, const Gazeteer&);
  friend QDebug& operator << (QDebug&, const Gazeteer&);
  bool operator==(const LimaString& alias) const { return m_alias==alias; }

  uint64_t numberOfWords() const;
  const LimaString& alias() const;
  LimaString stringAutomaton(const LimaString& constraint=LimaString()) const;
  const AutomatonString& getAutomatonString() const { return m_automatonString; }

  void setAlias(const LimaString&);
  void addWord(const LimaString&, const vector<Gazeteer>& otherGazeteers);
  // TODO: unused operation, to be deleted?
  Gazeteer& add(const Gazeteer&);
  void buildAutomatonString(const std::vector<Gazeteer>& gazeteers,
                            const std::vector<SubAutomaton>& subAutomatons);

  void readFromFile(const std::string& filename);
  void read(RecognizerCompiler& reco);
  LimaString readName(RecognizerCompiler& reco);
  void readValues(RecognizerCompiler& reco,
                  const LimaString& stringBegin=LimaString());
  void readValues(RecognizerCompiler& reco,
                  const vector<Gazeteer>& otherGazeteers,
                  const LimaString& stringBegin=LimaString());
  bool hasMultiTermWord() const { return m_hasMultiTermWord; }
  bool hasOnlyWords() const { return m_hasOnlyWords; }
  void setHasMultiTermWordFlag() { m_hasMultiTermWord = true; }

 private:
  LimaString m_alias;
  bool m_hasMultiTermWord;
  bool m_hasOnlyWords;
  AutomatonString m_automatonString;
};

/***********************************************************************/
// inline access functions
/***********************************************************************/
inline uint64_t Gazeteer::numberOfWords() const { return size(); }
inline const LimaString& Gazeteer::alias() const { return m_alias; }
inline void Gazeteer::setAlias(const LimaString& a) { m_alias = a; }


} // end namespace
} // end namespace
} // end namespace

#endif
