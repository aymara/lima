#include "AbstractTaggerFactory.h"
//#include "ToolsCRF.h"
#include "TaggerWap.h"
#include <iostream>

namespace Lima {
namespace LinguisticProcessing {

  AbstractTaggerFactory::AbstractTaggerFactory(const std::string& factoryId):
RegistrableFactory<AbstractTaggerFactory>(factoryId)
  {
}


}
}

/*
FabriqueTagger::FabriqueTagger() {

}

FabriqueTagger::~FabriqueTagger() {
}

AbstractTagger* FabriqueTagger::createTagger(const std::string& libTyp) {


  if (!libTyp.compare("crfsuite")) {
    //LINFO<<"tagger crfsuite"<<LENDL;
    return new TaggerSE;
  } else if (!libTyp.compare("wapiti")){
    //LINFO<<"tagger wapiti"<<LENDL;
    return new TaggerWap;
  }

  //erreur si on arrive ici
  return NULL;

}

*/
