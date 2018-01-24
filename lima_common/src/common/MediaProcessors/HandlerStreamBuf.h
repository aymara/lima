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
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSINGHANDLERSTREAMBUF_H
#define LIMA_LINGUISTICPROCESSINGHANDLERSTREAMBUF_H

#include <iostream>
#include "common/Handler/AbstractAnalysisHandler.h"
#include "common/LimaCommon.h"

namespace Lima{

/**
@author Benoit Mathieu
*/
class LIMA_MEDIAPROCESSORS_EXPORT HandlerStreamBuf : public std::streambuf
{
public:
  HandlerStreamBuf(AbstractAnalysisHandler* handler);

  virtual ~HandlerStreamBuf();

protected:
  int overflow(int c) override;
  int sync() override;
  
  void flushBuffer();
  
private:
  AbstractAnalysisHandler* m_handler;
  char* m_pbuf;

};

}


#endif
