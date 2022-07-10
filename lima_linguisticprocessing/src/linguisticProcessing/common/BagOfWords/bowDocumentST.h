// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       bowDocumentST.h
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Fri Oct 10 2003
 * copyright   Copyright (C) 2003 by CEA LIST
 * Project     BagOfWords
 * 
 * @brief      bag-of-words representation of a document
 * 
 * 
 ***********************************************************************/

#ifndef BOWDOCUMENTST_H
#define BOWDOCUMENTST_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "bowDocument.h"
#include <iostream>

namespace Lima {
namespace Common {
namespace BagOfWords {

class LIMA_BOW_EXPORT BoWDocumentST : public BoWDocument
{
 public:
  BoWDocumentST();
  BoWDocumentST(const BoWDocumentST&);
  virtual ~BoWDocumentST();
  BoWDocumentST& operator = (const BoWDocumentST&);
  
  const std::vector<BoWText::const_iterator>& getSentenceBreaks() const;
  const std::vector<BoWText::const_iterator>& getTopicShifts() const;

  void reinit() override;

  void addSentenceBreak(const BoWText::const_iterator);
  void addTopicShift(const BoWText::const_iterator);

    // text input/output
    /**
      * @brief   specialization of << operator for taking into account
      *          sentence breaks and topic shifts
      * @author  Olivier Ferret
      */
  //  friend std::wostream& operator << (std::wostream&, const BoWDocumentST&);
  friend LIMA_BOW_EXPORT std::ostream& operator << (std::ostream&, const BoWDocumentST&);

    // binary input/output
    /**
      * @brief   specialization of read/write functions for taking into account
      *          sentence breaks and topic shifts
      * @author  Olivier Ferret
      */
    void read(std::istream& file) override;
    void write(std::ostream& file) const;
    void writeSTData(std::ostream& file) const;

 private:
  std::vector<BoWText::const_iterator> m_sentenceBreaks;
  std::vector<BoWText::const_iterator> m_topicShifts;

  //private functions
  void copySTData(const BoWDocumentST& d);

};
//**********************************************************************
// inline functions
//**********************************************************************
inline const std::vector<BoWText::const_iterator>&
BoWDocumentST::getSentenceBreaks() const
{
  return m_sentenceBreaks;
}
inline const std::vector<BoWText::const_iterator>&
BoWDocumentST::getTopicShifts() const
{
  return m_topicShifts;
}

inline void BoWDocumentST::addSentenceBreak(const BoWText::const_iterator i)
{
  m_sentenceBreaks.push_back(i);
}
inline void BoWDocumentST::addTopicShift(const BoWText::const_iterator i)
{
  m_topicShifts.push_back(i);
}


} // end namespace
} // end namespace
} // end namespace

#endif
