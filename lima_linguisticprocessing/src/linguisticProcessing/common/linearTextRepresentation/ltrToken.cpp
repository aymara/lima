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

/** =======================================================================
    @file       ltrToken.cpp

    @version    $Id$
    @date       created       jul 6, 2004
    @date       last revised  nov 24, 2010

    @author     Olivier Ferret
    @brief      a token of a linear text representation

    Copyright (C) 2004-2010 by CEA LIST

    ======================================================================= */

#include "ltrToken.h"

#include "common/Data/readwritetools.h"
#include "common/BagOfWords/bowBinaryReaderWriter.h"


using namespace Lima::Common::Misc;
using namespace std;

namespace Lima {
namespace Common {
namespace BagOfWords {


/**
  * @class  LTR_Token
  */

// -----------------------------------------------------------------------------
// -- destructor
// -----------------------------------------------------------------------------

LTR_Token::~LTR_Token() {
}


// -----------------------------------------------------------------------------
// -- copying
// -----------------------------------------------------------------------------

LTR_Token::LTR_Token(const LTR_Token& tok):
vector<pair<QSharedPointer< BoWToken >, bool> >()
{

    for (LTR_Token::const_iterator itBow = tok.begin();
         itBow != tok.end(); itBow ++) {
        this->push_back(make_pair(QSharedPointer< BoWToken >(new BoWToken(*(itBow->first))), itBow->second));
    }
}


// -----------------------------------------------------------------------------
// -- reading/writing
// -----------------------------------------------------------------------------

void LTR_Token::binaryWriteOn(std::ostream& os) const {

    BoWBinaryWriter writer;
    writeCodedInt(os, this->size());
    for (LTR_Token::const_iterator itTok = this->begin();
         itTok != this->end(); itTok ++) {
         writer.writeSimpleToken(os, itTok->first);
        if (itTok->second) {
            writeCodedInt(os, 1);
        }
        else {
            writeCodedInt(os, 0);
        }
    }
}

void LTR_Token::binaryReadFrom(std::istream& is) {

    BoWBinaryReader reader;
    uint64_t tokenNb = readCodedInt(is);
    for (uint64_t i = 1; i <= tokenNb; i ++) {
        QSharedPointer< BoWToken > bowTok(new BoWToken());
        reader.readSimpleToken(is, bowTok);
        uint64_t selectionFlag = readCodedInt(is);
        this->push_back(make_pair(bowTok, selectionFlag != 0));
    }
}


// -----------------------------------------------------------------------------
// -- printing
// -----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const LTR_Token& tok) {
  
  LTR_Token::size_type alternativesNb = tok.size();
  LTR_Token::const_iterator itTok = tok.begin();
  for (LTR_Token::size_type alt = 0; alt < alternativesNb - 1; ++ alt) {
    os << *(itTok->first);
    if (itTok->second) os << "*";
    os << " | ";
    ++ itTok;
  }
  os << *(itTok->first);
  if (itTok->second) os << "*";
  return os;
}

QDebug& operator<<(QDebug& os, const LTR_Token& tok) {
  
  LTR_Token::size_type alternativesNb = tok.size();
  LTR_Token::const_iterator itTok = tok.begin();
  for (LTR_Token::size_type alt = 0; alt < alternativesNb - 1; ++ alt) {
    os << *(itTok->first);
    if (itTok->second) os << "*";
    os << " | ";
    ++ itTok;
  }
  os << *(itTok->first);
  if (itTok->second) os << "*";
  return os;
}


}  // BagOfWords
}  // Common
}  // Lima
