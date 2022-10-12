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

#include "RnnTokensAnalyzer.h"
#include "deeplima/token_sequence_analyzer.h"
#include "deeplima/token_type.h"
#include "deeplima/dumper_conllu.h"
#include "helpers/path_resolver.h"
#include "linguisticProcessing/core/DeepLimaUnits/TokenIteratorData.h"

#define DEBUG_THIS_FILE true

using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::PropertyCode;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::AnnotationGraphs;
using namespace std;
using namespace deeplima;


namespace Lima::LinguisticProcessing::DeepLimaUnits::RnnTokensAnalyzer {
#if defined(DEBUG_LP) && defined(DEBUG_THIS_FILE)
#define LOG_MESSAGE(stream, msg) stream << msg;
#define LOG_MESSAGE_WITH_PROLOG(stream, msg) LEMMALOGINIT; LOG_MESSAGE(stream, msg);
#else
    #define LOG_MESSAGE(stream, msg) ;
    #define LOG_MESSAGE_WITH_PROLOG(stream, msg) ;
#endif

    static SimpleFactory<MediaProcessUnit, RnnTokensAnalyzer> RnnTokensAnalyzerFactory(RNNTOKENSANALYZER_CLASSID); // clazy:exclude=non-pod-global-static

    CONFIGURATIONHELPER_LOGGING_INIT(LEMMALOGINIT);

    class RnnTokensAnalyzerPrivate: public ConfigurationHelper {
    public:
        RnnTokensAnalyzerPrivate();
        ~RnnTokensAnalyzerPrivate() ;
        void init(GroupConfigurationStructure& unitConfiguration);
        void analyzer(vector<segmentation::token_pos>& buffer);
        void insertTokenInfo(TokenSequenceAnalyzer<>::TokenIterator &ti);
        dumper::AnalysisToConllU<TokenSequenceAnalyzer<>::TokenIterator> m_dumper;

        Lima::AnalysisContent* m_analysis;
        MediaId m_language;
        FsaStringsPool* m_stringsPool;
        QString m_data;
        TokenSequenceAnalyzer<>* m_tokensAnalyzer;
        function<void()> m_load_fn;
        StringIndex m_stridx;
        PathResolver m_pResolver;
        std::vector<std::map<std::string,std::string>> m_tags;
        std::vector<QString> m_lemmas;
        const Common::PropertyCode::PropertyAccessor* m_microAccessor;
        bool m_loaded;
        TokenIteratorData* m_tiData;
        TokenSequenceAnalyzer<>::TokenIterator* m_ti;
        TokenSequenceAnalyzer<>::OutputMatrix* m_matrix;
    };

    RnnTokensAnalyzerPrivate::RnnTokensAnalyzerPrivate(): ConfigurationHelper("RnnTokensAnalyzerPrivate", THIS_FILE_LOGGING_CATEGORY()), m_stringsPool(nullptr), m_stridx(), m_loaded(false), m_tiData(new TokenIteratorData())
    {

    }

    RnnTokensAnalyzerPrivate::~RnnTokensAnalyzerPrivate() = default;


    RnnTokensAnalyzer::RnnTokensAnalyzer(): m_d(new RnnTokensAnalyzerPrivate()) {

    }

    RnnTokensAnalyzer::~RnnTokensAnalyzer() = default;

    void RnnTokensAnalyzer::init(GroupConfigurationStructure &unitConfiguration, Manager *manager)
    {
        LOG_MESSAGE_WITH_PROLOG(LDEBUG, "RnnTokensAnalyzer::init");

        m_d->m_language = manager->getInitializationParameters().media;
        m_d->m_stringsPool = &MediaticData::changeable().stringsPool(m_d->m_language);

        m_d->init(unitConfiguration);

    }

