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

#include "DynamicSvmToolPosTagger.h"


#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/traceUtils.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"

#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "common/Data/strwstrtools.h"
#include "common/time/timeUtilsController.h"
#include "svmtool/tagger.h"
#include "svmtool/nodo.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <boost/foreach.hpp>

#include <cfloat> // LDBL_MIN/MAX
#include <cmath> // log

using namespace Lima::Common::MediaticData;

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

/* Factory boilerplate code only used to initialize the DynamicSvmToolPosTagger module */
DynamicSvmToolPosTaggerFactory* DynamicSvmToolPosTaggerFactory::s_instance=new DynamicSvmToolPosTaggerFactory(DYNAMICSVMTOOLPOSTAGGER_CLASSID);

DynamicSvmToolPosTaggerFactory::DynamicSvmToolPosTaggerFactory(const std::string& id) :
    InitializableObjectFactory<MediaProcessUnit>(id)
{}

MediaProcessUnit* DynamicSvmToolPosTaggerFactory::create(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  MediaProcessUnit::Manager* manager) const 
{
  MediaProcessUnit* posTagger = new DynamicSvmToolPosTagger;
  posTagger->init(unitConfiguration,manager);

  return posTagger;
}

/* Retrieves the needed values from the configuration and inits the SVMTool tagger */
void DynamicSvmToolPosTagger::init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
      
{
  /** @addtogroup ProcessUnitConfiguration
  * - <b>&lt;group name="..." class="DynamicSvmToolPosTagger"&gt;</b>
  *    -  defaultCategory : micro category to use when no categories are
  *         available. For example, used before and after text to
  *         disambiguate.
  * -  model : the SVMTool model to use.
  * -  stopCategories : list of categories that delimits independant
  *         segment to disambiguate.
  */

  Lima::TimeUtilsController timer("DynamicSvmToolPosTagger init");

  PTLOGINIT;
  LDEBUG << "init!" << LENDL;

  m_language=manager->getInitializationParameters().media;
  const Common::MediaticData::LanguageData& ldata = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language));
  m_MicroManager=&(ldata.getPropertyCodeManager().getPropertyManager("MICRO"));
  m_microAccessor=&(m_MicroManager->getPropertyAccessor());
  string resourcesPath=MediaticData::single().getResourcesPath();  


  /* Retrieve the default category */
  std::string defaultName;
  try
  {
    defaultName = unitConfiguration.getParamsValueAtKey("defaultCategory");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LWARN << "No default microtageory for DynamicSvmToolPosTagger! using PONCTU_FORTE." << LENDL;
    defaultName = "PONCTU_FORTE";
  }
  m_defaultCateg = m_MicroManager->getPropertyValue(defaultName);


  /* Retrieve the stop categories */
  try
  {
    std::deque<std::string> cats=unitConfiguration.getListsValueAtKey("stopCategories");
    for (std::deque<std::string>::iterator it=cats.begin();
         it!=cats.end();
         it++)
    {
      m_stopCategories.push_back(m_MicroManager->getPropertyValue(*it));
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LWARN << "No stop categories defined! using the default category" << LENDL;
    m_stopCategories.push_back(m_defaultCateg);
  }

  /* Retrieve the SVMTool model */
  std::string model;
  try {
    model = unitConfiguration.getParamsValueAtKey("model");
  } catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    LERROR << "No SVMTool model defined in the configuration file!" << LENDL;
    throw InvalidConfiguration();
  }


  // Creates the tagger we use
  erCompRegExp();
  t = new tagger(resourcesPath + "/" + model);
  t->taggerLoadModelsForTagging();
  t->taggerShowComments();
  t->taggerActiveShowScoresFlag();
  t->taggerInit();

}

/* Actual pos-tagging code
 *
 * The goal of the DynamicSvmToolPosTagger is to apply the Viterbi algorithm
 * on top of the SVMTool code. Each word has an associated score given a
 * specific window. We try every possible window, and choose the best path
 * in our treillis. See the wiki (lima:lima:svmtool) for more details.
 *
 * Implementation note: the code below could be faster but this won't improve
 * the speed of the whole process: 99% of time and memory is spent in SVMTool++.
 */
