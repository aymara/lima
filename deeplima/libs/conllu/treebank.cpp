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

#include <iostream>
#include <fstream>
#include <regex>

#include <boost/filesystem.hpp>

#include <unicode/uchar.h>

#include "treebank.h"

using namespace std;
namespace fs = boost::filesystem;

namespace deeplima
{
namespace CoNLLU
{

size_t Sentence::calc_num_of_words(const Annotation& annot) const
{
  size_t counter = 0;
  size_t idx = m_first_token_line;
  for (; idx < m_first_token_line + m_num_tokens; idx++)
  {
    const CoNLLULine& line = annot.get_line(idx);
    if (line.is_empty_line())
    {
      std::cerr << "empty line" << std::endl;
    }
    if (line.is_real_word_line())
    {
      counter++;
    }
    else
    {
      std::cerr << "not a word" << std::endl;
    }
  }

  const CoNLLULine& line = annot.get_line(idx);
  assert(line.is_empty_line()); // double check the structure

  return counter;
}

bool Treebank::load(const std::string& path)
{
  for (fs::directory_entry& entry : fs::directory_iterator(path))
  {
    if (entry.path().extension().string() == ".conllu")
    {
      map<string, string> fields;
      if (!parse_ud_file_name(entry.path().filename().string(), fields)
          || fields.end() == fields.find("part")
          || fields["part"].size() == 0)
      {
        throw std::logic_error("Can't parse file name \"" + entry.path().filename().string() + "\"");
      }
      m_parts[fields["part"]] = AnnotatedDocument();

      auto plain_text_fn = entry.path();
      plain_text_fn.replace_extension("txt");
      m_parts[fields["part"]].doc.load(plain_text_fn.string());
      m_parts[fields["part"]].annot.m_pdoc = &m_parts[fields["part"]].doc;
      m_parts[fields["part"]].annot.load(entry.path().string());
      cout << "Success" << endl;
    }
  }
  return true;
}

void Document::load(const std::string& fn)
{
  ifstream input(fn);
  m_original_text = std::string((istreambuf_iterator<char>(input)), istreambuf_iterator<char>());
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  m_text = converter.from_bytes(m_original_text);
}

void Annotation::load(const std::string& fn)
{
  ifstream input(fn);
  if (!input.is_open())
  {
    throw invalid_argument(string("Can't open file \"") + fn + "\"");
  }

  load(input);
}

void Annotation::load(std::istream& input)
{
  string line;
  size_t num_sentences = 0;
  size_t num_tokens = 0;
  size_t num_words = 0;
  while (getline(input, line))
  {
    CoNLLULine l(line);
    m_lines.push_back(l);
    if (l.is_empty_line())
    {
      ++num_sentences;
    }

    if (l.is_token_line())
    {
      ++num_tokens;
    }

    if (l.is_real_word_line())
    {
      ++num_words;
    }
  }

  rebuild_structure(num_sentences, num_tokens, num_words);
}

wstring::size_type compare_ws_insensitive(const wchar_t *s1, const wchar_t *s2, size_t len)
{
  const wchar_t *p1 = s1, *p2 = s2;
  size_t i = 0;

  while (i < len)
  {
    if (*p1 == *p2 || (u_isUWhiteSpace(*p1) && u_isUWhiteSpace(*p2)))
    {
      i++; p1++; p2++;
    }
    else
    {
      if (u_isUWhiteSpace(*p1))
      {
        p1++;
      }
      else if (u_isUWhiteSpace(*p2))
      {
        p2++;
      }
      else
      {
        return wstring::npos;
      }
    }
  }

  return p1 - s1 - len;
}

void Annotation::rebuild_structure(size_t num_sentences, size_t num_tokens, size_t num_words)
{
  wstring_convert<codecvt_utf8<wchar_t>> converter;

  m_sentences.reserve(num_sentences);
  m_tokens.reserve(num_tokens);
  m_words.reserve(num_words);

  size_t sent_idx = 0;
  size_t pos = 0;
  size_t skip = 0;

  for (size_t i = 0; i < m_lines.size(); i++)
  {
    const CoNLLULine& line = m_lines[i];
    if (line.is_empty_line())
    {
      if (m_sentences.size() > 0)
      {
        sent_idx += 1;
        assert(m_tokens.size() > 0);
        m_tokens.back().m_flags = (token_t::token_flags_t)(m_tokens.back().m_flags | token_t::sentence_brk);
        m_words[m_words.size()-1].m_flags = m_tokens[m_tokens.size()-1].m_flags;
      }
      continue;
    }

    if (sent_idx == m_sentences.size())
    {
      m_sentences.push_back(Sentence());
      m_sentences[sent_idx].m_first_line = i;
      m_sentences[sent_idx].m_first_word = m_words.size();
    }
    else
    {
      if ((m_sentences.size() > 1 && sent_idx < m_sentences.size() - 1)
          || sent_idx > m_sentences.size() + 1)
      {
        throw;
      }
    }

    Sentence& sent = m_sentences[sent_idx];

    if (line.is_comment_line())
    {
      continue;
    }

    if (sent.m_num_tokens == 0)
    {
      sent.m_first_token_line = i;
    }

    sent.m_num_tokens += 1; // TODO -> num_lines

    // find token

    if (line.idx().is_empty())
      continue;

    if (skip > 0)
    {
      m_words.emplace_back(word_t(i, m_tokens.size()));
      skip -= 1;
      if (0 == skip)
      {
        m_words[m_words.size()-1].m_flags = m_tokens[m_tokens.size()-1].m_flags;
      }
      continue;
    }

    wstring wstr = converter.from_bytes(line.form());
    wstring::size_type size_ext = 0; // counts additional spaces in src text

    if (nullptr != m_pdoc)
    {
      while (wcsncmp(m_pdoc->get_text().c_str() + pos, wstr.c_str(), wstr.size()) != 0)
      {
        if (u_isUWhiteSpace(m_pdoc->get_text()[pos]) && pos < m_pdoc->get_text().size())
        {
          pos += 1;
        }
        else
        {
          // there is a whitespace possible inside a "form" field
          size_ext = compare_ws_insensitive(
                m_pdoc->get_text().c_str() + pos,
                wstr.c_str(), wstr.size());
          if (size_ext != wstring::npos)
          {
            break;
          }
          else
          {
            throw;
          }
        }
      }
    }

    m_tokens.emplace_back(token_t(i, pos, wstr.size() + size_ext));
    pos += m_tokens.back().m_len;

    if (line.idx().is_multiword())
    {
      skip = line.idx()._last - line.idx()._first + 1;
    }
    else
    {
      m_words.emplace_back(word_t(i, m_tokens.size()));
      m_words[m_words.size()-1].m_flags = m_tokens[m_tokens.size()-1].m_flags;
    }
  }

  if (!test_structure())
    throw;
}

bool Annotation::test_structure() const
{
  size_t last_sent_end = 0;
  for (const Sentence & sent : m_sentences)
  {
    if (sent.m_first_token_line < sent.m_first_line)
      return false;
    if (last_sent_end > 0)
    {
      if (sent.m_first_line <= last_sent_end)
        return false;
    }

    for (size_t i = sent.m_first_line; i < sent.m_first_token_line; i++)
    {
      if (i >= m_lines.size())
        return false;

      if (!m_lines[i].is_comment_line())
        return false;

      if (m_lines[i].is_empty_line() || m_lines[i].is_token_line())
        return false;
    }

    for (size_t i = sent.m_first_token_line; i < sent.m_first_token_line + sent.m_num_tokens; i++)
    {
      if (i >= m_lines.size())
        return false;

      if (!m_lines[i].is_token_line())
        return false;

      if (m_lines[i].is_empty_line() || m_lines[i].is_comment_line())
        return false;
    }

    last_sent_end = sent.m_first_line + sent.m_num_tokens;
  }

  return true;
}

bool parse_ud_file_name(const std::string& fn, map<string, string>& fields)
{
  smatch sm;
  if (regex_match(fn, sm, regex("(\\w+)_(\\w+)-(\\w+)-(\\w+)\\.conllu")))
  {
    if (sm.size() == 5)
    {
      fields["lang"] = sm[1];
      fields["corpus"] = sm[2];
      fields["format"] = sm[3];
      fields["part"] = sm[4];
      return true;
    }
  }
  return false;
}


const Sentence& Annotation::get_sentence(size_t idx) const
{
  return m_sentences[idx];
}

bool idx_t::parse(const std::string& s)
{
  _sub = 0;

  size_t dash_pos = s.find('-');
  if (string::npos != dash_pos)
  {
    _first = stoi(s);
    _last = stoi(s.substr(dash_pos + 1));
    return true;
  }

  size_t dot_pos = s.find('.');
  if (string::npos != dot_pos)
  {
    _first = _last = stoi(s);
    _sub = stoi(s.substr(dot_pos + 1));
    return true;
  }

  size_t i = 0;
  size_t value = stoi(s, &i);
  if (value > numeric_limits<base_int_t>::max())
  {
    throw std::overflow_error("Id too big for word \"" + s + "\"");
  }
  _first = _last = value;
  if (i < s.size())
  {
    throw logic_error("Wrong format of ID field: \"" + s + "\"");
  }

  return true;
}

string idx_t::serialize() const
{
  string rv = to_string(_first);

  if (is_multiword())
    rv += "-" + to_string(_last);

  if (is_empty())
    rv += "." + to_string(_sub);

  return rv;
}

} // namespace CoNLLU
} // namespace deeplima

