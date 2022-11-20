// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include <QtCore/QTemporaryFile>
#include <QtCore/QRegularExpression>
#include <QDir>


#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/tools/FileUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/timeUtilsController.h"

#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/common/helpers/ConfigurationHelper.h"
#include "linguisticProcessing/common/helpers/LangCodeHelpers.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"

#include "RnnDependencyParser.h"
#include "deeplima/dependency_parser.h"
#include "deeplima/token_sequence_analyzer.h"
#include "deeplima/token_type.h"
#include "deeplima/dumper_conllu.h"
#include "helpers/path_resolver.h"
#include "linguisticProcessing/core/DeepLimaUnits/TokenIteratorData.h"
#include "core/SyntacticAnalysis/SyntacticData.h"


#define DEBUG_THIS_FILE true

using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::PropertyCode;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::AnnotationGraphs;
using namespace std;
using namespace deeplima;


namespace Lima::LinguisticProcessing::DeepLimaUnits::RnnDependencyParser {

#if defined(DEBUG_LP) && defined(DEBUG_THIS_FILE)
#define LOG_MESSAGE(stream, msg) stream << msg;
#define LOG_MESSAGE_WITH_PROLOG(stream, msg) SALOGINIT; LOG_MESSAGE(stream, msg);
#else
    #define LOG_MESSAGE(stream, msg) ;
    #define LOG_MESSAGE_WITH_PROLOG(stream, msg) ;
#endif

    static SimpleFactory<MediaProcessUnit, RnnDependencyParser> RnnDependencyParserFactory(RNNDEPENDENCYPARSER_CLASSID); // clazy:exclude=non-pod-global-static

    CONFIGURATIONHELPER_LOGGING_INIT(SALOGINIT);

    class RnnDependencyParserPrivate: public ConfigurationHelper {
    public:
        RnnDependencyParserPrivate();
        ~RnnDependencyParserPrivate() = default;
        void init(GroupConfigurationStructure& unitConfiguration);
        void analyzer(std::shared_ptr<TokenSequenceAnalyzer<>::TokenIterator> ti);
        void insertDependencies(DependencyParser::TokenIterator& ti);
        MediaId m_language;
        QString m_data;
        std::shared_ptr<DependencyParser> m_dependencyParser = nullptr;
        // std::shared_ptr<TokenSequenceAnalyzer<>> m_sequenceAnalyser = nullptr;
        function<void()> m_load_fn;
        std::shared_ptr< StringIndex > m_stridx;
        PathResolver m_pResolver;
        std::vector<std::map<std::string,std::string>> m_tags;
        std::vector<QString> m_lemmas;
        // std::vector<typename DependencyParser::token_with_analysis_t> m_tokens;
        std::vector<std::string> m_class_names;
        std::vector<std::vector<string>> m_classes;
        std::vector<uint32_t> m_heads;
        bool m_loaded;
    };

    RnnDependencyParserPrivate::RnnDependencyParserPrivate(): ConfigurationHelper("RnnDependencyParserPrivate",
                                                                                  THIS_FILE_LOGGING_CATEGORY()),
                                                              m_stridx(new StringIndex()),
                                                              m_loaded(false)
    {

    }

    RnnDependencyParser::RnnDependencyParser(): m_d(new RnnDependencyParserPrivate()) {

    }

    RnnDependencyParser::~RnnDependencyParser() {
        delete m_d;
    }

    void RnnDependencyParser::init(GroupConfigurationStructure &unitConfiguration, Manager *manager)
    {
        LOG_MESSAGE_WITH_PROLOG(LDEBUG, "RnnDependencyParser::init");

        m_d->m_language = manager->getInitializationParameters().media;

        m_d->init(unitConfiguration);

    }

    Lima::LimaStatusCode
    RnnDependencyParser::process(Lima::AnalysisContent &analysis) const
    {
        TimeUtils::updateCurrentTime();
        TimeUtilsController RnnDependencyParserProcessTime("RnnDependencyParser");
        LOG_MESSAGE_WITH_PROLOG(LDEBUG, "RnnDependencyParser::process");
        auto tiData = dynamic_pointer_cast<TokenIteratorData>(analysis.getData("TokenIterator"));
        if (tiData == nullptr)
        {
            SALOGINIT;
            LERROR << "Can't Process RnnDependencyParser : missing data 'TokenIterator'";
            return MISSING_DATA;
        }
        auto stridxPtr = tiData->getStringIndex();
        m_d->m_dependencyParser->setStringIndex(stridxPtr);
        auto tokenIterator = tiData->getTokenIterator();
        tokenIterator->reset(0);
        LERROR << "is end : " << tokenIterator->end() << "\n";
        LERROR << "position : " << tokenIterator->position() << "\n";
        while(!tokenIterator->end()){
            LERROR <<"text: "<< tokenIterator->form() << "\n";
            for(uint cat=0;cat < m_d->m_class_names.size();cat++){
                LERROR << "RnnDependencyParser::process class:" << m_d->m_class_names[cat]
                        << "index: " << tokenIterator->token_class(cat) << "\n";
            }
            tokenIterator->next();
        }
        tokenIterator->reset();


        auto anagraph = dynamic_pointer_cast<AnalysisGraph>(analysis.getData("PosGraph"));
        if (anagraph == nullptr)
        {
            LERROR << "no PosGraph ! abort";
            return MISSING_DATA;
        }

        auto syntacticData = dynamic_pointer_cast<SyntacticAnalysis::SyntacticData>(analysis.getData("SyntacticData"));
        if (syntacticData == nullptr)
        {
            syntacticData=std::make_shared<SyntacticAnalysis::SyntacticData>(anagraph.get(), nullptr);
            analysis.setData("SyntacticData",syntacticData);
        }
        syntacticData->setupDependencyGraph();

        m_d->analyzer(tokenIterator);

        uint curToken=0;
        for(auto head: m_d->m_heads) {
            curToken++;
            syntacticData->addVertex();
            LOG_MESSAGE(LERROR,"head is : " << head);
            if(head != 0) {
                syntacticData->addRelationNoChain(1,curToken,head);
            }
        }
        TimeUtils::logElapsedTime("RnnDependencyParser");
        return SUCCESS_ID;
    }

