// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <functional>
#include <memory>
#include <boost/program_options.hpp>

#include "version/version.h"
#include "helpers/path_resolver.h"
#include "deeplima/segmentation/impl/segmentation_impl.h"

namespace po = boost::program_options;

#define READ_BUFFER_SIZE (1024 * 1024 * 2)
#define TAG_BUFFER_SIZE (1024)
// #define DP_BUFFER_SIZE (16384)
#define DP_BUFFER_SIZE (128)

// void init(const std::map<std::string, std::string>& models_fn,
//                 const deeplima::PathResolver& path_resolver,
//                 size_t threads,
//                 size_t out_fmt=1,
//                 bool tag_use_mp=true);
// void parse_file(std::istream& input,
//                 const std::map<std::string, std::string>& models_fn,
//                 const deeplima::PathResolver& path_resolver,
//                 size_t threads,
//                 size_t out_fmt=1,
//                 bool tag_use_mp=true);

#include "deeplima/segmentation/impl/segmentation_impl.h"
#include "deeplima/ner.h"
#include "deeplima/token_sequence_analyzer.h"
#include "deeplima/dependency_parser.h"
#include "deeplima/dumper_conllu.h"
#include "deeplima/reader_conllu.h"

using namespace deeplima;

class file_parser
{
public:
std::shared_ptr<segmentation::ISegmentation> psegm = nullptr;
std::shared_ptr< ITokenSequenceAnalyzer > panalyzer = nullptr;
std::shared_ptr< dumper::AbstractDumper > pdumper_segm_only = nullptr; // used when using segmentation only
std::shared_ptr< dumper::DumperBase > pdumper_complete = nullptr; // used when using tagger
std::shared_ptr<DependencyParser> parser = nullptr;

/**
 * @param models_fn models file names, a map associating a kind of model to its file path
 *                  keys are:
 *                  - tok: the tokenizer model (can be absent if loading a pre-tokenized text)
 *                  - tag: the tagger model (can be absent if doing tokenization only)
 *                  - lem: the lemmatizer moder (can be absent)
 *                  - lem_dict: the lemmatizer dictionary (can be absent)
 *                  - fixed_ini:
 *                  - lower_ini:
 *                  - fixed_lemm:
 *                  - dp: the dependency parser model (can be absent)
 * @param tag_use_mp if true, use mixed precision (int16) model, otherwise, use
 *                   full precision (float)
 */
void init(const std::map<std::string, std::string>& models_fn,
                const PathResolver& path_resolver,
                size_t threads,
                size_t out_fmt,
                bool tag_use_mp)
{
  // std::cerr << "deeplima parse_file threads = " << threads << std::endl;

  // if (models_fn.end() != models_fn.find("tok"))
  // {
  //   psegm = std::make_shared<segmentation::impl::SegmentationImpl>();
  //   try
  //   {
  //     std::dynamic_pointer_cast<segmentation::impl::SegmentationImpl>(psegm)->load(models_fn.find("tok")->second);
  //   }
  //   catch (std::runtime_error& e)
  //   {
  //     std::cerr << "In parse_file: failed to load model file " << models_fn.find("tok")->second << ": "
  //               << e.what() << std::endl;
  //     throw;
  //   }
  //   std::dynamic_pointer_cast<segmentation::impl::SegmentationImpl>(psegm)->init(threads, 16*1024);
  // }
  // else
  // {
  //   // Not a real segmenter but a CoNLLU reader
  //   psegm = std::make_shared<segmentation::CoNLLUReader>();
  // }

  if (models_fn.end() != models_fn.find("tag"))
  {
    auto find_or_empty_line = [](const auto& m, const std::string& k)
    {
      const auto it = m.find(k);
      if (m.end() != it)
      {
        return it->second;
      }
      using mt = typename std::remove_reference<decltype(m)>::type::mapped_type;
      return mt("");
    };
    std::string lemm_model_fn = find_or_empty_line(models_fn, "lem");
    std::string lemm_dict_fn = find_or_empty_line(models_fn, "lem_dict");
    std::string fixed_ini_fn = find_or_empty_line(models_fn, "fixed_ini");
    std::string lower_ini_fn = find_or_empty_line(models_fn, "lower_ini");
    std::string fixed_lemm_fn = find_or_empty_line(models_fn, "fixed_lemm");

    try
    {
      if (tag_use_mp)
      {
          panalyzer = std::make_shared< TokenSequenceAnalyzer<int16_t> >(
              models_fn.find("tag")->second,
              lemm_model_fn,
              lemm_dict_fn,
              fixed_ini_fn,
              lower_ini_fn,
              fixed_lemm_fn,
              path_resolver,
              TAG_BUFFER_SIZE,
              8);
      }
      else
      {
        panalyzer = std::make_shared< TokenSequenceAnalyzer<float> >(
            models_fn.find("tag")->second,
            lemm_model_fn,
            lemm_dict_fn,
            fixed_ini_fn,
            lower_ini_fn,
            fixed_lemm_fn,
            path_resolver,
            TAG_BUFFER_SIZE,
            8);
      }
    }
    catch (const std::runtime_error& e)
    {
      std::cerr << "parse_file failed to initialize analyzer: " << e.what() << std::endl;
      throw std::runtime_error(std::string("parse_file failed to initialize analyzer: ") + e.what());
    }

    if (models_fn.end() != models_fn.find("dp"))
    {
      // with dependency parsing
      auto conllu_dumper = std::make_shared<dumper::AnalysisToConllU<typename DependencyParser::TokenIterator> >();
      pdumper_complete = conllu_dumper;

      try
      {
        parser = std::make_shared<DependencyParser>(models_fn.find("dp")->second,
                                                          path_resolver,
                                                          panalyzer->get_stridx(),
                                                          panalyzer->get_class_names(),
                                                          DP_BUFFER_SIZE,
                                                          8);
      }
      catch (const std::runtime_error& e)
      {
        std::cerr << "parse_file failed to initialize parser: " << e.what() << std::endl;
        throw std::runtime_error(std::string("parse_file failed to initialize parser: ") + e.what());
      }

      for (size_t i = 0; i < panalyzer->get_classes().size(); ++i)
      {
        parser->set_classes(i, panalyzer->get_class_names()[i], panalyzer->get_classes()[i]);
        conllu_dumper->set_classes(i, panalyzer->get_class_names()[i], panalyzer->get_classes()[i]);
      }

      panalyzer->register_handler([this](std::shared_ptr< StringIndex > stridx,
                                  const token_buffer_t<>& tokens,
                                  const std::vector<StringIndex::idx_t>& lemmata,
                                  std::shared_ptr< StdMatrix<uint8_t> > classes,
                                  size_t begin,
                                  size_t end)
      {
        typename TokenSequenceAnalyzer<>::TokenIterator ti(*stridx,
                                                           tokens,
                                                           lemmata,
                                                           classes,
                                                           begin,
                                                           end);
        // std::cerr << "In panalyzer handler. Calling parser functor" << std::endl;
        (*parser)(ti);
      });

      parser->register_handler([conllu_dumper](const StringIndex& stridx,
                                  const std::vector<typename DependencyParser::token_with_analysis_t>& tokens,
                                  std::shared_ptr< StdMatrix<uint32_t> > classes,
                                  size_t begin,
                                  size_t end)
      {
        typename DependencyParser::TokenIterator ti(stridx,
                                                           tokens,
                                                           classes,
                                                           begin,
                                                           end);
        // std::cerr << "In parser handler. Calling conllu_dumper functor " << begin << ", " << end << std::endl;
        (*conllu_dumper)(ti, begin, end, true);
      });
      // conllu_dumper->flush();

    }
    else
    {
      // without dependency parsing
      auto conllu_dumper = std::make_shared< dumper::AnalysisToConllU<TokenSequenceAnalyzer<>::TokenIterator> >();
      pdumper_complete = conllu_dumper;

      for (size_t i = 0; i < panalyzer->get_classes().size(); ++i)
      {
        conllu_dumper->set_classes(i, panalyzer->get_class_names()[i], panalyzer->get_classes()[i]);
      }

      panalyzer->register_handler([conllu_dumper](
        std::shared_ptr< StringIndex > stridx,
        const token_buffer_t<>& tokens,
        const std::vector<StringIndex::idx_t>& lemmata,
        std::shared_ptr< StdMatrix<uint8_t> > classes,
        size_t begin,
        size_t end)
      {
        typename TokenSequenceAnalyzer<>::TokenIterator ti(*stridx,
                                                           tokens,
                                                           lemmata,
                                                           classes,
                                                           begin,
                                                           end);
        // std::cerr << "In panalyzer handler. Calling conllu_dumper functor" << std::endl;
        (*conllu_dumper)(ti, begin, end);
      });
    }

    // psegm->register_handler([panalyzer]
    //                         (const std::vector<segmentation::token_pos>& tokens,
    //                          uint32_t len)
    // {
    //   // std::cerr << "In psegm handler. Calling panalyzer functor" << std::endl;
    //   (*panalyzer)(tokens, len);
    // });
  }
  else
  {
    switch (out_fmt) {
    case 1:
      pdumper_segm_only = std::make_shared<dumper::TokensToConllU>();
      break;
    case 2:
      pdumper_segm_only = std::make_shared<dumper::Horizontal>();
      break;
    default:
      throw std::runtime_error("Unknown output format");
      break;
    }
    // psegm->register_handler([pdumper_segm_only]
    //                         (const std::vector<segmentation::token_pos>& tokens,
    //                          uint32_t len)
    // {
    //   // std::cerr << "In psegm handler. Calling pdumper_segm_only functor" << std::endl;
    //   (*pdumper_segm_only)(tokens, len);
    // });
  }

}

void parse_file(std::istream& input,
                const std::map<std::string, std::string>& models_fn,
                const PathResolver& path_resolver,
                size_t threads,
                size_t out_fmt,
                bool tag_use_mp)
{
  if (models_fn.end() != models_fn.find("tok"))
  {
    psegm = std::make_shared<segmentation::impl::SegmentationImpl>();
    try
    {
      std::dynamic_pointer_cast<segmentation::impl::SegmentationImpl>(psegm)->load(models_fn.find("tok")->second);
    }
    catch (std::runtime_error& e)
    {
      std::cerr << "In parse_file: failed to load model file " << models_fn.find("tok")->second << ": "
                << e.what() << std::endl;
      throw;
    }
    std::dynamic_pointer_cast<segmentation::impl::SegmentationImpl>(psegm)->init(threads, 16*1024);
  }
  else
  {
    // Not a real segmenter but a CoNLLU reader
    psegm = std::make_shared<segmentation::CoNLLUReader>();
  }
  if (models_fn.end() != models_fn.find("tag"))
  {
    psegm->register_handler([this]
                            (const std::vector<segmentation::token_pos>& tokens,
                             uint32_t len)
    {
      // std::cerr << "In psegm handler. Calling panalyzer functor" << std::endl;
      (*panalyzer)(tokens, len);
    });
  }
  else
  {
    psegm->register_handler([this]
                            (const std::vector<segmentation::token_pos>& tokens,
                             uint32_t len)
    {
      // std::cerr << "In psegm handler. Calling pdumper_segm_only functor" << std::endl;
      (*pdumper_segm_only)(tokens, len);
    });

  }
  auto parsing_begin = std::chrono::high_resolution_clock::now();
  std::dynamic_pointer_cast<deeplima::segmentation::impl::SegmentationClassifier>(psegm)->reset();
  psegm->parse_from_stream([&input]
                         (uint8_t* buffer,
                          int32_t& read,
                          int32_t max)
  {
    // std::cerr << "In psegm parse_from_stream lambda" << std::endl;
    input.read((std::istream::char_type*)buffer, max);
    read = input.gcount();
    return (bool)input;
  });

  if (nullptr != panalyzer)
  {

    // std::cerr << "Waiting for PoS tagger to stop. Calling panalyzer->finalize" << std::endl;
    panalyzer->finalize();
    pdumper_complete->flush();
    std::cerr << "Analyzer stopped. panalyzer->finalize returned" << std::endl;
  }

  if (parser)
  {
    // std::cerr << "Waiting for dependency parser to stop. Calling parser->finalize" << std::endl;
    parser->finalize();
    // std::cerr << "Calling parser.reset. parser->finalize returned" << std::endl;
    parser.reset();
    // std::cerr << "Dependency parser stopped. " << std::endl;
  }
  auto parsing_end = std::chrono::high_resolution_clock::now();
  auto parsing_duration = std::chrono::duration_cast<std::chrono::milliseconds>(parsing_end - parsing_begin);

  uint64_t token_counter = 0;
  if(nullptr != pdumper_segm_only)
    token_counter = pdumper_segm_only->get_token_counter();
  else if (nullptr != pdumper_complete)
  {
    token_counter = pdumper_complete->get_token_counter();
  }
  else
  {
    throw std::runtime_error("parse_file: no dumper is available.");
  }

  float speed = float(token_counter) * 1000 / parsing_duration.count();
  std::cerr << "Parsed: " << token_counter << " in "
            << std::fixed << std::setprecision(2)
            << float(parsing_duration.count()) / 1000 << " seconds." << std::endl;
  std::cerr << "Parsing speed: " << speed << " tokens / sec." << std::endl;

  if (!input.eof() && (input.fail() || input.bad()))
  {
    throw std::runtime_error("parse_file: error while reading the input file.");
  }

  psegm->finalize();
}
};


