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

#ifndef CONLLU_LINE_H
#define CONLLU_LINE_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <stdexcept>

namespace deeplima
{
class ISerializable
{
public:
    virtual void read(const std::string& buf, char sep = '\t') = 0;
    virtual std::string write(char sep = '\t') const = 0;
};

namespace CoNLLU
{
struct idx_t
{
  typedef uint16_t base_int_t;
  base_int_t _first;   // First word in token.
  base_int_t _last;    // Last word in token.
                       // In case of single word token _idx_first == _idx_last
  base_int_t _sub;     // index after dot in case of empty nodes (i.1)
                       // Empty nodes start from 1.
                       // Zero in this field means this is a real word.

  idx_t() : _first(0), _last(0), _sub(0) { }
  idx_t(base_int_t first, base_int_t last, base_int_t sub = 0) : _first(first), _last(last), _sub(sub) { }
  explicit idx_t(base_int_t first) : _first(first), _last(first), _sub(0) { }
  explicit idx_t(const std::string& s)
  {
    if (!parse(s))
    {
      throw std::logic_error("Can't parse string \"" + s + "\" as idx_t");
    }
  }

  inline bool is_multiword() const
  {
    return _first != _last;
  }

  inline bool is_empty() const
  {
    return _sub != 0;
  }

  inline bool is_real_word() const
  {
    return _first == _last && 0 == _sub;
  }

  bool parse(const std::string& s);
  std::string serialize() const;
};

class Line : public ISerializable, public std::vector<std::string>
{
public:
  explicit Line(const std::string& buf, char sep = '\t')
  {
    read(buf, sep);
  }

  Line() { }

  virtual void read(const std::string& buf, char sep = '\t');
  virtual std::string write(char sep = '\t') const;
};

class CoNLLULine : public ISerializable
{
public:
  explicit CoNLLULine(const std::string& buf, char sep = '\t')
  {
    read_impl(buf, sep);
  }

  explicit CoNLLULine(const Line& line)
    : _empty(false)
  {
    init_impl(line);
  }

  virtual void read(const std::string& buf, char sep = '\t')
  {
    read_impl(buf, sep);
  }

  virtual std::string write(char sep = '\t') const;

  struct feature_t
  {
    feature_t() { }
    feature_t(const std::string& key, const std::string& value) : _key(key) { _values.insert(value); }
    feature_t(const std::string& key, const std::set<std::string>& values) : _key(key), _values(values) { }

    std::string _key;
    std::set<std::string> _values;

    std::string serialize() const;
  };

  struct dep_t
  {
    dep_t(idx_t head, std::string deprel) : _head(head), _deprel(deprel) { }
    dep_t() : _head(0) { }

    idx_t _head;
    std::string _deprel;

    std::string serialize() const;
  };

  inline bool is_token_line() const
  {
    return (!_empty) && (_comment.size() == 0);
  }

  inline bool is_comment_line() const
  {
    return (!_empty) && (_comment.size() > 0);
  }

  inline bool is_empty_line() const
  {
    return _empty;
  }

  inline bool is_real_word_line() const
  {
    return (!_empty) && (_comment.size() == 0) && (_idx.is_real_word());
  }

  inline const idx_t& idx() const
  {
    return _idx;
  }

  inline const std::string& form() const
  {
    return _form;
  }

  inline const std::string& lemma() const
  {
    return _lemma;
  }

  inline const std::string& upos() const
  {
    return _upos;
  }

  inline const std::string& xpos() const
  {
    return _xpos;
  }

  inline const std::string& deprel() const
  {
    return _deprel;
  }

  inline const std::map<std::string, std::set<std::string>>& feats() const
  {
    return _feats;
  }

protected:
  virtual void init(const Line& line)
  {
    init_impl(line);
  }

  void init_impl(const Line& line);

  inline void read_impl(const std::string& buf, char sep = '\t')
  {
    _empty = true;

    if (buf.size() > 0)
    {
      _empty = false;

      if ('#' == buf[0])
      {
        _comment = buf;
      }
      else
      {
        init_impl(Line(buf, sep));
      }
    }
  }

  // empty line?
  bool _empty;

  // comment (in case of comment line)
  std::string _comment;

  // ID
  idx_t _idx;

  // FORM
  std::string _form;

  // LEMMA
  std::string _lemma;

  // UPOS
  std::string _upos;

  // XPOS
  std::string _xpos;

  // FEATS
  std::map<std::string, std::set<std::string>> _feats;

  // HEAD
  idx_t _head;

  // DEPREL
  std::string _deprel;

  // DEPS
  std::vector<dep_t> _deps;

  // MISC
  std::map<std::string, std::string> _misc;

  bool parse_feats(const std::string& s);
  bool parse_deps(const std::string& s);
  bool parse_misc(const std::string& s);
  bool parse_string_field(const std::string& s, std::string& out, bool raw=false);

  std::string serialize_feats() const;
  std::string serialize_deps() const;
  std::string serialize_misc() const;
  std::string serialize_string(const std::string& s) const;
};

} // namespace CoNLLU
} // namespace deeplima

#endif
