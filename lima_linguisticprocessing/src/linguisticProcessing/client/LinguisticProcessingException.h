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

#ifndef LIMA_LINGUISTICPROCESSING_LINGUISTICPROCESSINGEXCEPTION_H
#define LIMA_LINGUISTICPROCESSING_LINGUISTICPROCESSINGEXCEPTION_H

#include "common/LimaCommon.h"
#include <string>

namespace Lima {
namespace LinguisticProcessing {

class LinguisticProcessingException : public LimaException  {
  public:
    explicit LinguisticProcessingException(const char* mess):
      LimaException(mess) {};
    explicit LinguisticProcessingException(const std::string& mess):
      LimaException(mess) {};
    explicit LinguisticProcessingException(const QString& mess):
      LimaException(mess) {};
};

/**
 * This macro writes the message @ref X to a previously configured error stream
 * before throwing a LinguisticProcessingException with the same message
 */
#define LIMA_LP_EXCEPTION(X) { \
    QString errorString; \
    QTextStream qts(&errorString); \
    qts << __FILE__ << ":" << __LINE__ << ": " << X ; \
    LERROR << errorString; \
    throw LinguisticProcessingException(errorString); \
}

} // LinguisticProcessing
} // Lima



#endif
