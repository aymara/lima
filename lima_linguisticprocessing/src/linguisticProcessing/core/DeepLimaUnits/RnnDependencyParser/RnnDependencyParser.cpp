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
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"

#include <queue>

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

class RnnDependencyParserPrivate: public ConfigurationHelper
{
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
    std::function<void()> m_load_fn;
    std::shared_ptr< StringIndex > m_stridx;
    PathResolver m_pResolver;
    std::vector< std::map<std::string, std::string> > m_tags;
    std::vector<QString> m_lemmas;
    // std::vector<typename DependencyParser::token_with_analysis_t> m_tokens;
    std::vector<std::string> m_class_names;
    std::vector< std::vector<std::string> > m_classes;
    std::vector<uint32_t> m_heads;
    std::vector<std::string> m_deprels; // predicted deprel per token, parallel to m_heads
    std::vector<bool> m_isRoot;         // true for synthetic <ROOT> tokens (no real vertex)
    std::vector<std::string> m_relClassNames; // deprel id -> string, from the model
    bool m_loaded;
    bool m_enabled; // false when no parser model is available: process() is a no-op
};

RnnDependencyParserPrivate::RnnDependencyParserPrivate(): ConfigurationHelper("RnnDependencyParserPrivate",
                                                                              THIS_FILE_LOGGING_CATEGORY()),
                                                          m_stridx(new StringIndex()),
                                                          m_loaded(false),
                                                          m_enabled(true)
{
}

RnnDependencyParser::RnnDependencyParser(): m_d(new RnnDependencyParserPrivate())
{
}

RnnDependencyParser::~RnnDependencyParser()
{
    delete m_d;
}

void RnnDependencyParser::init(GroupConfigurationStructure &unitConfiguration, Manager *manager)
{
    LOG_MESSAGE_WITH_PROLOG(LDEBUG, "RnnDependencyParser::init");

    m_d->m_language = manager->getInitializationParameters().media;

    m_d->init(unitConfiguration);

}

