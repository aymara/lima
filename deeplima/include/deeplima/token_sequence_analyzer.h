// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_TOKEN_SEQUENCE_ANALYZER
#define DEEPLIMA_TOKEN_SEQUENCE_ANALYZER

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include "unicode/utypes.h"
#include <iostream>

// #include "segmentation.h"
#include "helpers/path_resolver.h"
#include "utils/str_index.h"
#include "token_type.h"
#include "ner.h"
#include "conllu/line.h"
#include "deeplima/nets/birnn_seq2seq.h"
#include "deeplima/lemmatization/impl/lemmatization_impl.h"
#include "deeplima/segmentation/impl/segmentation_decoder.h"
#include "deeplima/tagging/impl/tagging_impl.h"


template<> struct std::hash<deeplima::morph_model::morph_feats_t> {
    std::size_t operator()(deeplima::morph_model::morph_feats_t const& s) const noexcept {
        return s.hash();
    }
};

namespace deeplima
{

class ITokenSequenceAnalyzer
{
public:
  typedef std::function < void (std::shared_ptr< StringIndex > stridx,
                                const token_buffer_t<>& tokens,
                                const std::vector<StringIndex::idx_t>& lemmata,
                                std::shared_ptr< StdMatrix<uint8_t> > classes,
                                size_t begin,
                                size_t end) > output_callback_t;

  virtual void register_handler(const output_callback_t fn) = 0;

  virtual const std::vector<std::vector<std::string>>& get_classes() const = 0;

  virtual const std::vector<std::string>& get_class_names() const = 0;

  virtual std::shared_ptr<StringIndex> get_stridx() const = 0;

  virtual void finalize() = 0;

  virtual void operator()(const std::vector<segmentation::token_pos>& tokens, uint32_t len) = 0;
};

template <typename TaggingAuxScalar=float>
class TokenSequenceAnalyzer : public ITokenSequenceAnalyzer
{
public:
  class TokenIterator
  {
    const StringIndex& m_stridx;
    const token_buffer_t<>& m_buffer;
    const std::vector<StringIndex::idx_t> m_lemm_buffer;
    std::shared_ptr< StdMatrix<uint8_t> > m_classes;
    size_t m_current;
    size_t m_offset;
    size_t m_end;

  public:
    TokenIterator(const StringIndex& stridx, const token_buffer_t<>& buffer,
                  const std::vector<StringIndex::idx_t>& lemm_buffer,
                  std::shared_ptr< StdMatrix<uint8_t> > classes, size_t offset, size_t end)
      : m_stridx(stridx), m_buffer(buffer), m_lemm_buffer(lemm_buffer), m_classes(classes),
        m_current(0), m_offset(offset), m_end(end - offset)
    {
      assert(end > offset + 1);
    }

    inline bool end() const
    {
      return m_current >= m_end;
    }

    inline impl::token_t::token_flags_t flags() const
    {
      assert(! end());
      return m_buffer[m_current].m_flags;
    }

    inline uint16_t token_offset() const
    {
      return m_buffer[m_current].m_offset;
    }

    inline uint16_t token_len() const
    {
      return m_buffer[m_current].m_len;
    }

    inline uint32_t form_idx() const
    {
      return m_buffer[m_current].m_form_idx;
    }

    inline uint32_t lemma_idx() const
    {
      return m_lemm_buffer[m_current];
    }

    inline const char* form() const
    {
      assert(! end());
      const std::string& f = m_stridx.get_str(m_buffer[m_current].m_form_idx);
      return f.c_str();
    }

    inline const char* lemma() const
    {
      assert(! end());
      const std::string& f = m_stridx.get_str(m_lemm_buffer[m_current]);
      return f.c_str();
    }

    inline uint32_t head() const
    {
      throw std::runtime_error("TokenSequenceAnalyzer<T> does not implement head");
      return 0;
    }

    inline void next()
    {
      m_current++;
    }

    inline void reset(size_t position = 0)
    {
      m_current = position;
    }