    Lima::LimaStatusCode
    RnnTokensAnalyzer::process(Lima::AnalysisContent &analysis) const {
        LOG_MESSAGE_WITH_PROLOG(LDEBUG, "start RnnPosTager");
        m_d->m_analysis = &analysis;
        auto anagraph = dynamic_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
        auto srcgraph = anagraph->getGraph();
        auto endVx = anagraph->lastVertex();
        /// Creates the posgraph with the second parameter (deleteTokenWhenDestroyed)
        /// set to false as the tokens are owned by the anagraph
        /// @note : tokens newly created later will be owned by their creator and have
        /// to be deleted by this one
        auto posgraph = new LinguisticAnalysisStructure::AnalysisGraph("PosGraph",
                                                                       m_d->m_language,
                                                                       false,
                                                                       true);
        analysis.setData("PosGraph",posgraph);
        const auto& propertyCodeManager = dynamic_cast<const LanguageData&>(
                MediaticData::single().mediaData(m_d->m_language)).getPropertyCodeManager();
        const auto& microManager = propertyCodeManager.getPropertyManager("MICRO");
        /** Creation of an annotation graph if necessary*/
        AnnotationData* annotationData = dynamic_cast< AnnotationData* >(analysis.getData("AnnotationData"));
        if (annotationData==nullptr)
        {
            annotationData = new AnnotationData();
            /** Creates a node in the annotation graph for each node of the
            * morphosyntactic graph. Each new node is annotated with the name mrphv and
            * associated to the morphosyntactic vertex number */
            if (dynamic_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph")) != nullptr)
            {
                dynamic_cast<AnalysisGraph*>(
                        analysis.getData("AnalysisGraph"))->populateAnnotationGraph(
                        annotationData,
                        "AnalysisGraph");
            }
            analysis.setData("AnnotationData",annotationData);
        }
        if (num_vertices(*srcgraph)<=2)
        {
            return SUCCESS_ID;
        }

        VertexTokenPropertyMap vTokens = get(vertex_token, *srcgraph);
        auto currentVx = anagraph->firstVertex();
        LinguisticGraph* resultgraph=posgraph->getGraph();
        remove_edge(posgraph->firstVertex(),posgraph->lastVertex(),*resultgraph);

        std::vector<segmentation::token_pos> buffer;
        std::vector< LinguisticGraphVertex > anaVertices;
        std::vector<std::string> v;

        while(currentVx != endVx){
            if (currentVx != 0 && vTokens[currentVx] != nullptr) {
                const auto& src = vTokens[currentVx];
                v.push_back(src->stringForm().toStdString());
                buffer.emplace_back();
                anaVertices.push_back(currentVx);
            }
            LinguisticGraphOutEdgeIt it, it_end;
            boost::tie(it, it_end) = boost::out_edges(currentVx, *srcgraph);
            if (it != it_end)
            {
                currentVx = boost::target(*it, *srcgraph);
            }
            else
            {
                currentVx = endVx;
            }
        }
        /**
         * Construction of the tokens used in the tagger.
         */
        for(unsigned long k=0;k<anaVertices.size();k++){
            currentVx = anaVertices[k];
            if (currentVx != 0 && vTokens[currentVx] != nullptr) {
                const auto& src = vTokens[currentVx];
                auto& token = buffer[k];
                token.m_offset = src->position();
                token.m_len = src->length();
                token.m_pch = v[k].c_str();
                token.m_flags = segmentation::token_pos::flag_t(src->status().getStatus() & StatusType::T_SENTENCE_BRK);
            }
        }
        m_d->analyzer(buffer);
        std::vector<LinguisticGraphVertex>::size_type anaVerticesIndex = 0;
        LinguisticGraphVertex previousPosVertex = posgraph->firstVertex();
        /*
         * Here we add the part of speech data to the tokens
         * Adding link beetween the node in the analysis graph and the pos graph.
         */
        while (anaVerticesIndex < anaVertices.size()){
            auto anaVertex = anaVertices[anaVerticesIndex];
            auto newVx = boost::add_vertex(*resultgraph);
            auto agv =  annotationData->createAnnotationVertex();
            annotationData->addMatching("PosGraph", newVx, "annot", agv);
            annotationData->addMatching("AnalysisGraph", anaVertex, "PosGraph", newVx);
            annotationData->annotate(agv, QString::fromUtf8("PosGraph"), newVx);

            auto srcToken = get(vertex_token,*srcgraph,anaVertex);


            auto posData = new MorphoSyntacticData();

            LinguisticElement lElement = LinguisticElement();
            for(const auto& name: m_d->m_tokensAnalyzer->get_class_names()){
                PropertyManager propertyManager = microManager;
                if(name=="upos"){
                    propertyManager = propertyCodeManager.getPropertyManager("MICRO");
                }
                else if(name=="ExtPos" || name=="Style"){
                    continue;
                }
                else if(name=="xpos")
                {
                    propertyManager = propertyCodeManager.getPropertyManager("MACRO");
                }
                else{
                    propertyManager = propertyCodeManager.getPropertyManager(name);
                }
                auto value = propertyManager.getPropertyValue(m_d->m_tags[anaVerticesIndex][name]);
                if(value.toBool()){
                    LOG_MESSAGE(LDEBUG, "value: " << value);
                    propertyManager.getPropertyAccessor().writeValue(value,lElement.properties);
                }
            }
            FsaStringsPool* sp = &MediaticData::changeable().stringsPool(m_d->m_language);
            lElement.lemma = (*sp)[m_d->m_lemmas[anaVerticesIndex]];
            posData->push_back(lElement);

            put(vertex_data,*resultgraph,newVx,posData);
            put(vertex_token,*resultgraph,newVx,srcToken);


            boost::add_edge(previousPosVertex, newVx, *resultgraph);

            previousPosVertex = newVx;
            anaVerticesIndex++;
        }
        boost::add_edge(previousPosVertex, posgraph->lastVertex(), *resultgraph);
        LOG_MESSAGE(LDEBUG, "RnnPosLemmatizer postagging done.");
        return SUCCESS_ID;
    }

    void RnnTokensAnalyzerPrivate::init(GroupConfigurationStructure& unitConfiguration)
    {
        m_data = QString(getStringParameter(unitConfiguration, "data", 0, "SentenceBoundaries").c_str());
        QString tagger_model_prefix = getStringParameter(unitConfiguration, "tagger_model_prefix", ConfigurationHelper::REQUIRED | ConfigurationHelper::NOT_EMPTY).c_str();
        QString lemmatizer_model_prefix = getStringParameter(unitConfiguration, "lemmatizer_model_prefix", ConfigurationHelper::REQUIRED | ConfigurationHelper::NOT_EMPTY).c_str();
        LOG_MESSAGE_WITH_PROLOG(LDEBUG, "RnnTokensAnalyzerPrivate::init tagger model: " << tagger_model_prefix << " lemmatizer model: " << lemmatizer_model_prefix);

        QString lang_str = MediaticData::single().media(m_language).c_str();
        QString resources_path = MediaticData::single().getResourcesPath().c_str();
        QString tagger_model_name = tagger_model_prefix;
        QString lemmatizer_model_name = lemmatizer_model_prefix;
        string udlang;
        MediaticData::single().getOptionValue("udlang", udlang);

        if (!fix_lang_codes(lang_str, udlang))
        {
            LIMA_EXCEPTION_SELECT_LOGINIT(LEMMALOGINIT,
                                          "RnnTokensAnalyzerPrivate::init: Can't parse language id " << udlang.c_str(),
                                          Lima::InvalidConfiguration);
        }

        tagger_model_name.replace(QString("$udlang"), QString(udlang.c_str()));
        lemmatizer_model_name.replace(QString("$udlang"), QString(udlang.c_str()));

        auto tagger_model_file_name = findFileInPaths(resources_path,
                                                      QString::fromUtf8("/RnnTagger/%1/%2.pt")
                                                       .arg(lang_str, tagger_model_name));
        auto lemmatizer_model_file_name = findFileInPaths(resources_path,
                                                      QString::fromUtf8("/RnnLemmatizer/%1/%2.pt")
                                                              .arg(lang_str, lemmatizer_model_name));
        if (tagger_model_file_name.isEmpty())
        {
            throw InvalidConfiguration("RnnTokensAnalyzerPrivate::init: tagger model file not found.");
        }
        if (lemmatizer_model_file_name.isEmpty())
        {
            //throw InvalidConfiguration("RnnTokensAnalyzerPrivate::init: lemmatizer model file not found.");
            lemmatizer_model_file_name = "";
        }

        m_load_fn = [this, tagger_model_file_name, lemmatizer_model_file_name]()
        {
            if (m_loaded)
            {
                return;
            }
            m_tokensAnalyzer = new TokenSequenceAnalyzer<>(tagger_model_file_name.toStdString(),lemmatizer_model_file_name.toStdString(),m_pResolver,1024,8);
            m_loaded = true;
        };

        if (!isInitLazy())
        {
            m_load_fn();
        }
        for (size_t i = 0; i < m_tokensAnalyzer->get_classes().size(); ++i)
        {
            m_dumper.set_classes(i, m_tokensAnalyzer->get_class_names()[i], m_tokensAnalyzer->get_classes()[i]);
        }

    }

    void RnnTokensAnalyzerPrivate::analyzer(vector<segmentation::token_pos> &buffer) {
        m_tokensAnalyzer->register_handler([this](const StringIndex& stridx,
                                                  const token_buffer_t<>& tokens,
                                                  const std::vector<StringIndex::idx_t>& lemmata,
                                                  const TokenSequenceAnalyzer<>::OutputMatrix& classes,
                                                  size_t begin,
                                                  size_t end){
            TokenSequenceAnalyzer<>::TokenIterator ti(stridx, tokens, lemmata, classes, begin, end);
            insertTokenInfo(ti);
            ti.reset(0);
            m_ti = new TokenSequenceAnalyzer<>::TokenIterator(ti);
            m_matrix = new TokenSequenceAnalyzer<>::OutputMatrix(classes);
            m_tiData->setTokenIterator(m_ti);
            m_analysis->setData("TokenIterator", m_tiData);
        });
        LOG_MESSAGE_WITH_PROLOG(LDEBUG,buffer[0].m_pch);
        (*m_tokensAnalyzer)(buffer, buffer.size());
        m_tokensAnalyzer->finalize();
    }

    void RnnTokensAnalyzerPrivate::insertTokenInfo(TokenSequenceAnalyzer<>::TokenIterator &ti) {
        auto classes = m_dumper.getMClasses();
        auto class_names = m_tokensAnalyzer->get_class_names();
        LOG_MESSAGE_WITH_PROLOG(LDEBUG, "classes: " << class_names);
        while(!ti.end()){
            auto tag = std::map<std::string,std::string>();
            LOG_MESSAGE(LDEBUG, ti.lemma());
            for(uint cat=0;cat < class_names.size();cat++){
                tag.insert({class_names[cat],classes[cat][ti.token_class(cat)]});
            }
            m_tags.push_back(tag);
            m_lemmas.emplace_back(ti.lemma());
            ti.next();
        }

    }


}
