/************************************************************************
 *
 * @file       limasimple-analyze.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Dec 15 2017
 * copyright   Copyright (C) 2017 by CEA - LIST
 * 
 ***********************************************************************/

#include "linguisticProcessing/limaSimpleClient.h"
#include "common/Data/strwstrtools.h"
#include <boost/program_options.hpp>
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
}

param={
  vector<string>(),
  "eng",
  ""
};

void readCommandLineArguments(unsigned int argc, char *argv[])
{
  po::options_description desc("Options");
    
  desc.add_options()
    ("help,h", "Display this help message")
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
  if (!vm.count("input-files") || !vm.count("language") || !vm.count("pipeline")) {
    cerr << USAGE << endl; 
    exit(EXIT_FAILURE);
  }

}

int main(int argc, char **argv)
{
  readCommandLineArguments(argc,argv);
  
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
  
}



