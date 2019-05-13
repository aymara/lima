/***************************************************************************
                          FrCeaLima::LimaException.h  -  description
                             -------------------
    begin                : jeu avr 17 2003
    copyright            : (C) 2003 by CEA
    email                : de-chalendarg@zoe.cea.fr
 ***************************************************************************/

#ifndef FRCEALIC2M_EXCEPTION
#define FRCEALIC2M_EXCEPTION

#include "documentsreader_export.h"
#include <string>
#include <stdexcept>

namespace Lima {

class DOCUMENTSREADER_EXPORT LogicalException : public std::logic_error{
  public:
    LogicalException(const std::string &mess);
};

class DOCUMENTSREADER_EXPORT TechnicalException : public std::runtime_error{
  public:
    TechnicalException(const std::string &mess);
};

class DOCUMENTSREADER_EXPORT ReaderXMLException : public std::runtime_error{
  public:
    ReaderXMLException(const std::string &mess);
    virtual const char *what () const throw();
    virtual const XMLCh* getType() const;
  private:
    std::string m_comment;
};


} // namespace Lima

#endif // FRCEALIC2M_EXCEPTION