    inline size_t position() const
    {
      return m_current;
    }

    inline uint8_t token_class(size_t cls_idx) const
    {
        // std::cerr << "time: " << m_current + m_offset << "\n";
        // std::cerr << "cls_idx: " << cls_idx << "\n";
      uint8_t val = m_classes->get(m_current + m_offset, cls_idx);
      return val;
    }
  };

  typedef TokenIterator output_iterator_t;

protected:

  class enriched_token_t
  {
    friend class TokenSequenceAnalyzer;

  protected:
    const StringIndex& m_stridx;
    const token_buffer_t<>::token_t* m_ptoken;

    inline void set_token(const token_buffer_t<>::token_t* p)
    {
      m_ptoken = p;
    }
  public:

    enriched_token_t(const StringIndex& stridx)
      : m_stridx(stridx),
        m_ptoken(nullptr)
    { }

    inline token_buffer_t<>::token_t::token_flags_t flags() const
    {
      assert(nullptr != m_ptoken);
      return m_ptoken->m_flags;
    }

    inline bool eos() const
    {
      assert(nullptr != m_ptoken);
      return flags() & token_buffer_t<>::token_t::token_flags_t::sentence_brk;
    }

    inline const std::string& form() const
    {
      assert(nullptr != m_ptoken);
      const std::string& f = m_stridx.get_str(m_ptoken->m_form_idx);
      return f;
    }
  };

  class enriched_token_buffer_t
  {
    const token_buffer_t<>& m_data;
    mutable enriched_token_t m_token; // WARNING: only one iterator is supported

  public:
    typedef enriched_token_t token_t;

    enriched_token_buffer_t(const token_buffer_t<>& data, const StringIndex& stridx)
      : m_data(data), m_token(stridx) { }

    inline token_buffer_t<>::size_type size() const
    {
      return m_data.size();
    }

    inline const enriched_token_t& operator[](size_t idx) const
    {
      m_token.set_token(m_data.data() + idx);
      return m_token;
    }
  };

  //typedef tagging::impl::FeaturesVectorizer<
  //                                    enriched_token_buffer_t,
  //                                    typename enriched_token_buffer_t::token_t> FeaturesVectorizer;

  //typedef tagging::impl::FeaturesVectorizerWithCache<
  //                                    enriched_token_buffer_t,
  //                                    typename enriched_token_buffer_t::token_t> FeaturesVectorizer;

  typedef tagging::impl::EntityTaggingClassifier<TaggingAuxScalar> Classifier;

  typedef tagging::impl::FeaturesVectorizerWithPrecomputing<
                                      Classifier,
                                      enriched_token_buffer_t,
                                      typename enriched_token_buffer_t::token_t> FeaturesVectorizer;

  // typedef tagging::impl::TaggingImpl< Classifier,
  //                                     FeaturesVectorizer,
  //                                     Matrix > EntityTaggingModule;

  typedef DictEmbdVectorizer<EmbdUInt64FloatHolder, EmbdUInt64Float, eigen_wrp::EigenMatrixXf> EmbdVectorizer;
  // typedef lemmatization::impl::LemmatizationImpl< RnnSeq2Seq, EmbdVectorizer, Matrix> LemmatizationModule;

public:
  TokenSequenceAnalyzer() :
      m_buffer_size(0),
      m_current_buffer(0),
      m_current_timepoint(0),
      m_stridx_ptr(std::make_shared<StringIndex>()),
      m_stridx(*m_stridx_ptr),
      m_classes(std::make_shared<StdMatrix<uint8_t>>())
  {
  }

  TokenSequenceAnalyzer(const std::string& model_fn,
                        const std::string& lemm_model_fn,
                        const std::string& lemm_dict_fn,
                        const std::string& fixed_lemm_fn,
                        const std::string& lowercase_lemm_fn,
                        const PathResolver& path_resolver,
                        size_t buffer_size,
                        size_t num_buffers)
    : m_buffer_size(buffer_size),
      m_current_buffer(0),
      m_current_timepoint(0),
      m_stridx_ptr(std::make_shared<StringIndex>()),
      m_stridx(*m_stridx_ptr),
      m_classes(std::make_shared<StdMatrix<uint8_t>>())
  {
    std::cerr << "TokenSequenceAnalyzer::TokenSequenceAnalyzer " << model_fn << ", "
              << lemm_model_fn << ", " << lemm_dict_fn << ", "
              << fixed_lemm_fn << ", " << lowercase_lemm_fn << std::endl;
    assert(m_buffer_size > 0);
    assert(num_buffers > 0);
    m_buffers.resize(num_buffers);
    m_lemm_buffers.resize(num_buffers);
    for ( token_buffer_t<>& b : m_buffers ) b.resize(m_buffer_size);
    for ( auto& b : m_lemm_buffers ) b.resize(m_buffer_size);

    m_cls.load(model_fn, path_resolver);
    m_cls.init(1, num_buffers, buffer_size, m_stridx);

    m_unk_idx = m_stridx.get_idx("_");
    for ( auto& b : m_lemm_buffers )
      std::fill(b.begin(), b.end(), m_unk_idx);

    {
      token_buffer_t<> buff(m_stridx.size());
      for (uint32_t i = 0; i < buff.size(); ++i)
      {
        buff[i].m_form_idx = i;
      }

      m_cls.precompute_inputs(tagging::impl::enriched_token_buffer_t(buff, m_stridx));
    }

    if (lemm_model_fn.size() > 0)
    {
      try
      {
        m_lemm.load(lemm_model_fn, path_resolver);
      }
      catch (const std::runtime_error& e)
      {
        std::cerr << "TokenSequenceAnalyzer failed to load lemmatization model " << lemm_model_fn << std::endl;
        throw;
      }
      // TODO replace value 128 below (max_input_word_len) by an optimizable parameter
      m_lemm.init(128, m_cls.get_output_str_dicts_names(), m_cls.get_output_str_dicts());

      m_cls.register_handler([this](
                             std::shared_ptr< StdMatrix<uint8_t> > classes,
                             size_t begin, size_t end, size_t slot_idx){
        // std::cerr << "handler called: " << slot_idx << std::endl;

        lemmatize(m_buffers[slot_idx], m_lemm_buffers[slot_idx], classes, begin, end);

        m_output_callback(m_stridx_ptr,
                          m_buffers[slot_idx],
                          m_lemm_buffers[slot_idx],
                          classes,
                          begin,
                          end);

        m_buffers[slot_idx].unlock();
      });
    }
    else
    {
      m_cls.register_handler([this](
                             std::shared_ptr< StdMatrix<uint8_t> > classes,
                             size_t begin, size_t end, size_t slot_idx)
      {
        // std::cerr << "handler called: " << slot_idx << std::endl;
        m_classes = classes;
        m_output_callback(m_stridx_ptr,
                          m_buffers[slot_idx],
                          m_lemm_buffers[slot_idx],
                          m_classes,
                          begin,
                          end);

        m_buffers[slot_idx].unlock();
      });
    }

    if (lemm_dict_fn.size() > 0)
    {
      load_lemm_cache(lemm_dict_fn);
    }
    if (fixed_lemm_fn.size() > 0)
    {
      load_fixed_lemm(fixed_lemm_fn);
    }
    if (lowercase_lemm_fn.size() > 0)
    {
      load_lowercase_lemm(lowercase_lemm_fn);
    }
  }

  void get_classes_from_fn(const std::string& fn,
                           std::vector<std::string>& classes_names,
                           std::vector<std::vector<std::string>>& classes)
  {
    m_cls.get_classes_from_fn(fn, classes_names, classes);
  }

  virtual ~TokenSequenceAnalyzer()
  {
  }

  virtual void register_handler(const output_callback_t fn) override
  {
    m_output_callback = fn;
  }

