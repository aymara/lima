// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       bowXMLReader.h
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Wed May  5 2004
 * copyright   Copyright (C) 2004 by CEA LIST
 * Project     BagOfWords
 *
 * @brief      XML input/output for BoW elements
 *
 *
 ***********************************************************************/

#ifndef BOWXMLREADER_H
#define BOWXMLREADER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "bowToken.h"
#include "bowComplexToken.h"
#include "bowText.h"
#include "bowDocument.h"
#include <stack>

#include <QtCore/QDate>

namespace Lima {
namespace Common {
namespace BagOfWords {

class BoWXmlReaderPrivate;
/**
 * @brief      XML input/output for BoW elements
 * @author     Besancon Romaric <romaric.besancon@cea.fr>
 * @date       Wed May  5 2004
 **/
class LIMA_BOW_EXPORT BoWXmlReader
{
 public:
  BoWXmlReader(const std::string& filename,
               std::ostream& output);
  virtual ~BoWXmlReader();

  // -----------------------------------------------------------------------
  // local functions

 private:
   BoWXmlReaderPrivate* m_d;
};

} // end namespace
} // end namespace
} // end namespace

#endif
