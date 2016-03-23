/*
    Copyright 2002-2013 CEA LIST

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
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_ABSTRACTLINGUISTICANALYZERCLIENT_H
#define LIMA_LINGUISTICPROCESSING_ABSTRACTLINGUISTICANALYZERCLIENT_H

#include "LinguisticProcessingClientExport.h"
#include "common/Handler/AbstractAnalysisHandler.h"
#include "LinguisticProcessingException.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/Data/LimaString.h"
#include "common/AbstractFactoryPattern/RegistrableFactory.h"
#include "common/AbstractProcessingClient/AbstractProcessingClient.h"


namespace Lima
{
namespace LinguisticProcessing
{

/**
  * AbstractLinguisticProcessingClient defines the common interface
  * that provides all the services of the LIMA linguistic analyzer.
  * It is implemented in several clients: at least one client that
  * implements the core treatment and links with the dynamic library,
  * and also clients that can wrap, with the same interface local
  * clients that communicate with a LIMA server (e.g. CORBA client),
  * or clients that embed the core processing in a more complex one,
  * such as the parsing of complex XML files.
  */
class LIMA_LINGUISTICPROCESSIONGCLIENT_EXPORT AbstractLinguisticProcessingClient:public AbstractProcessingClient
{

public:

  /**
  * define a virtual destructor to ensure concrete client destructors
  * to be called
  */
  virtual ~AbstractLinguisticProcessingClient() {}

  /**
    * This function runs the linguistic analysis on a simple text,
    * specifying the language, the pipeline and the expected
    * resultType
    *
    * @param text the text to analyze in LimaString format
    *             (LimaString format is an internal UTF16 format,
    *             functions to convert from UTF8 to/from LimaString
    *             are available in Lima::Common::Misc)
    * @param metaData some metadata used by the analyzer: the
    *                 minimal metadata required is 
    *    - 'FileName' : filename (used to generate file log names)
    *    - 'Lang'     : language as string in a 3-letter format (following
    *                   the ISO 639-2 code): 'fre','eng',...
    * @param pipeline the name of the analysis pipeline to use (an
    *                 analysis pipeline is a chain of
    *                 processUnit). Possible pipelines are defined in
    *                 the lima-analysis.xml configuration file. Their
    *                 implementations are defined in the
    *                 language-specific configuration files
    *                 lima-lp-xxx.xml (where xxx is the 3-letter code of
    *                 the language). The pipeline must have been
    *                 configured in the configuration of the
    *                 LinguisticProcessingClientFactory.
    *
    */
  virtual void analyze(const LimaString& text,
                       const std::map<std::string,std::string>& metaData,
                       const std::string& pipeline,
                       const std::map<std::string, AbstractAnalysisHandler*>& handlers,
                       const std::set<std::string>& inactiveUnits = std::set<std::string>()) const = 0;

  /**
    * This function is the same as the previous one but takes a text
    * in UTF-8 format
    *
    */
  virtual void analyze(const std::string& text,
                       const std::map<std::string,std::string>& metaData,
                       const std::string& pipeline,
                       const std::map<std::string, AbstractAnalysisHandler*>& handlers,
                       const std::set<std::string>& inactiveUnits = std::set<std::string>()) const = 0;
};

/**
 * A factory for the AbstractLinguisticProcessingClient: contains the
 * registration of all implemented clients that are linked with the
 * program. The factory dynamically creates the actual clients from
 * their names.
 */
class AbstractLinguisticProcessingClientFactory : public RegistrableFactory<AbstractLinguisticProcessingClientFactory>,public AbstractProcessingClientFactory
{
public:

  /**
   * This function configures the factory, using an XML configuration file
   * S2-lp.xml
   *
   * @param configuration the result of the parsing of the XML
   *                      configuration file
   *
   * @param langs the languages to configure: several languages may be
   *              configured in the same linguistic analyzer client,
   *              the actual language of the analysis is given in the
   *              analyze() function of the client.
   *
   * @param pipelines the pipelines to configure: several pipelines
   *                  may be configured in the same linguistic
   *                  analyzer client, the actual language of the
   *                  analysis is given in the analyze() function of
   *                  the client. Each pipeline configured will
   *                  initialize all needed processing units and the
   *                  corresponding linguistic resources.
   *
   */
  virtual void configure(
    Common::XMLConfigurationFiles::XMLConfigurationFileParser& configuration,
    std::deque<std::string> langs,
    std::deque<std::string> pipelines) = 0;

  /**
   * This function create a LinguisticProcessing client 
   */
  virtual std::shared_ptr< AbstractProcessingClient > createClient() const = 0;

  /**
   * virtual destructor of the LinguisticProcessing client factory
   */
  virtual ~AbstractLinguisticProcessingClientFactory() {};

protected:
  AbstractLinguisticProcessingClientFactory(const std::string& id) : 
    RegistrableFactory<AbstractLinguisticProcessingClientFactory>(id), AbstractProcessingClientFactory(id)
    {};

};

