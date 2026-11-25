//
// Created by tderouet on 09/08/22.
//

#ifndef LIMA_TRAIN_PARAMS_SEGMENTATION_H
#define LIMA_TRAIN_PARAMS_SEGMENTATION_H
#include <string>

#include "tasks/common/train_params.h"

namespace deeplima
{
    namespace segmentation
    {
        namespace train
        {

            struct train_params_segmentation_t : public deeplima::train::train_params_t
            {

                size_t m_rnn_hidden_dim;
                size_t m_sequence_length;
                bool train_ss;
                bool train_mwt; // also predict multiword-token surfaces (implies train_ss)
                // Word list of the lexicon feature (segmentation/impl/segm_lexicon.h),
                // copied beside the model as <model>.pt.lex; empty: no lexicon
                std::string m_lexicon_fn;
                // Embedding size of the character unigrams, bigrams and trigrams
                // (0: 2, 3 and 4, the sizes before this option)
                size_t m_char_embd_dim = 0;
                // Character n-grams seen at most this many times per million
                // training characters are unknown
                uint32_t m_min_ipm = 100;
                // Loss weight of the end-of-sentence tags (1: none)
                float m_eos_weight = 1.0;
                // Keep the epoch with the best mean of dev character accuracy and
                // sentence-end F1 instead of the best accuracy
                bool m_select_eos = false;
                // Learning rate factor after an epoch without improvement
                double m_lr_decay = 0.9;

                train_params_segmentation_t()
                        : deeplima::train::train_params_t(),
                          m_rnn_hidden_dim(8),
                          m_sequence_length(256),
                          train_ss(false),
                          train_mwt(false){}
            };

        } // namespace train
    } // namespace tagging
} // namespace deeplima
#endif //LIMA_TRAIN_PARAMS_SEGMENTATION_H
