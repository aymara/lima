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
 * @file       DumperStream.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Jan 21 2011
 * copyright   Copyright (C) 2011-2012 by CEA LIST
 * Project     MM
 * 
 * @brief      a class defining a stream to be used in dumper (abstract layer on top of handler or file)
 * 
 * 
 ***********************************************************************/

#ifndef DUMPERSTREAM_H
#define DUMPERSTREAM_H

#include "HandlerStreamBuf.h"
#include "common/Handler/AbstractAnalysisHandler.h"
#include "common/LimaCommon.h"

#include <iostream>

namespace Lima {

class LIMA_MEDIAPROCESSORS_EXPORT DumperStream
{
 public:
  DumperStream(AbstractAnalysisHandler* handler); 
  DumperStream(const std::string& filename, bool append=false); 

  ~DumperStream();
  
  std::ostream& out() { return *m_out; }

  /*template<typename T> DumperStream& operator<<(const T& t) {
    m_out << t;
    return *this;
  }*/
  
 private:
  bool m_useHandler;
  AbstractAnalysisHandler* m_handler;
  HandlerStreamBuf* m_buffer;
  std::ostream* m_out;
};

} // end namespace

#endif
