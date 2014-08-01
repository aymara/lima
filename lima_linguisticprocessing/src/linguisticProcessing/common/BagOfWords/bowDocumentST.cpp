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
/************************************************************************
 *
 * @file       boWDocumentST.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Fri Oct 10 2003
 * copyright   Copyright (C) 2003 by CEA LIST
 *
 ***********************************************************************/

#include "bowDocumentST.h"
#include "bowToken.h"
#include "common/LimaCommon.h"
#include "common/Data/readwritetools.h"

#include <vector>
#include <map>

namespace Lima {
namespace Common {
namespace BagOfWords {

//***********************************************************************
// constructors
//***********************************************************************
BoWDocumentST::BoWDocumentST():
BoWDocument(),
m_sentenceBreaks(),
m_topicShifts()
{
}

BoWDocumentST::BoWDocumentST(const BoWDocumentST& d):
BoWDocument(d),
m_sentenceBreaks(),
m_topicShifts()
{
  copySTData(d);
}


//***********************************************************************
// destructor
//***********************************************************************
BoWDocumentST::~BoWDocumentST() {
  m_sentenceBreaks.clear();
  m_topicShifts.clear();
}

//***********************************************************************
// assignment operator
//***********************************************************************
BoWDocumentST& BoWDocumentST::operator = (const BoWDocumentST& d) {
  if (&d != this) {
    BoWText::operator=(d);
    m_sentenceBreaks.clear();
    m_topicShifts.clear();
    copySTData(d);
  }
  return *this;
}

//***********************************************************************
// member functions
//***********************************************************************
void BoWDocumentST::copySTData(const BoWDocumentST& d) {
  // first build correspondance between iterators and indexes
  std::map<BoWText::const_iterator, BoWText::const_iterator> iteratorMap;
  BoWText::const_iterator thisIt=this->begin();
  BoWText::const_iterator it = d.begin();
  while (it != d.end() && thisIt != this->end()) {
    iteratorMap[it] = thisIt;
    it++;
    thisIt++; 
  }
  if (thisIt != this->end() || it != d.end()) {
    throw std::runtime_error("cannot build iterator map in BoWDocumentST copy");
  }

  // copy sentenceBreaks and topicShifts;
  for (std::vector<BoWText::const_iterator>::const_iterator 
         it=d.m_sentenceBreaks.begin(); it!=d.m_sentenceBreaks.end(); it++) {
    m_sentenceBreaks.push_back(iteratorMap[*it]);
  }
  for (std::vector<BoWText::const_iterator>::const_iterator 
         it=d.m_topicShifts.begin(); it!=d.m_topicShifts.end(); it++) {
    m_topicShifts.push_back(iteratorMap[*it]);
  }
}

void BoWDocumentST::reinit() {
  BoWDocument::reinit();
  m_sentenceBreaks.clear();
  m_topicShifts.clear();
}

//***********************************************************************
// text input/output
//***********************************************************************
/*
std::wostream& operator << (std::wostream& os, const BoWDocumentST& d) {
      // dump BoWDocument part
    os << static_cast<BoWDocument>(d);
      // dump sentence breaks
    os << L"## sentence breaks" << std::endl;
    std::vector<BoWText::const_iterator>::const_iterator itSentBrk;
    for (itSentBrk = d.m_sentenceBreaks.begin();
         itSentBrk != d.m_sentenceBreaks.end(); itSentBrk ++) {
        os << ***itSentBrk << std::endl;
    }

      // dump topic shifts
    os << L"## topic shifts" << std::endl;
//if (d.m_topicShifts.empty()) {
//    os << L"no topic shifts" << std::endl;
//}
    std::vector<BoWText::const_iterator>::const_iterator itTopSht;
    for (itTopSht = d.m_topicShifts.begin();
         itTopSht != d.m_topicShifts.end(); itTopSht ++) {
        os << ***itTopSht << std::endl;
    }

    return os;
}
*/

//!!!! ugly cut'n'paste !!!!

std::ostream& operator << (std::ostream& os, const BoWDocumentST& d) {

      // dump BoWDocument part
    os << static_cast<BoWDocument>(d);

      // dump sentence breaks
    os << "## sentence breaks" << std::endl;
    std::vector<BoWText::const_iterator>::const_iterator itSentBrk;
    for (itSentBrk = d.m_sentenceBreaks.begin();
         itSentBrk != d.m_sentenceBreaks.end(); itSentBrk ++) {
        os << (**itSentBrk)->getOutputUTF8String() << std::endl;
    }

      // dump topic shifts
    os << "## topic shifts" << std::endl;
//if (d.m_topicShifts.empty()) {
//    os << L"no topic shifts" << std::endl;
//}
    std::vector<BoWText::const_iterator>::const_iterator itTopSht;
    for (itTopSht = d.m_topicShifts.begin();
         itTopSht != d.m_topicShifts.end(); itTopSht ++) {
        os << (**itTopSht)->getOutputUTF8String() << std::endl;
    }

    return os;

}


//***********************************************************************
// binary input/output
//***********************************************************************

void BoWDocumentST::read(std::istream& file) {

      // a BoWDocument is a BoWDocument with more data; hence, first
      // read the BoWDocument part
    BoWDocument::read(file);

      // build a dictionary for mapping numeric indexes in file
      // and BoWTokens
      // certainly not the most efficient way to do it
    std::map<uint64_t, BoWText::const_iterator> indexToIterator;
    uint64_t tokenCounter = 1;
    for (BoWText::const_iterator itTok = this->begin();
         itTok != this->end(); itTok ++) {
        indexToIterator[tokenCounter] = itTok;
        tokenCounter ++;
    }

      // sentence breaks
      //  read the number of sentence breaks
    uint64_t sentBrkNb = Misc::readCodedInt(file);
      //  read sentence break indexes and map them to BoWTokens
    for (uint64_t sentBrkInd = 1; sentBrkInd <= sentBrkNb; sentBrkInd ++) {
        uint64_t sentBrkIndex = Misc::readCodedInt(file);
        std::map<uint64_t, BoWText::const_iterator>::const_iterator itMap =
            indexToIterator.find(sentBrkIndex);
        if (itMap != indexToIterator.end()) {
            m_sentenceBreaks.push_back(itMap->second);
        }
        else {
            throw std::runtime_error("invalid sentence break reference\n");
        }
    }

      // read topic shifts
      //  read the number of topic shifts
    uint64_t topShtNb = Misc::readCodedInt(file);
      //  read topic shift indexes and map them to BoWTokens
    for (uint64_t topShtInd = 1; topShtInd <= topShtNb; topShtInd ++) {
        uint64_t topShtIndex = Misc::readCodedInt(file);
        std::map<uint64_t, BoWText::const_iterator>::const_iterator itMap =
            indexToIterator.find(topShtIndex);
        if (itMap != indexToIterator.end()) {
            m_topicShifts.push_back(itMap->second);
        }
        else {
          throw std::runtime_error("invalid topic shift reference\n");
        }
    }

}

void BoWDocumentST::write(std::ostream& file) const {

      // a BoWDocument is a BoWDocument with more data; hence, first
      // write the BoWDocument part
    BoWDocument::write(file);

      // write BoWDocument specific data
    this->writeSTData(file);

}


void BoWDocumentST::writeSTData(std::ostream& file) const
{

      // convert references to tokens into numeric indexes
    std::vector<uint64_t> sentenceBreaks;
    std::vector<uint64_t> topicShifts;
    uint64_t tokenCounter = 1;
    std::vector<BoWText::const_iterator>::const_iterator itSentBrk = m_sentenceBreaks.begin();
    std::vector<BoWText::const_iterator>::const_iterator itTopSht = m_topicShifts.begin();
    for (BoWText::const_iterator itTok = this->begin();
         itTok != this->end(); itTok ++) {
        if ((itSentBrk != m_sentenceBreaks.end()) && (*itTok == **itSentBrk)) {
            sentenceBreaks.push_back(tokenCounter);
            itSentBrk ++;
        }
        if ((itTopSht != m_topicShifts.end()) && (*itTok == **itTopSht)) {
//           std::cerr << "added topic shift index: " << tokenCounter 
//                     <<"(on token " << ***itTopSht << ")" << std::endl;
          topicShifts.push_back(tokenCounter);
          itTopSht ++;
        }
        tokenCounter ++;
    }
    if (itTopSht != m_topicShifts.end()) {
      BOWLOGINIT;
      std::ostringstream oss;
      do { 
        oss << (**itTopSht)->getOutputUTF8String() << " "; itTopSht++;
      } while (itTopSht != m_topicShifts.end());
      LERROR << "Write BoWDocumentST: missing topic shifts on tokens " 
             << oss.str();
    }
    if (itSentBrk != m_sentenceBreaks.end()) {
      BOWLOGINIT;
      std::ostringstream oss;
      do { 
        oss << (**itSentBrk)->getOutputUTF8String() << " "; itSentBrk++;
      } while (itSentBrk != m_sentenceBreaks.end());
      LERROR << "Write BoWDocumentST: missing sentence breaks on tokens " 
             << oss.str();
    }

      // write sentence breaks and topic shifts
      //  write the number of sentence breaks
    Misc::writeCodedInt(file, sentenceBreaks.size());
    //  write sentence breaks
    for (std::vector<uint64_t>::const_iterator itSent = sentenceBreaks.begin();
         itSent != sentenceBreaks.end(); itSent ++) {
        Misc::writeCodedInt(file, *itSent);
    }

    //   write the number of topic shifts
    Misc::writeCodedInt(file, topicShifts.size());
    //   write topic shifts
    for (std::vector<uint64_t>::const_iterator itTop = topicShifts.begin();
         itTop != topicShifts.end(); itTop ++) {
        Misc::writeCodedInt(file, *itTop);
    }

}



} // end namespace
} // end namespace
} // end namespace
