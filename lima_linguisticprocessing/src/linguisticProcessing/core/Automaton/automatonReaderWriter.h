// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       automatonReader.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Wed Mar 16 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * Project     Automaton
 * 
 * @brief      
 * 
 ***********************************************************************/

#ifndef AUTOMATONREADER_H
#define AUTOMATONREADER_H

#include "AutomatonExport.h"
#include "common/LimaCommon.h"
#include "common/MediaticData/EntityType.h"
#include <iostream>
#include <map>
#include <fstream>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

// classes defined external
class Recognizer;
class Rule;
class Automaton;
class TransitionUnit;
class Constraint;
class ConstraintFunction;

class LIMA_AUTOMATON_EXPORT AutomatonReader
{
 public:
  AutomatonReader();
  ~AutomatonReader();

  void readRecognizer(const std::string& filename,
                      Recognizer& reco);
  
  void readRecognizer(std::ifstream& file,
                      Recognizer& reco);
  Rule* readRule(std::ifstream& file,MediaId language);
  void readAutomaton(std::ifstream& file,
                     Automaton& a,
                     MediaId language);
  void readConstraint(std::ifstream& file,
                      Constraint& c);
  TransitionUnit* readTransitionUnit(std::ifstream& file,MediaId language);
  void readRegisteredConstraints(std::ifstream& file,
                                 MediaId language);
  MediaId readHeader(std::ifstream& file);
  
 private:
  std::map<uint64_t,ConstraintFunction*> m_constraintFunctionMap;
  std::map<Common::MediaticData::EntityGroupId,Common::MediaticData::EntityGroupId> m_entityGroupMapping;
  std::map<Common::MediaticData::EntityType,Common::MediaticData::EntityType> m_entityTypeMapping;
  bool m_debug;
};

class LIMA_AUTOMATON_EXPORT AutomatonWriter
{
 public:
  AutomatonWriter();
  ~AutomatonWriter();

  void writeRecognizer(const Recognizer& reco,
                       const std::string& filename,
                       MediaId language,
                       bool debug=false);

  void writeRecognizer(std::ofstream& file,
                       const Recognizer& reco,
                       MediaId language,
                       bool debug=false);
                       
  void writeRule(std::ofstream& file,
                 const Rule& rule,
                 MediaId language);
  void writeAutomaton(std::ofstream& file,
                      const Automaton& automaton,
                      MediaId language);
  void writeTransitionUnit(std::ofstream& file,
                           TransitionUnit* transition,
                           MediaId language);
  void writeConstraint(std::ofstream& file,
                       const Constraint& c);
  void writeRegisteredConstraints(std::ofstream& file);
  void writeHeader(std::ofstream& file,MediaId lang);
  
 private:
  std::map<ConstraintFunction*,uint64_t> m_constraintFunctionMap;
  bool m_debug;
};

} // end namespace
} // end namespace
} // end namespace

#endif
