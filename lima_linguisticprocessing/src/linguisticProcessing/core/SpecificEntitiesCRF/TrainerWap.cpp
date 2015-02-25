/*
#include <ctype.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdbool.h>
//#include <stddef.h>
//#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <climits>


#include "TrainerWap.h"
#include "decoder.h"
#include "model.h"
#include "options.h"
#include "progress.h"
#include "quark.h"
#include "reader.h"
#include "sequence.h"
#include "tools.h"
#include "trainers.h"
#include "pattern.h"
#include "sequence.h"

#include "GraphToWap.h"

*/


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
  opt.input ="nptrain.txt";
  //strcpy(opt.input, "nptrain.txt");
  opt.output="blob";
  //strcpy(opt.output, "blob");
  opt.type="crf";
  //strcpy(opt.type, "crf");
  opt.maxent=false;
  opt.algo="l-bfgs";
  //strcpy(opt.algo, "l-bfgs");
  opt.pattern="nppattern.txt";
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


void TrainerWap::testTrain(AnalysisContent& ac) {

  
  if (mod->opt->maxiter == 0)
    mod->opt->maxiter = INT_MAX;
  
  // Check if the user requested the type or trainer list. If this is not
  // the case, search them in the lists.
  if (!strcmp(mod->opt->type, "list")) {
    //info("Available types of models:\n");
    std::cout << "Available types of models" << std::endl;
    for (uint32_t i = 0; i < typ_cnt; i++) {
      // info("\t%s\n", typ_lst[i]);
      std::cout << typ_lst[i] << std::endl;
    }
    exit(EXIT_SUCCESS);
  }
  
  if (!strcmp(mod->opt->algo, "list")) {
    //info("Available training algorithms:\n");
    std::cout << "Available training algorithms" << std::endl;
    for (uint32_t i = 0; i < trn_cnt; i++) {
      //info("\t%s\n", trn_lst[i].name);
      std::cout << trn_lst[i].name << std::endl;
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
    // fatal("unknown algorithm '%s'", mod->opt->algo);
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
    //info("* Load patterns\n");
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
  //info("* Load training data\n");
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

  //debut test
  
  uint32_t size = 1000;
  //dat_t *dat =(dat_t*) xmalloc(sizeof( dat_t));
  dat_t *dat =(dat_t*) malloc(sizeof( dat_t));
  dat->nseq = 0;
  dat->mlen = 0;
  dat->lbl = true;
  
  //dat->seq = (seq_t**)xmalloc(sizeof(seq_t *) * size);
  dat->seq = (seq_t**)malloc(sizeof(seq_t *) * size);
  
  while (!feof(file)) {
    // Read the next sequence
    raw_t *rawro;//=graphToRaw(ac, lg);
    //seq_t *seq;// = rdr_readseq(rdr, file, lbl);
    // raw=read_raw(rdr, file);
    
    //READRAW
    uint32_t size = 32, cnt = 0;
    //raw_t *raw = (raw_t*)xmalloc(sizeof(raw_t) + sizeof(char *) * size);
    raw_t *raw = (raw_t*)malloc(sizeof(raw_t) + sizeof(char *) * size);
    // And read the next sequence in the file, this will skip any blank line
    // before reading the sequence stoping at end of file or on a new blank
    // line.
    while (!feof(file)) {
      char *line = rdr_readline(file); //  pas necessaire ?
      if (line == NULL)
	break;
      // Check for empty line marking the end of the current sequence
      int len = strlen(line);
      //info("len1: %d\n", len);
      while (len != 0 && isspace(line[len - 1]))
	len--;
      //info("len: %d\n", len);
      if (len == 0) {
	free(line);
	// Special case when no line was already read, we try
	// again. This allow multiple blank lines beetwen
	// sequences.
	//info("cnt: %d\n", cnt);
	if (cnt == 0)
	  continue;
	break;
      }
      // Next, grow the buffer if needed and add the new line in it
      if (size == cnt) {
	size *= 1.4;
	//raw = (raw_t*)xrealloc(raw, sizeof(raw_t)+ sizeof(char *) * size);
	raw = (raw_t*)realloc(raw, sizeof(raw_t)+ sizeof(char *) * size);
      }
      raw->lines[cnt++] = line;
      // In autouni mode, there will be only unigram features so we
      // can use small sequences to improve multi-theading.
      if (mod->reader->autouni) 
	break;
    }
    // If no lines was read, we just free allocated memory and return NULL
    // to signal the end of file to the caller. Else, we adjust the object
    // size and return it.
    if (cnt == 0) {
      free(raw);
      //return NULL;
    }
    //raw = (raw_t*)xrealloc(raw, sizeof(raw_t) + sizeof(char *) * cnt);
    raw = (raw_t*)realloc(raw, sizeof(raw_t) + sizeof(char *) * cnt);
    raw->len = cnt;
    //return raw;

    //readseq
    seq_t *seq;// = rdr_raw2seq(rdr, raw, true);
    if (seq == NULL)
      break;
    // Grow the buffer if needed
    if (dat->nseq == size) {
      size *= 1.4;
      //dat->seq = (seq_t**)xrealloc(dat->seq, sizeof(seq_t *) * size);
      dat->seq = (seq_t**)realloc(dat->seq, sizeof(seq_t *) * size);
    }
    // And store the sequence
    dat->seq[dat->nseq++] = seq;
    dat->mlen = std::max(dat->mlen, seq->len);
    if (dat->nseq % 1000 == 0) {
      //info("%7"PRIu32" sequences loaded\n", dat->nseq);
      std::cout << dat->nseq << " sequences loaded" << std::endl;
    }
  }
  // If no sequence readed, cleanup and repport
  if (dat->nseq == 0) {
    free(dat->seq);
    free(dat);
    //return NULL;
  }
  // Adjust the dataset size and return
  if (size > dat->nseq)
    //dat->seq = (seq_t**)xrealloc(dat->seq, sizeof(seq_t *) * dat->nseq);
   dat->seq = (seq_t**)realloc(dat->seq, sizeof(seq_t *) * dat->nseq); 
  //return dat;
  


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
    //info("* Load development data\n");
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
  //info("* Summary\n");
  std::cout << "* Summary" << std::endl;
  //info("    nb train:    %"PRIu32"\n", mod->train->nseq);
  std::cout << "    nb train:   " << mod->train->nseq << std::endl;
  if (mod->devel != NULL) {
    //info("    nb devel:    %"PRIu32"\n", mod->devel->nseq);
    std::cout << "    nb devel:    " << mod->devel->nseq << std::endl;
  }
  //info("    nb labels:   %"PRIu32"\n", mod->nlbl);
  std::cout << "    nb labels:    " << mod->nlbl << std::endl;
  //info("    nb blocks:   %"PRIu64"\n", mod->nobs);
  std::cout << "    nb blocks:    " << mod->nobs << std::endl;
  //info("    nb features: %"PRIu64"\n", mod->nftr);
  std::cout << "    nb features:    " << mod->nftr << std::endl;
  // And train the model...
  //info("* Train the model with %s\n", mod->opt->algo);
  std::cout << "* Train the model with " << mod->opt->algo << std::endl;
  uit_setup(mod);
  trn_lst[trn].train(mod);
  uit_cleanup(mod);
  // If requested compact the model.
  if (mod->opt->compact) {
    const uint64_t O = mod->nobs;
    const uint64_t F = mod->nftr;
    //info("* Compacting the model\n");
    std::cout << "* Compacting the model" << std::endl;
    mdl_compact(mod);
    //std::cout << "    " << 0 - mod->nobs  << " observations removed" << std::endl;
    std::cout << "    " << F - mod->nftr << " features removed" << std::endl;
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
  //info("* Done\n");
  std::cout << "* Done" << std::endl;
 
  }


void TrainerWap::learning(AnalysisContent& ac, MediaId lg) {

  if (mod->opt->maxiter == 0) {
    mod->opt->maxiter = INT_MAX;
  }  

  // Check if the user requested the type or trainer list. If this is not
  // the case, search them in the lists.
  if (!strcmp(mod->opt->type, "list")) {
    std::cout << "Available types of models" << std::endl;
    for (uint32_t i = 0; i < typ_cnt; i++) {
      std::cout << typ_lst[i] << std::endl;
    }
    exit(EXIT_SUCCESS);
  }
  
  if (!strcmp(mod->opt->algo, "list")) {
    std::cout << "Available types of models" << std::endl;
    for (uint32_t i = 0; i < trn_cnt; i++) {
      std::cout << typ_lst[i] << std::endl;
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
  //mod->train = rdr_readdat(mod->reader, file, true);

  //debut test
  
  uint32_t size = 1000;
  //dat_t *dat =(dat_t*) xmalloc(sizeof( dat_t));
  dat_t *dat =(dat_t*) malloc(sizeof( dat_t));
  dat->nseq = 0;
  dat->mlen = 0;
  dat->lbl = true;
  
  //dat->seq = (seq_t**)xmalloc(sizeof(seq_t *) * size);
   dat->seq = (seq_t**)malloc(sizeof(seq_t *) * size);
  
 
  // Read the next sequence
  raw_t *raw=graphToRawTrain(ac, lg);
  //seq_t *seq;// = rdr_readseq(rdr, file, lbl);
  // raw=read_raw(rdr, file);
  //READRAW
  //  uint32_t size = 32, cnt = 0;
  //raw_t *raw = (raw_t*)xmalloc(sizeof(raw_t) + sizeof(char *) * size);
  //raw_t *raw = (raw_t*)malloc(sizeof(raw_t) + sizeof(char *) * size);
  
  //readseq
  seq_t *seq = rdr_raw2seq(mod->reader, raw, true);
  if (seq == NULL) {
    //break;
  }
  // Grow the buffer if needed
  if (dat->nseq == size) {
    size *= 1.4;
    //dat->seq = (seq_t**)xrealloc(dat->seq, sizeof(seq_t *) * size);
    dat->seq = (seq_t**)realloc(dat->seq, sizeof(seq_t *) * size);
  }
  // And store the sequence
  dat->seq[dat->nseq++] = seq;
  dat->mlen = std::max(dat->mlen, seq->len);
  if (dat->nseq % 1000 == 0) {
    //info("%7"PRIu32" sequences loaded\n", dat->nseq);
    std::cout << dat->nseq << " sequences loaded" << std::endl;
  }
  // If no sequence readed, cleanup and repport
  if (dat->nseq == 0) {
    free(dat->seq);
    free(dat);
    //return NULL;
  }
  // Adjust the dataset size and return
  if (size > dat->nseq)
    //dat->seq = (seq_t**)xrealloc(dat->seq, sizeof(seq_t *) * dat->nseq);
    dat->seq = (seq_t**)realloc(dat->seq, sizeof(seq_t *) * dat->nseq);
  //return dat;
  
  mod->train=dat;

  if (mod->opt->input != NULL)
    fclose(file);
  qrk_lock(mod->reader->lbl, true);
  qrk_lock(mod->reader->obs, true);
  if (mod->train == NULL || mod->train->nseq == 0) {
    //fatal("no train data loaded");
    std::cout <<"ERREUR NO TRAIN DATA !!!  " <<  std::endl;
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
    std::cout << "* Initialize the model" << std::endl;
  }
  else {
    std::cout << "* Resync the model" << std::endl;
  }
  mdl_sync(mod);
  // Display some statistics as we all love this.
  std::cout << "* Summary" << std::endl;
  
  std::cout << "    nb train:   " << mod->train->nseq << std::endl;
  if (mod->devel != NULL) {
    std::cout << "    nb devel:    " << mod->devel->nseq << std::endl;
  }
  std::cout << "    nb labels:    " << mod->nlbl << std::endl;
  std::cout << "    nb blocks:    " << mod->nobs << std::endl;
  std::cout << "    nb features:    " << mod->nftr << std::endl;

  // And train the model...
  std::cout << "* Train the model with " << mod->opt->algo << std::endl;
  uit_setup(mod);
  trn_lst[trn].train(mod);
  uit_cleanup(mod);
  // If requested compact the model.
  if (mod->opt->compact) {
    const uint64_t O = mod->nobs;
    const uint64_t F = mod->nftr;
    //info("* Compacting the model\n");
    std::cout << "* Compacting the model" << std::endl;
    mdl_compact(mod);
    //std::cout << "    " << 0 - mod->nobs  << " observations removed" << std::endl;
    std::cout << "    " << F - mod->nftr << " features removed" << std::endl;
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
