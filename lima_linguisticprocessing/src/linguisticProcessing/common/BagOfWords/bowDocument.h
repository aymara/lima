// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       bowDocument.h
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Wed Oct  8 2003
 * copyright   Copyright (C) 2003 by CEA LIST
 * Project     BagOfWords
 *
 * @brief      bag-of-words representation of a document
 *
 *
 ***********************************************************************/

#ifndef BOWDOCUMENT_H
#define BOWDOCUMENT_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "bowText.h"
#include "common/Data/genericDocumentProperties.h"
#include <iostream>

namespace Lima
{
namespace Common
{
namespace BagOfWords
{

enum BoWBlocType : unsigned char {
  DOCUMENT_PROPERTIES_BLOC,
  BOW_TEXT_BLOC,
  BIN_NUM_BLOC,
  ST_BLOC,
  END_BLOC,
  NODE_PROPERTIES_BLOC,
  HIERARCHY_BLOC,
  INDEXING_BLOC
};

LIMA_BOW_EXPORT std::ostream& operator<<(std::ostream& out,const BoWBlocType& blocType);
LIMA_BOW_EXPORT QDebug& operator<<(QDebug& out,const BoWBlocType& blocType);
LIMA_BOW_EXPORT std::istream& operator>>(std::istream& in,BoWBlocType& blocType);

std::string getBlocTypeString(BoWBlocType blocType);

/**
* @brief represent a document as a Bag Of Word, with associated document properties.
*/
class LIMA_BOW_EXPORT BoWDocument :
    public ::Lima::Common::Misc::GenericDocumentProperties,
    public BoWText
{
public:
  BoWDocument();
  BoWDocument(const BoWDocument&);
  virtual ~BoWDocument();
  BoWDocument& operator = (const BoWDocument&);

  virtual void clear() override;

  virtual void reinit() override;

  virtual void setProperties(const Misc::GenericDocumentProperties& properties);

  //@{ binary input/output
  //virtual void read(std::istream& file);
  //virtual void write(std::ostream& file) const;
/*   virtual void readPart(std::istream& file,  */
/*                         AbstractBowDocumentHandler& handler,  */
/*                         bool useIterator,  */
/*                         std::ostream& os); */
//  //@}

friend LIMA_BOW_EXPORT std::ostream& operator << (std::ostream&, const BoWDocument&);
friend LIMA_BOW_EXPORT QDebug& operator << (QDebug&, const BoWDocument&);

private:
};


} // end namespace BagOfWords
} // end namespace Common
} // end namespace Lima

#endif
