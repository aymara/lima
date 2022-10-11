// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "SvmToolPosTagger.h"

#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "common/tools/FileUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/time/timeUtilsController.h"
#include "svmtool/tagger.h"

#include <boost/algorithm/string.hpp>


int verbose = FALSE;

using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::Common::Misc;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::PropertyCode;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

SvmToolPosTaggerFactory* SvmToolPosTaggerFactory::s_instance=new SvmToolPosTaggerFactory(SVMTOOLPOSTAGGER_CLASSID);

SvmToolPosTaggerFactory::SvmToolPosTaggerFactory(const std::string& id) :
    InitializableObjectFactory<MediaProcessUnit>(id)
{}

MediaProcessUnit* SvmToolPosTaggerFactory::create(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  MediaProcessUnit::Manager* manager) const
{
//   PTLOGINIT;
  MediaProcessUnit* posTagger = new SvmToolPosTagger;
  posTagger->init(unitConfiguration,manager);

  return posTagger;
}

  struct PredData
  {
    PredData() :
        m_predMicro(),
        m_predIndex(),
    m_predPredMicros() {}
    LinguisticCode m_predMicro;
    uint64_t m_predIndex;
    std::vector<LinguisticCode> m_predPredMicros;
    inline bool operator<(const PredData& pd) const { return m_predMicro<pd.m_predMicro; }
  };

  typedef std::map< LinguisticCode, std::vector<PredData> > MicroCatDataMap;
  typedef std::map< LinguisticCode, std::vector<PredData> >::iterator MicroCatDataMapItr;
  typedef std::vector<PredData>::iterator PredDataVectorItr;
  typedef std::vector<PredData>::const_iterator PredDataVectorCItr;

  struct StepData
  {
    LinguisticGraphVertex m_srcVertex;
    std::vector<uint64_t> m_predStepIndexes;
    MicroCatDataMap m_microCatsData;
  };

  typedef std::vector<StepData> StepDataVector;
  typedef std::vector<StepData>::iterator StepDataVectorItr;

  struct TargetVertexId
  {
    LinguisticGraphVertex m_sourceVx;
    LinguisticCode m_categ;
    std::vector<LinguisticCode> m_preds;
    bool operator<(const TargetVertexId& tvi) const
    {
      if (m_sourceVx!=tvi.m_sourceVx) return m_sourceVx<tvi.m_sourceVx;
      if (m_categ!=tvi.m_categ) return m_categ<tvi.m_categ;
      return m_preds<tvi.m_preds;
    }
  };

class SvmToolPosTaggerPrivate
{
  friend class SvmToolPosTagger;

  SvmToolPosTaggerPrivate();
  ~SvmToolPosTaggerPrivate() = default;
  SvmToolPosTaggerPrivate(const SvmToolPosTaggerPrivate&) = delete;
  SvmToolPosTaggerPrivate& operator=(const SvmToolPosTaggerPrivate&) = delete;

  MediaId m_language;
  std::unique_ptr<tagger> m_tagger;
  std::string m_model;
  bool m_allFeatures;
  QStringList m_features;
};

SvmToolPosTaggerPrivate::SvmToolPosTaggerPrivate() :
  m_language(0),
  m_tagger(nullptr),
  m_model(),
  m_allFeatures(false),
  m_features()
{
}

SvmToolPosTagger::SvmToolPosTagger() :
  m_d(new SvmToolPosTaggerPrivate())
{
}

SvmToolPosTagger::~SvmToolPosTagger()
{
  delete m_d;
}


void SvmToolPosTagger::init(
    GroupConfigurationStructure& unitConfiguration,
    Manager* manager)

