//
// Created by tderouet on 06/10/22.
//


#ifndef LIMA_TOKENITERATORDATA_H
#define LIMA_TOKENITERATORDATA_H

#include "deeplima/token_sequence_analyzer.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include "deeplima/utils/str_index.h"


class TokenIteratorData: public Lima::AnalysisData{
public:

    virtual ~TokenIteratorData() = default;

    void setTokenIterator(std::shared_ptr<deeplima::TokenSequenceAnalyzer<>::TokenIterator> ti){
        tokenIterator = ti;
    }

    std::shared_ptr<deeplima::TokenSequenceAnalyzer<>::TokenIterator> getTokenIterator() {
        return tokenIterator;
    }

    void setStringIndex(std::shared_ptr<deeplima::StringIndex> stringIndex){
        m_strIndex = stringIndex;
    }

    std::shared_ptr<deeplima::StringIndex> getStringIndex() {
        return m_strIndex;
    }


private:
    std::shared_ptr<deeplima::TokenSequenceAnalyzer<>::TokenIterator> tokenIterator;
    std::shared_ptr< deeplima::StringIndex > m_strIndex;
};
#endif //LIMA_TOKENITERATORDATA_H
