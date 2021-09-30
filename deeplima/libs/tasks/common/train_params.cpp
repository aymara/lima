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

#include <string>
#include <boost/filesystem.hpp>

#include "conllu/treebank.h"
#include "train_params.h"

namespace deeplima
{
namespace train
{

  void train_params_t::guess_data_sets(const std::string& ud_path, const std::string& corpus)
  {
    boost::filesystem::path path = boost::filesystem::path(ud_path) / corpus;

    for (boost::filesystem::directory_entry& entry : boost::filesystem::directory_iterator(path))
    {
      if (entry.path().extension().string() == ".conllu")
      {
        std::map<std::string, std::string> fields;
        if (!CoNLLU::parse_ud_file_name(entry.path().filename().string(), fields)
            || fields.end() == fields.find("part")
            || fields["part"].size() == 0)
        {
          throw std::logic_error("Can't parse file name \"" + entry.path().filename().string() + "\"");
        }

        if (m_train_set_fn.empty() && fields["part"] == "train")
        {
          m_train_set_fn = entry.path().string();
        }
        else if (m_dev_set_fn.empty() && fields["part"] == "dev")
        {
          m_dev_set_fn = entry.path().string();
        }
      }
    }
  }

} // namespace train
} // namespace deeplima
