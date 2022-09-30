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
#define LOG_MESSAGE_WITH_PROLOG(stream, msg) LEMMALOGINIT; LOG_MESSAGE(stream, msg);
#else
    #define LOG_MESSAGE(stream, msg) ;
    #define LOG_MESSAGE_WITH_PROLOG(stream, msg) ;
#endif

    static SimpleFactory<MediaProcessUnit, RnnDependencyParser> RnnDependencyParserFactory(RNNDEPENDENCYPARSER_CLASSID); // clazy:exclude=non-pod-global-static

    CONFIGURATIONHELPER_LOGGING_INIT(LEMMALOGINIT);
    typedef DependencyParser<typename TokenSequenceAnalyzer<>::TokenIterator> DependencyParserFromTSA;
    class RnnDependencyParserPrivate: public ConfigurationHelper {
    public:
        RnnDependencyParserPrivate();
        ~RnnDependencyParserPrivate() ;
        void init(GroupConfigurationStructure& unitConfiguration);
        void analyzer(vector<segmentation::token_pos>& buffer);
        void insertTokenInfo(TokenSequenceAnalyzer<>::TokenIterator &ti);

        MediaId m_language;
        FsaStringsPool* m_stringsPool;
        QString m_data;
        DependencyParserFromTSA* m_dependencyParser;
        TokenSequenceAnalyzer<>* m_sequenceAnalyser;
        function<void()> m_load_fn;
        std::shared_ptr<StringIndex> m_stridx;
        PathResolver m_pResolver;
        std::vector<std::map<std::string,std::string>> m_tags;
        std::vector<QString> m_lemmas;
        const Common::PropertyCode::PropertyAccessor* m_microAccessor;
        bool m_loaded;
    };

    RnnDependencyParserPrivate::RnnDependencyParserPrivate(): ConfigurationHelper("RnnDependencyParserPrivate", THIS_FILE_LOGGING_CATEGORY()), m_stringsPool(nullptr), m_stridx(), m_loaded(false)
    {

    }

    RnnDependencyParserPrivate::~RnnDependencyParserPrivate() = default;


    RnnDependencyParser::RnnDependencyParser(): m_d(new RnnDependencyParserPrivate()) {

    }

    RnnDependencyParser::~RnnDependencyParser() = default;

    void RnnDependencyParser::init(GroupConfigurationStructure &unitConfiguration, Manager *manager)
    {
        LOG_MESSAGE_WITH_PROLOG(LDEBUG, "RnnDependencyParser::init");

        m_d->m_language = manager->getInitializationParameters().media;
        m_d->m_stringsPool = &MediaticData::changeable().stringsPool(m_d->m_language);

        m_d->init(unitConfiguration);

    }

    Lima::LimaStatusCode
    RnnDependencyParser::process(Lima::AnalysisContent &analysis) const {
        LOG_MESSAGE_WITH_PROLOG(LDEBUG, "start RnnPosTager");
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
            LIMA_EXCEPTION_SELECT_LOGINIT(LEMMALOGINIT,
                                          "RnnDependencyParserPrivate::init: Can't parse language id " << udlang.c_str(),
                                          Lima::InvalidConfiguration);
        }

        dependency_parser_name.replace(QString("$udlang"), QString(udlang.c_str()));
        tagger_model_name.replace(QString("$udlang"), QString(udlang.c_str()));


        auto dependency_parser_file_name = findFileInPaths(resources_path,
                                                      QString::fromUtf8("/RnnTagger/%1/%2.pt")
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

        std::vector<std::string> class_names;
        std::vector<std::vector<string>> classes;
        m_sequenceAnalyser->get_classes_from_fn(tagger_model_file_name.toStdString(),class_names, classes);

        m_load_fn = [this, dependency_parser_file_name, tagger_model_file_name, class_names]()
        {
            if (m_loaded)
            {
                return;
            }

            m_dependencyParser = new DependencyParserFromTSA(dependency_parser_file_name.toStdString(),m_pResolver,m_stridx,class_names, 1024, 8);
            m_loaded = true;
        };

        if (!isInitLazy())
        {
            m_load_fn();
        }
        for (size_t i = 0; i < classes.size(); ++i)
        {
            m_dependencyParser->set_classes(i, class_names[i], classes[i]);
        }

    }


}