LimaStatusCode DynamicSvmToolPosTagger::process(AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("DynamicSvmToolPosTagger");
  PTLOGINIT;

  /* This modules creates a PosGraph from an AnalysisGraph */
  LinguisticAnalysisStructure::AnalysisGraph *analysisGraph;
  LinguisticAnalysisStructure::AnalysisGraph *posGraph;
  /* We also store the actual graphs */
  LinguisticGraph *srcGraph;
  LinguisticGraph *resultGraph;

  /* First retrieve the source graph (Analysis) */
  analysisGraph = static_cast<LinguisticAnalysisStructure::AnalysisGraph*>
    (analysis.getData("AnalysisGraph"));
  srcGraph = analysisGraph->getGraph();

  /* Let's start with the "forward" algorithm
   * 
   * The goal here is to go through each node microtag to be able to tell
   * what is the best way to go throught this microtag. This will enable us
   * to simply go back from the end of the graph and choose the most
   * probable path.
   */
  std::queue<LinguisticGraphVertex> tokenQueue;
  std::set<LinguisticGraphVertex> visited;
  std::map<LinguisticGraphVertex, struct PathInfo > maxAncestor;

  /* Push every vertex coming from vertex 0 onto the "tokens to be visited" list */
  BOOST_FOREACH(LinguisticGraphVertex vertex,
      nextTokens(analysisGraph->firstVertex(), srcGraph))
  {
    tokenQueue.push(vertex);
  }

  posGraph = new LinguisticAnalysisStructure
  ::AnalysisGraph("PosGraph", m_language, false, true);
  analysis.setData("PosGraph", posGraph);

  /* For every node in the graph (this is a BFS on a treillis, ie. a topological sort) */
  while (!tokenQueue.empty()) {
    LinguisticGraphVertex vertex = tokenQueue.front();
    tokenQueue.pop();
    LDEBUG << "\n" << vertex << " -> " << getWord(vertex, srcGraph) << LENDL;


    uint64_t logMaxWeight = -LDBL_MAX;
    int maxLength = 0;

    /* For every ancestor of our node */
    std::set<LinguisticGraphVertex> previousTokens = getPreviousTokens(vertex, srcGraph);
    if(previousTokens.empty()) previousTokens.insert(posGraph->firstVertex());
    BOOST_FOREACH(LinguisticGraphVertex prevVertex, previousTokens) {

      std::string pos = "";
      uint64_t logCurWeight = log(1.0), w;
      if (vertex != 1) {
        /* Call SVMTool */
        boost::tie(pos, w) = SVMTool(srcGraph, vertex, prevVertex, maxAncestor);
        logCurWeight = log(w);
        LDEBUG << "weight = " <<  (float)logCurWeight << " -> " << pos << "(" << (float)w << ")" << LENDL;
      }


      /* find out the previous weight */
      uint64_t logPrevPrice = log(1.0);
      int prevLength = 0;
      if(maxAncestor.find(prevVertex) != maxAncestor.end()) {
        struct PathInfo prevPath = maxAncestor[prevVertex];
        logPrevPrice = prevPath.score;
        prevLength = prevPath.pathLength;
      }

      /* Did we find a better weight ? */
      if((logPrevPrice + logCurWeight) / (prevLength+1)
          > (logMaxWeight / (maxLength+1))) {
        /* update the max ancestor */
        struct PathInfo currentPath = { prevVertex, logCurWeight + logPrevPrice, pos, prevLength+1 };
        maxAncestor[vertex] = currentPath;
        logMaxWeight = logCurWeight + logPrevPrice;
        maxLength = prevLength;
        LDEBUG << "  -> " << (float)logMaxWeight << " (" << maxLength << ")" << LENDL;
      } 
    }

    LDEBUG << getWord(vertex, srcGraph) << " -> " << maxAncestor[vertex].pos << LENDL;


    /* we're only adding the vertices we never added before */
    LinguisticGraphOutEdgeIt outItr,outItrEnd;
    boost::tie(outItr,outItrEnd)=out_edges(vertex,*srcGraph);

    for (;outItr!=outItrEnd;outItr++) {
      LinguisticGraphVertex nextToken = target(*outItr,*srcGraph);

      if (visited.find(nextToken) == visited.end()) {
        tokenQueue.push(nextToken);
        visited.insert(nextToken);
      }
    }
  }

  /* Construct the stack which is going to serve as a basis to build our resultGraph */
  std::stack<boost::tuple<LinguisticGraphVertex, LinguisticCode> > chosenPath;
  LinguisticGraphVertex backVertex = 1;
  while ((backVertex = maxAncestor[backVertex].prev) != 0) {
    LinguisticCode categ = m_MicroManager->getPropertyValue(maxAncestor[backVertex].pos);
    chosenPath.push(boost::make_tuple(backVertex, categ));
  }

  /* Then start building the result graph (Pos) */
  resultGraph = posGraph->getGraph();
  // remove the edge between those two vertices will enable us to add nodes inbetween
  remove_edge(posGraph->firstVertex(),posGraph->lastVertex(),*resultGraph);

  /* Build everything needed to populate the PosGraph */
  Common::AnnotationGraphs::AnnotationData* annotationData =
  static_cast< Common::AnnotationGraphs::AnnotationData* >(analysis.getData("AnnotationData"));

  LinguisticGraphVertex previousPosVertex = 0;

  while(!chosenPath.empty()) {
    LinguisticCode code; LinguisticGraphVertex vertex;
    boost::tie(vertex, code) = chosenPath.top(); chosenPath.pop();

    LinguisticGraphVertex newVertex = boost::add_vertex(*resultGraph);
    LDEBUG << "create vertex " << newVertex << LENDL;
    annotationData->addMatching("PosGraph", newVertex, "annot", vertex);
    annotationData->addMatching("AnalysisGraph", vertex, "PosGraph", newVertex);
    AnnotationGraphVertex annotVertex = annotationData->createAnnotationVertex();
    annotationData->annotate(annotVertex, Common::Misc::utf8stdstring2limastring("PosGraph"), newVertex);

    // set linguistic infos
    LinguisticAnalysisStructure::MorphoSyntacticData* morphoData=get(vertex_data,*srcGraph, vertex);
    LinguisticAnalysisStructure::Token* srcToken=get(vertex_token,*srcGraph,vertex);
    if (morphoData!=0)
    {
      LinguisticAnalysisStructure::MorphoSyntacticData* posData=new LinguisticAnalysisStructure::MorphoSyntacticData();
      LinguisticAnalysisStructure::CheckDifferentPropertyPredicate differentMicro(m_microAccessor, code);
      std::back_insert_iterator<LinguisticAnalysisStructure::MorphoSyntacticData> backInsertItr(*posData);
      remove_copy_if(morphoData->begin(),morphoData->end(),backInsertItr,differentMicro);
      if (posData->empty() || morphoData->empty()) {
        LWARN << "No matching category found for tagger result " << getWord(vertex, srcGraph) << " " << m_MicroManager->getPropertySymbolicValue(code) << LENDL;
        if (!morphoData->empty())
        {
          LWARN << "Taking any one" << LENDL;
          posData->push_back(morphoData->front());
        }
      }
      put(vertex_data,*resultGraph,newVertex,posData);
      put(vertex_token,*resultGraph,newVertex,srcToken);
    }

    boost::add_edge(previousPosVertex, newVertex, *resultGraph);


    LDEBUG << getWord(vertex, srcGraph) << " -> " << m_MicroManager->getPropertySymbolicValue(code) << LENDL;
    previousPosVertex = newVertex;
  }

  boost::add_edge(previousPosVertex, posGraph->lastVertex(), *resultGraph);

  return SUCCESS_ID;
}

