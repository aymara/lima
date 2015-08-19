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

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

template<typename Cost,typename CostFunction>
void PosTagger::ViterbiPosTagger<Cost,CostFunction>::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{

  PTLOGINIT;
  m_language=manager->getInitializationParameters().media;
  const Common::MediaticData::LanguageData& ldata = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language));
  const Common::PropertyCode::PropertyManager& microManager=ldata.getPropertyCodeManager().getPropertyManager("MICRO");
  m_microAccessor=&(microManager.getPropertyAccessor());
  // setting default category
  try
  {
    std::string id=unitConfiguration.getParamsValueAtKey("defaultCategory");
    m_defaultCateg=microManager.getPropertyValue(id);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LWARN << "No default microcateg category ! use category PONCTU_FORTE";
    m_defaultCateg=microManager.getPropertyValue("PONCTU_FORTE");
  }
  /* add every stop microcategoriess: they should precede the first word */
  /* in French and English this is only the full stop */
  try
  {
    std::deque<std::string> cats=unitConfiguration.getListsValueAtKey("stopCategories");
    for (std::deque<std::string>::iterator it=cats.begin();
         it!=cats.end();
         it++)
    {
      m_stopCategories.push_back(microManager.getPropertyValue(*it));
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LWARN << "No stop categories defined ! use the default category";
    m_stopCategories.push_back(m_defaultCateg);
  }

}

template<typename Cost,typename CostFunction>
LimaStatusCode ViterbiPosTagger<Cost,CostFunction>::process(
  AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("ViterbiPosTagger");
  // start postagging here !
  PTLOGINIT;
  LINFO << "start ViterbiPosTager";

  // Retrieve morphosyntactic graph
  LinguisticAnalysisStructure::AnalysisGraph* anagraph=static_cast<LinguisticAnalysisStructure::AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  LinguisticGraph* srcgraph=anagraph->getGraph();
  LinguisticGraphVertex currentVx=anagraph->firstVertex();
  LinguisticGraphVertex endVx=anagraph->lastVertex();

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

  // Create postagging graph
  LinguisticGraph* resultgraph=posgraph->getGraph();
  remove_edge(posgraph->firstVertex(),posgraph->lastVertex(),*resultgraph);


  // start processing postagging
  LinguisticGraphVertex currentResultVx=posgraph->firstVertex();
  std::list<LinguisticCode> predCats;
  predCats.push_back(m_defaultCateg);
  LinguisticCode currentResultVxMicro=m_defaultCateg;
  std::map<LinguisticGraphVertex, std::set<LinguisticCode> > splittedData;
  StepDataVector stepData;

  /**
   * Main loop.
   * We're going to process every sentence one at a time.
   * currentVx is the start of the sentence, and endVx is the end of the text.
   */
  while (currentVx!=endVx)
  {
    /**
     * Put the start of the next sentence in nextVx
     */
    LinguisticGraphVertex nextVx=anagraph->nextMainPathVertex(currentVx,
        *m_microAccessor,m_stopCategories,endVx);

    stepData.clear();
    /**
    * Order vertices and store micro-categories in stepData. 
    */
    initializeStepDataFromGraph(srcgraph,currentVx,currentResultVxMicro,
                                predCats,nextVx,stepData);

    /**
    * Associate a cost to each vertex using the Viterbi algorithm
    */
    performViterbiOnStepData(stepData);

    /**
     * Determine the best path using the computed costs
     * If some costs are equal, we could have two or more paths.
     */
    currentResultVx=reportPathsInGraph(srcgraph,resultgraph,currentResultVx,
                                       stepData,annotationData);

    // go on to next sentence
    currentVx=nextVx;

    // update current micros
    LinguisticAnalysisStructure::MorphoSyntacticData* data = get(vertex_data,
        *resultgraph, currentResultVx);
    if (data!=0) {
      if (data->empty()) {
        LERROR << "vertex " << currentResultVx << " has empty MorphoSyntacticData !";
        currentResultVxMicro=m_defaultCateg;
      } else {
        currentResultVxMicro=m_microAccessor->readValue(data->begin()->properties);
      }
    } else {
      currentResultVxMicro=m_defaultCateg;
    }

    // we want to use the last nodes of the previous sentence to predict
    // the first nodes of the next sentence, which is why we need to
    // remember about them
    predCats.clear();
    LinguisticGraphInEdgeIt inItr,inItrEnd;
    boost::tie(inItr,inItrEnd) = in_edges(currentResultVx,*resultgraph);
    for (;inItr!=inItrEnd;inItr++)
    {
      LinguisticAnalysisStructure::MorphoSyntacticData* srcVxData=get(vertex_data, *resultgraph, source(*inItr,*resultgraph));
      if (srcVxData!=0 && !srcVxData->empty()) {
        predCats.push_back( m_microAccessor->readValue(srcVxData->begin()->properties) );
      } else {
        predCats.push_back(m_defaultCateg);
      }
    }
  }

  // last currentResultVx must be the last vertex
  {
    LinguisticGraphVertex lastVx=posgraph->lastVertex();
//     LDEBUG << "replace last vertex " << currentResultVx << " by " << lastVx;
    LinguisticGraphInEdgeIt inItr,inItrEnd;
    boost::tie(inItr,inItrEnd) = in_edges(currentResultVx,*resultgraph);
    for (;inItr!=inItrEnd;inItr++)
    {
      add_edge(source(*inItr,*resultgraph),lastVx,*resultgraph);
    }
    // the only vertex to remove ;-)
    clear_vertex(currentResultVx,*resultgraph);
//remove_vertex(currentResultVx,*resultgraph);
  }

//   LDEBUG << "postagging done.";

  return SUCCESS_ID;
}


