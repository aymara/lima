/*
    Copyright 2002-2022 CEA LIST

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
/**
  * @author      Gael de Chalendar <Gael.de-Chalendar@cea.fr>
  * @date        Created on  : Mon Aug 04 2003
  */


#ifndef XMLSYNTAGMATICMATRIXFILEHANDLER_H
#define XMLSYNTAGMATICMATRIXFILEHANDLER_H

#include "SyntacticAnalysisExport.h"
#include "SyntagmaticMatrix.h"
#include "TokenFilter.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "common/misc/fsaStringsPool.h"

#include <string>


namespace Lima {
namespace LinguisticProcessing {
namespace SyntacticAnalysis {

class XmlSyntagmaticMatrixFileReaderPrivate;
/**
 * @brief This is a SAX document handler the Detect configuration files
 * @author Gael de Chalendar
 */
class LIMA_SYNTACTICANALYSIS_EXPORT XmlSyntagmaticMatrixFileReader
{
public:
  // -----------------------------------------------------------------------
  //  Constructors
  // -----------------------------------------------------------------------
  XmlSyntagmaticMatrixFileReader(SyntagmDefStruct& theMatrices,MediaId language);
  virtual ~XmlSyntagmaticMatrixFileReader() = default;

  bool parse(QIODevice *device);

  QString errorString() const;

private:
  XmlSyntagmaticMatrixFileReaderPrivate* m_d;
};

} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif
