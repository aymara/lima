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
 * @file       AbstractTextualAnalysisDumper.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Jan 21 2011
 * copyright   Copyright (C) 2011 by CEA LIST (LVIC)
 * Project     MM
 * 
 * @brief a abstract class the dumpers, containing the common
 * mechanisms for communication with handler or local file
 * 
 * 
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
    Manager* manager)
  ;

  virtual LimaStatusCode process(AnalysisContent& analysis) const=0;

  // create the stream on which the data will be dump
  // the pointer must be deleted by caller
  DumperStream* initialize(AnalysisContent& analysis) const;
  
 protected:
  MediaId m_language;
  std::ostream* m_out;
  
  std::string m_handlerName;      /* < the handler for communication with the client */
  
  // members for file management
  std::string m_outputFile;   /* < the file name for local file logging */
  std::string m_outputSuffix; /* < the suffix for local file logging */
  bool m_append;
  QString m_temporaryFileMetadata;
  
};

} // end namespace
} // end namespace

#endif
