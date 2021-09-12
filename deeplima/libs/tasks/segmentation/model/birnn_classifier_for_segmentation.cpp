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

#include "birnn_classifier_for_segmentation.h"

using namespace torch;
using namespace std;

namespace deeplima
{
namespace segmentation
{
namespace train
{

void BiRnnClassifierForSegmentationImpl::load(serialize::InputArchive& archive)
{
  BiRnnClassifierImpl::load(archive);

  assert(m_ngram_descr.size() == 0);

  c10::IValue v;
  if (archive.try_read("ngram_descr", v))
  {
    if (!v.isList())
    {
      throw std::runtime_error("ngram_descr must be a list.");
    }

    const c10::List<c10::IValue>& l = v.toList();
    m_ngram_descr.reserve(l.size());
    for (size_t i = 0; i < l.size(); i++)
    {
      if (!l.get(i).isString())
      {
        throw std::runtime_error("ngram_descr must be a list of strings.");
      }
      const std::string& str = l.get(i).toStringRef();
      m_ngram_descr.emplace_back(impl::ngram_descr_t(str));
    }
  }
  else
  {
    throw std::runtime_error("Can't load ngram_descr.");
  }
}

void BiRnnClassifierForSegmentationImpl::save(serialize::OutputArchive& archive) const
{
  BiRnnClassifierImpl::save(archive);

  // Save ngram descriptions
  c10::List<std::string> ngram_descr_list;
  for (size_t i = 0; i < m_ngram_descr.size(); i++)
  {
    std::string ngram_str = m_ngram_descr[i].to_string();
    assert(impl::ngram_descr_t(ngram_str) == m_ngram_descr[i]);
    ngram_descr_list.push_back(ngram_str);
  }

  archive.write("ngram_descr", ngram_descr_list);
}

} // namespace train
} // namespace segmentation
} // namespace deeplima

