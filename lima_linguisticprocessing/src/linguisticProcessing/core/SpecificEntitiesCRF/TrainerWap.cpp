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

#include <queue>

#include "progress.h"
#include "trainers.h"
#include "TrainerWap.h"

#include "common/Data/strwstrtools.h"

#include "SpecificEntitiesCRFExport.h"

#include "linguisticProcessing/core/AnalysisDumpers/WordFeatureExtractor.h"

#include "GraphToWap.h"


using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {

namespace LinguisticProcessing {

class TrainerWapPrivate {

  friend class TrainerWap;
public:
  TrainerWapPrivate();
  ~TrainerWapPrivate();
  TrainerWapPrivate(const TrainerWapPrivate& tw);
  TrainerWapPrivate& operator= (const TrainerWapPrivate& tw);
  
  mdl_t *m_mod;
  opt_t m_opt;

};


TrainerWapPrivate::TrainerWapPrivate() {
  m_mod = mdl_new(rdr_new(false));
  m_opt.mode=-1;
  m_opt.input ="";
  m_opt.output="";
  m_opt.type="crf";
  m_opt.maxent=false;
  m_opt.algo="l-bfgs";
  m_opt.pattern="";
  m_opt.model=NULL;
  m_opt.devel=NULL;
  m_opt.rstate=NULL;
  m_opt.sstate=NULL;
  m_opt.compact=false;
  m_opt.sparse=NULL;
  m_opt.nthread=1;
  m_opt.jobsize=64;
  m_opt.maxiter=0;
  m_opt.rho1=0.5;
  m_opt.rho2=0.0001;
  m_opt.objwin=5;
  m_opt.stopwin=5;
  m_opt.stopeps=0.02;
  m_opt.lbfgs.clip=false;
  m_opt.lbfgs.histsz=5;
  m_opt.lbfgs.maxls=40;
  m_opt.sgdl1.eta0=0.8;
  m_opt.sgdl1.alpha=0.85;
  m_opt.bcd.kappa=1.5;
  m_opt.rprop.stpmin= 1e-8;
  m_opt.rprop.stpmax=50.0;
  m_opt.rprop.stpdec=0.5;
  m_opt.rprop.cutoff=false;
  m_opt.label=false;
  m_opt.check=false;
  m_opt.outsc=false;
  m_opt.lblpost=false;
  m_opt.nbest=1;
  m_opt.force=false;
  m_opt.prec=5;
  m_opt.all=false;
  
  m_mod->opt=(&m_opt);

}

TrainerWapPrivate::~TrainerWapPrivate() {
  mdl_free(m_mod);
}

TrainerWapPrivate::TrainerWapPrivate(const TrainerWapPrivate& tw) {
  m_opt=tw.m_opt;
  *m_mod=*(tw.m_mod);
}

TrainerWapPrivate& TrainerWapPrivate::operator= (const TrainerWapPrivate& tw) {
  if (this!=&tw) {
    m_opt=tw.m_opt;
    *m_mod=*(tw.m_mod);
  }
  return *this;
}


TrainerWap::TrainerWap() {
 
  m_tw=new TrainerWapPrivate();

}

TrainerWap::~TrainerWap() {
  delete m_tw;
}



void TrainerWap::initOptions(const std::map<std::string, std::string>& options) {
  
  std::map<std::string, std::string>::const_iterator it;
  std::string output;
  std::string testcomp="";

  //model file
   it=options.find("model");
  if (((*it).second).compare(testcomp)!=0  && it!=options.end()) {
    m_tw->m_mod->opt->model=strdup(((*it).second).c_str());
  } else {
    m_tw->m_mod->opt->model=NULL;
  }

  //input output file
  it=options.find("inputFile");
  if (it!=options.end()) {
    m_tw->m_mod->opt->input=strdup(((*it).second).c_str());
    output=(*it).second + ".mod";
    m_tw->m_mod->opt->output=strdup(output.c_str());
  } 

  //pattern file
  it=options.find("patternFile");
  if (((*it).second).compare(testcomp)!=0 && it!=options.end()) {
    m_tw->m_mod->opt->pattern=strdup(((*it).second).c_str());
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

  if (m_tw->m_mod->opt->maxiter == 0) {
    m_tw->m_mod->opt->maxiter = INT_MAX;
  }
  
  // Check if the user requested the type or trainer list. If this is not
  // the case, search them in the lists.
  if (!strcmp(m_tw->m_mod->opt->type, "list")) {
    //info("Available types of models:\n");
    std::cout << "Available types of models: " << std::endl;
    for (uint32_t i = 0; i < typ_cnt; i++) {
      //info("\t%s\n", typ_lst[i]);
      std::cout << typ_lst[i] << std::endl;
    }
    exit(EXIT_SUCCESS);
  }
  
  if (!strcmp(m_tw->m_mod->opt->algo, "list")) {
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
    if (!strcmp(m_tw->m_mod->opt->type, typ_lst[typ]))
      break;
  if (typ == typ_cnt) {
    //fatal("unknown model type '%s'", mod->opt->type);
  }
  m_tw->m_mod->type = typ;
  for (trn = 0; trn < trn_cnt; trn++)
    if (!strcmp(m_tw->m_mod->opt->algo, trn_lst[trn].name))
      break;
  if (trn == trn_cnt) {
    //fatal("unknown algorithm '%s'", mod->opt->algo);
  }
  
  // Load a previous model to train again if specified by the user.
  if (m_tw->m_mod->opt->model != NULL) {
    //info("* Load previous model\n");
    std::cout << "* Load previous model" << std::endl;
    FILE *file = fopen(m_tw->m_mod->opt->model, "r");
    if (file == NULL) {
      //pfatal("cannot open input model file");
    }
    mdl_load(m_tw->m_mod, file);
  }
  // Load the pattern file. This will unlock the database if previously
  // locked by loading a model.
  if (m_tw->m_mod->opt->pattern != NULL) {
    std::cout << "* Load patterns" << std::endl;
    FILE *file = fopen(m_tw->m_mod->opt->pattern, "r");
    if (file == NULL) {
      //pfatal("cannot open pattern file");
    }
    rdr_loadpat(m_tw->m_mod->reader, file);
    fclose(file);
    qrk_lock(m_tw->m_mod->reader->obs, false);
  }

  // Load the training data. When this is done we lock the quarks as we
  // don't want to put in the model, informations present only in the
  // devlopment set.
  std::cout << "* Load training data" << std::endl;
  FILE *file = stdin;
  if (m_tw->m_mod->opt->input != NULL) {
    file = fopen(m_tw->m_mod->opt->input, "r");
    if (file == NULL) {
      //pfatal("cannot open input data file");
    }
  }
  else {
    //pfatal("no input file !");
  }
  m_tw->m_mod->train = rdr_readdat(m_tw->m_mod->reader, file, true);
  if (m_tw->m_mod->opt->input != NULL)
    fclose(file);
  qrk_lock(m_tw->m_mod->reader->lbl, true);
  qrk_lock(m_tw->m_mod->reader->obs, true);
  if (m_tw->m_mod->train == NULL || m_tw->m_mod->train->nseq == 0) {
    //fatal("no train data loaded");
  }
  // If present, load the development set in the model. If not specified,
  // the training dataset will be used instead.
  if (m_tw->m_mod->opt->devel != NULL) {
    std::cout << "* Load development data" << std::endl;
    FILE *file = fopen(m_tw->m_mod->opt->devel, "r");
    if (file == NULL) {
      //pfatal("cannot open development file");
    }
    m_tw->m_mod->devel = rdr_readdat(m_tw->m_mod->reader, file, true);
    fclose(file);
  }

  // Initialize the model. If a previous model was loaded, this will be
  // just a resync, else the model structure will be created.
  if (m_tw->m_mod->theta == NULL) {
    //info("* Initialize the model\n");
    std::cout << "* Initialize the model" << std::endl;
  }
  else {
    //info("* Resync the model\n");
    std::cout << "* Resync the model" << std::endl;
  }
  mdl_sync(m_tw->m_mod);
  // Display some statistics as we all love this.
  std::cout << "* Summary" << std::endl;
  std::cout << "    nb train: " <<  m_tw->m_mod->train->nseq << std::endl;
  if (m_tw->m_mod->devel != NULL) {
    std::cout << "    nb devel:   " <<  m_tw->m_mod->devel->nseq << std::endl;
  }
  std::cout << "    nb labels:   " << m_tw->m_mod->nlbl << std::endl;
  std::cout << "    nb blocks:    " << m_tw->m_mod->nobs << std::endl;
  std::cout << "    nb features: " << m_tw->m_mod->nftr << std::endl;
  // And train the model...
  std::cout << "* Train the model with " << m_tw->m_mod->opt->algo << std::endl;
  uit_setup(m_tw->m_mod);
  trn_lst[trn].train(m_tw->m_mod);
  uit_cleanup(m_tw->m_mod);
  // If requested compact the model.
  if (m_tw->m_mod->opt->compact) {
    const uint64_t O = m_tw->m_mod->nobs;
    const uint64_t F = m_tw->m_mod->nftr;
    std::cout << "* Compacting the model" << std::endl;
    mdl_compact(m_tw->m_mod);
    //std::cout << "    " << 0 - mod->nobs << " observations removed" << std::endl;
    //std::cout << "    " << F - mod->nftr << " features removed" << std::endl;
  }
  // And save the trained model
  std::cout << "* Save the model" << std::endl;
  file = stdout;
  if (m_tw->m_mod->opt->output != NULL) {
    file = fopen(m_tw->m_mod->opt->output, "w");
    if (file == NULL) {
      //pfatal("cannot open output model");
    }
  }
  mdl_save(m_tw->m_mod, file);
  if (m_tw->m_mod->opt->output != NULL)
    fclose(file);
  std::cout << "* Done" << std::endl;
 

  mdl_free(m_tw->m_mod);

}

   

  }


}
