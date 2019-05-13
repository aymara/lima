/***************************************************************************
                          FrCeaLima::LimaException.cpp  -  description
                             -------------------
    begin                : jeu avr 17 2003
    copyright            : (C) 2003 by CEA
    email                : de-chalendarg@zoe.cea.fr
 ***************************************************************************/

#include <string>
#include <stdexcept>
#include <FrCeaLima::LimaException.h>

namespace Lima {

LogicalException::LogicalException(const std::string &mess) : std::logic_error(mess){}

TechnicalException::TechnicalException(const std::string &mess) : std::runtime_error(mess){}

ReaderXMLException::ReaderXMLException(const std::string &mess) : m_comment(mess){}
const char *ReaderXMLException::what () const throw() { return m_comment.c_str(); }
// TODO: replace NULL with something !!!
const XMLCh* ReaderXMLException::getType() const { return NULL; }

} // namespace Lima
