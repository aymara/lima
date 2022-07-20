// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       readBoWFile.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Thu Oct  9 2003
 * @version    $Id: getLexiconFromBoW.cpp 9610 2008-07-18 16:15:02Z de-chalendarg $
 * copyright   Copyright (C) 2003 by CEA LIST
 *
 ***********************************************************************/

#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
#include "linguisticProcessing/common/BagOfWords/bowNamedEntity.h"
#include "linguisticProcessing/common/BagOfWords/bowDocument.h"
#include "linguisticProcessing/common/BagOfWords/bowDocumentST.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"
#include "linguisticProcessing/common/BagOfWords/bowTokenIterator.h"
#include "linguisticProcessing/common/BagOfWords/AbstractBoWDocumentHandler.h"
#include "common/Data/strwstrtools.h"
#include <fstream>
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"


#include <QtCore/QCoreApplication>

using namespace std;
using namespace Lima;
using namespace Lima::Common::BagOfWords;
using namespace Lima::Common::PropertyCode;
using namespace Lima::Common::Misc;
using namespace Lima::Common;

//****************************************************************************
// declarations
//****************************************************************************
// help mode & usage
Q_GLOBAL_STATIC_WITH_ARGS(string, USAGE, ("USAGE : getLexiconFromBoW [--onlyLemma] fileIn\ntype getLexiconFromBoW -h for help\n"));

Q_GLOBAL_STATIC_WITH_ARGS(string, HELP, (std::string("Count words in BoW files\n")
                         +*USAGE
                         +"INPUT:\n"
+"\tfileIn                text BoW files\n"
+"\t                      (obsolete: BoW documents; TODO XmlBoW)\n"
+"OPTIONS:\n"
+"\t--onlyLemma           use only lemma\n"
+"\t--macro=NAME          extract only terms with given macro category\n"
+"\t--language=<trigram>  resources for code values and symbolic names\n"
+"\t-l <trigram>          resources for code values and symbolic names\n"
+"\t--output=FILENAME     file name to save the created lexicon\n"
+"\t                      (default: display on screen)\n"
+"\n"
                        ));

//****************************************************************************
// GLOBAL variable -> the command line arguments
struct Param
{
    std::vector<string> inputFiles;           // input file
    std::vector<string> macro;            // macro
    string codeFile;
    string language;
    string outputFilename;
    bool help;                  // help mode
}
param={std::vector<string>(0),
       std::vector<string>(0),
       std::string(""),
       std::string(""),
       std::string(""),
       false
      };

void readCommandLineArguments(uint64_t argc, char *argv[])
{
    for (uint64_t i(1); i<argc; i++)
    {
        string s(argv[i]);
        std::string::size_type pos;
        if (s=="-h" || s=="--help")
            param.help=true;
        else if (s=="--onlyLemma") {
            BoWToken::setUseOnlyLemma(true);
        }
        else if ( (pos = s.find("--code=")) != std::string::npos )
        {
            param.codeFile = s.substr(pos+7);
        }
        else if ( (pos = s.find("--language=")) != std::string::npos )
        {
            param.language = s.substr(pos+11);
        }
        else if  ( (pos = s.find("-l")) != std::string::npos )
        {
            param.language = argv[i+1];
            i++;
        }
        else if ( (pos = s.find("--macro=")) != std::string::npos )
        {
            param.macro.push_back(s.substr(pos+8));
        }
        else if ( (pos = s.find("--output=")) != std::string::npos )
        {
            param.outputFilename = s.substr(pos+9);
        }
        else if (s[0]=='-')
        {
            cerr << "unrecognized option " <<  s
                 << endl;
            cerr << *USAGE << endl;
            exit(1);
        }
        else
        {
            param.inputFiles.push_back(s);
        }
    }
}

// Lexicon: map< XXX ,std::pair< YYY , ZZZ > >
//   XXX cle d'indexation. Pour les mots composés: "motA;motB;motC"
//   YYY frequence
//   ZZZ lemme simple ou composé 'entier'
class Lexicon : public map<LimaString,std::pair<uint64_t,LimaString> >
{
public:
    Lexicon():map<LimaString,std::pair<uint64_t,LimaString> >() {}
    ~Lexicon() {}
    void add(const LimaString& str,
             const LimaString& strconv) {
        map<LimaString,std::pair<uint64_t,LimaString> >::iterator
        it=find(str);
        if (it==end()) {
            (*this)[str]=make_pair(1,strconv);
        }
        else {
            (*it).second.first++;
        }
    }
private:
};

// For simple tokens, it's themselves
// For complex tokens, each simple token is separated with a ';'
LimaString getStringDecomp(boost::shared_ptr< BoWToken > token) {
    LimaString str;
    std::deque< BoWComplexToken::Part >::const_iterator it, it_end;
    boost::shared_ptr< BoWComplexToken> complexToken;
    switch (token->getType()) {
    case BoWType::BOW_TOKEN:
        //cerr << "token is a simple token -> " << token->getString() << endl;
        return token->getString();
        break;
    case BoWType::BOW_TERM:
    case BoWType::BOW_NAMEDENTITY:
        //cerr << "token is a complex token" << endl;
        complexToken=boost::dynamic_pointer_cast<BoWComplexToken>(token);
        it=complexToken->getParts().begin(); it_end=complexToken->getParts().end();
        str=getStringDecomp((*it).getBoWToken());
        it++;
        for (; it!=it_end; it++) {
            str+=Misc::utf8stdstring2limastring(";")+getStringDecomp((*it).getBoWToken());
        }
        break;
    default: ;
    }
    //cerr << "-> " << str << endl;
    return str;
}

