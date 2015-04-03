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


 ***************************************************************************
 *   Copyright (C) 2004-2014 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/


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
#include "TrainerWap.h"

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


using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {

  namespace LinguisticProcessing {


TrainerWap::TrainerWap() {

  mod = mdl_new(rdr_new(false));
  opt.mode=-1;
  opt.input ="";
  opt.output="";
  opt.type="crf";
  opt.maxent=false;
  opt.algo="l-bfgs";
  opt.pattern="";
  opt.model=NULL;
  opt.devel=NULL;
  opt.rstate=NULL;
  opt.sstate=NULL;
  opt.compact=false;
  opt.sparse=NULL;
  opt.nthread=1;
  opt.jobsize=64;
  opt.maxiter=0;
  opt.rho1=0.5;
  opt.rho2=0.0001;
  opt.objwin=5;
  opt.stopwin=5;
  opt.stopeps=0.02;
  opt.lbfgs.clip=false;
  opt.lbfgs.histsz=5;
  opt.lbfgs.maxls=40;
  opt.sgdl1.eta0=0.8;
  opt.sgdl1.alpha=0.85;
  opt.bcd.kappa=1.5;
  opt.rprop.stpmin= 1e-8;
  opt.rprop.stpmax=50.0;
  opt.rprop.stpdec=0.5;
  opt.rprop.cutoff=false;
  opt.label=false;
  opt.check=false;
  opt.outsc=false;
  opt.lblpost=false;
  opt.nbest=1;
  opt.force=false;
  opt.prec=5;
  opt.all=false;
  
  mod->opt=(&opt);

}

TrainerWap::~TrainerWap() {
  mdl_free(mod);
}


void TrainerWap::setModelFile(std::string st) {

  opt.model=strdup(st.c_str());
  

}

void TrainerWap::setInputFile(std::string st) {

  opt.input = strdup(st.c_str());
  
}


void TrainerWap::setOutputFile(std::string st) {

  opt.output=strdup(st.c_str());

}

void TrainerWap::setPatternFile(std::string st) {

  opt.pattern=strdup(st.c_str());

}


void TrainerWap::initOptions(const std::map<std::string, std::string>& options) {
  
  std::map<std::string, std::string>::const_iterator it;
  std::string output;
  std::string testcomp="";

  //model file
   it=options.find("model");
  if (((*it).second).compare(testcomp)!=0  && it!=options.end()) {
    mod->opt->model=strdup(((*it).second).c_str());
  } else {
    mod->opt->model=NULL;
  }

  //input output file
  it=options.find("inputFile");
  if (it!=options.end()) {
    mod->opt->input=strdup(((*it).second).c_str());
    output=(*it).second + ".mod";
    mod->opt->output=strdup(output.c_str());
  } 

  //pattern file
  it=options.find("patternFile");
  if (((*it).second).compare(testcomp)!=0 && it!=options.end()) {
    mod->opt->pattern=strdup(((*it).second).c_str());
  } 
  
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



void TrainerWap::training() {

  if (mod->opt->maxiter == 0) {
    mod->opt->maxiter = INT_MAX;
  }
  
  // Check if the user requested the type or trainer list. If this is not
  // the case, search them in the lists.
  if (!strcmp(mod->opt->type, "list")) {
    //info("Available types of models:\n");
    std::cout << "Available types of models: " << std::endl;
    for (uint32_t i = 0; i < typ_cnt; i++) {
      //info("\t%s\n", typ_lst[i]);
      std::cout << typ_lst[i] << std::endl;
    }
    exit(EXIT_SUCCESS);
  }
  
  if (!strcmp(mod->opt->algo, "list")) {
    //info("Available training algorithms:\n");
    std::cout << "Available training algorithms:" << std::endl;
    for (uint32_t i = 0; i < trn_cnt; i++) {
      //info("\t%s\n", trn_lst[i].name);
      std::cout <<trn_lst[i].name << std::endl;
    }
    exit(EXIT_SUCCESS);
  }
  uint32_t typ, trn;
  for (typ = 0; typ < typ_cnt; typ++)
    if (!strcmp(mod->opt->type, typ_lst[typ]))
      break;
  if (typ == typ_cnt) {
    //fatal("unknown model type '%s'", mod->opt->type);
  }
  mod->type = typ;
  for (trn = 0; trn < trn_cnt; trn++)
    if (!strcmp(mod->opt->algo, trn_lst[trn].name))
      break;
  if (trn == trn_cnt) {
    //fatal("unknown algorithm '%s'", mod->opt->algo);
  }
  
  // Load a previous model to train again if specified by the user.
  if (mod->opt->model != NULL) {
    //info("* Load previous model\n");
    std::cout << "* Load previous model" << std::endl;
    FILE *file = fopen(mod->opt->model, "r");
    if (file == NULL) {
      //pfatal("cannot open input model file");
    }
    mdl_load(mod, file);
  }
  // Load the pattern file. This will unlock the database if previously
  // locked by loading a model.
  if (mod->opt->pattern != NULL) {
    std::cout << "* Load patterns" << std::endl;
    FILE *file = fopen(mod->opt->pattern, "r");
    if (file == NULL) {
      //pfatal("cannot open pattern file");
    }
    rdr_loadpat(mod->reader, file);
    fclose(file);
    qrk_lock(mod->reader->obs, false);
  }

  // Load the training data. When this is done we lock the quarks as we
  // don't want to put in the model, informations present only in the
  // devlopment set.
  std::cout << "* Load training data" << std::endl;
  FILE *file = stdin;
  if (mod->opt->input != NULL) {
    file = fopen(mod->opt->input, "r");
    if (file == NULL) {
      //pfatal("cannot open input data file");
    }
  }
  else {
    //pfatal("no input file !");
  }
  mod->train = rdr_readdat(mod->reader, file, true);
  if (mod->opt->input != NULL)
    fclose(file);
  qrk_lock(mod->reader->lbl, true);
  qrk_lock(mod->reader->obs, true);
  if (mod->train == NULL || mod->train->nseq == 0) {
    //fatal("no train data loaded");
  }
  // If present, load the development set in the model. If not specified,
  // the training dataset will be used instead.
  if (mod->opt->devel != NULL) {
    std::cout << "* Load development data" << std::endl;
    FILE *file = fopen(mod->opt->devel, "r");
    if (file == NULL) {
      //pfatal("cannot open development file");
    }
    mod->devel = rdr_readdat(mod->reader, file, true);
    fclose(file);
  }

  // Initialize the model. If a previous model was loaded, this will be
  // just a resync, else the model structure will be created.
  if (mod->theta == NULL) {
    //info("* Initialize the model\n");
    std::cout << "* Initialize the model" << std::endl;
  }
  else {
    //info("* Resync the model\n");
    std::cout << "* Resync the model" << std::endl;
  }
  mdl_sync(mod);
  // Display some statistics as we all love this.
  std::cout << "* Summary" << std::endl;
  std::cout << "    nb train: " <<  mod->train->nseq << std::endl;
  if (mod->devel != NULL) {
    std::cout << "    nb devel:   " <<  mod->devel->nseq << std::endl;
  }
  std::cout << "    nb labels:   " << mod->nlbl << std::endl;
  std::cout << "    nb blocks:    " << mod->nobs << std::endl;
  std::cout << "    nb features: " << mod->nftr << std::endl;
  // And train the model...
  std::cout << "* Train the model with " << mod->opt->algo << std::endl;
  uit_setup(mod);
  trn_lst[trn].train(mod);
  uit_cleanup(mod);
  // If requested compact the model.
  if (mod->opt->compact) {
    const uint64_t O = mod->nobs;
    const uint64_t F = mod->nftr;
    std::cout << "* Compacting the model" << std::endl;
    mdl_compact(mod);
    //std::cout << "    " << 0 - mod->nobs << " observations removed" << std::endl;
    //std::cout << "    " << F - mod->nftr << " features removed" << std::endl;
  }
  // And save the trained model
  std::cout << "* Save the model" << std::endl;
  file = stdout;
  if (mod->opt->output != NULL) {
    file = fopen(mod->opt->output, "w");
    if (file == NULL) {
      //pfatal("cannot open output model");
    }
  }
  mdl_save(mod, file);
  if (mod->opt->output != NULL)
    fclose(file);
  std::cout << "* Done" << std::endl;
 

  mdl_free(mod);

}

   

  }


}
