/*
    Copyright 2002-2014 CEA LIST

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
 
************************************************************************/

#include <queue>

#include "decoder.h"
#include "sequence.h"
#include "trainers.h"
#include "TaggerWap.h"

#include "common/Data/strwstrtools.h"
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



class TaggerWapPrivate {

  friend class TaggerWap;

public:
  
  TaggerWapPrivate();
  ~TaggerWapPrivate();
  TaggerWapPrivate(const TaggerWapPrivate& tw);
  TaggerWapPrivate& operator= (const TaggerWapPrivate& tw);
  
  mdl_t *m_mod;
  
};

TaggerWapPrivate::TaggerWapPrivate() {

  
}

TaggerWapPrivate::~TaggerWapPrivate() {

}

TaggerWapPrivate::TaggerWapPrivate(const TaggerWapPrivate& tw) {

  *m_mod=*(tw.m_mod);
}

TaggerWapPrivate& TaggerWapPrivate::operator= (const TaggerWapPrivate& tw) {

  *m_mod=*(tw.m_mod);
  return *this;
}


TaggerWap::TaggerWap() {
  m_tw=new TaggerWapPrivate();
 
}

TaggerWap::~TaggerWap() {
  delete m_tw;
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
  
  m_tw->m_mod=m;

}

void TaggerWap::initOptions(const std::map<std::string, std::string>& options) {
 
  std::string outputFile;
  std::map<std::string, std::string>::const_iterator it;
  
  //input output file
  it=options.find("inputFile");
  if (it!=options.end()) {
    m_tw->m_mod->opt->input=strdup(((*it).second).c_str());
    
  } 
 
}

void TaggerWap::tag(AnalysisContent& analysis, MediaId lg) {
  
	// Open input and output files
	FILE *fin = stdin, *fout = stdout;
	if (m_tw->m_mod->opt->input != NULL) {
		fin = fopen(m_tw->m_mod->opt->input, "r");
		if (fin == NULL)
		  {
		  }//pfatal("cannot open input data file");
	}
	if (m_tw->m_mod->opt->output != NULL) {
		fout = fopen(m_tw->m_mod->opt->output, "w");
		if (fout == NULL) {
		  //LERROR << "cannot open output data file" << LENDL;
		}
		  //pfatal("cannot open output data file");
	}
	// Do the labelling
	
	raw_t *r=graphToRaw(analysis, lg);

	
	listDat_t *lst=firstList(analysis, lg);
	listDat_t *ltmp;
	ltmp=tag_label2(m_tw->m_mod, fin, fout, &lst, r);

	deleteList(lst);

	// And close files
	if (m_tw->m_mod->opt->input != NULL)
		fclose(fin);
	if (m_tw->m_mod->opt->output != NULL)
		fclose(fout);
  
	listDat_t *ll=ltmp;
	// add the SpecificEntities founded

	bool isEN=false;
	int positionDebut=0;
	int lastPos=0;
	int lastLength=0;
	std::string currentEN;
	std::string currentStr;
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
		
		addSpecificEntities(analysis,lg, currentStr , currentEN, positionDebut,(lastPos+lastLength)-positionDebut);
	      } else {
		isFirst=false;
	      }
	      positionDebut=ltmp->data->pos;
	      
	      lastPos=positionDebut;
	      lastLength=ltmp->data->lgth;
	      
	      resvalue[1].replace(0, 2, "");
	      currentEN=resvalue[0]+"."+resvalue[1];
	      currentStr=ltmp->data->str;
	      isEN=true;
	    } else if (resvalue[1][0]=='I') {
	      resvalue[1].replace(0, 2, "");
	      
	      if (resvalue[0]+"."+resvalue[1]!=currentEN || (ltmp->data->pos>(lastPos+lastLength+1))) {
		// if we don't start with a B-
		if (!isFirst) {
		  addSpecificEntities(analysis,lg, currentStr , currentEN, positionDebut,(lastPos+lastLength)-positionDebut);
		} else {
		  isFirst=false;
		}
		currentEN=resvalue[0]+"."+resvalue[1];
		currentStr=currentStr+ltmp->data->str;
		positionDebut=ltmp->data->pos;
		
	      }
	      lastPos=ltmp->data->pos;
	      lastLength=ltmp->data->lgth;
	      
	    } else {
	    }
	    
	  
	    ltmp=ltmp->next;
	    
	  }
	  //add the last entity
	  addSpecificEntities(analysis,lg, currentStr , currentEN, positionDebut,(lastPos+lastLength)-positionDebut);
	}

	deleteList(ll);
	
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
   
     //only the first node is selected to avoid ambiguity
    boost::tie(outItr,outItrEnd)=out_edges(v,*graph);
    LinguisticGraphVertex next=target(*outItr,*graph);
    if (visited.find(next)==visited.end()) {  
      visited.insert(next);
      toVisit.push(next);
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
      
      
	lst=addBack(lst, createDataSE((*it).first->position(), (*it).first->length(),"",""));
     
	
    }	
  
  lst=deleteFirst(lst);
  return lst;
    	
}
      


    } // end namespace LinguisticProcessing
} // end namespace Lima
  
