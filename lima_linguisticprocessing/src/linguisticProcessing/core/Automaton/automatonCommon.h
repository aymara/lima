// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       automatonCommon.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Tue Mar 22 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * Project     Automaton
 * 
 * @brief      common types and objects for automaton
 * 
 * 
 ***********************************************************************/

#ifndef AUTOMATONCOMMON_H
#define AUTOMATONCOMMON_H

#include "AutomatonExport.h"
#include "common/Data/LimaString.h"
#include "common/Data/readwritetools.h"
#include "common/Data/strwstrtools.h"
#include "common/misc/fsaStringsPool.h"
#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

//----------------------------------------------------------------------
// type definitions
typedef uint64_t Tstate;
typedef StringsPoolIndex Tword; // use string pool

// Part of speech: Macro and Micro are coded on the same LinguisticCode
typedef LinguisticCode Tpos;

//----------------------------------------------------------------------
// utility functions for defined types
inline Tstate readTstate(std::ifstream& file) { return Common::Misc::readCodedInt(file); }
inline void writeTstate(std::ofstream& file, const Tstate s) { Common::Misc::writeCodedInt(file,s); }

// reading and writing Tword in binary format
void readTword(std::ifstream& file, Tword& s,FsaStringsPool& sp);
void writeTword(std::ofstream& file,const Tword& s,const FsaStringsPool& sp);

// std::ostream& operator << (std::ostream&, const Tpos&);

// reading and writing Tpos in binary format
void readTpos(std::ifstream&, Tpos&);
void writeTpos(std::ofstream&,const Tpos&);

// reading and writing set of words (for gazeteer)
void readWordVector(std::ifstream& file, std::vector<LimaString>& wordVector);
void writeWordSet(std::ofstream& file,const std::set<LimaString>& wordSet);

//comparing the part-of-speech type with a LingPropertyEntry
// should take a const LingPropertyEntry& argument
// check with JYS
bool compareTpos(const Tpos&, const LinguisticCode& lingProperty,
                 const Common::PropertyCode::PropertyAccessor& macroAccessor,
                 const Common::PropertyCode::PropertyAccessor& microAccessor);

//----------------------------------------------------------------------
// exceptions
class OpenFileException : public std::runtime_error {
 public: OpenFileException(const std::string& mes):std::runtime_error(mes){
 };
};
class UnknownTypeException : public std::runtime_error {
 public: UnknownTypeException(const std::string& mes):
 std::runtime_error(mes){ }
};

//----------------------------------------------------------------------
// utility conversion function from limastring to int
LIMA_AUTOMATON_EXPORT uint64_t LimaStringToInt(LimaString s);

} // end namespace
} // end namespace
} // end namespace

#endif
