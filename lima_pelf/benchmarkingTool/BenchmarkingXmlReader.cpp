// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "BenchmarkingXmlReader.h"
#include "Pipeline.h"

#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QIODevice>

using namespace Lima::Pelf;

BenchmarkingXmlReader::BenchmarkingXmlReader (Pipeline* pipeline) : QXmlStreamReader(),
    m_pipeline(pipeline), m_workingDir(), m_fileName(), m_analyzerCmd(), m_evaluatorCmd(),
    m_viewerCommandLine(), m_concurrentProcesses(1)
{
}

BenchmarkingXmlReader::~BenchmarkingXmlReader()
{
}


bool BenchmarkingXmlReader::read(const QString& fileName)
{
  m_fileName = fileName;

  m_workingDir = "";
  m_analyzerCmd = "";
  m_evaluatorCmd = "";
  m_viewerCommandLine = "";
  m_concurrentProcesses = 1;
  m_sortedPipelineUnits.clear();

  
  QFile loadFile(fileName);
  if(!loadFile.open(QFile::ReadOnly))
  {
      qWarning() << "Unable to open file, aborting" << fileName;
      return false;
  }

  setDevice(&loadFile);

  while (!atEnd())
  {
      readNext();

      if (isStartElement())
      {
          if (name() == "benchmarking")
              readBenchmarking();
          else
              raiseError(QObject::tr("The file is not a Benchmarking file."));
      }
  }
  loadFile.close();
  return !error();
}

void BenchmarkingXmlReader::readBenchmarking()
{
//   qDebug() << "BenchmarkingXmlReader::readBenchmarking";
  Q_ASSERT(isStartElement() && name() == "benchmarking");

  while (!atEnd())
  {
      readNext();

      if (isEndElement())
          break;

      if (isStartElement())
      {
          if (name() == "pipeline")
              readPipeline();
          else if (name() == "evaluations")
              readEvaluations();
          else if (name() == "workingDir")
              readWorkingDir();
          else if (name() == "commands")
              readCommands();
          else if (name() == "concurrentProcesses")
              readConcurrentProcesses();
          else
              readUnknownElement();
      }
  }
//   qDebug() << "replaceConfiguration" << m_workingDir << m_analyzerCmd << m_evaluatorCmd << m_concurrentProcesses;
  m_pipeline->replaceConfiguration(m_workingDir, m_analyzerCmd, m_evaluatorCmd, m_concurrentProcesses);

//   qDebug() << "Benchmarking loaded";
}

void BenchmarkingXmlReader::readUnknownElement()
{
//   qDebug() << "BenchmarkingXmlReader::readUnknownElement" << name();
  Q_ASSERT(isStartElement());

  while (!atEnd())
  {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement())
      readUnknownElement();
  }
}

void BenchmarkingXmlReader::readCommands()
{
//   qDebug() << "BenchmarkingXmlReader::readCommands";
  Q_ASSERT(isStartElement() && name() == "commands");

  while (!atEnd())
  {
    readNext();
//     qDebug() << "BenchmarkingXmlReader::readCommands current is now" << name();

    if (isEndElement())
      break;

    if (isStartElement())
    {
      if (name() == "analyzer")
        readAnalyzer();
      else if (name() == "evaluator")
        readEvaluator();
      else if (name() == "viewer")
        readViewer();
      else
        readUnknownElement();
    }
  }
}

void BenchmarkingXmlReader::readPipeline()
{
//   qDebug() << "BenchmarkingXmlReader::readPipeline";
  Q_ASSERT(isStartElement() && name() == "pipeline");

  while (!atEnd())
  {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement())
    {
      if (name() == "unit")
        readUnit();
      else
        readUnknownElement();
    }
  }
}

void BenchmarkingXmlReader::readEvaluations()
{
//   qDebug() << "BenchmarkingXmlReader::readEvaluations";
  Q_ASSERT(isStartElement() && name() == "evaluations");

  while (!atEnd())
  {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement())
    {
      if (name() == "pipelineEvaluation")
        readPipelineEvaluation();
      else
        readUnknownElement();
    }
  }
}

