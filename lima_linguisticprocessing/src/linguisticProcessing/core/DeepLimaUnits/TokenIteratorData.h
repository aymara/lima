//
// Created by tderouet on 06/10/22.
//


#ifndef LIMA_TOKENITERATORDATA_H
#define LIMA_TOKENITERATORDATA_H

#include "deeplima/token_sequence_analyzer.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include "deeplima/utils/str_index.h"


// The tagger's output, for the dependency parser. The tagger delivers it in
// batches, from buffers it reuses for the next ones, and a sentence can
// straddle two batches: the batches are copied end to end, so that the parser
// gets the text in one sequence (it starts a sentence at each call).
class TokenIteratorData: public Lima::AnalysisData{
public:

    virtual ~TokenIteratorData() = default;

    // The tokens [0, end - begin) of tokens and lemmata, and the time steps
    // [begin, end) of classes, as TokenSequenceAnalyzer<>::TokenIterator reads them
    void addBatch(const deeplima::token_buffer_t<>& tokens,
                  const std::vector<deeplima::StringIndex::idx_t>& lemmata,
                  std::shared_ptr< StdMatrix<uint8_t> > classes,
                  size_t begin,
                  size_t end){
        const size_t n = end - begin;
        m_tokens.insert(m_tokens.end(), tokens.begin(), tokens.begin() + n);
        m_lemmata.insert(m_lemmata.end(), lemmata.begin(), lemmata.begin() + n);
        if (m_classes->size() < classes->size())
        {
            m_classes->resize(classes->size());
        }
        for (size_t feat = 0; feat < classes->size(); ++feat)
        {
            const auto& steps = (*classes)[feat];
            (*m_classes)[feat].insert((*m_classes)[feat].end(),
                                      steps.begin() + begin, steps.begin() + end);
        }
    }

    bool empty() const {
        return m_tokens.empty();
    }

    // An iterator on the whole sequence, pointing into this object
    std::shared_ptr<deeplima::TokenSequenceAnalyzer<>::TokenIterator> getTokenIterator() {
        return std::make_shared<deeplima::TokenSequenceAnalyzer<>::TokenIterator>(
            *m_strIndex, m_tokens, m_lemmata, m_classes, 0, m_tokens.size());
    }

    void setStringIndex(std::shared_ptr<deeplima::StringIndex> stringIndex){
        m_strIndex = stringIndex;
    }

    std::shared_ptr<deeplima::StringIndex> getStringIndex() {
        return m_strIndex;
    }


private:
    deeplima::token_buffer_t<> m_tokens;
    std::vector<deeplima::StringIndex::idx_t> m_lemmata;
    std::shared_ptr< StdMatrix<uint8_t> > m_classes = std::make_shared< StdMatrix<uint8_t> >();
    std::shared_ptr< deeplima::StringIndex > m_strIndex;
};
#endif //LIMA_TOKENITERATORDATA_H
