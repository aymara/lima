#include "AbstractTrainerFactory.h"
//#include "ToolsCRF.h"
#include "TrainerWap.h"
#include <iostream>

namespace Lima {
namespace LinguisticProcessing {

  AbstractTrainerFactory::AbstractTrainerFactory(const std::string& factoryId):
RegistrableFactory<AbstractTrainerFactory>(factoryId)
  {
}

}
}

/*
FabriqueTrainer::FabriqueTrainer() {
}

FabriqueTrainer::~FabriqueTrainer() {}

AbstractTrainer* FabriqueTrainer::createTrainer(const std::string& libTyp) {

  if (!libTyp.compare("crfsuite")) {
    //LINFO<<"on a fait un trainer crfsuite"<<LENDL;
    return new TrainerSE;
  } else if (!libTyp.compare("wapiti")) {
    //LINFO<<"on a fait un trainer wapiti"<<LENDL;
    return new TrainerWap;  
  }
 
  //erreur si on arrive ici
  return NULL;


}
*/