Lima::LimaStatusCode RnnDependencyParser::process(Lima::AnalysisContent &analysis) const
{
    TimeUtils::updateCurrentTime();
    TimeUtilsController RnnDependencyParserProcessTime("RnnDependencyParser");
    SALOGINIT;
    LOG_MESSAGE(LDEBUG, "RnnDependencyParser::process");
    if (!m_d->m_enabled)
    {
        LOG_MESSAGE(LDEBUG, "RnnDependencyParser disabled (no model); skipping.");
        return SUCCESS_ID;
    }
    // Ensure the model is loaded (no-op if already loaded or not lazy-initialized).
    if (m_d->m_load_fn)
    {
        m_d->m_load_fn();
    }
    auto tiData = std::dynamic_pointer_cast<TokenIteratorData>(analysis.getData("TokenIterator"));
    if (tiData == nullptr)
    {
        SALOGINIT;
        LERROR << "Can't Process RnnDependencyParser : missing data 'TokenIterator'";
        return MISSING_DATA;
    }
    auto stridxPtr = tiData->getStringIndex();
    m_d->m_dependencyParser->setStringIndex(stridxPtr);
    auto tokenIterator = tiData->getTokenIterator();
    tokenIterator->reset();

    auto anagraph = std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("PosGraph"));
    if (anagraph == nullptr)
    {
        LERROR << "no PosGraph ! abort";
        return MISSING_DATA;
    }

    auto syntacticData = std::dynamic_pointer_cast<SyntacticAnalysis::SyntacticData>(analysis.getData("SyntacticData"));
    if (syntacticData == nullptr)
    {
        syntacticData = std::make_shared<SyntacticAnalysis::SyntacticData>(anagraph.get(), nullptr);
        analysis.setData("SyntacticData",syntacticData);
    }
    syntacticData->setupDependencyGraph();

    m_d->analyzer(tokenIterator);

    const auto& languageData = static_cast<const Common::MediaticData::LanguageData&>(
        Common::MediaticData::MediaticData::single().mediaData(m_d->m_language));

    auto sd = std::dynamic_pointer_cast<SegmentationData>(
        analysis.getData(m_d->m_data.toStdString()));
    if (sd == nullptr)
    {
        LERROR << "RnnDependencyParser: missing segmentation data '" << m_d->m_data << "'";
        return MISSING_DATA;
    }
    LinguisticGraph* posGraph = anagraph->getGraph();
    const LinguisticGraphVertex lastVertex = anagraph->lastVertex();

    // The parser emits one sentence at a time: a synthetic <ROOT> followed by the
    // sentence's real tokens, whose heads are sentence-LOCAL (0 == that sentence's
    // root, 1..k == its tokens in order). Build, per sentence/segment, the ordered
    // list of real token vertices by walking the PoS graph the same way the CoNLL
    // dumper does (ordered[0] is the boundary, rendered as HEAD 0 / DEPREL root).
    std::vector<LinguisticGraphVertex> segmentBegin;
    std::vector<std::vector<LinguisticGraphVertex>> segmentTokens;
    for (auto segIt = sd->getSegments().begin(); segIt != sd->getSegments().end();
         ++segIt)
    {
        const LinguisticGraphVertex sentBegin = segIt->getFirstVertex();
        const LinguisticGraphVertex sentEnd = segIt->getLastVertex();
        segmentBegin.push_back(sentBegin);
        segmentTokens.emplace_back();
        std::vector<LinguisticGraphVertex>& tokens = segmentTokens.back();

        std::queue<LinguisticGraphVertex> toVisit;
        std::set<LinguisticGraphVertex> visited;
        toVisit.push(sentBegin);
        while (!toVisit.empty())
        {
            const LinguisticGraphVertex v = toVisit.front();
            toVisit.pop();
            if (visited.count(v) > 0)
            {
                continue;
            }
            visited.insert(v);
            // Real tokens (skip the sentence boundary and any non-token vertex).
            if (v != sentBegin && get(vertex_token, *posGraph, v) != nullptr)
            {
                tokens.push_back(v);
            }
            if (v == sentEnd)
            {
                break;
            }
            LinguisticGraphOutEdgeIt outIt, outItEnd;
            for (boost::tie(outIt, outItEnd) = boost::out_edges(v, *posGraph);
                 outIt != outItEnd; ++outIt)
            {
                const LinguisticGraphVertex tgt = boost::target(*outIt, *posGraph);
                if (visited.count(tgt) == 0 && tgt != lastVertex)
                {
                    toVisit.push(tgt);
                }
            }
        }
    }

    // Walk the parser output. Each <ROOT> starts a new sentence (matched to the
    // next segment in order); the following tokens map to that sentence's vertices.
    int segIdx = -1;
    size_t localPos = 0; // 1-based position of the current token within its sentence
    for (size_t i = 0; i < m_d->m_heads.size(); ++i)
    {
        if (i < m_d->m_isRoot.size() && m_d->m_isRoot[i])
        {
            ++segIdx;
            localPos = 0;
            continue;
        }
        ++localPos;
        if (segIdx < 0 || segIdx >= static_cast<int>(segmentTokens.size()))
        {
            continue;
        }
        const std::vector<LinguisticGraphVertex>& tokens = segmentTokens[segIdx];
        if (localPos > tokens.size())
        {
            continue; // parser/graph token counts disagree for this sentence
        }
        const uint32_t head = m_d->m_heads[i];
        const std::string& deprel = (i < m_d->m_deprels.size())
                                        ? m_d->m_deprels[i]
                                        : std::string("dep");
        const Common::MediaticData::SyntacticRelationId relType =
            languageData.getSyntacticRelationId(deprel);
        const LinguisticGraphVertex src = tokens[localPos - 1];
        // head == 0 -> this sentence's root boundary (HEAD 0 / DEPREL root);
        // otherwise the head-th token of the same sentence.
        const LinguisticGraphVertex dest =
            (head == 0 || head > tokens.size()) ? segmentBegin[segIdx]
                                                : tokens[head - 1];
        syntacticData->addRelationNoChain(relType, src, dest);
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

    std::string udlang;
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
        // No dependency parser model for this language yet: disable the unit
        // rather than aborting the whole pipeline. process() becomes a no-op, so
        // the rest of the pipeline (tagging, lemmatization, dumping) still runs.
        SALOGINIT;
        LWARN << "RnnDependencyParserPrivate::init: no dependency parser model found for "
              << lang_str << " (" << dependency_parser_name
              << "); dependency parsing disabled.";
        m_enabled = false;
        return;
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

        m_dependencyParser = std::make_shared<DependencyParser>(dependency_parser_file_name.toStdString(),
                                                                        m_pResolver,m_stridx,m_class_names, 1024, 8);
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

void RnnDependencyParserPrivate::analyzer(std::shared_ptr<TokenSequenceAnalyzer<>::TokenIterator> ti)
{
    // deprel id -> string vocabulary from the model; empty if the model has no
    // label decoder (then deprel() falls back to "dep").
    m_relClassNames = m_dependencyParser->get_rel_class_names();
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
                                                        end,
                                                        m_relClassNames.empty() ? nullptr : &m_relClassNames);
                                  insertDependencies(dti);
                              });
    (*m_dependencyParser)(*ti);
    m_dependencyParser->finalize();
}

void RnnDependencyParserPrivate::insertDependencies(DependencyParser::TokenIterator& ti)
{
    // The iterator prefixes the stream with a synthetic <ROOT> token; real tokens
    // carry global head indices (0 == root). Mark the <ROOT>(s) so process() skips
    // them when assigning relations to actual graph vertices.
    while (!ti.end())
    {
        m_heads.push_back(ti.head());
        m_deprels.push_back(ti.deprel());
        m_isRoot.push_back(std::string(ti.form()) == "<ROOT>");
        ti.next();
    }
}

}
