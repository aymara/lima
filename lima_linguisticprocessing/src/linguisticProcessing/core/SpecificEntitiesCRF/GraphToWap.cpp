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
#include <map>
#include <string>

#include "decoder.h"
//#include "model.h"
#include "options.h"
#include "progress.h"
#include "quark.h"
#include "reader.h"
#include "sequence.h"
//#include "tools.h"
#include "trainers.h"
#include "TaggerWap.h"

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

#include "GraphToWap.h"

using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {

  namespace LinguisticProcessing {

/**
     @brief Separe a token composed of more than one word
   */
static std::vector<std::string> separeToken(const std::string& line) {
  
  uint i;
  std::string res = "";
  std::vector<std::string> resLine;
  for (i=0; i<line.size(); i++) {
    if (isspace(line[i]) ) {
      if (res.size()!=0) {
	resLine.push_back(res);
      }
      res="";
    }
    else {
      res.push_back(line[i]);
    }
  }
  //add last element if necessary
  if (res.size()>0) {
    resLine.push_back(res);
  }
  return resLine;
}




raw_t* graphToRaw(AnalysisContent& analysis, MediaId lg) {
  
  FILE *file;
  uint32_t size = 32, cnt = 0;
  raw_t *raw =(raw_t*)malloc(sizeof(raw_t) + sizeof(char *) * size);
	// And read the next sequence in the file, this will skip any blank line
	// before reading the sequence stoping at end of file or on a new blank
	// line.


  

  // parcours

  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  LinguisticGraph* graph=anagraph->getGraph();
  LinguisticGraphVertex lastVertex=anagraph->lastVertex();
  std::map<Token*, std::vector<LinguisticGraphVertex>, Lima::LinguisticProcessing::AnalysisDumpers::lTokenPosition> sortedTokens;
  
  std::queue<LinguisticGraphVertex> toVisit;
  std::set<LinguisticGraphVertex> visited;
  
  LinguisticGraphOutEdgeIt outItr,outItrEnd;
  
  std::string word, prop, linestr;

 
  const Common::PropertyCode::PropertyAccessor* propertyAccessor;
  const Common::PropertyCode::PropertyManager* propertyManager;
  std::string propertyName="MICRO";
  const Common::PropertyCode::PropertyCodeManager& codeManager=
  static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(lg)).getPropertyCodeManager();
  propertyAccessor=&codeManager.getPropertyAccessor(propertyName);
  propertyManager=&codeManager.getPropertyManager(propertyName);

  // output vertices between begin and end,
  // but do not include begin (beginning of text or previous end of sentence) and include end (end of sentence)
  
  toVisit.push(anagraph->firstVertex());
  bool first=true;
  bool last=false;
  while (!toVisit.empty()) {
    LinguisticGraphVertex v=toVisit.front();
    toVisit.pop();
    if (last || v == lastVertex) {
      continue;
    }
    if (v == anagraph->lastVertex()) {
      last=true;
    }
    
    for (boost::tie(outItr,outItrEnd)=out_edges(v,*graph); outItr!=outItrEnd; outItr++) 
      {
	LinguisticGraphVertex next=target(*outItr,*graph);
	if (visited.find(next)==visited.end())
	  {
	    visited.insert(next);
	    toVisit.push(next);
	  }
      }
    
    if (first) {
      first=false;
    }
    else {
      Token* t=get(vertex_token,*graph,v);
      if( t!=0) {
	sortedTokens[t].push_back(v);
      }
    }
  }
  
  for (std::map< Token*,std::vector<LinguisticGraphVertex>,Lima::LinguisticProcessing::AnalysisDumpers::lTokenPosition >::const_iterator
	 it=sortedTokens.begin(),it_end=sortedTokens.end(); it!=it_end; it++)
    {
      if ((*it).second.size()==0) {
	continue;
      }
      
      // if several interpretation of the token (i.e several LinguisticGraphVertex associated),
      // it is not a specific entity, => then just print all interpretations
      else if ((*it).second.size()>1) {
	 // same as if <=1, to modify ?
	 
	  // word
	word= Common::Misc::limastring2utf8stdstring((*it).first->stringForm());
	  // property
	MorphoSyntacticData* data=get(vertex_data,*(anagraph->getGraph()),(*it).second[0]);
	if (data==0) {
	  //return;
	}
	std::ostringstream oss;
	// take first / take all ?
	for (MorphoSyntacticData::const_iterator it=data->begin(),it_end=data->end();it!=it_end;it++) {
	 
	  LinguisticCode code=propertyAccessor->readValue((*it).properties);
	  oss << propertyManager->getPropertySymbolicValue(code);
	  break;
	}
	prop= oss.str();
	if (prop=="") {
	  prop="L-NAN";
	}

      }
      else {
	
	// word
	word= Common::Misc::limastring2utf8stdstring((*it).first->stringForm());

	// property
	MorphoSyntacticData* data=get(vertex_data,*(anagraph->getGraph()),(*it).second[0]);
	if (data==0) {
	  //return;
	}
	std::ostringstream oss;
	// take first / take all ?
	for (MorphoSyntacticData::const_iterator it=data->begin(),it_end=data->end();it!=it_end;it++) {
	 
	  LinguisticCode code=propertyAccessor->readValue((*it).properties);
	  oss << propertyManager->getPropertySymbolicValue(code);
	  break;
	}
	prop= oss.str();
	if (prop=="") {
	  prop="L-NAN";
	}
      }
      
      std::vector<std::string> lineVec;
      lineVec=separeToken(word);
      std::string partWord;
      for (int k=0; k<lineVec.size(); k++) {
	partWord=lineVec[k];
	
	
	linestr=partWord+" "+prop+" NAN";
	char *line=strdup(linestr.c_str());
	if (line == NULL)
	  break;
	// Check for empty line marking the end of the current sequence
	int len = strlen(line);
	while (len != 0 && isspace(line[len - 1]))
	  len--;
	if (len == 0) {
	  free(line);
	  // Special case when no line was already read, we try
	  // again. This allow multiple blank lines beetwen
	  // sequences.
	  if (cnt == 0)
	    continue;
	  break;
	}
	// Next, grow the buffer if needed and add the new line in it
	if (size == cnt) {
	  size *= 1.4;
	  raw =(raw_t*) realloc(raw, sizeof(raw_t)
				+ sizeof(char *) * size);
	}
	raw->lines[cnt++] = line;
	// In autouni mode, there will be only unigram features so we
	// can use small sequences to improve multi-theading.
	//if (rdr->autouni) 
	//  break;
      }
      
    }
  
  //End parcours
  
  
  // If no lines was read, we just free allocated memory and return NULL
  // to signal the end of file to the caller. Else, we adjust the object
  // size and return it.
  if (cnt == 0) {
    free(raw);
    return NULL;
  }
  raw =(raw_t*) realloc(raw, sizeof(raw_t) + sizeof(char *) * cnt);
  raw->len = cnt;
  
  
  
  return raw;
  
}
    


raw_t* graphToRawTrain(AnalysisContent& analysis, MediaId lg) {
  
  FILE *file;
  uint32_t size = 32, cnt = 0;
  raw_t *raw =(raw_t*)malloc(sizeof(raw_t) + sizeof(char *) * size);
	// And read the next sequence in the file, this will skip any blank line
	// before reading the sequence stoping at end of file or on a new blank
	// line.


  

  // parcours
 AnalysisGraph* posgraph=static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  if (posgraph==0) {
    std::cout<< "POSGRAPH NULL" << std::endl;
  }
  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  LinguisticGraph* graph=posgraph->getGraph();
  LinguisticGraphVertex lastVertex=posgraph->lastVertex();
  std::map<Token*, std::vector<LinguisticGraphVertex>, Lima::LinguisticProcessing::AnalysisDumpers::lTokenPosition> sortedTokens;
  
  std::queue<LinguisticGraphVertex> toVisit;
  std::set<LinguisticGraphVertex> visited;
  
  LinguisticGraphOutEdgeIt outItr,outItrEnd;
  
  std::string word, prop, specEntity, linestr;

 
  const Common::PropertyCode::PropertyAccessor* propertyAccessor;
  const Common::PropertyCode::PropertyManager* propertyManager;
  std::string propertyName="MICRO";
  const Common::PropertyCode::PropertyCodeManager& codeManager=
  static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(lg)).getPropertyCodeManager();
  propertyAccessor=&codeManager.getPropertyAccessor(propertyName);
  propertyManager=&codeManager.getPropertyManager(propertyName);

  // output vertices between begin and end,
  // but do not include begin (beginning of text or previous end of sentence) and include end (end of sentence)
  
  toVisit.push(anagraph->firstVertex());
  bool first=true;
  bool last=false;
  while (!toVisit.empty()) {
    LinguisticGraphVertex v=toVisit.front();
    toVisit.pop();
    if (last || v == lastVertex) {
      continue;
    }
    if (v == anagraph->lastVertex()) {
      last=true;
    }
    
    for (boost::tie(outItr,outItrEnd)=out_edges(v,*graph); outItr!=outItrEnd; outItr++) 
      {
	LinguisticGraphVertex next=target(*outItr,*graph);
	if (visited.find(next)==visited.end())
	  {
	    visited.insert(next);
	    toVisit.push(next);
	  }
      }
    
    if (first) {
      first=false;
    }
    else {
      Token* t=get(vertex_token,*graph,v);
      if( t!=0) {
	sortedTokens[t].push_back(v);
      }
    }
  }
  
  for (std::map< Token*,std::vector<LinguisticGraphVertex>,Lima::LinguisticProcessing::AnalysisDumpers::lTokenPosition >::const_iterator
	 it=sortedTokens.begin(),it_end=sortedTokens.end(); it!=it_end; it++)
    {
      if ((*it).second.size()==0) {
	continue;
      }
      
      // if several interpretation of the token (i.e several LinguisticGraphVertex associated),
      // it is not a specific entity, => then just print all interpretations
      else if ((*it).second.size()>1) {
	  // word
	word= Common::Misc::limastring2utf8stdstring((*it).first->stringForm());
	  // property
	MorphoSyntacticData* data=get(vertex_data,*(posgraph->getGraph()),(*it).second[0]);
	if (data==0) {
	  //return;
	}
	std::ostringstream oss;
	// take first / take all ?
	for (MorphoSyntacticData::const_iterator it=data->begin(),it_end=data->end();it!=it_end;it++) {
	 
	  LinguisticCode code=propertyAccessor->readValue((*it).properties);
	  oss << propertyManager->getPropertySymbolicValue(code);
	  break;
	}
	prop= oss.str();
	if (prop=="") {
	  prop="L-NAN";
	}



      }
      else {
	// word
	word= Common::Misc::limastring2utf8stdstring((*it).first->stringForm());
	// property
	MorphoSyntacticData* data=get(vertex_data,*(posgraph->getGraph()),(*it).second[0]);
	if (data==0) {
	  //return;
	}
	std::ostringstream oss;
	// take first / take all ?
	for (MorphoSyntacticData::const_iterator it=data->begin(),it_end=data->end();it!=it_end;it++) {
	 
	  LinguisticCode code=propertyAccessor->readValue((*it).properties);
	  oss << propertyManager->getPropertySymbolicValue(code);
	  break;
	}
	prop= oss.str();
	if (prop=="") {
	  prop="L-NAN";
	}

	//Specific Entity
	std::string typeName("");
	std::map<std::string, std::string>::const_iterator itMSS;
	int isPresent;
	
	LinguisticGraphVertex lgvx=(*it).second[0];

	Lima::Common::AnnotationGraphs::AnnotationData* annot = static_cast< Lima::Common::AnnotationGraphs::AnnotationData* >(analysis.getData("AnnotationData"));
	std::set< AnnotationGraphVertex > anaVertices = annot->matches("PosGraph",(*it).second[0],"AnalysisGraph");
	if (anaVertices.size()==0) {
	  specEntity= "NAN" ;
	} 
	// note: anaVertices size should be 0 or 1
	for (std::set< AnnotationGraphVertex >::const_iterator anaVerticesIt = anaVertices.begin();
	     anaVerticesIt != anaVertices.end(); anaVerticesIt++)
	  {
	    std::set< AnnotationGraphVertex > matches = annot->matches("AnalysisGraph",*anaVerticesIt,"annot");
	    for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
		 it != matches.end(); it++)
	      {
		AnnotationGraphVertex vx=*it;
		if (annot->hasAnnotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
		  {
		    const SpecificEntities::SpecificEntityAnnotation* se = annot->annotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")).pointerValue<SpecificEntities::SpecificEntityAnnotation>();
		    try {
		      LimaString str= Common::MediaticData::MediaticData::single().getEntityName(se->getType());
		      typeName=Common::Misc::limastring2utf8stdstring(str);
		      specEntity=Common::Misc::limastring2utf8stdstring(str);
		      Token* token=get(vertex_token,*(posgraph->getGraph()),lgvx);
		    }
		    catch (std::exception& ) {
		      DUMPERLOGINIT;
		      LERROR << "Undefined entity type " << se->getType() << LENDL;
		    
		    }
		    
		    
		  } else {
		  // we didn't find any SE
		  specEntity= "NAN";
		}
	      }
	  }


      }
      
      std::vector<std::string> lineVec;
      lineVec=separeToken(word);
      std::string partWord;
      for (int k=0; k<lineVec.size(); k++) {
	partWord=lineVec[k];
	
	linestr=partWord+" "+prop+" "+specEntity;
	char *line=strdup(linestr.c_str());
	if (line == NULL)
	  break;
	// Check for empty line marking the end of the current sequence
	int len = strlen(line);
	while (len != 0 && isspace(line[len - 1]))
	  len--;
	if (len == 0) {
	  free(line);
	  // Special case when no line was already read, we try
	  // again. This allow multiple blank lines beetwen
	  // sequences.
			
	  if (cnt == 0)
	    continue;
	  break;
	}
	// Next, grow the buffer if needed and add the new line in it
	if (size == cnt) {
	  size *= 1.4;
	  raw =(raw_t*) realloc(raw, sizeof(raw_t)
				+ sizeof(char *) * size);
	}
	raw->lines[cnt++] = line;
	// In autouni mode, there will be only unigram features so we
	// can use small sequences to improve multi-theading.
	//if (rdr->autouni) 
	//  break;
      }
      
    }
  
  // End parcours
	
      
      // If no lines was read, we just free allocated memory and return NULL
      // to signal the end of file to the caller. Else, we adjust the object
      // size and return it.
      if (cnt == 0) {
	free(raw);
	return NULL;
      }
      raw =(raw_t*) realloc(raw, sizeof(raw_t) + sizeof(char *) * cnt);
      raw->len = cnt;
      

      return raw;
      
}





    
}

}
