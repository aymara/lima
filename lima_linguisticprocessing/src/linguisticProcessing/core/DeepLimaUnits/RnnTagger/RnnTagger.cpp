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

#include "common/misc/Exceptions.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/tools/FileUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/timeUtilsController.h"

#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/common/helpers/ConfigurationHelper.h"
#include "linguisticProcessing/common/helpers/DeepTokenizerBase.h"
#include "linguisticProcessing/common/helpers/LangCodeHelpers.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"

#include "RnnTagger.h"
#include "deeplima/ner.h"
#include "deeplima/token_type.h"
#include "deeplima/dumper_conllu.h"
#include "helpers/path_resolver.h"

using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::PropertyCode;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::AnnotationGraphs;
using namespace std;
using namespace deeplima;

#if defined(DEBUG_LP) && defined(DEBUG_THIS_FILE)
#define LOG_MESSAGE(stream, msg) stream << msg;
  #define LOG_MESSAGE_WITH_PROLOG(stream, msg) TAGGERLOGINIT; LOG_MESSAGE(stream, msg);
#else
#define LOG_MESSAGE(stream, msg) ;
#define LOG_MESSAGE_WITH_PROLOG(stream, msg) ;
#endif


namespace Lima::LinguisticProcessing::DeepLimaUnits::RnnTagger {

    static SimpleFactory<MediaProcessUnit, RnnTagger> rnntaggerFactory(RNNTAGGER_CLASSID); // clazy:exclude=non-pod-global-static

    CONFIGURATIONHELPER_LOGGING_INIT(TOKENIZERLOGINIT);

    class RnnTaggerPrivate: public ConfigurationHelper {
    public:
        RnnTaggerPrivate();
        ~RnnTaggerPrivate() ;
        void init(GroupConfigurationStructure& unitConfiguration);
        void tagger(token_buffer_t& buffer);
        void insertTags(tagging::impl::TokenIterator &ti);
        dumper::AnalysisToConllU<tagging::impl::TokenIterator> m_dumper;

        MediaId m_language;
        FsaStringsPool* m_stringsPool;
        LinguisticGraphVertex m_currentVx;
        QString m_data;
        tagging::impl::EntityTaggingModule m_tag;
        function<void()> m_load_fn;
        StringIndex m_stridx;
        PathResolver m_pResolver;
        std::vector<string> m_tags;
        const Common::PropertyCode::PropertyAccessor* m_microAccessor;
        bool m_loaded;
    };

