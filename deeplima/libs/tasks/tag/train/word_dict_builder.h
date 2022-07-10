// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_LIBS_TASKS_NER_TRAIN_WORD_DICT_BUILDER
#define DEEPLIMA_LIBS_TASKS_NER_TRAIN_WORD_DICT_BUILDER

#include <string>
#include <vector>
#include <map>

#include <static_graph/dict.h>

namespace deeplima
{

template <class DataSet, class FeatExtractor>
class WordDictBuilderImpl
{
public:

  FeatExtractor preprocess(const DataSet& src,
                           const std::string& feat_extractor_init = "")
  {
    FeatExtractor fe(feat_extractor_init);

    if (fe.needs_preprocessing())
    {
      typename DataSet::const_iterator it = src.begin();
      while (src.end() != it)
      {
        fe.preprocess(*it);
        it++;
      }
    }

    return fe;
  }

  DictsHolder process(const DataSet& src,
                      int32_t min_ipm,
                      const std::string unk_str = "<<UNK>>")
  {
    FeatExtractor fe;
    return process(src, fe, min_ipm, unk_str);
  }

  DictsHolder process(const DataSet& src,
                      const FeatExtractor& fe,
                      int32_t min_ipm,
                      const std::string unk_str = "<<UNK>>")
  {
    std::vector<std::map<std::string, uint64_t>> values(fe.size());
    uint64_t total = 0;

    typename DataSet::const_iterator it = src.begin();
    while (src.end() != it)
    {
      for (size_t ifeat = 0; ifeat < fe.size(); ifeat++)
      {
        const std::string& feat_val = fe.feat_value(*it, ifeat);
        if (0 == feat_val.size())
        {
          continue;
        }
        values[ifeat][feat_val]++;
        total++;
      }
      it++;
    }

    DictsHolder dicts;
    dicts.resize(values.size());
    for (size_t ifeat = 0; ifeat < values.size(); ifeat++)
    {
      if (unk_str.size() > 0)
      {
        dicts[ifeat] = std::shared_ptr<StringDict>(new StringDict(unk_str,
                                                   values[ifeat].begin(), values[ifeat].end(),
                                                   [total, min_ipm](uint64_t c) {
                                                       return ipm(c, total) > min_ipm;
                                                    })
                                                  );
      }
      else
      {
        dicts[ifeat] = std::shared_ptr<StringDict>(new StringDict(values[ifeat].begin(),
                                                                  values[ifeat].end(),
                                                   [total, min_ipm](uint64_t c) {
                                                       return ipm(c, total) > min_ipm;
                                                    })
                                                  );
      }

    }

    return dicts;
  }

protected:

  inline static float ipm(uint64_t count, uint64_t total)
  {
    return float(count * 1000000) / total;
  }
};

} // namespace deeplima

#endif
