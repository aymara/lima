/************************************************************************
 *
 * @file       documentReaderException.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Wed Jul  6 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005 by CEA LIST (LVIC)
 * Project     DocumentReader
 *
 * @brief      Exceptions returned by the document reader
 *
 *
 ***********************************************************************/

#ifndef DOCUMENTREADEREXCEPTION_H
#define DOCUMENTREADEREXCEPTION_H

#include "documentsreader_export.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include <stdexcept>
#include <iostream>

namespace Lima {
namespace DocumentsReader {

class DOCUMENTSREADER_EXPORT DocumentReaderException : public Lima::LimaException
{
 public:
  DocumentReaderException() = default;
  virtual ~DocumentReaderException() throw () = default;
  DocumentReaderException(const std::string& msg): Lima::LimaException(msg) {};
};

} // end namespace
} // end namespace

#endif
