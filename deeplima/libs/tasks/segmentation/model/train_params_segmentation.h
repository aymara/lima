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

                train_params_segmentation_t()
                        : m_rnn_hidden_dim(8),
                          m_sequence_length(256),
                          train_ss(false){}
            };

        } // namespace train
    } // namespace tagging
} // namespace deeplima
#endif //LIMA_TRAIN_PARAMS_SEGMENTATION_H
