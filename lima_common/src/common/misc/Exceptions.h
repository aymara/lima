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

// NAUTITIA
//
// jys 18-JUL-2002
//
// This file holds exceptions classes for S2 project

#ifndef Exceptions_H
#define Exceptions_H

//#include "common/misc/LimaString.h"

#include <stdexcept>

namespace Lima {


// when binary file is syntactically wrong
class BadFileException : public std::runtime_error {
public:
    uint32_t _expectedAddress;
    BadFileException(const std::string s = "Bad file error") :
        runtime_error(s),
        _expectedAddress(0) {}
    BadFileException(uint32_t expectedAddress) :
        runtime_error("Bad file error"),
        _expectedAddress(expectedAddress) {}
        virtual ~BadFileException() throw() {}
};

// when character is searched out of text buffer
class BoundsErrorException : public std::out_of_range {
public:
    BoundsErrorException(const std::string s = "Bound Error") :
    out_of_range(s) {}
};

// when attemp to use an empty entry from dictionary
class EmptyEntryException : public std::domain_error {
public:
    EmptyEntryException(const std::string s = "Empty entry") :
    domain_error(s) {}
};
// when EOF occurs (it can be a valid test of EOF)
class EofException : public std::runtime_error {
public:
    EofException(const std::string s = "Eof") :
    runtime_error(s) {}
};
// when an main Tokenizer Automaton goes in infinite loop
class InfiniteLoopException : public std::runtime_error {
public:
    InfiniteLoopException(const std::string s = "Infinite loop") :
    runtime_error(s) {}
};
// when an unexpected case appears
class InternalErrorException : public std::logic_error {
public:
    InternalErrorException(const std::string s = "Internal error") :
    logic_error(s) {}
};
// when an invalid character is used
class InvalidCharException : public std::runtime_error {
public:
    InvalidCharException(const std::string s = "Invalid char") :
    runtime_error(s) {}
};
// when an invalid UNICODE file is used
class InvalidFileException : public std::runtime_error {
public:
    InvalidFileException(const std::string s = "Invalid file") :
    runtime_error(s) {}
};
// when an invalid automaton is used
class InvAutomatonException : public std::runtime_error {
public:
    InvAutomatonException(const std::string s = "Invalid automaton") :
    runtime_error(s) {}
};
// when an invalid coded number is encountered in binary file
class InvNumberException : public std::runtime_error {
public:
    InvNumberException(const std::string s = "Invalid number") :
    runtime_error(s) {}
};
// when new() fails
class MemoryErrorException : public std::bad_alloc {
public:
    MemoryErrorException() :
    bad_alloc() {}
};
// when open file fails
class OpenFileException : public std::runtime_error 
{
public:
  OpenFileException(const std::string& s) : runtime_error(s) {}
};
// when read error occurs on a file
class ReadErrorException : public std::runtime_error {
public:
    ReadErrorException(const std::string s = "read error") :
    runtime_error(s) {}
};
// when both parts of a dictionary are incompatible
class UnconsistentDictionaryException : public std::domain_error {
public:
    UnconsistentDictionaryException(const std::string s = "Unconsistent dictionary") :
    domain_error(s) {}
};
// when an unknown character class name is found
class UnknownCharClassException : public std::runtime_error {
public:
    UnknownCharClassException(const std::string s = "Unknown char class") :
    runtime_error(s) {}
};
// when an unknown language is specified
class UnknownLanguageException : public std::runtime_error {
public:
    UnknownLanguageException(const std::string s = "Unknown language") :
    runtime_error(s) {}
};
// when input XML file is syntactically wrong
class XmlSyntaxException : public std::runtime_error {
public:
    enum Why {
        SYNTAX_EXC,            // syntax exception
        NO_DATA_EXC,        // no enclosed datas found
        DOUBLE_EXC,            // same object already defined
        FWD_CLASS_EXC,        // forward class declaration
        MULT_CLASS_EXC,        // multi class declaration
        EOF_EXC,            // unexpected EOF
        NO_CODE_EXC,        // no <code> found
        BAD_CODE_EXC,        // <code> exceeds FFFF
        NO_CLASS_EXC,        // no <char_class> found
        UNK_CLASS_EXC,        // unknown class found
        INT_ERROR_EXC,        // internal error
        INV_CLASS_EXC        // invalid class found
    };
    uint32_t _lineNumber;
    Why _why;
    Lima::LimaString _fileName;
    XmlSyntaxException(const std::string s = "XML syntax error") :
        runtime_error(s) {}
        XmlSyntaxException(Why why) :
        runtime_error("XML syntax error"),
        _why(why) {}
    XmlSyntaxException(
        uint32_t lineNumber, 
        Why why) :
        runtime_error("XML syntax error"),
        _lineNumber(lineNumber),
        _why(why) {}
    XmlSyntaxException(
        uint32_t lineNumber,
        Why why,
        const Lima::LimaString& fileName) :
        runtime_error("XML syntax error"),
        _lineNumber(lineNumber),
        _why(why),
        _fileName(fileName) {}
        virtual ~XmlSyntaxException() throw() {}
    void set (uint32_t lineNumber) {
        _lineNumber = lineNumber; }
    void set (Why why) {
        _why = why; }
    void set (const Lima::LimaString& fileName) {
        _fileName = fileName; }
};
// when input XML file is syntactically wrong
class XmlS2SyntaxException : public std::runtime_error {
public:
    XmlS2SyntaxException(const std::string s = "XML syntax error") :
    runtime_error(s) {}
};


} // Lima

#endif  
