// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       TextWriterBoWDocumentHandler.h
 * @author     Mesnard Olivier (olivier.mesnard@.cea.fr)
 * @date       Wed Oct  12 2005
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * Project     BagOfWords
 * 
 * @brief      handler for structured bow document that produces a text output
 * 
 * 
 ***********************************************************************/

#ifndef TEXTWRITERBOWDOCUMENTHANDLER_H
#define TEXTWRITERBOWDOCUMENTHANDLER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "AbstractBoWDocumentHandler.h"
#include <vector>

namespace Lima {
namespace Common {
  namespace Misc {
    class GenericDocumentProperties;
  }
namespace BagOfWords {

class BoWText;

class TextWriterBoWDocumentHandlerPrivate;
class LIMA_BOW_EXPORT TextWriterBoWDocumentHandler : public AbstractBoWDocumentHandler
{
 public:
  TextWriterBoWDocumentHandler(std::ostream& os);
  ~TextWriterBoWDocumentHandler();
  
  void openSBoWNode(const Misc::GenericDocumentProperties* properties,
                    const std::string& elementName) override;
  void openSBoWIndexingNode(const Misc::GenericDocumentProperties* properties,
                            const std::string& elementName) override;
  void processSBoWText(const BoWText* boWText, 
                       bool useIterators, bool useIndexIterator) override;
  void processProperties(const Misc::GenericDocumentProperties* properties, 
                         bool useIterators, bool useIndexIterator) override;
  void closeSBoWNode() override;

 private:
   TextWriterBoWDocumentHandler(const TextWriterBoWDocumentHandler&);
   TextWriterBoWDocumentHandler& operator=(const TextWriterBoWDocumentHandler&);
   TextWriterBoWDocumentHandlerPrivate* m_d;
};

} // end namespace
} // end namespace
} // end namespace

#endif
