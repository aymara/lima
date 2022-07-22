// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_BENCHMARKINGXMLWRITER_H
#define LIMA_PELF_BENCHMARKINGXMLWRITER_H

#include <QString>
#include <QMap>
#include <QXmlStreamWriter>

namespace Lima {
namespace Pelf {

class Pipeline;
class PipelineUnit;
class EvaluationResultSet;
class EvaluationResult;

class BenchmarkingXmlWriter : public QXmlStreamWriter
{

public:

  BenchmarkingXmlWriter(Pipeline* pipeline, const QString& viewerCommandLine, QIODevice* device);
  virtual ~BenchmarkingXmlWriter();

  bool write();

private:
  Pipeline* m_pipeline;
  QString m_viewerCommandLine;
};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_BENCHMARKINGXMLWRITER_H