  virtual const std::vector<std::vector<std::string>>& get_classes() const override
  {
    return m_cls.get_output_str_dicts();
  }

  virtual const std::vector<std::string>& get_class_names() const override
  {
    return m_cls.get_output_str_dicts_names();
  }

  virtual std::shared_ptr<StringIndex> get_stridx() const override
  {
    return m_stridx_ptr;
  }

  virtual void finalize() override
  {
    if (m_current_timepoint > 0)
    {
      if (m_current_timepoint < m_buffer_size)
      {
        start_analysis(m_current_buffer, m_current_timepoint);
      }
      else
      {
        m_cls.no_more_data(m_current_buffer);
      }
    }

    m_cls.send_all_results();
  }

  virtual void operator()(const std::vector<deeplima::segmentation::token_pos>& tokens, uint32_t len) override
  {
    if (m_current_timepoint >= m_buffer_size)
    {
      acquire_buffer();
    }

    for (size_t i = 0; i < len; i++)
    {
      assert(m_current_timepoint < m_buffer_size);
      assert(m_current_buffer < m_buffers.size());

      impl::token_t& token = m_buffers[m_current_buffer][m_current_timepoint];
      const segmentation::token_pos& src = tokens[i];
      token.m_offset = src.m_offset;
      token.m_len = src.m_len;
      token.m_form_idx = m_stridx.get_idx(src.m_pch, src.m_len);
      token.m_flags = impl::token_t::token_flags_t(src.m_flags);

      m_current_timepoint++;
      if (m_current_timepoint >= m_buffer_size)
      {
        start_analysis(m_current_buffer);
        if (i < len - 1)
        {
          // if we can't wait until next call
          acquire_buffer();
        }
      }
    }
  }

protected:

  void acquire_buffer()
  {
    size_t next_buffer_idx = (m_current_buffer + 1 < m_buffers.size()) ? (m_current_buffer + 1) : 0;
    const token_buffer_t<>& next_buffer = m_buffers[next_buffer_idx];

    // wait for buffer
    while (next_buffer.locked())
    {
      m_cls.send_next_results();
    }
    assert(!next_buffer.locked());

    m_current_buffer = next_buffer_idx;
    m_current_timepoint = 0;
  }

  void start_analysis(size_t buffer_idx, int count = -1)
  {
    // std::cerr << "TokenSequenceAnalyzer::start_analysis " << buffer_idx << ", " << count << std::endl;
    assert(!m_buffers[buffer_idx].locked());
    m_buffers[buffer_idx].lock();

    const token_buffer_t<>& current_buffer = m_buffers[buffer_idx];
    m_cls.handle_token_buffer(buffer_idx, tagging::impl::enriched_token_buffer_t(current_buffer, m_stridx), count);
  }

  void process_buffer(size_t buffer_idx)
  {
    const token_buffer_t<>& current_buffer = m_buffers[buffer_idx];
    m_cls.handle_token_buffer(buffer_idx, tagging::impl::enriched_token_buffer_t(current_buffer, m_stridx));
  }

  typedef std::pair<StringIndex::idx_t, morph_model::morph_feats_t> lemm_cache_key_t;
  struct lemm_cache_key_hash
  {
    std::size_t operator() (const lemm_cache_key_t &arg) const {
        return std::hash<StringIndex::idx_t>()(arg.first) ^ arg.second.hash();
    }
  };

