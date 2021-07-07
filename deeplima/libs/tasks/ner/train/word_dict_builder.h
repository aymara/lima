/*
    Copyright 2021 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/

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

  DictsHolder process(const DataSet& src, int32_t min_ipm, const std::string unk_str = "<<UNK>>")
  {
    std::vector<std::unordered_map<std::string, uint64_t>> values(FeatExtractor::size());
    uint64_t total = 0;

    typename DataSet::const_iterator it = src.begin();
    while (src.end() != it)
    {
      for (size_t ifeat = 0; ifeat < FeatExtractor::size(); ifeat++)
      {
        const std::string& feat_val = FeatExtractor::feat_value(/*src.data(),*/ *it, ifeat);
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
                                                   values[ifeat],
                                                   [total, min_ipm](uint64_t c) {
                                                       return ipm(c, total) > min_ipm;
                                                    })
                                                  );
      }
      else
      {
        dicts[ifeat] = std::shared_ptr<StringDict>(new StringDict(values[ifeat],
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
