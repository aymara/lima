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

class DOCUMENTSREADER_EXPORT DocumentReaderException : public Lima::LimaException {
 public:
  DocumentReaderException() {};   
  virtual ~DocumentReaderException() throw () {};
  DocumentReaderException(const std::string& msg):m_msg(msg) {}; 
  const char* what() const throw() override { return m_msg.c_str(); }
 protected:
  std::string m_msg;
};

// utility function
DOCUMENTSREADER_EXPORT void handleXercesException(const Lima::XMLException& e) {
  std::ostringstream oss;
  oss << "Lima::XMLException: " << e.what();
  DRLOGINIT;
  LWARN << "Lima::XMLException: " << QString::fromUtf8(e.what());
  throw DocumentReaderException(oss.str());
}

} // end namespace
} // end namespace

#endif
