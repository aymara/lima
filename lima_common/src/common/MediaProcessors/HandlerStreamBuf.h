// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
