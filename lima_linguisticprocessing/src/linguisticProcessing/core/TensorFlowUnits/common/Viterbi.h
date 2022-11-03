// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_TENSORFLOWUNITS_VITERBI_H
#define LIMA_TENSORFLOWUNITS_VITERBI_H

#include <vector>

namespace Lima
{
namespace LinguisticProcessing
{
namespace TensorFlowUnits
{
namespace Common
{

inline float viterbi_decode(const std::vector<std::vector<float>>& scores,
                            const std::vector<std::vector<float>>& transitions,
                            std::vector<size_t>& result)
{
  size_t max_steps = scores.size();
  size_t max_states = transitions.size();

  std::vector<std::vector<float>> trellis;
  std::vector<std::vector<size_t>> backpointers;
  std::vector<std::vector<float>> v;

  trellis.resize(max_steps);
  for (auto& x : trellis)
    x.resize(max_states, 0.0);

  backpointers.resize(max_steps);
  for (auto& x : backpointers)
    x.resize(max_states, 0);

  v.resize(max_states);
  for (auto& x : v)
    x.resize(max_states, 0.0);

  trellis[0] = scores[0];

  for (size_t k = 1; k < scores.size(); ++k)
  {
    for (size_t i = 0; i < max_states; ++i)
      for (size_t j = 0; j < max_states; ++j)
        v[i][j] = trellis[k-1][i] + transitions[i][j];
        // transitions from [i] to [j]

    trellis[k] = scores[k];
    for (size_t i = 0; i < max_states; ++i)
    {
      size_t max_prev_state_no = 0;
      for (size_t j = 1; j < max_states; ++j)
        if (v[j][i] > v[max_prev_state_no][i])
          max_prev_state_no = j;
      trellis[k][i] += v[max_prev_state_no][i];
      backpointers[k][i] = max_prev_state_no;
    }
  }

  result.clear();
  result.resize(scores.size());
  size_t max_pos = 0;
  for (size_t i = 1; i < max_states; ++i)
    if (trellis[max_steps - 1][i] > trellis[max_steps - 1][max_pos])
      max_pos = i;

  result[0] = max_pos;
  std::vector<std::vector<size_t>> bp = backpointers;
  reverse(bp.begin(), bp.end());

  for (size_t i = 0; i < max_steps - 1; ++i)
    result[i+1] = bp[i][result[i]];

  reverse(result.begin(), result.end());

  max_pos = 0;
  for (size_t i = 1; i < max_states; ++i)
    if (trellis[trellis.size() - 1][i] > trellis[trellis.size() - 1][max_pos])
      max_pos = i;

  auto viterbi_score = trellis[trellis.size() - 1][max_pos];

  return viterbi_score;
}

}
}
}
}

#endif
