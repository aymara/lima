/*
 *    Copyright 2002-2013 CEA LIST
 * 
 *    This file is part of LIMA.
 * 
 *    LIMA is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Affero General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 * 
 *    LIMA is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 * 
 *    You should have received a copy of the GNU Affero General Public License
 *    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
 */
#ifndef LIMA_PELF_BENCHMARKINGXMLREADER_H
#define LIMA_PELF_BENCHMARKINGXMLREADER_H

#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtXml/QXmlStreamReader>

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