// local handler to read document
class GetLexiconBoWDocumentHandler : public AbstractBoWDocumentHandler
{
public:
    GetLexiconBoWDocumentHandler(Lexicon& lex,
                                 const PropertyManager& macroManager,
                                 const PropertyAccessor& propertyAccessor,
                                 set<LinguisticCode>& referenceProperties,
                                 bool filterCategory):
            m_lex(lex),
            m_propertyAccessor(propertyAccessor),
            m_referenceProperties(referenceProperties),
            m_filterCategory(filterCategory)
    {LIMA_UNUSED(macroManager)}

    ~GetLexiconBoWDocumentHandler() {}

    void openSBoWNode(const Misc::GenericDocumentProperties* /*properties*/,
                      const std::string& /*elementName*/) override
    {}
    void openSBoWIndexingNode(const Misc::GenericDocumentProperties* /*properties*/,
                              const std::string& /*elementName*/) override
    {}
    void processSBoWText(const BoWText* boWText,
                         bool useIterators, bool /*useIndexIterator*/) override;
    void processProperties(const Misc::GenericDocumentProperties* /*properties*/,
                           bool /*useIterators*/, bool /*useIndexIterator*/) override
    {}
    void closeSBoWNode() override
    {}

private:
    Lexicon& m_lex;
    const PropertyAccessor& m_propertyAccessor;
    set<LinguisticCode>& m_referenceProperties;
    bool m_filterCategory;
};

void GetLexiconBoWDocumentHandler::processSBoWText(const BoWText* text,
        bool useIterators, bool /*useIndexIterator*/)
{
  LIMA_UNUSED(useIterators);
    BoWTokenIterator it(*text);
    while (! it.isAtEnd()) {
        boost::shared_ptr< BoWToken > token = boost::dynamic_pointer_cast<BoWToken>((it.getElement()));
//     const std::string& stringProp = m_macroManager.getPropertySymbolicValue(token.getCategory());
        if (m_filterCategory) {
            set<LinguisticCode>::const_iterator referencePropertyIt =
                m_referenceProperties.find(m_propertyAccessor.readValue(token->getCategory()));
            if ( referencePropertyIt != m_referenceProperties.end() ) {
                m_lex.add(getStringDecomp(token),token->getString());
            }
        }
        else {
            m_lex.add(getStringDecomp(token),token->getString());
        }
        it++;
    }
}

//! @brief read a text Bow file and fills a lexicon
//! @param fileIn the file to read
//! @param reader the file reader
//! @param lex the lexicon to fill
//! @param propertyAccessor
//! @param referenceProperties
void readBowFileText(ifstream& fileIn,
                     BoWBinaryReader& reader,
                     Lexicon& lex,
                     const PropertyAccessor& propertyAccessor,
                     set<LinguisticCode>& referenceProperties
                    )
{
    BoWText text;
    reader.readBoWText(fileIn,text);

    bool filterCategory = false;
    if ( referenceProperties.size() > 0 ) {
        filterCategory = true;
    }

    BoWTokenIterator it(text);
    while (! it.isAtEnd()) {
        boost::shared_ptr< BoWToken > token = boost::dynamic_pointer_cast<BoWToken>((it.getElement()));
        if (filterCategory) {
            set<LinguisticCode>::const_iterator referencePropertyIt =
                referenceProperties.find(propertyAccessor.readValue(token->getCategory()));
            if ( referencePropertyIt != referenceProperties.end() ) {
                lex.add(getStringDecomp(token),token->getString());
            }
        }
        else {
            lex.add(getStringDecomp(token),token->getString());
        }
        it++;
    }
}

// read documents in a file, using the document buffer given as argument
// (can be BoWDocument* or BoWDocumentST*)
void readDocuments(ifstream& fileIn, BoWDocument* document,
                   BoWBinaryReader& reader,
                   Lexicon& lex,
                   const PropertyManager& macroManager,
                   const PropertyAccessor& propertyAccessor,
                   set<LinguisticCode>& referenceProperties )
{
    bool filterCategory = false;
//  LinguisticCode referenceProperty;
    if ( referenceProperties.size() > 0 ) {
//    referenceProperty =  *(referenceProperties.begin());
        filterCategory = true;
    }

    GetLexiconBoWDocumentHandler handler(lex,macroManager,propertyAccessor,
                                         referenceProperties,filterCategory);
    while (! fileIn.eof())
    {
        reader.readBoWDocumentBlock(fileIn,*document,handler,true,false);
    }
}

