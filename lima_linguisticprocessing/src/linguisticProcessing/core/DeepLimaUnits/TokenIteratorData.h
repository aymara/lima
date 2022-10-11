//
// Created by tderouet on 06/10/22.
//


#ifndef LIMA_TOKENITERATORDATA_H
#define LIMA_TOKENITERATORDATA_H

#include "deeplima/token_sequence_analyzer.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"


class TokenIteratorData: public Lima::AnalysisData{
public:

    virtual ~TokenIteratorData(){

    }

    void setTokenIterator(deeplima::TokenSequenceAnalyzer<>::TokenIterator* ti){
        tokenIterator = ti;
    }

    deeplima::TokenSequenceAnalyzer<>::TokenIterator* getTokenIterator(){
        return tokenIterator;
    }
private:
    deeplima::TokenSequenceAnalyzer<>::TokenIterator* tokenIterator;
};
#endif //LIMA_TOKENITERATORDATA_H
