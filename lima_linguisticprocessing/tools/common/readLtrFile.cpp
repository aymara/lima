// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** =======================================================================
    @file       readLtrFile.cpp

    @version    $Id$
    @date       created       nov 20, 2007
    @date       last revised  dec 31, 2010

    @author     Olivier Ferret
    @brief      read an analyzed document file in the LTR (Linear Text
                Representation) format and dump its content on the standard
                ouput

    Copyright (C) 2007-2010 by CEA LIST

    ======================================================================= */

#include "common/LimaCommon.h"
#include "linguisticProcessing/core/AnalysisDumpers/LTRTextBuilder.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

#include <boost/program_options.hpp>

#include <QtCore/QCoreApplication>

namespace po = boost::program_options;

using namespace std;
using namespace boost::program_options;
using namespace Lima;




/**
  * @fn     printUsageOn
  * @brief  print help message
  */

void printUsageOn(
    ostream& os,
    const po::options_description& opt_desc,
    const po::options_description& arg_desc) {

    os << "usage: readLtrFile OPTIONS ltrFile" << endl << endl;
    os << opt_desc;
    os << "Arguments:" << endl;
    const option_description& arg = arg_desc.find("ltrFile", false);
    os << "  " << arg.long_name() << "\t	" << arg.description() << endl << endl;
}




/**
  * @fn     dumpLtrFileOn
  * @brief  dump the content of a LTR file on a stream
  */

void dumpLtrFileOn(ifstream& ifs, ostream& os) {

    Lima::Common::BagOfWords::LTR_Text text;
    text.binaryReadFrom(ifs);
    os << text << endl;
}


#include "common/tools/LimaMainTaskRunner.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include <QtCore/QTimer>

int run(int aargc,char** aargv);

int main(int argc, char **argv)
{
  QCoreApplication a(argc, argv);

  // Task parented to the application so that it
  // will be deleted by the application.
  LimaMainTaskRunner* task = new LimaMainTaskRunner(argc, argv, run, &a);

  // This will cause the application to exit when
  // the task signals finished.
  QObject::connect(task, SIGNAL(finished(int)), &a, SLOT(quit()));

  // This will run the task from the application event loop.
  QTimer::singleShot(0, task, SLOT(run()));

  return a.exec();

}


int run(int argc,char** argv)
{
  QsLogging::initQsLog();
  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();
  
    // process options and arguments
    po::options_description opt_desc("Options");
    opt_desc.add_options()
            ("help,h", "produce help message");
    po::options_description arg_desc("Arguments");
    arg_desc.add_options()
            ("ltrFile", po::value<string>(), "LTR document file");
    po::positional_options_description arg_pos;
    arg_pos.add("ltrFile", 1);
    po::options_description all_desc;
    all_desc.add(opt_desc).add(arg_desc);
    po::variables_map vm;
    try {
        store(
            po::command_line_parser(argc, argv).options(all_desc).positional(arg_pos).run(),
            vm);
        notify(vm);
    }
    catch(exception& e) {
      cerr << "error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    catch(...) {
      cerr << "unexpected exception!\n";
        return EXIT_FAILURE;
    }
    if (vm.find("help") != vm.end()) {
         printUsageOn(cout, opt_desc, arg_desc);
         return EXIT_SUCCESS;
    }
    string docFile;
    if (vm.find("ltrFile") != vm.end()) {
        // get LTR file and dump it on the standard output
        docFile = vm["ltrFile"].as<string>();
        ifstream ltrStr(docFile.c_str(), std::ifstream::binary);
        if (!ltrStr) {
            cerr << "unable to open file: " << docFile << endl;
            return EXIT_FAILURE;
        }
        dumpLtrFileOn(ltrStr, cout);
        ltrStr.close();
    }
    else {
        cout << "---- no LTR document file!"<< endl << endl;
        printUsageOn(cout, opt_desc, arg_desc);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