void BenchmarkingXmlReader::readWorkingDir()
{
//   qDebug() << "BenchmarkingXmlReader::readWorkingDir";
  Q_ASSERT(isStartElement() && name() == "workingDir");
  QXmlStreamAttributes attribs = attributes();
  m_workingDir = attribs.value("path").toString();
  if(m_workingDir.isEmpty())
    m_workingDir = QFileInfo(m_fileName).dir().absolutePath();

  finishEmptyElement();
}

void BenchmarkingXmlReader::readConcurrentProcesses()
{
//   qDebug() << "BenchmarkingXmlReader::readConcurrentProcesses";
  Q_ASSERT(isStartElement() && name() == "concurrentProcesses");
  QXmlStreamAttributes attribs = attributes();
  m_concurrentProcesses = attribs.value("max").toString().toInt();

  finishEmptyElement();
}

void BenchmarkingXmlReader::readPipelineEvaluation()
{
//   qDebug() << "BenchmarkingXmlReader::readPipelineEvaluation";
  Q_ASSERT(isStartElement() && name() == "pipelineEvaluation");
  QXmlStreamAttributes attribs = attributes();
  QMap<PipelineUnit*, EvaluationResultSet*> unitEvaluations;

  while (!atEnd())
  {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement())
    {
      if (name() == "unitEvaluation")
        readUnitEvaluation(unitEvaluations);
      else
        readUnknownElement();
    }
  }
  QDateTime resultsTime = QDateTime::fromTime_t(attribs.value("time").toString().toInt());
///@TODO Possible memory leak:  ensure that this object will be deleted
  BenchmarkingResult* benchmarkingResult = new BenchmarkingResult();
  benchmarkingResult->time = resultsTime;
  benchmarkingResult->comment = attribs.value("comments").toString();
  benchmarkingResult->resultUnits = unitEvaluations;
  m_pipeline->results[resultsTime] = benchmarkingResult;
  m_pipeline->startTime = resultsTime;
}

void BenchmarkingXmlReader::readUnit()
{
//   qDebug() << "BenchmarkingXmlReader::readUnit";
  Q_ASSERT(isStartElement() && name() == "unit");
  QXmlStreamAttributes attribs = attributes();
  int id = attribs.value("id").toString().toInt();
  m_sortedPipelineUnits[id] = m_pipeline->replaceUnit(
            attribs.value("textPath").toString(),
            attribs.value("referencePath").toString(),
            attribs.value("fileType").toString(),
            (PipelineUnit::STATES)attribs.value("processingStatus").toString().toInt());

  finishEmptyElement();
}

void BenchmarkingXmlReader::readAnalyzer()
{
//   qDebug() << "BenchmarkingXmlReader::readAnalyzer";
  Q_ASSERT(isStartElement() && name() == "analyzer");
  QXmlStreamAttributes attribs = attributes();
  m_analyzerCmd = attribs.value("commandLine").toString();

  finishEmptyElement();
}

void BenchmarkingXmlReader::readEvaluator()
{
//   qDebug() << "BenchmarkingXmlReader::readEvaluator";
  Q_ASSERT(isStartElement() && name() == "evaluator");
  QXmlStreamAttributes attribs = attributes();
  m_evaluatorCmd = attribs.value("commandLine").toString();

  finishEmptyElement();
}

void BenchmarkingXmlReader::readViewer()
{
//   qDebug() << "BenchmarkingXmlReader::readViewer";
  Q_ASSERT(isStartElement() && name() == "viewer");
  QXmlStreamAttributes attribs = attributes();
  m_viewerCommandLine = attribs.value("commandLine").toString();

  finishEmptyElement();
}

