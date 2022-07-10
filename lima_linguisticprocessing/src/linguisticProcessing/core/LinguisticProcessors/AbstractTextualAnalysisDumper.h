// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Jan 21 2011
 * @brief a abstract class the dumpers, containing the common
 * mechanisms for communication with handler or local file
 ***********************************************************************/

#ifndef ABSTRACTTEXTUALANALYSISDUMPER_H
#define ABSTRACTTEXTUALANALYSISDUMPER_H

#include "LinguisticProcessorsExport.h"
#include "common/MediaProcessors/DumperStream.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima {
namespace LinguisticProcessing {

  // TODO a faire hériter de AbstractAnalysisDumper dans Common.
  // [hlb] Pour une raison à déterminer, il y a un pb (pour les dumper fils) si cette classe n'hérite pas directement de MediaProcessUnit
class LIMA_LINGUISTICPROCESSORS_EXPORT AbstractTextualAnalysisDumper : public MediaProcessUnit
{
 public:
  AbstractTextualAnalysisDumper();
  virtual ~AbstractTextualAnalysisDumper();

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  ;

  virtual LimaStatusCode process(AnalysisContent& analysis) const override=0;

  // create the stream on which the data will be dumped
  std::shared_ptr<DumperStream> initialize(AnalysisContent& analysis) const;

 protected:
  MediaId m_language;
  std::ostream* m_out;
  std::string m_handlerName;      /* < the handler for communication with the client */

  // members for file management
  std::string m_outputFile;   /* < the file name for local file logging */
  std::string m_outputSuffix; /* < the suffix for local file logging */
  bool m_stripInputSuffix; /* < whether to remove the input file suffix before
                                adding (or not) its suffix to the local file */
  bool m_append;
  QString m_temporaryFileMetadata;

};

} // end namespace
} // end namespace

#endif