  void load_lemm_cache(const std::string& fn)
  {
    const morph_model::morph_model_t& mm = m_lemm.get_morph_model();
    std::ifstream f(fn, std::ios::in);
    if (!f) {
        std::cerr << "load_lemm_cache failed to open file " << fn << ".\n";
        throw std::runtime_error(std::string("load_lemm_cache failed to open file ") + fn);
    }
    std::string line;
    while (std::getline(f, line))
    {
      if (line.size() == 0)
      {
        continue;
      }
      const std::vector<std::string> v = utils::split(line, '\t');
      if (v.size() != 3)
      {
        throw std::runtime_error(std::string("Can't decode dict line \"") + line + "\"");
      }

      const std::vector<std::string> upos_feats = utils::split(v[1], ' ');
      if (upos_feats.size() != 2)
      {
        throw std::runtime_error(std::string("Can't decode upos and feats in dict line \"") + line + "\"");
      }

      std::map<std::string, std::set<std::string>> feats;
      if (!deeplima::CoNLLU::CoNLLULine::parse_feats(upos_feats[1], feats))
      {
        throw std::runtime_error(std::string("Can't parse feats in dict line \"") + line + "\"");
      }

      morph_model::morph_feats_t encoded_feats = mm.convert(upos_feats[0], feats);

      const StringIndex::idx_t form_idx = m_stridx.get_idx(v[0]);

      const lemm_cache_key_t form_key(form_idx, encoded_feats);

      const auto it = m_lemm_cache.find(form_key);
      if (m_lemm_cache.end() != it)
      {
        throw std::runtime_error(std::string("Duplicate keys in dict: \"") + line + "\"");
      }

      m_lemm_cache[form_key] = m_stridx.get_idx(v[2]);
    }

  }

  // struct fixed_lemm_cache_key_hash
  // {
  //   std::size_t operator() (const morph_model::morph_feats_t &arg) const {
  //       return arg.hash();
  //   }
  // };

  void load_fixed_lemm(const std::string& fn)
  {
    const morph_model::morph_model_t& mm = m_lemm.get_morph_model();
    std::ifstream f(fn, std::ios::in);
    if (!f) {
        std::cerr << "load_fixed_lemm failed to open file " << fn << ".\n";
        throw std::runtime_error(std::string("load_fixed_lemm failed to open file ") + fn);
    }
    std::string line;
    while (std::getline(f, line))
    {
      if (line.size() == 0 || line[0] == '#')
      {
        continue;
      }

      std::map<std::string, std::set<std::string>> feats;
      morph_model::morph_feats_t encoded_feats = mm.convert(line, feats);
      std::cerr << "load_fixed_lemm add " << line << " " << encoded_feats.toBaseType() << std::endl;
      m_fixed_lemm_cache.insert(encoded_feats);
    }

  }

  void load_lowercase_lemm(const std::string& fn)
  {
    const morph_model::morph_model_t& mm = m_lemm.get_morph_model();
    std::ifstream f(fn, std::ios::in);
    if (!f) {
        std::cerr << "load_lowercase_lemm failed to open file " << fn << ".\n";
        throw std::runtime_error(std::string("load_lowercase_lemm failed to open file ") + fn);
    }
    std::string line;
    while (std::getline(f, line))
    {
      if (line.size() == 0 || line[0] == '#')
      {
        continue;
      }

      std::map<std::string, std::set<std::string>> feats;
      morph_model::morph_feats_t encoded_feats = mm.convert(line, feats);

      std::cerr << "load_lowercase_lemm add " << line << " " << encoded_feats.toBaseType() << std::endl;
      m_lowercase_lemm_cache.insert(encoded_feats);
    }

  }

  /**
   * This well lower onlu Latin1 characters
   */
  inline static std::u32string to_lower(const std::u32string& src)
  {
    std::u32string copy = src;
    std::transform(copy.begin(), copy.end(), copy.begin(),
      [](unsigned char c){ return std::tolower(c); });
    return copy;
  }

  /**
   * TODO correct this function. It currently returns only empty strings
   * TODO use this function instead of the one above as soon as it is corrected
   */
  // inline std::u32string to_lower(const std::u32string& utf32String)
  // {
  //   // Convert the UTF-32 string to a UnicodeString
  //   icu::UnicodeString unicodeString = icu::UnicodeString::fromUTF32(
  //     (const UChar32*)(utf32String.c_str()), utf32String.size());
  //
  //   // Convert to lowercase
  //   unicodeString.toLower();
  //   std::cerr << unicodeString.toUTF8() << std::endl;
  //   // Convert back to UTF-32 string
  //   std::u32string lowercaseString;
  //   lowercaseString.resize(unicodeString.length());
  //   UErrorCode 	errorCode ;
  //   unicodeString.toUTF32((UChar32*)(lowercaseString.c_str()),
  //                         unicodeString.length(), errorCode);
  //   return lowercaseString;
  //
  // }

