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


#ifndef LIMA_COMMON_ANNOTATIONGRAPHWRITER_H
#define LIMA_COMMON_ANNOTATIONGRAPHWRITER_H

#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "common/Data/strwstrtools.h"
// #include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "AnnotationData.h"

#include <string>

namespace Lima
{
namespace Common
{
namespace AnnotationGraphs
{

class LIMA_ANNOTATIONGRAPH_EXPORT AnnotationGraphWriter
{

public:
  AnnotationGraphWriter(
    const AnnotationData& data,
    MediaId language) :
      m_data(data),
      m_language(language) {}
  virtual ~AnnotationGraphWriter() {}
  void writeToDotFile(const std::string& dotFileName);

private:
  AnnotationGraphWriter& operator=(const AnnotationGraphWriter&) {return *this;}

  const AnnotationData& m_data;
  const MediaId m_language;
};

} // closing namespace AnnotationGraphs
} // closing namespace Common
} // closing namespace Lima

#endif