    RnnTaggerPrivate::RnnTaggerPrivate(): ConfigurationHelper("RnnTaggerPrivate", THIS_FILE_LOGGING_CATEGORY()), m_stringsPool(nullptr), m_currentVx(0), m_stridx(), m_loaded(false)
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
        auto posgraph = new LinguisticAnalysisStructure::AnalysisGraph("PosGraph",
                                                                       m_d->m_language,
                                                                       false,
                                                                       true);
        analysis.setData("PosGraph",posgraph);
        const auto& propertyCodeManager = static_cast<const LanguageData&>(
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
        m_d->m_currentVx = posgraph->firstVertex();
        LinguisticGraph* resultgraph=posgraph->getGraph();
        remove_edge(posgraph->firstVertex(),posgraph->lastVertex(),*resultgraph);

        token_buffer_t buffer;
        std::vector< LinguisticGraphVertex > anaVertices;
        while(m_d->m_currentVx != endVx){
            Token* src = vTokens[m_d->m_currentVx];
            impl::token_t token;
            token.m_offset = src->position();
            token.m_len = src->length();
            token.m_form_idx = m_d->m_stridx.get_idx(src->stringForm().toUtf8(), src->length());
            token.m_flags = impl::token_t::token_flags_t(src->status().getStatus() & StatusType::T_SENTENCE_BRK);
            buffer.push_back(token);
            LinguisticGraphOutEdgeIt it, it_end;
            boost::tie(it, it_end) = boost::out_edges(m_d->m_currentVx, *srcgraph);
            if (it != it_end)
            {
                m_d->m_currentVx = boost::target(*it, *srcgraph);
            }
            else
            {
                m_d->m_currentVx = endVx;
            }
            anaVertices.push_back(m_d->m_currentVx);
        }
        m_d->tagger(buffer);
        std::vector<LinguisticGraphVertex>::size_type anaVerticesIndex = 0;
        LinguisticGraphVertex previousPosVertex = posgraph->firstVertex();

        while (anaVerticesIndex < anaVertices.size()){
            auto anaVertex = anaVertices[anaVerticesIndex];
            auto currentAnaToken = vTokens[anaVertex];
            auto newVx = boost::add_vertex(*resultgraph);
            auto agv =  annotationData->createAnnotationVertex();
            annotationData->addMatching("PosGraph", newVx, "annot", agv);
            annotationData->addMatching("AnalysisGraph", anaVertex, "PosGraph", newVx);
            annotationData->annotate(agv, QString::fromUtf8("PosGraph"), newVx);
            auto morphoData = get(vertex_data,*srcgraph,anaVertex);
            auto srcToken = get(vertex_token,*srcgraph,anaVertex);

            if (morphoData!=nullptr)
            {
                auto posData = new MorphoSyntacticData();
                CheckDifferentPropertyPredicate differentMicro(
                        m_d->m_microAccessor,
                        microManager.getPropertyValue(m_d->m_tags[anaVerticesIndex]));
                std::back_insert_iterator<MorphoSyntacticData> backInsertItr(*posData);
                remove_copy_if(morphoData->begin(),
                               morphoData->end(),
                               backInsertItr,
                               differentMicro);

                if (posData->empty() || morphoData->empty())
                {
                    PTLOGINIT;
                    LWARN << "No matching category found for tagger result "
                          << m_d->m_tags[anaVerticesIndex];
                    if (!morphoData->empty())
                    {
                        LWARN << "Taking any one";
                        posData->push_back(morphoData->front());
                    }
                }
                LOG_MESSAGE(LDEBUG, "      Adding tag '" << m_d->m_tags[anaVerticesIndex] << "'");
                put(vertex_data,*resultgraph,newVx,posData);
                put(vertex_token,*resultgraph,newVx,srcToken);
            }
            boost::add_edge(previousPosVertex, newVx, *resultgraph);

            previousPosVertex = newVx;
            anaVerticesIndex++;
        }
        boost::add_edge(previousPosVertex, posgraph->lastVertex(), *resultgraph);
        LOG_MESSAGE_WITH_PROLOG(LDEBUG, "RnnPosTagger postagging done.");
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
            LIMA_EXCEPTION_SELECT_LOGINIT(TOKENIZERLOGINIT,
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
            m_tag.load(model_file_name.toStdString(),m_pResolver);
            m_tag.init(1, 1, 16*1024,m_stridx); // threads, buffer size per thread

            m_loaded = true;
        };

        if (!isInitLazy())
        {
            m_load_fn();
        }
        for (size_t i = 0; i < m_tag.get_classes().size(); ++i)
        {
            m_dumper.set_classes(i, m_tag.get_class_names()[i], m_tag.get_classes()[i]);
        }
    }

    void RnnTaggerPrivate::tagger(token_buffer_t &buffer) {

        m_tag.register_handler([this,&buffer](
                const typename tagging::impl::EntityTaggingModule::OutputMatrix& classes,
                size_t begin, size_t end, size_t slot_idx){
            std::cerr << "handler called: " << slot_idx << std::endl;
            tagging::impl::TokenIterator ti(m_stridx, buffer, vector<unsigned int>(), classes, begin, end);
            insertTags(ti);
            buffer.unlock();
        });
        m_tag.handle_token_buffer(0, tagging::impl::enriched_token_buffer_t(buffer, m_stridx));
        while (buffer.locked())
        {
            m_tag.send_next_results();
        }
    }

    void RnnTaggerPrivate::insertTags(tagging::impl::TokenIterator &ti) {
        auto classes = m_dumper.getMClasses();
        while(!ti.end()){
            m_tags.push_back(classes[0][ti.token_class(0)]);
            ti.next();
        }
    }

}