  void lemmatize(const token_buffer_t<>& buffer,
                 std::vector<StringIndex::idx_t>& lemm_buffer,
                 std::shared_ptr< StdMatrix<uint8_t> > classes,
                 size_t offset, size_t end)
  {
    std::u32string target;
    const auto& lang_morph_model = m_lemm.get_morph_model();
    for (size_t i = 0; i < end - offset; ++i)
    {
      if (m_lemm.is_fixed(classes, i + offset))
      {
        // std::cerr << "lemmatize " << m_stridx.get_str(buffer[i].m_form_idx)
        //           << ": use buffer" << std::endl;
        lemm_buffer[i] = buffer[i].m_form_idx;
      }
      else
      {
        const auto& morph_feats_i = m_lemm.get_morph_feats(classes, i + offset);

        auto upos = morph_model::morph_feats_t(lang_morph_model.decode_upos(morph_feats_i));

        if (m_fixed_lemm_cache.end() != m_fixed_lemm_cache.find(upos))
        {
          // std::cerr << "lemmatize " << m_stridx.get_str(buffer[i].m_form_idx)
          //           << ": fixed POS" << std::endl;
          lemm_buffer[i] = buffer[i].m_form_idx;
        }
        else if (m_lowercase_lemm_cache.end() != m_lowercase_lemm_cache.find(upos))
        {
          // std::cerr << "lemmatize " << m_stridx.get_str(buffer[i].m_form_idx)
          //           << ": lowercase POS" << std::endl;
          target = to_lower(m_stridx.get_ustr(buffer[i].m_form_idx));
          lemm_buffer[i] = m_stridx.get_idx(target);
        }
        else
        {
          // std::cerr << "lemmatize " << m_stridx.get_str(buffer[i].m_form_idx)
          //           << ": use model" << std::endl;
          const lemm_cache_key_t form_key(buffer[i].m_form_idx, m_lemm.get_morph_feats(classes, i + offset) );
          const auto it = m_lemm_cache.find(form_key);
          if (m_lemm_cache.end() == it)
          {
            const std::u32string& f = m_stridx.get_ustr(buffer[i].m_form_idx);
            m_lemm.predict(f, classes, i + offset, target);

            lemm_buffer[i] = m_stridx.get_idx(target);

            // add form to cache
            m_lemm_cache[form_key] = lemm_buffer[i];
          }
          else
          {
            lemm_buffer[i] = it->second;
          }
        }
      }
    }
  }

  size_t m_buffer_size;
  size_t m_current_buffer;
  size_t m_current_timepoint;

  std::shared_ptr<StringIndex> m_stridx_ptr;
  StringIndex& m_stridx;
  StringIndex::idx_t m_unk_idx;
  std::vector<token_buffer_t<>> m_buffers;
  std::vector<std::vector<StringIndex::idx_t>> m_lemm_buffers; // access control is sync with m_buffers

  tagging::impl::TaggingImpl<TaggingAuxScalar> m_cls;
  lemmatization::impl::LemmatizationImpl m_lemm;
  std::shared_ptr< StdMatrix<uint8_t> > m_classes;

  std::unordered_map<lemm_cache_key_t, StringIndex::idx_t, lemm_cache_key_hash> m_lemm_cache;

  // The two caches below has been added to allow to force copying the
  // (possibly lowercased) token as lemma, for a fixed list of pos tags for a given language
  // TODO implement this process
  std::unordered_set<morph_model::morph_feats_t> m_fixed_lemm_cache;
  std::unordered_set<morph_model::morph_feats_t> m_lowercase_lemm_cache;

  output_callback_t m_output_callback;
};

} // namespace deeplima

#endif
