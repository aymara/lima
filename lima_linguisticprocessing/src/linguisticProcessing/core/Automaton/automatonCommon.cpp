// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       automatonCommon.cpp
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Tue Mar 22 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2020 by CEA LIST
 *
 ***********************************************************************/

#include "automatonCommon.h"
#include "common/MediaticData/mediaticData.h"

#include <cmath>

using namespace Lima::Common;

namespace Lima
{
namespace LinguisticProcessing
{
namespace Automaton
{

// utility function for conversion of limastring to int
uint64_t LimaStringToInt(LimaString s)
{
  return s.toUInt();
}

//----------------------------------------------------------------------
// Tword type
void readTword(std::ifstream& file, Tword& s,FsaStringsPool& sp)
{
  LimaString str;
  Misc::readUTF8StringField(file,str);
  s=sp[str];
}
void writeTword(std::ofstream& file,const Tword& s,const FsaStringsPool& sp)
{
  Misc::writeUTF8StringField(file,sp[s]);
}

// LimaString type
void readLimaString(std::ifstream& file, LimaString& s)
{
  Misc::readUTF8StringField(file,s);
}
void writeLimaString(std::ofstream& file,const LimaString& s)
{
  Misc::writeUTF8StringField(file,s);
}

// wordSet = set of multi-term
void readWordVector(std::ifstream& file, std::vector<LimaString>& wordVector)
{
  auto i = Misc::readCodedInt(file);
  for( ; i > 0 ; i-- ) {
    LimaString s;
    Misc::readUTF8StringField(file,s);
    wordVector.push_back(s);
  }
}

void writeWordSet(std::ofstream& file,const std::set<LimaString>& wordSet)
{
  int i = wordSet.size();
  Misc::writeCodedInt(file,i);
  std::set<LimaString>::const_iterator wordIt =  wordSet.begin();
  for( ; wordIt !=  wordSet.end() ; wordIt++ ) {
    Misc::writeUTF8StringField(file,*wordIt);
  }
}

//----------------------------------------------------------------------
// Part-of-speech type
void readTpos(std::ifstream& file, Tpos& p)
{
  p = LinguisticCode::decodeFromBinary(file);
}
void writeTpos(std::ofstream& file, const Tpos& p)
{
  LinguisticCode::encodeToBinary(file, p);
}

bool compareTpos(const Tpos& pos,  const LinguisticCode& lingProperty,
                 const PropertyCode::PropertyAccessor& macroAccessor,
                 const PropertyCode::PropertyAccessor& microAccessor)
{
  return ( (microAccessor.equal(lingProperty,pos)) ||
           (microAccessor.empty(pos) &&
            macroAccessor.equal(lingProperty,pos)));
}

} // end namespace
} // end namespace
} // end namespace