template<typename Cost,typename CostFunction>
void ViterbiPosTagger<Cost,CostFunction>::initializeStepDataFromGraph(
      const LinguisticGraph* srcgraph,
      LinguisticGraphVertex start,
      LinguisticCode startMicro,
      const std::list< LinguisticCode >& predCats,
      LinguisticGraphVertex end,
      StepDataVector& stepData) const
{
//     PTLOGINIT;
//     DEBUG << "initializeStepDataFromGraph ...";

    CVertexDataPropertyMap dataMap=get(vertex_data,*srcgraph);

    // fill info for start vertex
    // start vertex has index 0
//    LDEBUG << "index 0 : first vx : " << start;
    stepData.push_back(StepData());
    StepData& sd=stepData.back();
    sd.m_srcVertex=start;
    std::vector<PredData>& pds=sd.m_microCatsData[startMicro];
    for (std::list< LinguisticCode >::const_iterator predCategItr=predCats.begin();
         predCategItr!=predCats.end();
         predCategItr++)
         {
            pds.push_back(PredData());
            pds.back().m_predMicro=*predCategItr;
         }
    sort(pds.begin(),pds.end());

    // walk trough the graph
    std::map<LinguisticGraphVertex,std::vector<uint64_t> > predIndexes;
    std::queue<LinguisticGraphVertex,std::list<LinguisticGraphVertex> > toVisit;
    LinguisticGraphOutEdgeIt outItr,outItrEnd;
    boost::tie(outItr,outItrEnd)=out_edges(start,*srcgraph);
    for (;outItr!=outItrEnd;outItr++)
    {
      predIndexes[target(*outItr,*srcgraph)].push_back(0);
      toVisit.push(target(*outItr,*srcgraph));
    }
    while (!toVisit.empty()) {
      // retrieve vertex to examine
      LinguisticGraphVertex current=toVisit.front();
      toVisit.pop();

      // build StepData
      uint64_t currentIndex=stepData.size();
//      LDEBUG << "index " << currentIndex << " : vx " << current;
      stepData.push_back(StepData());
      StepData& sd=stepData.back();
      sd.m_srcVertex=current;
      sd.m_predStepIndexes=predIndexes[current];
      std::set<LinguisticCode> micros;
      LinguisticAnalysisStructure::MorphoSyntacticData* mdata=dataMap[current];
      if (mdata!=0) {
        mdata->allValues(*m_microAccessor,micros);
      }
      if (micros.empty())
      {
        if (current != start && current != end)
        {
          PTLOGINIT;
          LWARN << "No microcategory found for morphograph vertex " << current << " ! Use" << m_defaultCateg;
        }
        micros.insert(m_defaultCateg);
      }
      for (std::set<LinguisticCode>::const_iterator catItr=micros.begin();
           catItr!=micros.end();
           catItr++)
      {
        sd.m_microCatsData[*catItr];
      }

      // examine next vertices
      if (current!=end) {
        boost::tie(outItr,outItrEnd)=out_edges(current,*srcgraph);
        for (;outItr!=outItrEnd;outItr++)
        {
          LinguisticGraphVertex v=target(*outItr,*srcgraph);
          uint64_t deg=in_degree(v,*srcgraph);
          std::vector<uint64_t>& preds=predIndexes[v];
          preds.push_back(currentIndex);
          // only visit if we saw every predecessor
          if (preds.size()==deg) toVisit.push(v);
        }
      }
    }
    if (stepData.back().m_srcVertex!=end) {
      PTLOGINIT;
      LERROR << "Invalid Graph ! Following analysis will fail !";
    }
}


