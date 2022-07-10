// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <sstream>
#include <iostream>
#include <limits>

#include "line.h"

using namespace std;

namespace deeplima
{
namespace CoNLLU
{
#define RESERVE_FIELDS 16

void Line::read(const std::string& buf, char sep)
{
  reserve(RESERVE_FIELDS);
  size_t eol_pos = buf.find_first_of("\r\n");
  size_t pos = 0, sep_pos = buf.find(sep, pos+1);

  while (string::npos != sep_pos)
  {
    push_back(buf.substr(pos, sep_pos - pos));
    pos = sep_pos + 1;
    sep_pos = buf.find(sep, pos);
  }

  sep_pos = eol_pos;
  push_back(buf.substr(pos, sep_pos - pos));
}

string Line::write(char sep) const
{
  string rv;

  for (auto s : *this)
  {
    if (rv.size() > 0)
    {
      rv = rv + sep;
    }
    rv += s;
  }

  return rv;
}

string CoNLLULine::write(char sep) const
{
  if (is_empty_line())
  {
    return "";
  }

  if (is_comment_line())
  {
    return _comment;
  }

  Line l;
  l.resize(10);

  l[0] = _idx.serialize();
  l[1] = serialize_string(_form);
  l[2] = serialize_string(_lemma);
  l[3] = serialize_string(_upos);
  l[4] = serialize_string(_xpos);
  l[5] = serialize_feats();
  l[6] = (std::numeric_limits<size_t>::max() == _head._first) ? "_" : _head.serialize();
  l[7] = serialize_string(_deprel);
  l[8] = serialize_deps();
  l[9] = serialize_misc();

  return l.write(sep);
}

void CoNLLULine::init_impl(const Line& line)
{
  // 1. ID
  if (!_idx.parse(line[0]))
  {
    throw logic_error("Can't parse ID field \"" + line[0] + "\" in line \"" + line.write() + "\"");
  }

  // 2. FORM
  if (!parse_string_field(line[1], _form, true))
  {
    throw logic_error("Can't parse FORM field \"" + line[1] + "\" in line \"" + line.write() + "\"");
  }

  // 3. LEMMA
  if (!parse_string_field(line[2], _lemma))
  {
    throw logic_error("Can't parse LEMMA field \"" + line[2] + "\" in line \"" + line.write() + "\"");
  }

  // 4. UPOS
  if (!parse_string_field(line[3], _upos))
  {
    throw logic_error("Can't parse UPOS field \"" + line[3] + "\" in line \"" + line.write() + "\"");
  }

  // 5. XPOS
  if (!parse_string_field(line[4], _xpos))
  {
    throw logic_error("Can't parse XPOS field \"" + line[4] + "\" in line \"" + line.write() + "\"");
  }

  // 6. FEATS
  if (!parse_feats(line[5]))
  {
    throw logic_error("Can't parse FEATS field \"" + line[5] + "\" in line \"" + line.write() + "\"");
  }

  // 7. HEAD
  _head = (line[6] == "_") ? idx_t(std::numeric_limits<idx_t::base_int_t>::max()) : idx_t(line[6]);

  // 8. DEPREL
  if (!parse_string_field(line[7], _deprel)) // TODO: check result and remaining part of the string
  {
    throw logic_error("Can't parse DEPREL field \"" + line[7] + "\" in line \"" + line.write() + "\"");
  }

  // 9. DEPS
  if (!parse_deps(line[8]))
  {
    throw logic_error("Can't parse DEPS field \"" + line[8] + "\" in line \"" + line.write() + "\"");
  }

  // 10. MISC
  if (!parse_misc(line[9]))
  {
    throw logic_error("Can't parse MISC field \"" + line[9] + "\" in line \"" + line.write() + "\"");
  }
}

bool CoNLLULine::parse_feats(const std::string& s)
{
  if ("_" == s)
  {
    return true;
  }

  Line l(s, '|');

  for (const string& f : l)
  {
    size_t pos = f.find('=');
    if (string::npos == pos)
    {
      throw logic_error("Wrong format of FEAT field: \"" + s + "\"");
    }

    string k = f.substr(0, pos);
    string v = f.substr(pos + 1);
    _feats[k].insert(v);
  }

  return true;
}

bool CoNLLULine::parse_deps(const std::string& s)
{
  if ("_" == s)
  {
    return true;
  }

  Line l(s, '|');

  for (const string& f : l)
  {
    size_t pos = f.find(':');
    if (string::npos == pos)
    {
      throw logic_error("Wrong format of DEPS field: \"" + s + "\"");
    }

    idx_t head;
    head.parse(f.substr(0, pos));
    string deprel = f.substr(pos + 1);
    _deps.push_back(dep_t(head, deprel));
  }

  return true;
}

bool CoNLLULine::parse_misc(const std::string& s)
{
  if ("_" == s)
  {
    return true;
  }

  Line l(s, '|');

  for (const string& f : l)
  {
    if ("_" == f)
    {
      _misc["_"] = ""; // TODO: I don't understand what does it mean.
                       // Happens in en_ewt-ud-train.conllu
    }
    else
    {
      size_t pos = f.find('=');
      if (string::npos == pos)
      {
        pos = f.find(':');
        if (string::npos == pos)
        {
          //throw logic_error("Wrong format of MISC field: \"" + s + "\"");
          std::cerr << "Wrong format of MISC field: \"" << s << "\"" << std::endl;
        }
      }

      string k = f.substr(0, pos);
      string v = f.substr(pos + 1);
      _misc[k] = v;
    }
  }

  return true;
}

bool CoNLLULine::parse_string_field(const string& s, string& out, bool raw)
{
  if (!raw && "_" == s)
  {
    out = "";
  }
  else
  {
    out = s;
  }

  return true;
}

string CoNLLULine::serialize_feats() const
{
  if (_feats.size() == 0)
  {
    return "_";
  }

  Line l;
  l.reserve(_feats.size());

  for (auto& kv : _feats)
  {
    l.push_back(feature_t(kv.first, kv.second).serialize());
  }

  return l.write('|');
}

string CoNLLULine::serialize_deps() const
{
  if (_deps.size() == 0)
  {
    return "_";
  }

  Line l;
  l.reserve(_deps.size());

  for (auto& d : _deps)
  {
    l.push_back(d.serialize());
  }

  return l.write('|');
}

string CoNLLULine::serialize_misc() const
{
  if (_misc.size() == 0)
  {
    return "_";
  }

  Line l;
  l.reserve(_misc.size());

  for (auto& kv : _misc)
  {
    l.push_back(feature_t(kv.first, kv.second).serialize());
  }

  return l.write('|');
}

string CoNLLULine::serialize_string(const string& s) const
{
  if (s.size() == 0)
  {
    return "_";
  }

  return s;
}

string CoNLLULine::feature_t::serialize() const
{
  if ("_" == _key)
  {
    return "_";
  }

  string serialized_values;

  for (auto& s : _values)
  {
    if (serialized_values.size() > 0)
    {
      serialized_values += string(",");
    }

    serialized_values += s;
  }

  return _key + "=" + serialized_values;
}

string CoNLLULine::dep_t::serialize() const
{
  return _head.serialize() + ":" + _deprel;
}

} // namespace CoNLLU
} // namespace deeplima

