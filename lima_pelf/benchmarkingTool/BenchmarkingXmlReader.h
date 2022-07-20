// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_BENCHMARKINGXMLREADER_H
#define LIMA_PELF_BENCHMARKINGXMLREADER_H

#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QXmlStreamReader>

namespace Lima {
namespace Pelf {

class Pipeline;
class PipelineUnit;
class EvaluationResultSet;
class EvaluationResult;

class BenchmarkingXmlReader : public QXmlStreamReader
{

public:

  BenchmarkingXmlReader(Pipeline* pipeline);
  virtual ~BenchmarkingXmlReader();

  bool read(const QString& fileName);

private:
  void readUnknownElement();
  void readBenchmarking();
  void readCommands();
  void readPipeline();
  void readEvaluations();
  void readWorkingDir();
  void readConcurrentProcesses();
  void readPipelineEvaluation();
  void readUnit();
  void readAnalyzer();
  void readEvaluator();
  void readViewer();
  void readUnitEvaluation(QMap<PipelineUnit*, EvaluationResultSet*>& unitEvaluations);
  void readResult(EvaluationResultSet* resultSet);
  void readErrors(EvaluationResult* result);
  void readT(EvaluationResult* result);
  void readA(EvaluationResult* result);
  void readF(EvaluationResult* result);
  void finishEmptyElement();

  Pipeline* m_pipeline;
  QString m_workingDir;
  QString m_fileName;
  QString m_analyzerCmd;
  QString m_evaluatorCmd;
  QString m_viewerCommandLine;
  int m_concurrentProcesses;
  QMap<int, PipelineUnit*> m_sortedPipelineUnits;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_BENCHMARKINGXMLREADER_H
