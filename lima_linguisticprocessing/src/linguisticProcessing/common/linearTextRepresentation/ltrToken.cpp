// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
vector<pair<boost::shared_ptr< BoWToken >, bool> >()
{

    for (LTR_Token::const_iterator itBow = tok.begin();
         itBow != tok.end(); itBow ++) {
        this->push_back(make_pair(boost::shared_ptr< BoWToken >(new BoWToken(*(itBow->first))), itBow->second));
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
        boost::shared_ptr< BoWToken > bowTok(new BoWToken());
        reader.readSimpleToken(is, bowTok);
        uint64_t selectionFlag = readCodedInt(is);
        this->push_back(make_pair(bowTok, selectionFlag != 0));
    }
}


// -----------------------------------------------------------------------------
// -- printing
// -----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const LTR_Token& tok) 
{
  bool first=true;
  for (const auto& t: tok) {
    if (first) { first=false; } else { os << " | "; }
    if (t.first==nullptr) { os << "NULL"; } else { os << *(t.first); }
    if (t.second) os << "*";
  }
  return os;
}

QDebug& operator<<(QDebug& os, const LTR_Token& tok) 
{
  bool first=true;
  for (const auto& t: tok) {
    if (first) { first=false; } else { os << " | "; }
    if (t.first==nullptr) { os << "NULL"; } else { os << *(t.first); }
    if (t.second) os << "*";
  }
  return os;
}


}  // BagOfWords
}  // Common
}  // Lima
