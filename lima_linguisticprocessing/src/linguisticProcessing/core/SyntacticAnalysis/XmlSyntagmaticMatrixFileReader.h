// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
