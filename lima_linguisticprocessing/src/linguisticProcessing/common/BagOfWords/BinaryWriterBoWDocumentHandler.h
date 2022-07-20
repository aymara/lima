// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       BinaryWriterBoWDocumentHandler.h
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
#include "bowToken.h"
#include "bowText.h"
#include "bowBinaryReaderWriter.h"
#include "AbstractBoWDocumentHandler.h"
#include "common/Data/genericDocumentProperties.h"

namespace Lima {
namespace Common {
namespace BagOfWords {

class LIMA_BOW_EXPORT BinaryWriterBoWDocumentHandler : 
    public AbstractBoWDocumentHandler
{
 public:
  BinaryWriterBoWDocumentHandler(std::ostream& os);
  ~BinaryWriterBoWDocumentHandler();
  
  void openSBoWNode(const Misc::GenericDocumentProperties* properties,
                    const std::string& elementName) override;

  void openSBoWIndexingNode(const Misc::GenericDocumentProperties* properties,
                            const std::string& elementName) override;

  void processSBoWText(const BoWText* boWText, 
                       bool useIterators, 
                       bool useIndexIterator) override;

  void processProperties(const Misc::GenericDocumentProperties* properties, 
                         bool useIterators, 
                         bool useIndexIterator) override;

  void closeSBoWNode() override;

 private:
  BoWBinaryWriter m_writer;
  std::ostream& m_outputStream;
};

} // end namespace
} // end namespace
} // end namespace

#endif
