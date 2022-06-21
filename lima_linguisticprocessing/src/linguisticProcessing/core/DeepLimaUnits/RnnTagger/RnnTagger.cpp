/*
    Copyright 2002-2021 CEA LIST

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

#include "RnnTagger.h"
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


namespace Lima::LinguisticProcessing::DeepLimaUnits::RnnTagger {
    #if defined(DEBUG_LP) && defined(DEBUG_THIS_FILE)
    #define LOG_MESSAGE(stream, msg) stream << msg;
    #define LOG_MESSAGE_WITH_PROLOG(stream, msg) PTLOGINIT; LOG_MESSAGE(stream, msg);
    #else
        #define LOG_MESSAGE(stream, msg) ;
    #define LOG_MESSAGE_WITH_PROLOG(stream, msg) ;
    #endif

    static SimpleFactory<MediaProcessUnit, RnnTagger> rnntaggerFactory(RNNTAGGER_CLASSID); // clazy:exclude=non-pod-global-static

    CONFIGURATIONHELPER_LOGGING_INIT(PTLOGINIT);

    class RnnTaggerPrivate: public ConfigurationHelper {
    public:
        RnnTaggerPrivate();
        ~RnnTaggerPrivate() ;
        void init(GroupConfigurationStructure& unitConfiguration);
        void tagger(vector<segmentation::token_pos>& buffer);
        void insertTags(TokenSequenceAnalyzer<>::TokenIterator &ti);
        dumper::AnalysisToConllU<TokenSequenceAnalyzer<>::TokenIterator> m_dumper;

        MediaId m_language;
        FsaStringsPool* m_stringsPool;
        QString m_data;
        TokenSequenceAnalyzer<>* m_tag;
        function<void()> m_load_fn;
        StringIndex m_stridx;
        PathResolver m_pResolver;
        std::vector<string> m_tags;
        const Common::PropertyCode::PropertyAccessor* m_microAccessor;
        bool m_loaded;
    };

    RnnTaggerPrivate::RnnTaggerPrivate(): ConfigurationHelper("RnnTaggerPrivate", THIS_FILE_LOGGING_CATEGORY()), m_stringsPool(nullptr), m_stridx(), m_loaded(false), m_tag()
    {

    }

    RnnTaggerPrivate::~RnnTaggerPrivate() = default;


    RnnTagger::RnnTagger():m_d(new RnnTaggerPrivate()) {

    }

    RnnTagger::~RnnTagger() = default;

    void RnnTagger::init(GroupConfigurationStructure &unitConfiguration, Manager *manager)
    {
        LOG_MESSAGE_WITH_PROLOG(LDEBUG, "RnnTagger::init");

        m_d->m_language = manager->getInitializationParameters().media;
        m_d->m_stringsPool = &MediaticData::changeable().stringsPool(m_d->m_language);

        m_d->init(unitConfiguration);

    }

    Lima::LimaStatusCode
    RnnTagger::process(Lima::AnalysisContent &analysis) const {
        LOG_MESSAGE_WITH_PROLOG(LDEBUG, "start RnnPosTager");
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
        m_d->tagger(buffer);
        LOG_MESSAGE(LDEBUG, "tag size: " << m_d->m_tags.size());
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
            auto morphoData = get(vertex_data,*srcgraph,anaVertex);
            auto srcToken = get(vertex_token,*srcgraph,anaVertex);

            if (morphoData!=nullptr)
            {
                LOG_MESSAGE(LDEBUG, "tag: "<< m_d->m_tags[anaVerticesIndex]);
                auto posData = new MorphoSyntacticData();
                LOG_MESSAGE(LDEBUG, "coded value: " << microManager.getPropertyValue(m_d->m_tags[anaVerticesIndex]));
                CheckDifferentPropertyPredicate differentMicro(
                        m_d->m_microAccessor,
                        microManager.getPropertyValue(m_d->m_tags[anaVerticesIndex]));
                LinguisticElement le = LinguisticElement();
                le.properties = microManager.getPropertyValue(m_d->m_tags[anaVerticesIndex]);
                posData->push_back(le);

                LOG_MESSAGE(LDEBUG, "      Adding tag '" << m_d->m_tags[anaVerticesIndex] << "'");
                put(vertex_data,*resultgraph,newVx,posData);
                put(vertex_token,*resultgraph,newVx,srcToken);
            }
            boost::add_edge(previousPosVertex, newVx, *resultgraph);

            previousPosVertex = newVx;
            anaVerticesIndex++;
        }
        boost::add_edge(previousPosVertex, posgraph->lastVertex(), *resultgraph);
        LOG_MESSAGE(LDEBUG, "RnnPosTagger postagging done.");
        return SUCCESS_ID;
    }

    void RnnTaggerPrivate::init(GroupConfigurationStructure& unitConfiguration)
    {
        m_data = QString(getStringParameter(unitConfiguration, "data", 0, "SentenceBoundaries").c_str());
        QString model_prefix = getStringParameter(unitConfiguration, "model_prefix", ConfigurationHelper::REQUIRED | ConfigurationHelper::NOT_EMPTY).c_str();

        LOG_MESSAGE_WITH_PROLOG(LDEBUG, "RnnTaggerPrivate::init" << model_prefix);

        QString lang_str = MediaticData::single().media(m_language).c_str();
        QString resources_path = MediaticData::single().getResourcesPath().c_str();
        QString model_name = model_prefix;
        string udlang;
        MediaticData::single().getOptionValue("udlang", udlang);

        if (!fix_lang_codes(lang_str, udlang))
        {
            LIMA_EXCEPTION_SELECT_LOGINIT(PTLOGINIT,
                                          "RnnTaggerPrivate::init: Can't parse language id " << udlang.c_str(),
                                          Lima::InvalidConfiguration);
        }

        model_name.replace(QString("$udlang"), QString(udlang.c_str()));

        auto model_file_name = findFileInPaths(resources_path,
                                               QString::fromUtf8("/RnnTagger/%1/%2.pt")
                                                       .arg(lang_str, model_name));
        if (model_file_name.isEmpty())
        {
            throw InvalidConfiguration("RnnTaggerPrivate::init: tagger model file not found.");
        }

        m_load_fn = [this, model_file_name]()
        {
            if (m_loaded)
            {
                return;
            }
            m_tag = new TokenSequenceAnalyzer<>(model_file_name.toStdString(),"",m_pResolver,1024,8);


            m_loaded = true;
        };

        if (!isInitLazy())
        {
            m_load_fn();
        }
        for (size_t i = 0; i < m_tag->get_classes().size(); ++i)
        {
            m_dumper.set_classes(i, m_tag->get_class_names()[i], m_tag->get_classes()[i]);
        }
    }

    void RnnTaggerPrivate::tagger(vector<segmentation::token_pos> &buffer) {
        m_tag->register_handler([this](const StringIndex& stridx,
                                               const token_buffer_t& tokens,
                                               const std::vector<StringIndex::idx_t>& lemmata,
                                               const TokenSequenceAnalyzer<>::OutputMatrix& classes,
                                               size_t begin,
                                               size_t end){
            TokenSequenceAnalyzer<>::TokenIterator ti(stridx, tokens, lemmata, classes, begin, end);
            insertTags(ti);
        });
        LOG_MESSAGE_WITH_PROLOG(LDEBUG,buffer[0].m_pch);
        (*m_tag)(buffer, buffer.size());
        m_tag->finalize();
    }

    void RnnTaggerPrivate::insertTags(TokenSequenceAnalyzer<>::TokenIterator &ti) {
        auto classes = m_dumper.getMClasses();
        while(!ti.end()){
            LOG_MESSAGE_WITH_PROLOG(LDEBUG, "index: " << ti.token_class(0));
            m_tags.push_back(classes[0][ti.token_class(0)]);
            ti.next();
        }
    }

}