boost::tuple<std::string, uint64_t> DynamicSvmToolPosTagger::SVMTool(
    const LinguisticGraph* srcGraph,
    LinguisticGraphVertex vertex,
    LinguisticGraphVertex prevVertex,
    std::map<LinguisticGraphVertex, struct PathInfo > &maxAncestor) const {
      PTLOGINIT;
      /* We now build the window to give to SVMTool. */
      LinguisticGraphVertex prevPrevVertex = 0;
      if(maxAncestor.find(prevVertex) != maxAncestor.end()) {
        prevPrevVertex = maxAncestor[prevVertex].prev;
      } 
      std::vector<nodo*> node_context = buildContext(srcGraph, prevPrevVertex, prevVertex, vertex);
      std::vector<std::string> microsStr = getMicros(vertex, srcGraph);

      if (microsStr.empty()) {
        LERROR << getWord(vertex, srcGraph) << " has no attached microcategories" << LENDL;
        return boost::make_tuple("", LDBL_MIN);
      }


      /* Call SVMTool */
      t->sw->setWindow(node_context);
      t->setPossibles(microsStr);
      //showWindow(node_context);
      t->taggerGenerateScore(node_context[2],1);

      /* Normalize the weight */
      node_context[2]->weight += 10.0;
      node_context[2]->weight /= 20.0;

      LDEBUG << "§" << node_context[2]->pos << "|" << (float)(node_context[2]->weight) << "§" << LENDL;

      return boost::make_tuple(node_context[2]->pos, node_context[2]->weight);

}

