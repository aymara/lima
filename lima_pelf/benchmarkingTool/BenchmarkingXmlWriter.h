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
#ifndef LIMA_PELF_BENCHMARKINGXMLWRITER_H
#define LIMA_PELF_BENCHMARKINGXMLWRITER_H

#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtXml/QXmlStreamWriter>

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
