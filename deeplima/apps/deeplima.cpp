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
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <functional>
#include <boost/program_options.hpp>

using namespace std;
namespace po = boost::program_options;

void parse_file(istream& input, const map<string, string>& models_fn, size_t threads, size_t out_fmt=1);

int main(int argc, char* argv[])
{
  bool arg_tokenize = false, arg_tag = false, arg_entity = false;
  size_t threads = 1;
  string input_format, output_format, tok_model, upos_model;
  vector<string> input_files;

  po::options_description desc("DeepLima");
  desc.add_options()
  ("help,h",                                                                        "Display this help message")
  ("tokenize",        po::value<bool>(&arg_tokenize)->default_value(true),          "tokenize plain text")
  ("tag",             po::value<bool>(&arg_tag)->default_value(false),              "PoS and features tagging")
  ("entity",          po::value<bool>(&arg_entity)->default_value(false),           "entity tagging")
  ("input-format",    po::value<string>(&input_format)->default_value("plain"),     "Input format: plain|conllu")
  ("output-format",   po::value<string>(&output_format)->default_value("conllu"), "Output format: conllu|vertical|horizontal")
  ("tok-model",       po::value<string>(&tok_model)->default_value(""),             "Tokenization model")
  ("upos-model",      po::value<string>(&upos_model)->default_value(""),            "UPOS model")
  ("input-file",      po::value<vector<string>>(&input_files),                      "Input file names")
  ("threads",         po::value<size_t>(&threads),                                  "Max threads to use")
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
    cerr << e.what() << endl;
    return -1;
  }

  map<string, string> models;

  if (tok_model.size() > 0)
  {
    models["tok"] = tok_model;
  }

  if (upos_model.size() > 0)
  {
    models["upos"] = upos_model;
  }

  size_t out_fmt = 1;
  if (output_format.size() > 0)
  {
    if (output_format == "horizontal")
    {
      out_fmt = 2;
    }
  }

  if (vm.count("input-file") > 0)
  {
    for ( const string fn : input_files )
    {
      ifstream file(fn, ifstream::binary | ios::in);
      parse_file(file, models, threads, out_fmt);
    }
  }
  else
  {
    parse_file(cin, models, threads, out_fmt);
  }

  return 0;
}

#include "deeplima/segmentation.h"
#include "deeplima/ner.h"
#include "deeplima/token_sequence_analyzer.h"
#include "deeplima/dumper_conllu.h"
#include "deeplima/reader_conllu.h"

using namespace deeplima;

void parse_file(istream& input, const map<string, string>& models_fn, size_t threads, size_t out_fmt)
{
  cerr << "threads = " << threads << std::endl;
  segmentation::ISegmentation* psegm = nullptr;

  if (models_fn.end() != models_fn.find("tok"))
  {
    psegm = new segmentation::Segmentation();
    static_cast<segmentation::Segmentation*>(psegm)->load(models_fn.find("tok")->second);
    static_cast<segmentation::Segmentation*>(psegm)->init(threads, 16*1024);
  }
  else
  {
    psegm = new segmentation::CoNLLUReader();
  }

  TokenSequenceAnalyzer<eigen_wrp::EigenMatrixXf>* panalyzer = nullptr;
  dumper::AbstractDumper* pdumper = nullptr;
  if (models_fn.end() != models_fn.find("upos"))
  {
    panalyzer
        = new TokenSequenceAnalyzer<eigen_wrp::EigenMatrixXf>(models_fn.find("upos")->second, 1024, 8);

    psegm->register_handler([panalyzer]
                            (const vector<segmentation::token_pos>& tokens,
                             uint32_t len)
    {
      (*panalyzer)(tokens, len);
    });
  }
  else
  {
    switch (out_fmt) {
    case 1:
      pdumper = new dumper::TokensToConllU();
      break;
    case 2:
      pdumper = new dumper::Horizontal();
      break;
    default:
      throw runtime_error("Unknown output format");
      break;
    }
    psegm->register_handler([pdumper]
                            (const vector<segmentation::token_pos>& tokens,
                             uint32_t len)
    {
      (*pdumper)(tokens, len);
    });
  }

  chrono::steady_clock::time_point parsing_begin = chrono::steady_clock::now();

  psegm->parse_from_stream([&input]
                         (uint8_t* buffer,
                         uint32_t& read,
                         uint32_t max)
  {
    input.read((istream::char_type*)buffer, max);
    read = input.gcount();
    return (bool)input;
  });

  uint64_t token_counter = (nullptr != pdumper ? pdumper->get_token_counter() : 0);

  if (nullptr != panalyzer)
  {
    if (0 == token_counter)
    {
      token_counter = panalyzer->get_token_counter();
    }

    cerr << "Waiting for analyzer to stop" << endl;
    panalyzer->finalize();
    delete panalyzer;
    cerr << "Analyzer stopped" << endl;
  }
  chrono::steady_clock::time_point parsing_end = chrono::steady_clock::now();
  float parsing_duration = std::chrono::duration_cast<chrono::milliseconds>(parsing_end - parsing_begin).count();
  float speed = (float(token_counter) * 1000) / parsing_duration;
  cerr << "Parsing speed: " << speed << " tokens / sec" << endl;

  if (nullptr != psegm)
  {
    cerr << "Deleting psegm" << endl;
    delete psegm;
  }

  if (nullptr != pdumper)
  {
    delete pdumper;
  }

  if (!input.eof() && (input.fail() || input.bad()))
  {
    throw runtime_error("parse_file: error while reading the input file.");
  }
}

