// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 * @file               S2Commons.h
 * @date               Created on  : Thu May 06 200
 * @author             Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

 *                     Benoit Mathieu <mathieub@zoe.cea.fr> 

 *                     Copyright (c) 2003-2012 by CEA LIST
 * @version            $Id$
 */

#ifndef LIMA_FSA_EXCEPTIONS_H
#define LIMA_FSA_EXCEPTIONS_H

#include "common/LimaCommon.h"


namespace Lima {
namespace Common {
namespace FsaAccess {

class LIMA_FSAACCESS_EXPORT FsaNotSaved : public LimaException {
public :
  FsaNotSaved(const std::string& reason) : LimaException(reason) {};
  virtual ~FsaNotSaved() throw() {};
};


} // namespace FsaAccess
} // namespace Common
} // namespace Lima


#endif // LIMA_FSA_EXCEPTIONS_H