//documentation for Doxygen main page
/** @mainpage
 *
 * @section sec_principles Principles 
 *
 * The LIMA linguistic processing module is designed to work either
 * with a direct load of the dynamic libraries in the program or with
 * a client-server architecture. The API has then been designed as
 * sufficiently generic for this purpose. In particular, it uses a
 * @em callback strategy to manage in a single function several output
 * formats.
 *
 * The same API is also used either for analysis of simple text or for
 * analysis of structured XML documents.
 * 
 * @section sec_classes Main classes
 *  
 * AbstractLinguisticProcessingClient is the main class that
 * implements the generic LIMA API. Its main function to launch the
 * analysis on a text is AbstractLinguisticProcessingClient::analyze().
 *
 * The @em callback is implemented through a @em handler,
 * AbstractAnalysisHandler, that specifies the interface to handle the
 * different events generated by the LIMA analyzer. This handler is
 * closely related to the configuration of the analyzer concerning its
 * output format. The choice of LIMA output is specified by setting a
 * @em dumper as the last pipeline unit of the pipeline. A
 * dumper is a class that takes the results of the linguistic analysis
 * produced by LIMA and outputs a given subset of the information in a
 * given format. The list of possible dumpers is given in LIMA
 * configuration files.
 *
 * Implementations of the AbstractLinguisticProcessingClient included
 * by default in the LIMA analyzer are :
 *
 * - mm-core-client : the main client that performs the analysis
 *                    process on a simple text
 * - lp-xmlreader-client : a client that performs the analysis of a
 *                    structured XML document (XML parsing and
 *                    spotting of parts of text to analyze according
 *                    to an explicit configuration). This client uses
 *                    internally a core client to process the parts of
 *                    texts identified
 * 
 * @section How to call the LIMA analyzer
 *
 * To call the LIMA analyzer, one must indicate 
 * 
 * - the name of a pipeline to be used, specifying the set of
 * processing units that are to be activated for the text analysis:
 * such pipelines and processing units are configured in the LIMA
 * configuration files (lima-analysis.xml and lima-lp-xxx.ml, where xxx is the
 * 3-letter code of the language considered for analysis)
 *
 * - the handler, adapted to the chosen dumper, and specifying what to
 * do with this output (may be written to a file or stored in memory
 * for further processing...
 * 
 * The program analyzeText.cpp contains several examples of calls of
 * the LIMA analyze function. Here are some commented examples:
 *
 *
 * @subsection sec_example_0 Initialization of the LinguisticProcessing client
 *
 * @code
 // several languages or pipelines may be configured in the same
 // analyzer client, the ones to actually use are specified on the call of the
 // analyze() function
 // here, we only configure the only elements that we want to use
 std::deque<std::string> langs(1,"fre");           // the configured languages
 std::deque<std::string> pipelines(1,"main");      // the configured pipelines
 
 // the configuration file lima-analysis.xml containing all the configuration elements
 // for the different languages and pipelines is given as argument to the 
 // client factory
 // Then, an analyzer client is created by the factory: here, we choose a
 // local client that will load the implementation of the analyzer in
 // dynamic libraries
 
 AbstractLinguisticProcessingClient* client(0);
 XMLConfigurationFiles::XMLConfigurationFileParser lpconfig("conf/lima-analysis.xml");
 LinguisticProcessingClientFactory::changeable().configureClientFactory(
   "lima-coreclient",
   lpconfig,
   langs,
   pipelines);
 client=LinguisticProcessingClientFactory::single().createClient("lima-coreclient");

 // some metadata are used by the analysis: the minimal metadata required
 // are a language and a filename
 // (some other data can be given, such as date, that allows to
 // normalize relative dates)
 map<string,string> metaData;
 metaData["Lang"]="fre"
 metaData["FileName"]="test.txt"
 
 * @endcode
 *
 * @subsection sec_example_1 An example of call to analyze() with simple text output
 *
 * @code

 // for simple text output, the handler only redirects the stream of
 // data produced by the analyzer on a ostream (here, a file)

 //ofstream fout("output.txt");
 //SimpleStreamHandler handler(&fout);
 //client->setAnalysisHandler(&handler);
 client->analyze("ceci est un texte UTF-8",
                 metaData,
                 "main");

 * @endcode
 *
 * @subsection sec_example_2 An example of call to analyze() with BoW output 
 *
 * BoW is the LIMA binary format for an extended bag-of-words
 * representation, including compounds. This binary format is readable
 * with the readBoWFile utility program (--xml outputs an XML
 * representation of the format).
 *
 * @code
 
 // for BoW output, we use a specific handler that handles the binary 
 // format and write it to a file

// ofstream fout("output.bin");
// BowTextWriter bowWriterHandler(&fout);
// client->setAnalysisHandler(&bowWriter);
 client->analyze(text,metaData,"main");
 * @endcode
 *
 */

} // LinguisticAnalyzer
} // Lima

#endif