void BenchmarkingXmlReader::readUnitEvaluation(QMap<PipelineUnit*, EvaluationResultSet*>& unitEvaluations)
{
//   qDebug() << "BenchmarkingXmlReader::readUnitEvaluation";
  Q_ASSERT(isStartElement() && name() == "unitEvaluation");
  QXmlStreamAttributes attribs = attributes();

  PipelineUnit* unit = m_sortedPipelineUnits[attribs.value("unitId").toString().toInt()];
  EvaluationResultSet* resultSet = new EvaluationResultSet();

  while (!atEnd())
  {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement())
    {
      if (name() == "result")
        readResult(resultSet);
      else
        readUnknownElement();
    }
  }
  if(unit != 0)
  {
//     qDebug() << "BenchmarkingXmlReader::readUnitEvaluation store resultSet" << (void*)resultSet << "in";
///@TODO Possible memory leak:  ensure that this object will be deleted
      unitEvaluations[unit] = resultSet;
  }
  else
  {
    delete resultSet;
  }
}

void BenchmarkingXmlReader::readResult(EvaluationResultSet* resultSet)
{
  Q_ASSERT(isStartElement() && name() == "result");
  QXmlStreamAttributes attribs = attributes();

  ///@TODO Possible memory leak:  ensure that this object will be deleted
  EvaluationResult* result = new EvaluationResult();
  result->fc = attribs.value("fc").toString().toDouble();
  result->fp = attribs.value("fp").toString().toDouble();
  result->cr = attribs.value("cr").toString().toDouble();
//   qDebug() << "BenchmarkingXmlReader::readResult" << (void*)resultSet << attribs.value("type").toString().toInt() << result->fc << result->fp << result->cr ;

  (*resultSet)[(EvaluationResult::DIMENSION_ID)attribs.value("type").toString().toInt()] = result;

  while (!atEnd())
  {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement())
    {
      if (name() == "errors")
        readErrors(result);
      else
        readUnknownElement();
    }
  }
}

void BenchmarkingXmlReader::readErrors(EvaluationResult* result)
{
//   qDebug() << "BenchmarkingXmlReader::readErrors";
  Q_ASSERT(isStartElement() && name() == "errors");

  while (!atEnd())
  {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement())
    {
      if (name() == "F")
        readF(result);
      else if (name() == "A")
        readA(result);
      else if (name() == "T")
        readT(result);
      else
        readUnknownElement();
    }
  }
}

void BenchmarkingXmlReader::readT(EvaluationResult* result)
{
//   qDebug() << "BenchmarkingXmlReader::readT";
  Q_ASSERT(isStartElement() && name() == "T");
  QXmlStreamAttributes attribs = attributes();
  result->getTypeError().insert(
      attribs.value("sent").toString(),
      attribs.value("id").toString());

  finishEmptyElement();
}

void BenchmarkingXmlReader::readA(EvaluationResult* result)
{
//   qDebug() << "BenchmarkingXmlReader::readA";
  Q_ASSERT(isStartElement() && name() == "A");
  QXmlStreamAttributes attribs = attributes();
  result->getRefAbsentFromHyp().insert(
      attribs.value("sent").toString(),
      attribs.value("id").toString());

  finishEmptyElement();
}

void BenchmarkingXmlReader::readF(EvaluationResult* result)
{
//   qDebug() << "BenchmarkingXmlReader::readF";
  Q_ASSERT(isStartElement() && name() == "F");
  QXmlStreamAttributes attribs = attributes();
  result->getHypAbsentFromRef().insert(
      attribs.value("sent").toString(),
      attribs.value("id").toString());

  finishEmptyElement();
}

void BenchmarkingXmlReader::finishEmptyElement()
{
//   qDebug() << "BenchmarkingXmlReader::finishEmptyElement" << name();
  while (!atEnd())
  {
    readNext();
//     qDebug() << "BenchmarkingXmlReader::finishEmptyElement current is now" << name() << tokenType();

    if (isEndElement())
    {
      return;
    }
    else if (isStartElement())
    {
      readUnknownElement();
    }
  }
}
