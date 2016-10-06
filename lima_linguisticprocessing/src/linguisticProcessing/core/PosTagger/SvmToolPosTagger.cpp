/*
    Copyright 2002-2013 CEA LIST

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
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/

#include "SvmToolPosTagger.h"

#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "common/tools/FileUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/time/timeUtilsController.h"
#include "svmtool/tagger.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/replace.hpp>



int verbose = FALSE;

using namespace Lima::Common::MediaticData;

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

void SvmToolPosTagger::init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
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
  m_language=manager->getInitializationParameters().media;
  const Common::MediaticData::LanguageData& ldata = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language));
  const Common::PropertyCode::PropertyManager& microManager=ldata.getPropertyCodeManager().getPropertyManager("MICRO");
  m_microAccessor=&(microManager.getPropertyAccessor());
  string resourcesPath=MediaticData::single().getResourcesPath();  
  try
  {
    m_model = Common::Misc::findFileInPaths(resourcesPath.c_str(), (unitConfiguration.getParamsValueAtKey("model")+".DICT").c_str()).toUtf8().constData();
    m_model = QString::fromUtf8(m_model.c_str()).replace(".DICT","").toUtf8().constData();
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LWARN << "No SVMTool model defined in configuration file !";
    throw InvalidConfiguration();
  }
  LDEBUG << "Creating SVM Tagger with model: " << m_model;
  erCompRegExp();
  m_tagger = new tagger(m_model.c_str());
  m_tagger->taggerLoadModelsForTagging();
  
}

LimaStatusCode SvmToolPosTagger::process(AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("SvmToolPosTagger");

  // start postagging here !
  PTLOGINIT;
  LINFO << "start SvmToolPosTager";

  // Retrieve morphosyntactic graph
  LinguisticAnalysisStructure::AnalysisGraph* anagraph=static_cast<LinguisticAnalysisStructure::AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  LinguisticGraph* srcgraph=anagraph->getGraph();
  const LinguisticGraphVertex endVx=anagraph->lastVertex();

  /// Creates the posgraph with the second parameter (deleteTokenWhenDestroyed)
  /// set to false as the tokens are owned by the anagraph
  /// @note : tokens newly created later will be owned by their creator and have
  /// to be deleted by this one
  LinguisticAnalysisStructure::AnalysisGraph* posgraph=new LinguisticAnalysisStructure::AnalysisGraph("PosGraph",m_language,false,true);
  analysis.setData("PosGraph",posgraph);

  /** Creation of an annotation graph if necessary*/
  Common::AnnotationGraphs::AnnotationData* annotationData =
  static_cast< Common::AnnotationGraphs::AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    annotationData=new Common::AnnotationGraphs::AnnotationData();
    /** Creates a node in the annotation graph for each node of the
    * morphosyntactic graph. Each new node is annotated with the name mrphv and
    * associated to the morphosyntactic vertex number */
    if (static_cast<LinguisticAnalysisStructure::AnalysisGraph*>(analysis.getData("AnalysisGraph")) != 0)
    {
      static_cast<LinguisticAnalysisStructure::AnalysisGraph*>(analysis.getData("AnalysisGraph"))->populateAnnotationGraph(annotationData, "AnalysisGraph");
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

  const Common::PropertyCode::PropertyManager& microManager = static_cast<const Common::MediaticData::LanguageData&>(MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MICRO");
  // to add tokens possible tags to the tagger dictionary
  
  
  // TODO create a wrapper on the analysis graph to read tokens in a stream
  std::ostringstream oss("");
  LinguisticGraphVertex currentVx=anagraph->firstVertex();
  std::vector< LinguisticGraphVertex > anaVertices;
  while (currentVx != endVx)
  {
    if (currentVx != 0 && tokens[currentVx] != 0)
    {
      LinguisticAnalysisStructure::Token* tok = tokens[currentVx];
      LinguisticAnalysisStructure::MorphoSyntacticData* morphoData=get(vertex_data,*srcgraph,currentVx);
      std::string stringForm = Common::Misc::limastring2utf8stdstring(tok->stringForm());
      std::string token = Common::Misc::limastring2utf8stdstring(tok->stringForm());
      boost::replace_all(token," ","_");
      std::ostringstream lineoss("");
      lineoss << token << " (";
      LinguisticAnalysisStructure::MorphoSyntacticData::const_iterator morphDataIt = morphoData->begin();
      if (morphDataIt != morphoData->end())
      {
        std::string tag = microManager.getPropertySymbolicValue((*morphDataIt).properties);
        lineoss << tag;
        morphDataIt++;
      }
      for (;morphDataIt != morphoData->end(); morphDataIt++)
      {
        std::string tag = microManager.getPropertySymbolicValue((*morphDataIt).properties);
        lineoss << "," << tag;
      }
      lineoss << ")" << std::endl;
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
  LDEBUG << "Tagging '" << oss.str() << "'";
  std::istringstream iss(oss.str());

  std::stringstream resOss;
  // TODO create a wrapper with a ostream interface to put results into the pos graph
  // TODO have to take data from the analysis graph nodes too
  
  // start processing postagging
  //m_tagger->taggerShowNoComments();
  m_tagger->taggerInit(iss, resOss);
  m_tagger->taggerRun();

  // read results
  resOss.seekp(0, std::ios::beg);
  std::string resultLine;
  std::vector<LinguisticGraphVertex>::size_type anaVerticesIndex = 0;
  LinguisticGraphVertex previousPosVertex = posgraph->firstVertex();
  
  while (anaVerticesIndex < anaVertices.size() && std::getline(resOss,resultLine))
  {
    LDEBUG << "Result line: '" << resultLine << "'";
    std::vector<std::string> elements;
    boost::split(elements,resultLine,boost::is_any_of(" "));
    if (elements.size() < 2)
    {
      LERROR << "Error in SVMTagger result line: did not get 2 elements in '"<< resultLine << "'";
      return UNKNOWN_ERROR;
    }
    LinguisticGraphVertex anaVertex = anaVertices[anaVerticesIndex];
    LinguisticAnalysisStructure::Token* currentAnaToken = tokens[anaVertex];
    std::string token = Common::Misc::limastring2utf8stdstring(currentAnaToken->stringForm());
    boost::replace_all(token," ","_");
    if (token != elements[0])
    {
      LERROR << "Error in SVMTagger result alignement with analysis graph: got '"<< elements[0] << "' from SVMTagger and '" << Common::Misc::limastring2utf8stdstring(currentAnaToken->stringForm()) << "' from graph";
      return UNKNOWN_ERROR;
    }

    LinguisticGraphVertex newVx=boost::add_vertex(*resultgraph);
    annotationData->addMatching("PosGraph", newVx, "annot", anaVertex);
    annotationData->addMatching("AnalysisGraph", anaVertex, "PosGraph", newVx);
    AnnotationGraphVertex agv =  annotationData->createAnnotationVertex();
    annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring("PosGraph"), newVx);

    // set linguistic infos
    LinguisticAnalysisStructure::MorphoSyntacticData* morphoData=get(vertex_data,*srcgraph,anaVertex);
    LinguisticAnalysisStructure::Token* srcToken=get(vertex_token,*srcgraph,anaVertex);
    if (morphoData!=0)
    {
      LinguisticAnalysisStructure::MorphoSyntacticData* posData=new LinguisticAnalysisStructure::MorphoSyntacticData();
      LinguisticAnalysisStructure::CheckDifferentPropertyPredicate differentMicro(m_microAccessor,microManager.getPropertyValue(elements[1]));
      std::back_insert_iterator<LinguisticAnalysisStructure::MorphoSyntacticData> backInsertItr(*posData);
      remove_copy_if(morphoData->begin(),morphoData->end(),backInsertItr,differentMicro);
      if (posData->empty() || morphoData->empty())
      {
        LWARN << "No matching category found for tagger result " << elements[0] << " " << elements[1];
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
  
  LINFO << "SvmToolPosTagger postagging done.";

  return SUCCESS_ID;
}


LinguisticGraphVertex SvmToolPosTagger::reportPathsInGraph(
    LinguisticGraph* srcgraph,
    LinguisticGraph* resultgraph,
    LinguisticGraphVertex startVertex,
    StepDataVector& stepData,
    Common::AnnotationGraphs::AnnotationData* annotationData) const
{
  PTLOGINIT;
  LINFO << "reportPathsInGraph";

  std::map<TargetVertexId,LinguisticGraphVertex> vertexMapping;
  typedef std::map<TargetVertexId,LinguisticGraphVertex>::iterator VertexMappingItr;

  std::queue<std::pair<LinguisticGraphVertex,PredData>,std::list< std::pair<LinguisticGraphVertex,PredData> > > toProcess;
  LinguisticGraphVertex endVertex=add_vertex(*resultgraph);
  //    LDEBUG << "add end vertex " << endVertex;
  {

    {
      StepData& endStep=stepData.back();
      if (endStep.m_microCatsData.size()!=1) {
        LWARN << "Last vertex of POSTAGGING has more than 1 categories. This should never happen!";
      }

      // put linguistic data to end vertex
      LinguisticAnalysisStructure::MorphoSyntacticData* morphoData=get(vertex_data,*srcgraph,endStep.m_srcVertex);
      LinguisticAnalysisStructure::Token* srcToken=get(vertex_token,*srcgraph,endStep.m_srcVertex);
      if (morphoData!=0)
      {
        LinguisticAnalysisStructure::MorphoSyntacticData* posData=new LinguisticAnalysisStructure::MorphoSyntacticData(*morphoData);
        put(vertex_data,*resultgraph,endVertex,posData);
        put(vertex_token,*resultgraph,endVertex,srcToken);
      } else {
        put(vertex_data,*resultgraph,endVertex,morphoData);
        put(vertex_token,*resultgraph,endVertex,srcToken);
      }
    }
  }

  while (!toProcess.empty())
  {
    PredData& current=toProcess.front().second;
    LinguisticGraphVertex succVertex=toProcess.front().first;
    //        LDEBUG << "process index " << current.m_predIndex << " and categ " << current.m_predMicro;

    if (current.m_predIndex==0) {
      // rien �cr�r, juste �connecter
      add_edge(startVertex,succVertex,*resultgraph);
      //          LDEBUG << "just add link " << startVertex << " -> " << succVertex;
    } else {

      StepData& currentStep=stepData[current.m_predIndex];

      TargetVertexId tvi;
      tvi.m_categ=current.m_predMicro;
      tvi.m_sourceVx=currentStep.m_srcVertex;
      tvi.m_preds=current.m_predPredMicros;

      // check if vertex id already in graph
      VertexMappingItr tgtVxItr=vertexMapping.find(tvi);
      if (tgtVxItr==vertexMapping.end())
      {
        //            std::ostringstream os;
        //            copy(tvi.m_preds.begin(),tvi.m_preds.end(),std::ostream_iterator<LinguisticCode>(os,","));
        //            LDEBUG << "TargetVertexID source " << tvi.m_sourceVx << ", categ " << tvi.m_categ << " | pred categs " << os.str() << " is not graph. add it";
        // if not exists create an register it
        LinguisticGraphVertex newVx=add_vertex(*resultgraph);
        //            LDEBUG << "create vertex " << newVx;
        std::pair<
        VertexMappingItr,
        bool> insertStatus=vertexMapping.insert(make_pair(tvi,newVx));
        tgtVxItr=insertStatus.first;
        AnnotationGraphVertex agv =  annotationData->createAnnotationVertex();
        annotationData->addMatching("PosGraph", newVx, "annot", agv);
        annotationData->addMatching("AnalysisGraph", tvi.m_sourceVx, "PosGraph", newVx);
        annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring("PosGraph"), newVx);
        /*std::set< LinguisticGraphVertex > annotMatches = annotationData->matches("AnalysisGraph",tvi.m_sourceVx,"PosGraph");
        for (std::set< LinguisticGraphVertex >::const_iterator annotIt(annotMatches.begin());
        annotIt != annotMatches.end(); annotIt++)
        {
          std::set< std::string > excepted;
          excepted.insert("AnalysisGraph");
          annotationData->cloneAnnotations(*annotIt, agv, excepted);
        }*/

        // set linguistic infos
        LinguisticAnalysisStructure::MorphoSyntacticData* morphoData=get(vertex_data,*srcgraph,currentStep.m_srcVertex);
        LinguisticAnalysisStructure::Token* srcToken=get(vertex_token,*srcgraph,currentStep.m_srcVertex);
        if (morphoData!=0) {
          LinguisticAnalysisStructure::MorphoSyntacticData* posData=new LinguisticAnalysisStructure::MorphoSyntacticData();
          LinguisticAnalysisStructure::CheckDifferentPropertyPredicate differentMicro(m_microAccessor,current.m_predMicro);
          std::back_insert_iterator<LinguisticAnalysisStructure::MorphoSyntacticData> backInsertItr(*posData);
          remove_copy_if(morphoData->begin(),morphoData->end(),backInsertItr,differentMicro);
          put(vertex_data,*resultgraph,newVx,posData);
          put(vertex_token,*resultgraph,newVx,srcToken);
        }

        // add pred to process
        std::vector<PredData>& pds=currentStep.m_microCatsData[current.m_predMicro];
        PredDataVectorItr pdsItr=pds.begin();
        std::vector<LinguisticCode>::const_iterator predPredMicroItr=current.m_predPredMicros.begin();
        while (pdsItr!=pds.end() && predPredMicroItr!=current.m_predPredMicros.end())
        {
          if (*predPredMicroItr < pdsItr->m_predMicro) {
            predPredMicroItr++;
          } else if (*predPredMicroItr > pdsItr->m_predMicro) {
            pdsItr++;
          } else {
            //                LDEBUG << "add PredData to visit : index " << pdsItr->m_predIndex << " micro " << pdsItr->m_predMicro;
            toProcess.push(make_pair(tgtVxItr->second,*pdsItr));
            pdsItr++;
            predPredMicroItr++;
          }
        }

      }

      // link to pred
      //          LDEBUG << "add link " << tgtVxItr->second << " -> " << succVertex;
      add_edge(tgtVxItr->second,succVertex,*resultgraph);

    }

    toProcess.pop();
  }

  LDEBUG << "end reporting paths";
  return endVertex;
}

} // PosTagger

} // LinguisticProcessing

} // Lima
