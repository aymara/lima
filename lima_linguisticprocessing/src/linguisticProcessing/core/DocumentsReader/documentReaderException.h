// Copyright (C) 2005 by CEA LIST (LVIC)
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
