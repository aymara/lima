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

#ifndef DEEPLIMA_FEAT_EXTRACTORS_H
#define DEEPLIMA_FEAT_EXTRACTORS_H

namespace deeplima
{

template <typename Token>
class TokenStrFeatExtractor
{
protected:
  inline static std::string to_lower(const std::string& src)
  {
    std::string copy = src;
    std::transform(copy.begin(), copy.end(), copy.begin(),
      [](unsigned char c){ return std::tolower(c); });
    return copy;
  }

public:
  TokenStrFeatExtractor(const std::string& init_string = "")
  {
  }

  inline static bool needs_preprocessing()
  {
    return false;
  }

  inline void preprocess(const Token& token)
  {
  }

  inline static size_t get_feat_id(const std::string& name)
  {
    if (name == "form") return 0;
    if (name == "lc(form)") return 1;
    throw std::runtime_error("Unknown feature name.");
  }

  inline static std::string feat_value(const Token& token, size_t feat_no)
  {
    switch (feat_no)
    {
    case 0: // form as is
      return token.form();
    case 1: // form in lower case
      return to_lower(token.form());
    default:
      throw std::runtime_error("Unknown feature id.");
    }
  }

  inline static size_t size()
  {
    return 2;
  }
};

template <typename Token>
class TokenUIntFeatExtractor
{
public:
  inline static size_t get_feat_id(const std::string& name)
  {
    if (name == "eos") return 0;
    throw std::runtime_error("Unknown feature name.");
  }

  inline static uint64_t feat_value(const Token& token, size_t feat_no)
  {
    switch (feat_no)
    {
    case 0: // form as is
      return token.eos();
    default:
      throw std::runtime_error("Unknown feature id.");
    }
  }

  inline static size_t size()
  {
    return 1;
  }
};

} // namespace deeplima

#endif