{
  /** @addtogroup ProcessUnitConfiguration
  * - <b>&lt;group name="..." class="SvmToolPosTagger"&gt;</b>
  *    -  defaultCategory : micro category to use when no categories are
  *         available. For example, used before and after text to
  *         disambiguate.
  *  -  stopCategories : list of categories that delimits independant
  *         segment to disambiguate.
  *    -  trigramFile : file containing the trigram matrix
  *    -  bigramFile : file containing the bigram matrix
  */

  PTLOGINIT;
  m_d->m_language = manager->getInitializationParameters().media;
  auto resourcesPath = MediaticData::single().getResourcesPath();
  try
  {
    auto modelName = unitConfiguration.getParamsValueAtKey("model");
    // add .DICT to find the file, remove it to get the generic model name + path
    m_d->m_model = findFileInPaths(resourcesPath.c_str(),
                                   modelName.append(".DICT").c_str()).toUtf8().constData();
    boost::replace_last(m_d->m_model,".DICT","");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LWARN << "No SVMTool model defined in configuration file !";
    throw InvalidConfiguration();
  }
  try
  {
    m_d->m_allFeatures = unitConfiguration.getBooleanParameter("allFeatures");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    // Ignored parameters allFeatures and features are optional. Then use only
    // main tag (micro category)
  }
  if (!m_d->m_allFeatures)
  {
    try
    {
      auto features = unitConfiguration.getListsValueAtKey("features");
      for (const auto& feature: features)
      {
        m_d->m_features << QString::fromUtf8(feature.c_str());
      }
    }
    catch (Common::XMLConfigurationFiles::NoSuchList& )
    {
      // Ignored parameters allFeatures and features are optional. Then use only
      // main tag (micro category)
    }
  }
  LDEBUG << "Creating SVM Tagger with model: " << m_d->m_model;
  erCompRegExp();
  m_d->m_tagger = std::make_unique<tagger>(m_d->m_model.c_str());
// //   m_d->m_taggertaggerPutBackupDictionary(const std::string& dictName);
//   m_d->m_tagger->taggerPutStrategy(0);
//   m_d->m_tagger->taggerPutFlow("RL");
//   m_d->m_tagger->taggerPutKWeightFilter(0.455);
//   m_d->m_tagger->taggerPutUWeightFilter(0.1535);
  m_d->m_tagger->taggerLoadModelsForTagging();

// WinIndex and WinLength are loaded from model.WIN at run time
//   m_d->m_tagger->taggerPutWinLength(5);
//   m_d->m_tagger->taggerPutWinIndex(2);

}

