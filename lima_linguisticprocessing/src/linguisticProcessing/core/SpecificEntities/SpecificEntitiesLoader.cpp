// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @author     Romaric Besancon <romaric.besancon@cea.fr>
 * @date       Thu Jun 16 2011
 ***********************************************************************/

#include "SpecificEntitiesLoader.h"
#include "SpecificEntitiesConstraints.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/tools/FileUtils.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"
#include "linguisticProcessing/core/Automaton/recognizerData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include <queue>

#include <QXmlStreamReader>

using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::ApplyRecognizer;

namespace Lima {
namespace LinguisticProcessing {
namespace SpecificEntities {

SimpleFactory<MediaProcessUnit,SpecificEntitiesLoader> SpecificEntitiesLoaderFactory(SPECIFICENTITIESLOADER_CLASSID);

class SpecificEntitiesLoaderPrivate
{
  friend class SpecificEntitiesLoader;
 public:
  SpecificEntitiesLoaderPrivate();
  ~SpecificEntitiesLoaderPrivate() = default;

  bool parse(QIODevice *device);

  QString errorString() const;

  QXmlStreamReader m_reader;
  MediaId m_language;
  QString m_graphName;

//   AnalysisContent& m_analysis;
  LinguisticAnalysisStructure::AnalysisGraph* m_graph;
  uint64_t m_position;
  uint64_t m_length;
  std::string m_type;
  std::string m_string;
  std::string m_currentElement;

  // private member functions
  std::string toString(const QString& xercesString);