//**********************************************************************
//
// M A I N
//
//**********************************************************************
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

  if (argc<1) {
        cerr << *USAGE;
        return EXIT_FAILURE;
    }
    readCommandLineArguments(argc,argv);
    if (param.help) {
        cerr << *HELP;
        return EXIT_FAILURE;
    }


    string resourcesPath=qEnvironmentVariableIsEmpty("LIMA_RESOURCES")
        ?"/usr/share/apps/lima/resources"
        :string(qgetenv("LIMA_RESOURCES").constData());
    string configDir=qEnvironmentVariableIsEmpty("LIMA_CONF")
        ?"/usr/share/config/lima"
        :string(qgetenv("LIMA_CONF").constData());

    if ( (!param.language.size()) && (!param.codeFile.size()) ) {
        cerr << "no codefile nor language specified !" << endl;
        cerr << "Use e.g option '-l fre'." << endl;
        cerr << "Option '-h' gives full help" << endl;
        return EXIT_FAILURE;
    }
    else if ( param.language.size() ) {
        param.codeFile=resourcesPath+"/LinguisticProcessings/"+param.language+"/code-"+param.language+".xml";
    }

    cerr << "read proccodeManager from file " << param.codeFile << "..." << endl;
    PropertyCodeManager propcodemanager;
    propcodemanager.readFromXmlFile(param.codeFile);
    cerr << "get macroManager..." << endl;
    const PropertyManager& macroManager = propcodemanager.getPropertyManager("MACRO");
    const PropertyAccessor& propertyAccessor = macroManager.getPropertyAccessor();
    set<LinguisticCode> referenceProperties;
    for ( std::vector<string>::const_iterator macro = param.macro.begin() ;
            macro != param.macro.end() ; macro++ ) {
        cerr << "referenceProperties.insert(" << *macro << ")" << endl;
        LinguisticCode referenceProperty = macroManager.getPropertyValue(*macro);
        referenceProperties.insert(referenceProperty);
    }

    cerr << "referencePropertySet= ";
    set<LinguisticCode>::iterator propIt = referenceProperties.begin();
    if ( propIt != referenceProperties.end() ) {
        const std::string& symbol = macroManager.getPropertySymbolicValue(*propIt);
        cerr << symbol;
        propIt++;
    }
    for ( ; propIt != referenceProperties.end() ; propIt++ ) {
        const std::string& symbol = macroManager.getPropertySymbolicValue(*propIt);
        cerr << ", " << symbol;
    }
    cerr << endl;

    Lexicon lex;

    // read all files and count terms
    vector<string>::const_iterator
    file=param.inputFiles.begin(),
         file_end=param.inputFiles.end();
    for (;file!=file_end; file++) {

        ifstream fileIn((*file).c_str(), std::ifstream::binary);
        if (! fileIn) {
            cerr << "cannot open input file [" << *file << "]" << endl;
            continue;
        }
        BoWBinaryReader reader;
        try {
            reader.readHeader(fileIn);
        }
        catch (exception& e) {
            cerr << "Error: " << e.what() << endl;
            return EXIT_FAILURE;
        }

        switch (reader.getFileType()) {
        case BOWFILE_TEXT: {
            cerr << "Build lexicon from BoWText [" << *file << "]" << endl;
            try {
                readBowFileText(fileIn,reader, lex, propertyAccessor, referenceProperties);
            }
            catch (exception& e) {
                cerr << "Error: " << e.what() << endl;
            }
            break;
        }
        case BOWFILE_DOCUMENTST: {
            cerr << "ReadBoWFile: file contains a BoWDocumentST  -> not treated" << endl;
            break;
        }
        case BOWFILE_DOCUMENT: {
            cerr << "ReadBoWFile: build BoWdocument from  " << *file<< endl;
            BoWDocument* document=new BoWDocument();
            try {
                cerr << "ReadBoWFile: extract terms... " << endl;
                readDocuments(fileIn,document,reader, lex, macroManager, propertyAccessor, referenceProperties);
            }
            catch (exception& e) {
                cerr << "Error: " << e.what() << endl;
            }
            fileIn.close();
            delete document;
            break;
        }
        default: {
            cerr << "format of file " << reader.getFileTypeString() << " not managed"
                 << endl;
            return EXIT_FAILURE;
        }
        }
    }

    // output stream (default is 'cout')
    std::ostream *s_out = NULL;

    // Manage output
    if (param.outputFilename.length() == 0)
        s_out = &std::cout;
    else
        s_out = new std::ofstream(param.outputFilename.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

    // output lexicon
    Lexicon::const_iterator w=lex.begin(), w_end=lex.end();
    for (;w!=w_end; w++) {
        (*s_out) << Common::Misc::limastring2utf8stdstring((*w).second.second) << "|"
        << Common::Misc::limastring2utf8stdstring((*w).first) << "|"
        << (*w).second.first << endl;
    }

    // Close output file (if any)
    if (param.outputFilename.length() != 0 && s_out != NULL) {
        dynamic_cast<std::ofstream*>(s_out)->close();
        delete s_out;
    }

    return EXIT_SUCCESS;
}