int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "en_US.UTF-8");
  // std::cerr << "deeplima (git commit hash: " << deeplima::version::get_git_commit_hash() << ", "
  //      << "git branch: " << deeplima::version::get_git_branch()
  //      << ")" << std::endl;

  size_t threads = 1;
  std::string input_format, output_format, tok_model, tag_model, lem_model, dp_model;
  std::string lem_dict;
  std::string fixed_ini;
  std::string lower_ini;
  std::string fixed_lemm;
  bool tag_use_mp;
  std::vector<std::string> input_files;

  po::options_description desc("deeplima (analysis demo)");
  desc.add_options()
  ("help,h",                                                                             "Display this help message")
  ("input-format",    po::value<std::string>(&input_format)->default_value("plain"),     "Input format: plain|conllu")
  ("output-format",   po::value<std::string>(&output_format)->default_value("conllu"),   "Output format: conllu|vertical|horizontal")
  ("tok-model",       po::value<std::string>(&tok_model)->default_value(""),             "Tokenization model")
  ("tag-model",       po::value<std::string>(&tag_model)->default_value(""),             "Tagging model")
  ("lem-model",       po::value<std::string>(&lem_model)->default_value(""),             "Lemmatization model")
  ("dp-model",        po::value<std::string>(&dp_model)->default_value(""),              "Dependency parsing model")
  ("lem-dict",        po::value<std::string>(&lem_dict)->default_value(""),              "Lemmatization dictionary")
  ("fixed-ini",      po::value<std::string>(&fixed_ini)->default_value(""),            "List of upos wiht fixed lemmas")
  ("lower-ini",  po::value<std::string>(&lower_ini)->default_value(""),        "List of upos wih lowercased lemmas")
  ("fixed-lemm",  po::value<std::string>(&fixed_lemm)->default_value(""),        "List of upos wih lowercased lemmas")
  ("input-file",      po::value<std::vector<std::string>>(&input_files),                 "Input file names")
  ("threads",         po::value<size_t>(&threads),                                       "Max threads to use")
  ("tag-use-mp",      po::value<bool>(&tag_use_mp)->default_value(true),                 "Use mixed-precision calculations in tagger")
  ;

  po::positional_options_description pos_desc;
  pos_desc.add("input-file", -1);

  po::variables_map vm;

  try
  {
    po::store(po::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), vm);
    po::notify(vm);
  }
  catch (const boost::program_options::unknown_option& e)
  {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  if (vm.count("help")) {
      std::cout << desc << std::endl;
      return 0;
  }

  if (tok_model.empty() && tag_model.empty())
  {
    std::cerr << "No model is provided: --tok-model or --tag-model parameters are required." << std::endl << std::endl;
    std::cout << desc << std::endl;
    return -1;
  }

  std::map<std::string, std::string> models;

  if (tok_model.size() > 0)
  {
    models["tok"] = tok_model;
  }

  if (tag_model.size() > 0)
  {
    models["tag"] = tag_model;
  }

  if (lem_model.size() > 0)
  {
    models["lem"] = lem_model;
  }

  if (dp_model.size() > 0)
  {
    models["dp"] = dp_model;
  }

  if (lem_dict.size() > 0)
  {
    models["lem_dict"] = lem_dict;
  }

  if (fixed_ini.size() > 0)
  {
    models["fixed_ini"] = fixed_ini;
  }

  if (lower_ini.size() > 0)
  {
    models["lower_ini"] = lower_ini;
  }

  if (fixed_lemm.size() > 0)
  {
    models["fixed_lemm"] = fixed_lemm;
  }

  size_t out_fmt = 1;
  if (output_format.size() > 0)
  {
    if (output_format == "horizontal")
    {
      out_fmt = 2;
    }
  }

  deeplima::PathResolver path_resolver;

  file_parser fp;
  fp.init(models, path_resolver, threads, out_fmt, tag_use_mp);

  if (vm.count("input-file") > 0)
  {

    char read_buffer[READ_BUFFER_SIZE];
    for ( const auto& fn : input_files )
    {
      std::cerr << "Reading file: " << fn << std::endl;
      std::ifstream file(fn, std::ifstream::binary | std::ios::in);
      if (!file.is_open())
      {
        std::cerr << "Failed to open file: " << fn << std::endl;
        throw std::runtime_error(std::string("Failed to open file "+fn));
      }
      file.rdbuf()->pubsetbuf(read_buffer, READ_BUFFER_SIZE);
      try
      {
        fp.parse_file(file, models, path_resolver, threads, out_fmt, tag_use_mp);
      }
      catch (const std::runtime_error& e)
      {
        std::cerr << "Analysis failure: Exception while analyzing file " << fn << ":" << std::endl
                  << e.what() << std::endl;
        return 1;
      }
    }
  }
  else
  {
    try
    {
      fp.parse_file(std::cin, models, path_resolver, threads, out_fmt, tag_use_mp);
    }
    catch (const std::runtime_error& e)
    {
      std::cerr << "Analysis failure: Exception while analyzing text from stdin:" << std::endl
                << e.what() << std::endl;
      return 1;
    }
  }

  return 0;
}