template<typename Cost,typename CostFunction>
void ViterbiPosTagger<Cost,CostFunction>::performViterbiOnStepData(StepDataVector& stepData) const
{
//  PTLOGINIT;
//  LINFO << "performViterbiOnStepData";

  // 1. foreach node of our lattice
  StepDataVectorItr stepItr=stepData.begin();
  stepItr++;
  for (;stepItr!=stepData.end();stepItr++)
  {
    // 2. foreach microdata of each node
    for (MicroCatDataMapItr microItr=stepItr->m_microCatsData.begin();
         microItr!=stepItr->m_microCatsData.end();
         microItr++)
    {
      // 3. foreach predecessor of a microdata, we look to the previous nodes
      for (std::vector<uint64_t>::const_iterator predIndexItr=stepItr->m_predStepIndexes.begin();
          predIndexItr!=stepItr->m_predStepIndexes.end();
          predIndexItr++)
      {
        StepData& predStep=stepData[*predIndexItr];

        // We're willing to store every previous microdata in microItr->second().
        // Since it should be sorted and we're worried about (premature?) optimization,
        // we put everything at the end of microItr->second(), and then use
        // C++ stdlib's inplace_merge to sort it only at the end.
        uint64_t sizeBefore=microItr->second.size();
        if (predStep.m_microCatsData.size()>0) {
          microItr->second.resize(sizeBefore + predStep.m_microCatsData.size());
          PredDataVectorItr targetPredData=microItr->second.begin() + sizeBefore;

          // 4. foreach microdata of each previous node
          for (MicroCatDataMapItr predMicroItr=predStep.m_microCatsData.begin();
              predMicroItr!=predStep.m_microCatsData.end();
              predMicroItr++,targetPredData++)
          {

            // fill half of the predData structure.
            targetPredData->m_predMicro=predMicroItr->first;
            targetPredData->m_predIndex=*predIndexItr;

            // fill the other half, along with the cost
            m_costFunction.apply(
              microItr->first,        // 1. current microdata
              *targetPredData,        // 2. previous microdata
              predMicroItr->second);  // 3. previous previous microdata. - tada, we have a trigram!
          }

          // Now that we added some data, sort everything back using the two
          // sorted range (what was already there, and what was added)
          if (sizeBefore>0)
          {
            PredDataVectorItr beginItr=microItr->second.begin();
            PredDataVectorItr endBeforeItr=microItr->second.begin() + sizeBefore;
            PredDataVectorItr endItr=microItr->second.end();

            inplace_merge(beginItr,endBeforeItr,endItr);
          }
        }
      }
    }
  }
}

template<typename Cost,typename CostFunction>
LinguisticGraphVertex ViterbiPosTagger<Cost,CostFunction>::reportPathsInGraph(
  LinguisticGraph* srcgraph,
  LinguisticGraph* resultgraph,
  LinguisticGraphVertex startVertex,
  StepDataVector& stepData,
  Common::AnnotationGraphs::AnnotationData* annotationData) const
{
//     PTLOGINIT;
//     LDEBUG << "reportPathsInGraph";


    std::map<TargetVertexId,LinguisticGraphVertex> vertexMapping;
    typedef typename std::map<TargetVertexId,LinguisticGraphVertex>::iterator VertexMappingItr;

    std::queue<std::pair<LinguisticGraphVertex,PredData>,std::list< std::pair<LinguisticGraphVertex,PredData> > > toProcess;
    LinguisticGraphVertex endVertex=add_vertex(*resultgraph);
//    LDEBUG << "add end vertex " << endVertex;
    {

      {
        StepData& endStep=stepData.back();
        if (endStep.m_microCatsData.size()!=1) {
          PTLOGINIT;
          LWARN << "Last vertex of POSTAGGING has more than 1 categories. This should never happen!";
        }
        MicroCatDataMapItr microItr=endStep.m_microCatsData.begin();

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

        // keep only better path to end;
//        LDEBUG << "search best cost";
        Cost minCost=m_costFunction.getMaximumCost();
        for (PredDataVectorItr predDataItr=microItr->second.begin();
            predDataItr!=microItr->second.end();
            predDataItr++)
        {
          if (predDataItr->m_cost < minCost) {
//            LDEBUG << "found better cost for categ " << predDataItr->m_predMicro;
            minCost=predDataItr->m_cost;
          }
        }
//        LDEBUG << "mincost = " << minCost;
        for (PredDataVectorItr predDataItr=microItr->second.begin();
            predDataItr!=microItr->second.end();
            predDataItr++)
        {
//          LDEBUG << "compare with " << predDataItr->m_cost;
          if (predDataItr->m_cost == minCost) {
//            LDEBUG << "add pred from categ " << predDataItr->m_predMicro;
            toProcess.push(std::make_pair(endVertex,*predDataItr));
          }
        }
      }
    }

    while (!toProcess.empty())
    {
        PredData& current=toProcess.front().second;
        LinguisticGraphVertex succVertex=toProcess.front().first;
//        LDEBUG << "process index " << current.m_predIndex << " and categ " << current.m_predMicro;

        if (current.m_predIndex==0) {
          // nothing to build, just connect it
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
              bool> insertStatus=vertexMapping.insert(std::make_pair(tvi,newVx));
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
                toProcess.push(std::make_pair(tgtVxItr->second,*pdsItr));
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

//     LDEBUG << "end reporting paths";
    return endVertex;
}

} // PosTagger
} // LinguisticProcessing
} // Lima
