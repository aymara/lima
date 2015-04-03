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
//#include <common/LimaCommon.h> 
#include "SpecificEntitiesCRFExport.h"

#include "linguisticProcessing/core/AnalysisDumpers/TextDumper.h" // for lTokenPosition comparison function to order tokens

#include "linguisticProcessing/core/AnalysisDumpers/WordFeatureExtractor.h"

#include "GraphToWap.h"
#include "AddSE.h"

using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
//using namespace Lima::LinguisticProcessing::Automaton;

namespace Lima
{

namespace LinguisticProcessing
{


TaggerWap::TaggerWap() {

 
}

TaggerWap::~TaggerWap() {
 
}
 
  
static const char *typ_lst[] = {
	"maxent",
	"memm",
	"crf"
};

static const uint32_t typ_cnt = sizeof(typ_lst) / sizeof(typ_lst[0]);


static const struct {
	char *name;
	void (* train)(mdl_t *mdl);
} trn_lst[] = {
	{"l-bfgs", trn_lbfgs},
	{"sgd-l1", trn_sgdl1},
	{"bcd",    trn_bcd  },
	{"rprop",  trn_rprop},
	{"rprop+", trn_rprop},
	{"rprop-", trn_rprop},
};


static const uint32_t trn_cnt = sizeof(trn_lst) / sizeof(trn_lst[0]);

 

void TaggerWap::setMod(mdl_t *m) {
  
  m_mod=m;

}

void TaggerWap::initOptions(const std::map<std::string, std::string>& options) {
 
  std::string outputFile;
  std::map<std::string, std::string>::const_iterator it;
  //model
  //it=options.find("model");
  //if (it!=options.end()) {
  //m_mod->opt->model=strdup(((*it).second).c_str());
  //} 
  
  //input output file
  it=options.find("inputFile");
  if (it!=options.end()) {
    m_mod->opt->input=strdup(((*it).second).c_str());
    //outputFile=(*it).second + ".tag";
    //m_mod->opt->output=strdup(outputFile.c_str());
  } 
 
}

void TaggerWap::tag(AnalysisContent& analysis, MediaId lg) {

	// Open input and output files
	FILE *fin = stdin, *fout = stdout;
	if (m_mod->opt->input != NULL) {
		fin = fopen(m_mod->opt->input, "r");
		if (fin == NULL)
		  {
		  }//pfatal("cannot open input data file");
	}
	if (m_mod->opt->output != NULL) {
		fout = fopen(m_mod->opt->output, "w");
		if (fout == NULL) {
		  //LERROR << "cannot open output data file" << LENDL;
		}
		  //pfatal("cannot open output data file");
	}
	// Do the labelling
	//info("* Label sequences\n");
	//std::cout << "* Label sequences" << std::endl;

	raw_t *r=graphToRaw(analysis, lg);

	
	listDat_t *lst=firstList(analysis, lg);
	listDat_t *ltmp;
	ltmp=tag_label2(m_mod, fin, fout, &lst, r);

	deleteList(lst);
	//std::cout << "* Done " << std::endl;
	// And close files
	if (m_mod->opt->input != NULL)
		fclose(fin);
	if (m_mod->opt->output != NULL)
		fclose(fout);
  
	listDat_t *ll=ltmp;
	// add the SpecificEntities founded

	bool isEN=false;
	int positionDebut=0;
	int lastPos=0;
	int lastLength=0;
	std::string currentEN;
	bool isFirst=true;

	if (ltmp!=NULL) {
	  while (ltmp!=NULL) {
	    
	    std::istringstream iss(ltmp->data->type ); 
	    std::string mot; 
	    std::vector<std::string> resvalue;
	    while ( std::getline( iss, mot, '.' ) ) {
	      resvalue.push_back(mot); 
	    } 
	    if (resvalue[1][0]=='B') {
	      if (!isFirst) {
		
		addSpecificEntities(analysis,lg, currentEN , currentEN, positionDebut,(lastPos+lastLength)-positionDebut);
	      } else {
		isFirst=false;
	      }
	      positionDebut=ltmp->data->pos;
	      
	      lastPos=positionDebut;
	      lastLength=ltmp->data->lgth;
	      
	      resvalue[1].replace(0, 2, "");
	      currentEN=resvalue[0]+"."+resvalue[1];
	      
	      isEN=true;
	    } else if (resvalue[1][0]=='I') {
	      resvalue[1].replace(0, 2, "");
	      
	      if (resvalue[0]+"."+resvalue[1]!=currentEN || (ltmp->data->pos>(lastPos+lastLength+1))) {
		// if we don't start with a B-
		if (!isFirst) {
		  addSpecificEntities(analysis,lg, currentEN , currentEN, positionDebut,(lastPos+lastLength)-positionDebut);
		} else {
		  isFirst=false;
		}
		currentEN=resvalue[0]+"."+resvalue[1];
		positionDebut=ltmp->data->pos;
		
	      }
	      lastPos=ltmp->data->pos;
	      lastLength=ltmp->data->lgth;
	      
	    } else {
	    }
	    
	  
	    ltmp=ltmp->next;
	    
	  }
	  //add the last entity
	  addSpecificEntities(analysis,lg, currentEN , currentEN, positionDebut,(lastPos+lastLength)-positionDebut);
	}

	deleteList(ll);
	
}
  

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



listDat_t* TaggerWap::firstList(AnalysisContent& analysis, MediaId lg) {
  
  listDat_t *lst=createList(createDataSE(0,0,"", ""));
  
  
  // parcours
  
  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  LinguisticGraph* graph=anagraph->getGraph();
  LinguisticGraphVertex lastVertex=anagraph->lastVertex();
  std::map<Token*, std::vector<LinguisticGraphVertex>, Lima::LinguisticProcessing::AnalysisDumpers::lTokenPosition> sortedTokens;
  
  std::queue<LinguisticGraphVertex> toVisit;
  std::set<LinguisticGraphVertex> visited;
  
  LinguisticGraphOutEdgeIt outItr,outItrEnd;
  
  std::string word, prop, linestr;

  int cb=0;

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

      }
      else {

	// word
	word= Common::Misc::limastring2utf8stdstring((*it).first->stringForm());
	
      }
      
      std::vector<std::string> lineVec;
      lineVec=separeToken(word);
      std::string partWord;
      if (lineVec.size()<2) {
	lst=addBack(lst, createDataSE((*it).first->position(), (*it).first->length(),"",""));
      } else {
	int currentPos=(*it).first->position();
	for (int k=0; k<lineVec.size(); k++) {
	  partWord=lineVec[k];
	  lst=addBack(lst, createDataSE(currentPos, partWord.size(),"partWord","partWord"));
	  currentPos+=partWord.size()+1;
	}
      }
      
	
    }	
  
  lst=deleteFirst(lst);
  return lst;
    	
}
      


    } // end namespace LinguisticProcessing
} // end namespace Lima
  
