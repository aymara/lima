// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