    void RnnDependencyParserPrivate::init(GroupConfigurationStructure& unitConfiguration)
    {

        m_data = QString(getStringParameter(unitConfiguration, "data", 0, "SentenceBoundaries").c_str());
        QString dependency_parser_model_prefix = getStringParameter(unitConfiguration, "dependency_parser_model_prefix", ConfigurationHelper::REQUIRED | ConfigurationHelper::NOT_EMPTY).c_str();
        QString tagger_model_prefix = getStringParameter(unitConfiguration, "tagger_model_prefix", ConfigurationHelper::REQUIRED | ConfigurationHelper::NOT_EMPTY).c_str();
        LOG_MESSAGE_WITH_PROLOG(LDEBUG, "RnnDependencyParserPrivate::init dependency parser model: " << dependency_parser_model_prefix);

        QString lang_str = MediaticData::single().media(m_language).c_str();
        QString resources_path = MediaticData::single().getResourcesPath().c_str();
        QString dependency_parser_name = dependency_parser_model_prefix;
        QString tagger_model_name = tagger_model_prefix;

        string udlang;
        MediaticData::single().getOptionValue("udlang", udlang);

        if (!fix_lang_codes(lang_str, udlang))
        {
            LIMA_EXCEPTION_SELECT_LOGINIT(SALOGINIT,
                                          "RnnDependencyParserPrivate::init: Can't parse language id " << udlang.c_str(),
                                          Lima::InvalidConfiguration);
        }

        dependency_parser_name.replace(QString("$udlang"), QString(udlang.c_str()));
        tagger_model_name.replace(QString("$udlang"), QString(udlang.c_str()));


        auto dependency_parser_file_name = findFileInPaths(resources_path,
                                                      QString::fromUtf8("/RnnDependencyParser/%1/%2.pt")
                                                              .arg(lang_str, dependency_parser_name));

        auto tagger_model_file_name = findFileInPaths(resources_path,
                                                      QString::fromUtf8("/RnnTagger/%1/%2.pt")
                                                              .arg(lang_str, tagger_model_name));
        if (dependency_parser_file_name.isEmpty())
        {
            throw InvalidConfiguration("RnnDependencyParserPrivate::init: dependency parser model file not found.");
        }

        if (tagger_model_file_name.isEmpty())
        {
            throw InvalidConfiguration("RnnTokensAnalyzerPrivate::init: tagger model file not found.");
        }

        LOG_MESSAGE(LDEBUG, "RnnDependencyParserPrivate::init call TokenSequenceAnalyzer<>().get_classes_from_fn");
        TokenSequenceAnalyzer<>().get_classes_from_fn(tagger_model_file_name.toStdString(), m_class_names, m_classes);
        auto temp_classes_names = m_class_names;
        auto temp_classes = m_classes;
        temp_classes_names.erase(temp_classes_names.begin()+1);
        temp_classes.erase(temp_classes.begin()+1);
        m_load_fn = [this, dependency_parser_file_name, tagger_model_file_name, temp_classes_names, temp_classes]()
        {
            if (m_loaded)
            {
                return;
            }

// <<<<<<< HEAD
            m_dependencyParser = std::make_shared<DependencyParser>(dependency_parser_file_name.toStdString(),
                                                                           m_pResolver,m_stridx,m_class_names, 1024, 8);
// =======
//             m_dependencyParser = std::make_shared<DependencyParserFromTSA>(dependency_parser_file_name.toStdString(),
//                                                                            m_pResolver,m_stridx,temp_classes_names, 1024, 8);
// >>>>>>> origin/dependency-parser-module
            for (size_t i = 0; i < temp_classes.size(); i++)
            {
                m_dependencyParser->set_classes(i, temp_classes_names[i], temp_classes[i]);
            }
            m_loaded = true;
        };

        if (!isInitLazy())
        {
            m_load_fn();
        }


    }

    void RnnDependencyParserPrivate::analyzer(std::shared_ptr<TokenSequenceAnalyzer<>::TokenIterator> ti) {
        m_dependencyParser->register_handler([this](const StringIndex& stridx,
                                                 const std::vector<typename DependencyParser::token_with_analysis_t>& tokens,
                                                 std::shared_ptr< StdMatrix<uint32_t> > classes,
                                                 size_t begin,
                                                 size_t end)
                                 {
        typename DependencyParser::TokenIterator dti(stridx,
                                                           tokens,
                                                           classes,
                                                           begin,
                                                           end);
                                     insertDependencies(dti);
                                 });
        (*m_dependencyParser)(*ti);
        m_dependencyParser->finalize();
    }

    void RnnDependencyParserPrivate::insertDependencies(DependencyParser::TokenIterator& ti) {
        while (!ti.end()) {
            m_heads.push_back(ti.head());
            ti.next();
        }
    }


}
