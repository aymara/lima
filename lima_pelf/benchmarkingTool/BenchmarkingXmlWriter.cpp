// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "BenchmarkingXmlWriter.h"
#include "Pipeline.h"

#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QIODevice>

using namespace Lima::Pelf;

BenchmarkingXmlWriter::BenchmarkingXmlWriter (Pipeline* pipeline, const QString& viewerCommandLine, QIODevice* device) :
    QXmlStreamWriter(device),
    m_pipeline(pipeline),m_viewerCommandLine(viewerCommandLine)
{
  setAutoFormatting(true);
}

BenchmarkingXmlWriter::~BenchmarkingXmlWriter()
{
}

bool BenchmarkingXmlWriter::write()
{
  writeStartDocument();
  writeComment("This file has been saved using Pelf Benchmarking Tool");
  writeStartElement("benchmarking");

  writeStartElement("pipeline");
  int unitId = 0;
  const QList<PipelineUnit*>& pipelineUnits = m_pipeline->getUnits();
  Q_FOREACH (PipelineUnit* unit, pipelineUnits)
  {
    writeStartElement("unit");
    unit->fileId = ++unitId;
    writeAttribute("id", QString::number(unitId));
    writeAttribute("textPath", unit->getTextPath());
    writeAttribute("referencePath", unit->referencePath);
    writeAttribute("fileType", unit->fileType);
    writeAttribute("processingStatus", QString::number(unit->status));
    writeEndElement();
  }
  writeEndElement();

  writeStartElement("evaluations");
  Q_FOREACH (BenchmarkingResult* benchmarkingResult, m_pipeline->results)
  {
    writeStartElement("pipelineEvaluation");
    QDateTime evaluationTime = benchmarkingResult->time;
    writeAttribute("time", QString::number(evaluationTime.toTime_t()));
    writeAttribute("comments", benchmarkingResult->comment);
    Q_FOREACH (PipelineUnit* unit, pipelineUnits)
    {
      if(benchmarkingResult->resultUnits.contains(unit))
      {
        writeStartElement("unitEvaluation");
        writeAttribute("unitId", QString::number(unit->fileId));
        EvaluationResultSet* unitResults = benchmarkingResult->resultUnits[unit];
        Q_FOREACH(EvaluationResult::DIMENSION_ID dimensionId, unitResults->keys())
        {
          EvaluationResult* result = (*unitResults)[dimensionId];
          writeStartElement("result");
          writeAttribute("type", QString::number(dimensionId));
          writeAttribute("fc", QString::number(result->fc));
          writeAttribute("fp", QString::number(result->fp));
          writeAttribute("cr", QString::number(result->cr));
          writeStartElement("errors");
          QMultiMap<QString, QString>::const_iterator fit = result->getHypAbsentFromRef().begin();
          for (; fit != result->getHypAbsentFromRef().end(); fit++)
          {
            writeEmptyElement("F");
            writeAttribute("sent", fit.key());
            writeAttribute("id", fit.value());
          }
          QMultiMap<QString, QString>::const_iterator ait = result->getRefAbsentFromHyp().begin();
          for (; ait != result->getRefAbsentFromHyp().end(); ait++)
          {
            writeEmptyElement("A");
            writeAttribute("sent", ait.key());
            writeAttribute("id", ait.value());
          }
          QMultiMap<QString, QString>::const_iterator tit = result->getTypeError().begin();
          for (; tit != result->getTypeError().end(); tit++)
          {
            writeEmptyElement("T");
            writeAttribute("sent", tit.key());
            writeAttribute("id", tit.value());
          }
          writeEndElement();
          writeEndElement();
        }
        writeEndElement();
      }
    }
    writeEndElement();
  }
  writeEndElement();

  writeEmptyElement("workingDir");
  writeAttribute("path", m_pipeline->workingDir);

  writeStartElement("commands");
  if(m_pipeline->analyzerCommand != 0)
  {
      writeEmptyElement("analyzer");
      writeAttribute("commandLine", m_pipeline->analyzerCommand->commandLine);
  }
  if(m_pipeline->evaluatorCommand != 0)
  {
      writeEmptyElement("evaluator");
      writeAttribute("commandLine", m_pipeline->evaluatorCommand->commandLine);
  }
  if(!m_viewerCommandLine.isEmpty())
  {
      writeEmptyElement("viewer");
      writeAttribute("commandLine", m_viewerCommandLine);
  }
  writeEndElement();

  writeEmptyElement("concurrentProcesses");
  writeAttribute("max", QString::number(m_pipeline->concurrentProcesses));

  writeEndDocument();
  m_pipeline->setClean();
  return true;
}