LimaStatusCode SvmToolPosTagger::process(AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("SvmToolPosTagger");

  // start postagging here !
#ifdef DEBUG_LP
  PTLOGINIT;
  LINFO << "start SvmToolPosTager";
#endif
  const auto& ldata = static_cast<const LanguageData&>(
      Common::MediaticData::MediaticData::single().mediaData(m_d->m_language));
  const auto& microManager = ldata.getPropertyCodeManager().getPropertyManager("MICRO");
  auto microAccessor = microManager.getPropertyAccessor();
  // Retrieve morphosyntactic graph
  auto anagraph = static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  auto srcgraph = anagraph->getGraph();
  auto endVx = anagraph->lastVertex();

  /// Creates the posgraph with the second parameter (deleteTokenWhenDestroyed)
  /// set to false as the tokens are owned by the anagraph
  /// @note : tokens newly created later will be owned by their creator and have
  /// to be deleted by this one
  auto posgraph = new LinguisticAnalysisStructure::AnalysisGraph("PosGraph",
                                                                 m_d->m_language,
                                                                 false,
                                                                 true);
  analysis.setData("PosGraph",posgraph);

  /** Creation of an annotation graph if necessary*/
  AnnotationData* annotationData =
  static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    annotationData = new AnnotationData();
    /** Creates a node in the annotation graph for each node of the
    * morphosyntactic graph. Each new node is annotated with the name mrphv and
    * associated to the morphosyntactic vertex number */
    if (static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph")) != 0)
    {
      static_cast<AnalysisGraph*>(
        analysis.getData("AnalysisGraph"))->populateAnnotationGraph(
          annotationData,
          "AnalysisGraph");
    }
    analysis.setData("AnnotationData",annotationData);
  }

  // if graph is empty then do nothing
  // graph is empty if it has only 2 vertices, start (0) and end (0)
  if (num_vertices(*srcgraph)<=2)
  {
    return SUCCESS_ID;
  }
  VertexTokenPropertyMap tokens = get(vertex_token, *srcgraph);

  // Create postagging graph
  LinguisticGraph* resultgraph=posgraph->getGraph();
  remove_edge(posgraph->firstVertex(),posgraph->lastVertex(),*resultgraph);

  const auto& propertyCodeManager = static_cast<const LanguageData&>(
    MediaticData::single().mediaData(m_d->m_language)).getPropertyCodeManager();
  // to add tokens possible tags to the tagger dictionary
  const auto& propertyManagers = propertyCodeManager.getPropertyManagers();

  // TODO create a wrapper on the analysis graph to read tokens in a stream
  std::ostringstream oss("");
  LinguisticGraphVertex currentVx=anagraph->firstVertex();
  std::vector< LinguisticGraphVertex > anaVertices;
  while (currentVx != endVx)
  {
    if (currentVx != 0 && tokens[currentVx] != 0)
    {
      auto tok = tokens[currentVx];
      QString token = tok->stringForm();
      // if token is a newline, the SVMToolPosTagger will fail, replace it by
      // Unicode char U+200B ZERO WIDTH SPACE
      if (token.contains('\n'))
      {
        PTLOGINIT;
        LWARN << "Error in SVMTagger. Invalid token with newline(s):" << token;
        LWARN << "Avoiding the problem but the tokenizer should be checked.";
      }
      if (token == QString::fromUtf8("\n")) token = QString::fromUtf8(u8"\u200B");
      token = token.trimmed();
      token.replace(" ", "_");
      token.replace("\n", QString::fromUtf8(u8"\u200B"));
      std::ostringstream lineoss("");
      lineoss << token.toStdString() << " (";
      auto morphoData = get(vertex_data,*srcgraph,currentVx);
      for (auto morphDataIt = morphoData->begin();
           morphDataIt != morphoData->end(); morphDataIt++)
      {
        if (morphDataIt != morphoData->begin())
        {
          lineoss << ",";
        }
        QString fullTag;
        QTextStream tagStream(&fullTag);
        auto tag = microManager.getPropertySymbolicValue(
            (*morphDataIt).properties);
        tagStream << QString::fromUtf8(tag.c_str());
        if (m_d->m_allFeatures ||!m_d->m_features.isEmpty())
        {
          QStringList features;
          for (auto propItr = propertyManagers.cbegin();
               propItr != propertyManagers.cend(); propItr++)
          {
            if (!propItr->second.getPropertyAccessor().empty((*morphDataIt).properties))
            {
              auto property = QString::fromUtf8(propItr->first.c_str());
              auto value = QString::fromUtf8(
                  propItr->second.getPropertySymbolicValue(
                      (*morphDataIt).properties).c_str());
              if (property != "MACRO" && property != "MICRO"
                  && (m_d->m_allFeatures || m_d->m_features.contains(property)))
              {
                features << QString("%1=%2").arg(property).arg(value);
              }
            }
          }
          features.sort();
          if (!features.isEmpty())
          {
            tagStream << "-";
            for (auto it = features.cbegin(); it != features.cend(); it++)
            {
              if (it != features.cbegin())
              {
                tagStream << "|";
              }
              tagStream << *it;
            }
          }
        }
        lineoss << fullTag.toUtf8().constData();;
      }
      lineoss << ")";
      lineoss << std::endl;
      oss << lineoss.str();
      anaVertices.push_back(currentVx);
    }
    LinguisticGraphOutEdgeIt it, it_end;
    boost::tie(it, it_end) = boost::out_edges(currentVx, *srcgraph);
    if (it != it_end)
    {
      currentVx = boost::target(*it, *srcgraph);
    }
    else
    {
      currentVx = endVx;
    }
  }
#ifdef DEBUG_LP
  LDEBUG << "Tagging '" << oss.str() << "'";
#endif
  std::istringstream iss(oss.str());

  std::stringstream resOss;
  // TODO create a wrapper with a ostream interface to put results into the pos graph
  // TODO have to take data from the analysis graph nodes too

  // start processing postagging
  //m_tagger->taggerShowNoComments();
  m_d->m_tagger->taggerInit(iss, resOss);
  m_d->m_tagger->taggerRun();

  // read results
  resOss.seekp(0, std::ios::beg);
  std::string resultLine;
  std::vector<LinguisticGraphVertex>::size_type anaVerticesIndex = 0;
  LinguisticGraphVertex previousPosVertex = posgraph->firstVertex();
  while (anaVerticesIndex < anaVertices.size()
          && std::getline(resOss,resultLine))
  {
#ifdef DEBUG_LP
    LDEBUG << "Result line: '" << resultLine << "'";
#endif
//     if (resultLine.empty())
//     {
//       continue;
//     }
    auto elements = QString::fromStdString(resultLine).split(" ");
    elements[0] = elements[0].trimmed();
    if (elements.size() < 2)
    {
      PTLOGINIT;
      LERROR << "Error in SVMTagger result line: did not get 2 elements in '"
              << resultLine << "'";
      LERROR << "Was tagging '" << oss.str() << "'";
      return UNKNOWN_ERROR;
    }
    auto anaVertex = anaVertices[anaVerticesIndex];
    auto currentAnaToken = tokens[anaVertex];
    QString token = currentAnaToken->stringForm();
    // if token is a newline, the SVMToolPosTagger will fail, replace it by
    // Unicode char U+200B ZERO WIDTH SPACE
    if (token.contains('\n'))
    {
      PTLOGINIT;
      LWARN << "Error in SVMTagger. Invalid token with newline(s):" << token;
      LWARN << "Avoiding the problem but the tokenizer should be checked.";
    }
    if (token == QString::fromUtf8("\n")) token = QString::fromUtf8(u8"\u200B");
    token = token.trimmed();
    token.replace(" ", "_");
    token.replace("\n", QString::fromUtf8(u8"\u200B"));
    if (token != elements[0])
    {
      PTLOGINIT;
      LERROR << "Error in SVMTagger result alignement with analysis graph: got '"
              << elements[0] << "' with tag '"<< elements[1] <<"' from SVMTagger and '"
              << token << "' from graph";
      return UNKNOWN_ERROR;
    }

    auto newVx = boost::add_vertex(*resultgraph);
    auto agv =  annotationData->createAnnotationVertex();
    annotationData->addMatching("PosGraph", newVx, "annot", agv);
    annotationData->addMatching("AnalysisGraph", anaVertex, "PosGraph", newVx);
    annotationData->annotate(agv, QString::fromUtf8("PosGraph"), newVx);

    // set linguistic infos
    auto morphoData = get(vertex_data,*srcgraph,anaVertex);
    auto srcToken = get(vertex_token,*srcgraph,anaVertex);
    if (morphoData!=0)
    {
      auto posData = new MorphoSyntacticData();
      CheckDifferentPropertyPredicate differentMicro(
          &microAccessor,
          microManager.getPropertyValue(elements[1].toStdString()));
      std::back_insert_iterator<MorphoSyntacticData> backInsertItr(*posData);
      remove_copy_if(morphoData->begin(),
                     morphoData->end(),
                     backInsertItr,
                     differentMicro);
      if (posData->empty() || morphoData->empty())
      {
        PTLOGINIT;
        LWARN << "No matching category found for tagger result "
              << elements[0] << " " << elements[1];
        if (!morphoData->empty())
        {
          LWARN << "Taking any one";
          posData->push_back(morphoData->front());
        }
      }
      put(vertex_data,*resultgraph,newVx,posData);
      put(vertex_token,*resultgraph,newVx,srcToken);
    }

    boost::add_edge(previousPosVertex, newVx, *resultgraph);

    previousPosVertex = newVx;
    anaVerticesIndex++;
  }
  boost::add_edge(previousPosVertex, posgraph->lastVertex(), *resultgraph);

#ifdef DEBUG_LP
  LINFO << "SvmToolPosTagger postagging done.";
#endif

  return SUCCESS_ID;
}

  typedef std::vector<StepData> StepDataVector;


} // PosTagger

} // LinguisticProcessing

} // Lima
