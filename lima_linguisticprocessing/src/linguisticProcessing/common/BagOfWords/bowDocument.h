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

typedef enum {
  DOCUMENT_PROPERTIES_BLOC,
  BOW_TEXT_BLOC,
  BIN_NUM_BLOC,
  ST_BLOC,
  END_BLOC,
  NODE_PROPERTIES_BLOC,
  HIERARCHY_BLOC,
  INDEXING_BLOC
} BoWBlocType;

LIMA_BOW_EXPORT std::ostream& operator<<(std::ostream& out,const BoWBlocType& blocType);
LIMA_BOW_EXPORT std::istream& operator>>(std::istream& in,BoWBlocType& blocType);

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


} // end namespace
} // end namespace
} // end namespace

#endif