std::vector<std::string> DynamicSvmToolPosTagger::getMicros(LinguisticGraphVertex token, const LinguisticGraph *srcGraph) const {
  CVertexDataPropertyMap dataMap = get(vertex_data, *srcGraph);
  LinguisticAnalysisStructure::MorphoSyntacticData *mdata = dataMap[token];
  const Common::PropertyCode::PropertyManager& microManager = static_cast<const Common::MediaticData::LanguageData&>(MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MICRO");

  std::set<LinguisticCode> micros;
  if (mdata == NULL) {
    micros.insert(m_defaultCateg);
  } else {
    mdata->allValues(*m_microAccessor, micros);
  }

  std::vector<std::string> microsStr;

  for(std::set<LinguisticCode>::iterator it = micros.begin(); it != micros.end(); ++it) {
    std::string tag = microManager.getPropertySymbolicValue(*it);
    microsStr.push_back(tag);
  }

  return microsStr;
}

std::set<LinguisticGraphVertex> DynamicSvmToolPosTagger::getPreviousTokens(LinguisticGraphVertex token, const LinguisticGraph *srcGraph) const {
  std::set<LinguisticGraphVertex> previous;

    LinguisticGraphInEdgeIt inItr,inItrEnd;
    boost::tie(inItr,inItrEnd) = in_edges(token, *srcGraph);
    for (;inItr!=inItrEnd;inItr++) {
      previous.insert(source(*inItr, *srcGraph));
    }

    return previous;
}

std::vector<nodo*> DynamicSvmToolPosTagger::buildContext(
    const LinguisticGraph *srcGraph,
    LinguisticGraphVertex prevPrevVertex,
    LinguisticGraphVertex prevVertex,
    LinguisticGraphVertex vertex) const
{
  /* If we were to handle languages such as arabic, we would want to improve
   * this part of the code. Indeed, the next token and the next next token
   * don't mean much in languages such as arabic. Thus, the goal would be to
   * "flatten" the next nodes: take every token with a distance of one, and
   * create a single token with all the possibles microcategories. Same thing
   * for tokens with a distance of two related to the current token.
   */

  std::string prevPrevWord = getWord(prevPrevVertex, srcGraph);
  std::string prevWord = getWord(prevVertex, srcGraph);
  std::string word = getWord(vertex, srcGraph);
  std::string nextWord = getWord(nextToken(vertex, srcGraph), srcGraph);
  std::string nextNextWord = getWord(nextToken(nextToken(vertex, srcGraph), srcGraph), srcGraph);

  std::vector<nodo*> context;
  for(int i = 0; i < 5; i++) {
    context.push_back(NULL);
  }

  static int ord_id = 0;

  if(prevPrevWord == "") {
    context[0] = NULL;
  } else {
    context[0] = new nodo;
    context[0]->ord = ord_id++;
    context[0]->wrd = prevPrevWord;
    context[0]->realWrd = prevPrevWord;
  }

  if(prevWord == "") {
    context[1] = NULL;
  } else {
    context[1] = new nodo;
    context[1]->ord = ord_id++;
    context[1]->wrd = prevWord;
    context[1]->realWrd = prevWord;
  }

  if(word == "") {
    context[2] = NULL;
  } else {
    context[2] = new nodo;
    context[2]->ord = ord_id++;
    context[2]->wrd = word;
    context[2]->realWrd = word;
  }

  if(nextWord == "") {
    context[3] = NULL;
  } else {
    context[3] = new nodo;
    context[3]->ord = ord_id++;
    context[3]->wrd = nextWord;
    context[3]->realWrd = nextWord;
  }

  if(nextNextWord == "") {
    context[4] = NULL;
  } else {
    context[4] = new nodo;
    context[4]->ord = ord_id++;
    context[4]->wrd = nextNextWord;
    context[4]->realWrd = nextNextWord;
  }

  return context;
  
}

/* Returns a word given its vertex in our treillis */
std::string DynamicSvmToolPosTagger::getWord(LinguisticGraphVertex token, const LinguisticGraph* srcGraph) const {
  if(token <= 1) {
    return "";
  } else {
    std::string word = Common::Misc::limastring2utf8stdstring(get(vertex_token, *srcGraph, token)->stringForm());
    boost::replace_all(word, " ", "_");
    return word;
  }
}

/* Returns the first following token of a given token */
LinguisticGraphVertex DynamicSvmToolPosTagger::nextToken(LinguisticGraphVertex token, const LinguisticGraph* srcGraph) const {
  std::set<LinguisticGraphVertex> tokens = nextTokens(token, srcGraph);
  return tokens.empty() ? 1 : *(tokens.begin());
}
      
/* Return every token following a given token */
std::set<LinguisticGraphVertex> DynamicSvmToolPosTagger::nextTokens(LinguisticGraphVertex token, const LinguisticGraph* srcGraph) const {
  std::set<LinguisticGraphVertex> tokens;
  LinguisticGraphOutEdgeIt outItr,outItrEnd;
  boost::tie(outItr,outItrEnd)=out_edges(token,*srcGraph);
  for(;outItr != outItrEnd; ++outItr) {
    tokens.insert(target(*outItr, *srcGraph));
  }

  return tokens;

}

} // PosTagger

} // LinguisticProcessing

} // Lima