  void addSpecificEntity(AnalysisContent& analysis,
                          LinguisticAnalysisStructure::AnalysisGraph* graph,
                          const std::string& str,
                          const std::string& type,
                          uint64_t position,
                          uint64_t length);
};

//***********************************************************************
// constructors and destructors
SpecificEntitiesLoader::SpecificEntitiesLoader():
  m_d(new SpecificEntitiesLoaderPrivate())
{
}

SpecificEntitiesLoaderPrivate::SpecificEntitiesLoaderPrivate():
  m_language(0),
  m_graphName("AnalysisGraph")
{
}

//***********************************************************************
void SpecificEntitiesLoader::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
          Manager* manager)

{
  LOGINIT("LP::SpecificEntities");

  m_d->m_language=manager->getInitializationParameters().media;

  AnalysisLoader::init(unitConfiguration,manager);
  try {
    m_d->m_graphName = QString::fromStdString(unitConfiguration.getParamsValueAtKey("graph"));
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // keep default value

  try {
    // may need to initialize a modex, to know about the entities in external file
    auto& modexes = unitConfiguration.getListsValueAtKey("modex");
    for (const auto& modex: modexes)
    {
      LDEBUG << "loader: initialize modex " << modex;
      QString filename = Common::Misc::findFileInPaths(
        QString::fromStdString(Common::MediaticData::MediaticData::single().getConfigPath()),
        QString::fromStdString(modex));
      Common::XMLConfigurationFiles::XMLConfigurationFileParser parser(filename);
      Common::MediaticData::MediaticData::changeable().initEntityTypes(parser);
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchList& ) {
    LWARN << "loader: no modex specified in parameter: types in file loaded may not be known";
  }
}

LimaStatusCode SpecificEntitiesLoader::process(AnalysisContent& analysis) const
{
  // get analysis graph
  auto graph = static_cast<AnalysisGraph*>(analysis.getData(m_d->m_graphName.toStdString()));
  if (graph == nullptr)
  {
    LOGINIT("LP::SpecificEntities");
    LERROR << "no graph '" << m_d->m_graphName << "' available !";
    return MISSING_DATA;
  }

  //create a RecognizerData (such as in ApplyRecognizer) to be able to use
  //CreateSpecificEntity actions
  auto recoData = new RecognizerData();
  analysis.setData("RecognizerData", recoData);
  auto resultData = new RecognizerResultData(m_d->m_graphName.toStdString());
  recoData->setResultData(resultData);

  try
  {
    auto filename = getInputFile(analysis);
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      LIMA_EXCEPTION_SELECT_LOGINIT(LOGINIT("LP::SpecificEntities"),
        "Cannot open file" << filename,
        Lima::XMLException);
    if (!m_d->parse(&file))
    {
      LIMA_EXCEPTION_SELECT_LOGINIT(LOGINIT("LP::SpecificEntities"),
        "Error: failed to parse XML input file" << filename << m_d->m_reader.errorString(),
        Lima::XMLException);
    }
  }
  catch (const XMLException& )
  {
  }

  // remove recognizer data (used only internally to this process unit)
  recoData->deleteResultData();
  resultData=0;
  analysis.removeData("RecognizerData");

  return SUCCESS_ID;
}

bool SpecificEntitiesLoaderPrivate::parse(QIODevice *device)
{
  LOGINIT("LP::SpecificEntities");
  LTRACE << "parse";
  m_reader.setDevice(device);
  while (m_reader.readNextStartElement())
  {
    bool ok;
    if (m_reader.name() == QLatin1String("specific_entity"))
    {
      if (m_type != "")
      {
        LDEBUG << "SpecificEntitiesLoaderPrivate add SE "  << m_type << "," << m_position << "," << m_length << "," << m_graph;
        addSpecificEntity(m_analysis, m_graph, m_string, m_type, m_position, m_length);
      }
      m_string="";
      m_type="";
      m_position=0;
      m_length=0;
    }
    else if (m_reader.name() == QLatin1String("position"))
    {
      m_position = m_reader.readElementText().toInt(&ok);
      if (!ok)
      {
        m_reader.raiseError(QObject::tr("Cannot convert position value %1 to integer.").arg(m_reader.text()));
      }
    }
    else if (m_reader.name() == QLatin1String("length"))
    {
      m_length = m_reader.readElementText().toInt(&ok);
      if (!ok)
      {
        m_reader.raiseError(QObject::tr("Cannot convert position value %1 to integer.").arg(m_reader.text()));
      }
    }
    else if (m_reader.name() == QLatin1String("type"))
    {
      m_type = m_reader.readElementText().toStdString();
    }
    else if (m_reader.name() == QLatin1String("string"))
    {
      m_string = m_reader.readElementText().toStdString();
    }
    else
    {
      m_reader.skipCurrentElement();
    }
  }
  return !m_reader.error();
}


//***********************************************************************
void SpecificEntitiesLoaderPrivate::addSpecificEntity(
    AnalysisContent& analysis,
    LinguisticAnalysisStructure::AnalysisGraph* anagraph,
    const std::string& str,
    const std::string& type,
    uint64_t position,
    uint64_t length)
{
  LOGINIT("LP::SpecificEntities");
  LDEBUG << "loader: add entity " << str << "," << type << ",[" << position << "," << length << "]";
  // create RecognizerMatch
  Automaton::RecognizerMatch match(anagraph);

  uint64_t posBegin=position;
  uint64_t posEnd=posBegin+length;

  // find vertices corresponding to pos/len : have to go through the graph
  LinguisticGraph* graph=anagraph->getGraph();
  std::queue<LinguisticGraphVertex> toVisit;
  std::set<LinguisticGraphVertex> visited;
  LinguisticGraphOutEdgeIt outItr,outItrEnd;

  // output vertices between begin and end,
  toVisit.push(anagraph->firstVertex());

  bool first=true;
  bool inEntity=false;
  while (!toVisit.empty()) {
    LinguisticGraphVertex v=toVisit.front();
    toVisit.pop();
    if (v == anagraph->lastVertex()) {
      break;
    }

    if (first) {
      first=false;
    }
    else if (inEntity) {
      LinguisticAnalysisStructure::Token* t=get(vertex_token,*graph,v);
      if (t->position() >= posEnd) {
        inEntity=false;
        break; // no need to go further
      }
      else {
        // OME?? valeur true?,"SE"?
        match.addBackVertex(v,true,"SE");
      }
    }
    else {
      LinguisticAnalysisStructure::Token* t=get(vertex_token,*graph,v);
      if(t!=0) {
        if (t->position() == posBegin) {
          match.addBackVertex(v,true,"SE");
          inEntity=true;
        }
      }
    }

    // add next vertices
    for (boost::tie(outItr,outItrEnd)=out_edges(v,*graph); outItr!=outItrEnd; outItr++)
    {
      LinguisticGraphVertex next=target(*outItr,*graph);
      if (visited.find(next)==visited.end())
      {
        visited.insert(next);
        toVisit.push(next);
      }
    }
  }

  if (match.size()==0) {
    LWARN << "Warning: no matching vertices for given position/length";
  }

  // set entity properties
  Common::MediaticData::EntityType entityType;
  try {
    entityType = Common::MediaticData::MediaticData::single().getEntityType(QString::fromStdString(type));
  } catch (const LimaException& e) {
    LIMA_EXCEPTION("Unknown entity" << QString::fromStdString(type) << e.what());
  }
  match.setType(entityType);
  // set normalized form similar to string (otherwise, may cause problem when trying to access the created specific entity)
  match.features().setFeature(DEFAULT_ATTRIBUTE,Common::Misc::utf8stdstring2limastring(str));

  // create specific entity from RecognizerMatch using default action
  CreateSpecificEntity createEntity(m_language);
  createEntity(match,analysis);
}

QString SpecificEntitiesLoaderPrivate::errorString() const
{
  LOGINIT("LP::SpecificEntities");
  auto errorStr = QObject::tr("%1, Line %2, column %3")
          .arg(m_reader.errorString())
          .arg(m_reader.lineNumber())
          .arg(m_reader.columnNumber());
  LERROR << errorStr;
  return errorStr;
}


} // end namespace
} // end namespace
} // end namespace
