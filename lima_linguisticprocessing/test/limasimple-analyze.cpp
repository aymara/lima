// Copyright (C) 2017 by CEA - LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "linguisticProcessing/limaSimpleClient.h"
#include "common/Data/strwstrtools.h"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>

using namespace std;

namespace po = boost::program_options;

static const std::string USAGE("Example of analysis with LIMA simple API\nUsage:\n  limasimple-analyze [options] file1 [file2...] \n");


//****************************************************************************
/*! @brief Command line parameters (global variable). */
struct Param {
  vector<string> inputFiles;
  string language;
  string pipeline;
  bool interactive;
}

param={
  vector<string>(),
  "eng",
  "",
  false
};

void readCommandLineArguments(unsigned int argc, char *argv[])
{
  po::options_description desc("Options");
    
  desc.add_options()
    ("help,h", "Display this help message")
    ("interactive", po::bool_switch(&(param.interactive)), "")
    ("language", po::value<std::string>(&(param.language)), "language of the documents")
    ("pipeline", po::value<std::string>(&(param.pipeline)), "pipeline to use")
    ;
      po::options_description backend("Backend options");
  backend.add_options()
    ("input-files", po::value<vector<std::string> >(&(param.inputFiles)),
     "files containing the annotations")
  ;

  po::options_description all("Allowed options");
  all.add(desc).add(backend);
  po::positional_options_description pd;
  pd.add("input-files", -1);  
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(all).positional(pd).run(), vm);
  po::notify(vm);
      
  if (vm.count("help")) {
    std::cout << USAGE << desc;
    exit(EXIT_SUCCESS);
  }

  // mandatory options
  if (!vm.count("interactive") && (!vm.count("input-files") || !vm.count("language") || !vm.count("pipeline"))) {
    cerr << USAGE << endl; 
    exit(EXIT_FAILURE);
  }

}

// run analysis in interactive commmand-line mode (for debug and test reload of resources)
void runInteractive();

int main(int argc, char **argv)
{
  readCommandLineArguments(argc,argv);
  if (param.interactive) {
    runInteractive();
    return EXIT_SUCCESS;
  }
  
  Lima::LimaSimpleClient lima;

  // initialize LIMA client with the language and pipeline passed as arguments
  lima.initialize(param.language,param.pipeline);
  
  // analyze each input file with LIMA client
  for (const auto& filename: param.inputFiles) {
    std::ifstream f(filename.c_str());
    std::string content;
    Lima::Common::Misc::readStream(f,content);
    std::cout << lima.analyze(content);
  }
  
  //cout << "--second LIMA client" << endl;
  // test instantiating a second LIMA client
//   Lima::LimaSimpleClient lima2;
//   lima2.initialize(param.language,param.pipeline);
//   for (const auto& filename: param.inputFiles) {
//     std::ifstream f(filename.c_str());
//     std::string content;
//     Lima::Common::Misc::readStream(f,content);
//     std::cout << lima2.analyze(content);
//   }

  
}

void runInteractive()
{
  Lima::LimaSimpleClient lima;
  bool quit(false);
  cout << "limasimple-analyze interactive mode (to test reinit after resource change)" << endl
  << "Enter a command. Possible commands are " << endl
  << "  - init <language> <pipeline>" << endl
  << "  - analyze <text>" << endl
  << endl;
  while (!quit) {
    cout << ">";
    string line;
    getline(cin,line);
    string::size_type s=line.find(" ");
    string command(line,0,s);
    if (command=="quit" || command.empty()) {
      cout << endl;
      quit=true;
    }
    if (command=="init") {
      vector<string> args;
      boost::split(args,line,boost::is_any_of(" "));
      if (args.size()<2) {
        cerr << "init command needs two arguments (language and pipeline)" << endl;
      }
      lima.initialize(args[1],args[2]);
    }
    else if (command=="analyze") {
      string text(line,s+1);
      if (text.empty()) {
        cerr << "init command needs two arguments (language and pipeline)" << endl;
      }
      cout << lima.analyze(text);
    }
  }
}
