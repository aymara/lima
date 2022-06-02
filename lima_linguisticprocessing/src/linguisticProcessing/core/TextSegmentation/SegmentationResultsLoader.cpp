/*
    Copyright 2002-2022 CEA LIST

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
 * @author     Romaric Besancon <romaric.besancon@cea.fr>
 * @date       Tue Jan 18 2011
 ***********************************************************************/

#include "SegmentationResultsLoader.h"
#include "SegmentationData.h"

#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"

#include <QFile>
#include <QXmlStreamReader>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {

SimpleFactory<MediaProcessUnit,SegmentationResultsLoader> SegmentationResultsLoaderFactory(SEGMENTATIONRESULTSLOADER_CLASSID);


class LIMA_TEXTSEGMENTATION_EXPORT SegmentationResultsLoaderPrivate
{
public:
  SegmentationResultsLoaderPrivate();

  ~SegmentationResultsLoaderPrivate() = default;

  bool parse(QIODevice *device);
  void readResults();

  QString errorString() const;

  QXmlStreamReader m_reader;

  QString m_graphName;
  QString m_dataName;

  LinguisticAnalysisStructure::AnalysisGraph* m_graph;
  SegmentationData* m_data;
  uint64_t m_position;
  uint64_t m_length;
  QString m_type;

};

//***********************************************************************
// constructors and destructors
SegmentationResultsLoader::SegmentationResultsLoader():
  m_d(new SegmentationResultsLoaderPrivate())
{
}

SegmentationResultsLoader::~SegmentationResultsLoader()
{
  delete m_d;
}

SegmentationResultsLoaderPrivate::SegmentationResultsLoaderPrivate() :
  m_graphName("AnalysisGraph"),
  m_dataName("segmentationData")
{
}

//***********************************************************************
void SegmentationResultsLoader::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
          Manager* manager)

{
  AnalysisLoader::init(unitConfiguration,manager);
  try
  {
    m_d->m_graphName = QString::fromStdString(unitConfiguration.getParamsValueAtKey("graph"));
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // keep default value

  try
  {
    m_d->m_dataName = QString::fromStdString(unitConfiguration.getParamsValueAtKey("data"));
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // keep default value
}

LimaStatusCode SegmentationResultsLoader::process(AnalysisContent& analysis) const
{
  // get analysis graph
  auto graph = static_cast<AnalysisGraph*>(analysis.getData(m_d->m_graphName.toStdString()));
  if (graph == nullptr)
  {
    LOGINIT("LP::AnalysisLoader");
    LERROR << "no graph '" << m_d->m_graphName << "' available !";
    return MISSING_DATA;
  }

  // get segmentation data or create new
  auto data = analysis.getData(m_d->m_dataName.toStdString());
  SegmentationData* segmData = nullptr;
  if (data == nullptr)
  {
    segmData = new SegmentationData();
    analysis.setData(m_d->m_dataName.toStdString(), segmData);
  }
  else
  {
    segmData = static_cast<SegmentationData*>(data);
    if (segmData == nullptr)
    {
      LOGINIT("LP::AnalysisLoader");
      LERROR << "data "<< data <<" is not an object of class SegmentationData";
      return MISSING_DATA;
    }
  }

  try
  {
    auto filename = getInputFile(analysis);
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      LIMA_EXCEPTION_SELECT_LOGINIT(LOGINIT("LP::AnalysisLoader"),
                                    "Cannot open" << filename,
                                    XMLException);
    if (!m_d->parse(&file))
    {
      LIMA_EXCEPTION_SELECT_LOGINIT(
        LOGINIT("LP::AnalysisLoader"),
        "Cannot parse" << filename << m_d->errorString(),
        XMLException);
    }
  }
  catch (const XMLException& )
  {
    return UNKNOWN_ERROR;
  }

  return SUCCESS_ID;
}

bool SegmentationResultsLoaderPrivate::parse(QIODevice *device)
{
  LOGINIT("LP::AnalysisLoader");
  LTRACE << "parse";
  m_reader.setDevice(device);
  readResults();
  return !m_reader.error();
}

void SegmentationResultsLoaderPrivate::readResults()
{
  LOGINIT("LP::AnalysisLoader");
  LTRACE << "SegmentationResultsLoaderPrivate::readResults";

  bool ok;
  while (m_reader.readNextStartElement())
  {
    if (m_reader.name() == QLatin1String("segment"))
    {
      LDEBUG << "SegmentationResultsLoader::XMLHandler add data "  << m_type << "," << m_position << ","
              << m_length << "," << m_graph;
      Segment s(m_type.toStdString());
      s.setVerticesFromPositions(m_position, m_length, m_graph);
      m_data->add(s);
    }
    else if (m_reader.name() == "position")
    {
      m_position = m_reader.readElementText().toInt(&ok);
      if (!ok)
      {
        m_reader.raiseError(QObject::tr("Cannot convert position value %1 to int.").arg(m_reader.text()));
      }
    }
    else if (m_reader.name() == "length")
    {
      m_length = m_reader.readElementText().toInt(&ok);
      if (!ok)
      {
        m_reader.raiseError(QObject::tr("Cannot convert length value %1 to int.").arg(m_reader.text()));
      }
    }
    else if (m_reader.name() == "type")
    {
      m_type = m_reader.readElementText();
    }
    else
    {
      m_reader.skipCurrentElement();
    }
  }
}

QString SegmentationResultsLoaderPrivate::errorString() const
{
  XMLCFGLOGINIT;
  auto errorStr = QObject::tr("%1, Line %2, column %3")
          .arg(m_reader.errorString())
          .arg(m_reader.lineNumber())
          .arg(m_reader.columnNumber());
  LERROR << errorStr;
  return errorStr;
}


} // end namespace
} // end namespace
