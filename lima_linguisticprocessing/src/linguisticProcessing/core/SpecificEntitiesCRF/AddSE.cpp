#include <ctype.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
//#include <stdbool.h>
//#include <stddef.h>
//#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

#include <queue>

#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/Automaton/recognizerData.h"
#include "linguisticProcessing/core/SpecificEntities/SpecificEntitiesConstraints.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"

#include "common/Data/strwstrtools.h"

#include "common/MediaProcessors/HandlerStreamBuf.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"

#include "SpecificEntitiesCRFExport.h"

#include "linguisticProcessing/core/AnalysisDumpers/TextDumper.h" // for lTokenPosition comparison function to order tokens

#include "linguisticProcessing/core/AnalysisDumpers/WordFeatureExtractor.h"

#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"

#include "AddSE.h"

using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {

  namespace LinguisticProcessing {

void addSpecificEntities(AnalysisContent& analysis, MediaId lg,  std::string str, std::string type, uint64_t pos, uint64_t length) {

AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));

  Lima::LinguisticProcessing::ApplyRecognizer::RecognizerData* recoData=new Lima::LinguisticProcessing::ApplyRecognizer::RecognizerData();
  
  analysis.setData("RecognizerData",recoData);
  Lima::LinguisticProcessing::ApplyRecognizer::RecognizerResultData* resultData=new Lima::LinguisticProcessing::ApplyRecognizer::RecognizerResultData("AnalysisGraph");
  recoData->setResultData(resultData);
 
   
  // create RecognizerMatch
  Automaton::RecognizerMatch match(anagraph);

  uint64_t posBegin=pos;
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
        match.addBackVertex(v);
      }
    }
    else {
      LinguisticAnalysisStructure::Token* t=get(vertex_token,*graph,v);
      if(t!=0) {
        if (t->position() == posBegin) {
	  
          match.addBackVertex(v);
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
    //LWARN << "Warning: no matching vertices for given position/length" << LENDL;
   
  }
 
  // set entity properties
  match.setType(Common::MediaticData::MediaticData::single().getEntityType(Common::Misc::utf8stdstring2limastring(type)));
  // set normalized form similar to string (otherwise, may cause problem when trying to access the created specific entity)

  match.features().addFeature(DEFAULT_ATTRIBUTE,Common::Misc::utf8stdstring2limastring(str));
 
  // create specific entity from RecognizerMatch using default action
  Lima::LinguisticProcessing::SpecificEntities::CreateSpecificEntity createEntity(lg);
  createEntity(match,analysis);
  analysis.removeData("RecognizerData");


    }

  } // end namespace Linguistic Processing

} // end namespace Lima 

