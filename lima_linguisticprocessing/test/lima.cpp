// Copyright 2019-2022 CEA LIST
// SPDX-FileCopyrightText: 2019-2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt for Python examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "lima.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include "common/LimaCommon.h"
#include "common/LimaVersion.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include <common/ProcessUnitFramework/AnalysisContent.h>
#include "common/QsLog/QsLog.h"
#include "common/QsLog/QsLogDest.h"
#include "common/QsLog/QsLogCategories.h"
#include "common/QsLog/QsDebugOutput.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/time/traceUtils.h"
#include "common/tools/FileUtils.h"
#include "common/tools/LimaMainTaskRunner.h"

#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/common/linguisticData/languageData.h"
#include <linguisticProcessing/common/linguisticData/LimaStringText.h>
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowTextWriter.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowTextHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/LTRTextHandler.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include <linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h>
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/SyntacticAnalysis/DependencyGraph.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include <deque>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/find_format.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/format.hpp>

#include <QtCore/QCoreApplication>
#include <QtCore/QString>

#include <QtCore>

#define DEBUG_LP

using namespace Lima::LinguisticProcessing;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::Common::MediaticData;
using LangData = Lima::Common::MediaticData::LanguageData;
using MedData = Lima::Common::MediaticData::MediaticData ;
using namespace Lima::Common::Misc;
using namespace Lima::Common::PropertyCode;
using namespace Lima;

struct character_escaper
{
    template<typename FindResultT>
    std::string operator()(const FindResultT& Match) const
    {
        std::string s;
        for (typename FindResultT::const_iterator i = Match.begin();
             i != Match.end();
             i++) {
            s += boost::str(boost::format("\\x%02x") % static_cast<int>(*i));
        }
        return s;
    }
};

std::shared_ptr< std::ostringstream > openHandlerOutputString(
    AbstractTextualAnalysisHandler* handler,
    const std::set<std::string>&dumpers,
    const std::string& dumperId);

int run(int aargc,char** aargv);

class LimaAnalyzerPrivate
{
  friend class LimaAnalyzer;
public:
  LimaAnalyzerPrivate(const QStringList& qlangs,
                      const QStringList& qpipelines,
                      const QString& modulePath,
                      const QString& user_config_path,
                      const QString& user_resources_path,
                      const QString& meta);
  ~LimaAnalyzerPrivate() = default;
  LimaAnalyzerPrivate(const LimaAnalyzerPrivate& a) = delete;
  LimaAnalyzerPrivate& operator=(const LimaAnalyzerPrivate& a) = delete;

  void initMetaData();

  const std::string analyzeText(const std::string& text,
                                const std::string& lang,
                                const std::string& pipeline,
                                const std::string& meta);

  void collectDependencyInformations(std::shared_ptr<Lima::AnalysisContent> analysis);
  void collectVertexDependencyInformations(LinguisticGraphVertex v,
                                           std::shared_ptr<Lima::AnalysisContent> analysis);

  /** Gets the named entity type for the PosGraph vertex @ref posGraphVertex
   * if it is a specific entity. Return "_" otherwise
   */
  QString getNeType(LinguisticGraphVertex posGraphVertex);

  std::pair<QString, int> getConllRelName(LinguisticGraphVertex v);

  const SpecificEntityAnnotation* getSpecificEntityAnnotation(LinguisticGraphVertex v) const;

  bool hasSpaceAfter(LinguisticGraphVertex v, LinguisticGraph* graph);

  QString getMicro(LinguisticAnalysisStructure::MorphoSyntacticData& morphoData);

  QString getFeats(const LinguisticAnalysisStructure::MorphoSyntacticData& morphoData);

  /** Reset all members used to store analysis states. To be called before handling a new analysis. */
  void reset();


  QString previousNeType;

  const FsaStringsPool* sp = nullptr;
  MediaId medId;


  const LanguageData* languageData = nullptr;
  const Common::PropertyCode::PropertyAccessor* propertyAccessor = nullptr;
  LinguisticGraph* posGraph = nullptr;
  LinguisticGraph* anaGraph = nullptr;
  AnnotationData* annotationData = nullptr;
  std::shared_ptr<SyntacticData> syntacticData = nullptr;
  std::map< LinguisticGraphVertex,
          std::pair<LinguisticGraphVertex,
                    std::string> > vertexDependencyInformations;
  QMap<QString, QString> conllLimaDepMapping;


  std::map<LinguisticGraphVertex, int> vertexToToken;

  const PropertyCodeManager* propertyCodeManager = nullptr;
  // Fixed members that do not change at each analysis
  std::map<std::string, AbstractAnalysisHandler*> handlers;
  std::unique_ptr<BowTextWriter> bowTextWriter = nullptr;
  std::unique_ptr<BowTextHandler> bowTextHandler = nullptr;
  std::unique_ptr<SimpleStreamHandler> simpleStreamHandler = nullptr;
  std::unique_ptr<SimpleStreamHandler> fullXmlSimpleStreamHandler = nullptr;
  std::unique_ptr<LTRTextHandler> ltrTextHandler = nullptr;
  std::set<std::string> dumpers = {"text"};
  std::shared_ptr< AbstractLinguisticProcessingClient > m_client;
  std::map<std::string,std::string> metaData;
  // Store constructor parameters to be able to implement copy constructor
  QStringList qlangs;
  QStringList qpipelines;
  QString modulePath;
  QString user_config_path;
  QString user_resources_path;
  QString meta;
};


LimaAnalyzerPrivate::LimaAnalyzerPrivate(const QStringList& iqlangs,
                                         const QStringList& iqpipelines,
                                         const QString& imodulePath,
                                         const QString& iuser_config_path,
                                         const QString& iuser_resources_path,
                                         const QString& imeta) :
    qlangs(iqlangs), qpipelines(iqpipelines), modulePath(imodulePath),
    user_config_path(iuser_config_path), user_resources_path(iuser_resources_path), meta(imeta)
{
  std::cerr << "LimaAnalyzerPrivate::LimaAnalyzerPrivate" << std::endl;
  int argc = 1;
  char* argv[2] = {(char*)("LimaAnalyzer"), NULL};
  QCoreApplication app(argc, argv);

  if(iqlangs.size() == 0)
  {
    throw LimaException("Must initialize at least one language");
  }
  if(iqpipelines.size() == 0)
  {
    throw LimaException("Must initialize at least one pipeline");
  }

  QStringList additionalPaths({modulePath+"/config"});
  // Add here LIMA_CONF content in front, otherwise it will be ignored
  auto limaConf = QString::fromUtf8(qgetenv("LIMA_CONF").constData());
  if (!limaConf.isEmpty())
    additionalPaths = limaConf.split(LIMA_PATH_SEPARATOR) + additionalPaths;
  // Add then the user path in front again such that it takes precedence on environment variable
  if (!user_config_path.isEmpty())
    additionalPaths.push_front(user_config_path);
  auto configDirs = buildConfigurationDirectoriesList(QStringList({"lima"}),
                                                      additionalPaths);
  auto configPath = configDirs.join(LIMA_PATH_SEPARATOR);

  QStringList additionalResourcePaths({modulePath+"/resources"});
  // Add here LIMA_RESOURCES content in front, otherwise it will be ignored
  auto limaRes = QString::fromUtf8(qgetenv("LIMA_RESOURCES").constData());
  if (!limaRes.isEmpty())
    additionalResourcePaths = limaRes.split(LIMA_PATH_SEPARATOR) + additionalResourcePaths;
  if (!user_resources_path.isEmpty())
    additionalResourcePaths.push_front(user_resources_path);
  auto resourcesDirs = buildResourcesDirectoriesList(QStringList({"lima"}),
                                                     additionalResourcePaths);
  auto resourcesPath = resourcesDirs.join(LIMA_PATH_SEPARATOR);

  QsLogging::initQsLog(configPath);
  std::cerr << "QsLog initialized " << configPath.toStdString() << std::endl;
  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();
  std::cerr << "LimaAnalyzerPrivate::LimaAnalyzerPrivate() plugins manager created" << std::endl;
  if (!Lima::AmosePluginsManager::changeable().loadPlugins(configPath))
  {
    throw InvalidConfiguration("loadLibrary method failed.");
  }
  qDebug() << "Amose plugins are now initialized";


  std::string lpConfigFile = "lima-analysis.xml";
  std::string commonConfigFile = "lima-common.xml";
  std::string clientId = "lima-coreclient";

  std::string strConfigPath;

  initMetaData();

  std::deque<std::string> pipelines;
  for (const auto& pipeline: qpipelines)
    pipelines.push_back(pipeline.toStdString());


  std::deque<std::string> langs;
  for (const auto& lang: qlangs)
    langs.push_back(lang.toStdString());
  std::cerr << "LimaAnalyzerPrivate::LimaAnalyzerPrivate() "
            << resourcesPath.toUtf8().constData() << ", "
            << configPath.toUtf8().constData() << ", "
            << commonConfigFile << ", "
            << qlangs.join(",").toStdString() << std::endl;
  // initialize common
  Common::MediaticData::MediaticData::changeable().init(
    resourcesPath.toUtf8().constData(),
    configPath.toUtf8().constData(),
    commonConfigFile,
    langs);
  std::cerr << "MediaticData initialized" << std::endl;

  bool clientFactoryConfigured = false;
  Q_FOREACH(QString configDir, configDirs)
  {
    if (QFileInfo::exists(configDir + "/" + lpConfigFile.c_str()))
    {
      std::cerr << "LimaAnalyzerPrivate::LimaAnalyzerPrivate() configuring "
                << (configDir + "/" + lpConfigFile.c_str()).toUtf8().constData() << ", "
                << clientId << std::endl;

      // initialize linguistic processing
      Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig(
        (configDir + "/" + lpConfigFile.c_str()));
      LinguisticProcessingClientFactory::changeable().configureClientFactory(
          clientId,
          lpconfig,
          langs,
          pipelines);
      clientFactoryConfigured = true;
      break;
    }
  }
  if(!clientFactoryConfigured)
  {
    std::cerr << "No LinguisticProcessingClientFactory were configured with"
              << configDirs.join(LIMA_PATH_SEPARATOR).toStdString()
              << "and" << lpConfigFile << std::endl;
    throw LimaException("Configuration failure");
  }
  std::cerr << "Client factory configured" << std::endl;

  m_client = std::shared_ptr< AbstractLinguisticProcessingClient >(
      std::dynamic_pointer_cast<AbstractLinguisticProcessingClient>(
          LinguisticProcessingClientFactory::single().createClient(clientId)));

  // Set the handlers
  bowTextWriter = std::make_unique<BowTextWriter>();
  handlers.insert(std::make_pair("bowTextWriter", bowTextWriter.get()));
  bowTextHandler = std::make_unique<BowTextHandler>();
  handlers.insert(std::make_pair("bowTextHandler", bowTextHandler.get()));
  simpleStreamHandler = std::make_unique<SimpleStreamHandler>();
  handlers.insert(std::make_pair("simpleStreamHandler", simpleStreamHandler.get()));
  fullXmlSimpleStreamHandler = std::make_unique<SimpleStreamHandler>();
  handlers.insert(std::make_pair("fullXmlSimpleStreamHandler", fullXmlSimpleStreamHandler.get()));
  ltrTextHandler= std::make_unique<LTRTextHandler>();
  handlers.insert(std::make_pair("ltrTextHandler", ltrTextHandler.get()));
  std::cerr << "LimaAnalyzerPrivate constructor done" << std::endl;

}

LimaAnalyzer::LimaAnalyzer(const std::string& langs,
                           const std::string& pipelines,
                           const std::string& modulePath,
                           const std::string& user_config_path,
                           const std::string& user_resources_path,
                           const std::string& meta)
{
  try
  {
    QStringList qlangs = QString::fromStdString(langs).split(",");
    QStringList qpipelines = QString::fromStdString(pipelines).split(",");
    m_d = new LimaAnalyzerPrivate(qlangs, qpipelines,
                                  QString::fromStdString(modulePath),
                                  QString::fromStdString(user_config_path),
                                  QString::fromStdString(user_resources_path),
                                  QString::fromStdString(meta));
  }
  catch (const Lima::LimaException& e)
  {
    std::cerr << "Lima internal error: " << e.what() << std::endl;
    m_d = nullptr;
    error = true;
    errorMessage = e.what();
  }
}

LimaAnalyzer::~LimaAnalyzer()
{
  delete m_d;
}

LimaAnalyzer::LimaAnalyzer(const LimaAnalyzer& a)
{
  try
  {
    m_d = new LimaAnalyzerPrivate(a.m_d->qlangs, a.m_d->qpipelines,
                                a.m_d->modulePath,
                                a.m_d->user_config_path,
                                a.m_d->user_resources_path,
                                a.m_d->meta);
  }
  catch (const Lima::LimaException& e)
  {
    std::cerr << "Lima internal error: " << e.what() << std::endl;
    m_d = nullptr;
    error = true;
    errorMessage = e.what();
  }
  // std::cerr << "LimaAnalyzer::LimaAnalyzer copy constructor" << std::endl;
}

LimaAnalyzer& LimaAnalyzer::operator=(const LimaAnalyzer& a)
{
  try
  {
    // std::cerr << "LimaAnalyzer::operator=" << std::endl;
    delete m_d;
    m_d = new LimaAnalyzerPrivate(a.m_d->qlangs, a.m_d->qpipelines,
                                  a.m_d->modulePath,
                                  a.m_d->user_config_path,
                                  a.m_d->user_resources_path,
                                  a.m_d->meta);
    return *this;
  }
  catch (const Lima::LimaException& e)
  {
    std::cerr << "Lima internal error: " << e.what() << std::endl;
    m_d = nullptr;
    error = true;
    errorMessage = e.what();
  }
  return *this;
}

void LimaAnalyzerPrivate::reset()
{
  previousNeType = "O";
  sp = nullptr;
  medId = 0;
  languageData = nullptr;
  syntacticData = nullptr;
  propertyAccessor = nullptr;
  posGraph = nullptr;
  anaGraph = nullptr;
  annotationData = nullptr;
  propertyCodeManager = nullptr;
  vertexDependencyInformations.clear();
  conllLimaDepMapping.clear();
  vertexToToken.clear();
}

void LimaAnalyzerPrivate::initMetaData ()
{
  // parse 'meta' argument to add metadata
  if (!meta.isEmpty())
  {
    auto metas = meta.split(",");
    for (const auto& aMeta: metas)
    {
      auto kv = aMeta.split(":");
      if (kv.size() != 2)
      {
        std::cerr << "meta argument '"<< aMeta.toStdString() << "' is not of the form XXX:YYY: ignored" << std::endl;
      }
      else
      {
        metaData[kv[0].toStdString()] = kv[1].toStdString();
      }
    }
  }
}

std::string LimaAnalyzer::analyzeText(const std::string& text,
                                    const std::string& lang,
                                    const std::string& pipeline,
                                    const std::string& meta)
{
  // std::cerr << "LimaAnalyzer::analyzeText" << std::endl;
  if (m_d == nullptr || error)
  {
    error = true;
    errorMessage = "Invalid Lima analyzer. Previous error message was: " + errorMessage;
    return "";
  }
  try
  {
    return m_d->analyzeText(text, lang, pipeline, meta);
  }
  catch (const Lima::LimaException& e)
  {
    std::cerr << "Lima internal error: " << e.what() << std::endl;
    error = true;
    errorMessage = e.what();
    return "";
  }
}

const std::string LimaAnalyzerPrivate::analyzeText(const std::string& text,
                                    const std::string& lang,
                                    const std::string& pipeline,
                                    const std::string& meta)
{
  auto txtofs  = openHandlerOutputString(simpleStreamHandler.get(), dumpers, "text");

  auto localMetaData = metaData;
  localMetaData["FileName"]="param";
  auto qmeta = QString::fromStdString(meta).split(",");
  for (const auto& m: qmeta)
  {
    auto kv = m.split(":");
    if (kv.size() == 2)
      localMetaData[kv[0].toStdString()] = kv[1].toStdString();
  }

  auto localLang = lang;
  if (localLang.size() == 0)
  {
    localLang = qlangs[0].toStdString();
  }
  auto localPipeline = pipeline;
  if (localPipeline.size() == 0)
  {
    localPipeline = qpipelines[0].toStdString();
  }
  localMetaData["Lang"] = localLang;

  QString contentText = QString::fromUtf8(text.c_str());
  if (contentText.isEmpty())
  {
    std::cerr << "Empty input ! " << std::endl;
  }
  else
  {
    // analyze it
      // std::cerr << "Analyzing " << contentText.toStdString() << std::endl;
    m_client->analyze(contentText, localMetaData, localPipeline, handlers);
  }
  auto result = txtofs->str();
  // std::cerr << "LimaAnalyzerPrivate::analyzeText result: " << result << std::endl;
  simpleStreamHandler->setOut(nullptr);
  return result;
}

std::shared_ptr< std::ostringstream > openHandlerOutputString(
    AbstractTextualAnalysisHandler* handler,
    const std::set<std::string>&dumpers,
    const std::string& dumperId)
{
  auto ofs = std::make_shared< std::ostringstream >();
  if (dumpers.find(dumperId)!=dumpers.end())
  {
    handler->setOut(ofs.get());
  }
  else
  {
    DUMPERLOGINIT;
    LERROR << dumperId << "is not in the dumpers list";
  }
  return ofs;
}

QString LimaAnalyzerPrivate::getFeats(const LinguisticAnalysisStructure::MorphoSyntacticData& morphoData)
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LDEBUG << "getFeats";
#endif
  auto managers = propertyCodeManager->getPropertyManagers();

  QStringList featuresList;
  for (auto i = managers.cbegin(); i != managers.cend(); i++)
  {
    auto key = QString::fromUtf8(i->first.c_str());
    if (key != "MACRO" && key != "MICRO")
    {
      const auto& pa = propertyCodeManager->getPropertyAccessor(key.toStdString());
      LinguisticCode lc = morphoData.firstValue(pa);
      auto value = QString::fromUtf8(i->second.getPropertySymbolicValue(lc).c_str());
      if (value != "NONE")
      {
        featuresList << QString("%1=%2").arg(key).arg(value);
      }
    }
  }

  featuresList.sort();
  QString features;
  QTextStream featuresStream(&features);
  if (featuresList.isEmpty())
  {
    features = "_";
  }
  else
  {
    for (auto featuresListIt = featuresList.cbegin(); featuresListIt != featuresList.cend(); featuresListIt++)
    {
      if (featuresListIt != featuresList.cbegin())
      {
        featuresStream << "|";
      }
      featuresStream << *featuresListIt;
    }
  }
#ifdef DEBUG_LP
  LDEBUG << "LimaAnalyzerPrivate::getFeats features:" << features;
#endif

  return features;
}

void LimaAnalyzerPrivate::collectDependencyInformations(std::shared_ptr<Lima::AnalysisContent> analysis)
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LDEBUG << "LimaAnalyzerPrivate::collectVertexDependencyInformations";
#endif

  auto posGraphData = std::dynamic_pointer_cast<LinguisticAnalysisStructure::AnalysisGraph>(analysis->getData("PosGraph"));
  if (posGraphData == nullptr)
  {
    std::cerr << "Error: PosGraph has not been produced: check pipeline";
    return;
  }

  auto firstVertex = posGraphData->firstVertex();
  auto lastVertex = posGraphData->lastVertex();
  auto v = firstVertex;
  auto [it, it_end] = boost::out_edges(v, *posGraph);
  if (it != it_end)
  {
      v = boost::target(*it, *posGraph);
  }
  else
  {
      v = lastVertex;
  }

  syntacticData = std::dynamic_pointer_cast<SyntacticData>(analysis->getData("SyntacticData"));
  if (syntacticData == nullptr)
  {
    syntacticData = std::make_shared<SyntacticData>(posGraphData.get(), nullptr);
    syntacticData->setupDependencyGraph();
    analysis->setData("SyntacticData", syntacticData);
  }
  int tokenId = 0;

  while (v != lastVertex)
  {
    QString neType = getNeType(v);
    QString neIOB = "O";
    // Collect NE vertices and output them instead of a single line for
    // current v. NE vertices can not only be PosGraph
    // vertices (and thus can just call dumpPosGraphVertex
    // recursively) but also AnalysisGraph vertices. In the latter case, data
    // come partly from the AnalysisGraph and partly from the PosGraph
    // Furthermore, named entities can be recursive...
    if (neType != "_")
    {
      auto matches = annotationData->matches("PosGraph", v, "annot");
      if (!matches.empty())
      {
        for (const auto& vx: matches)
        {
          if (annotationData->hasAnnotation(vx, QString::fromUtf8("SpecificEntity")))
          {
            auto se = annotationData->annotation(vx, QString::fromUtf8("SpecificEntity"))
              .pointerValue<SpecificEntityAnnotation>();
            previousNeType = "O";
            bool first = true;
            for (const auto& vse : se->vertices())
            {
              collectVertexDependencyInformations(vse, analysis);
              vertexToToken[vse] = tokenId;
              first = false;
            }
            break;
          }
        }
      }
      else
      {
        auto anaVertices = annotationData->matches("PosGraph", v, "AnalysisGraph");
        auto anaVertex = *anaVertices.begin();
        if (annotationData->hasAnnotation(anaVertex, QString::fromUtf8("SpecificEntity")))
        {
          auto se = annotationData->annotation(anaVertex, QString::fromUtf8("SpecificEntity"))
            .pointerValue<SpecificEntityAnnotation>();
          // All retrieved lines/tokens have the same netype. Depending on the
          // output style (CoNLL 2003, CoNLL-U, …), the generated line is different
          // and the ne-Type includes or not BIO information using in this case the
          // previousNeType member.
          previousNeType = "O";
          bool first = true;
          vertexToToken[v] = tokenId;
          tokenId++;
          for (const auto& vse : se->vertices())
          {
            auto posVertices = annotationData->matches("AnalysisGraph", vse, "PosGraph");
            auto posVertex = *posVertices.begin();
            // @TODO Should follow instructions here to output all MWE:
            // https://universaldependencies.org/format.html#words-tokens-and-empty-nodes

            // TODO Get correct UD dep relation for relations inside the named entity
            // and for the token that must be linked to the outside. For this one, the
            // relation is the one which links to posGraphVertex to the rest of the pos
            // graph.
            auto [conllRelName, targetConllId] = getConllRelName(v);

            vertexToToken[posVertex] = tokenId;
            // std::cerr << "docFrom_analysis pushing token" << std::endl;
            first = false;
          }
          previousNeType = neType;
        }
      }
    }
    else
    {
      vertexToToken[v] = tokenId;
      tokenId++;
    }

    collectVertexDependencyInformations(v, analysis);

    auto [it, it_end] = boost::out_edges(v, *posGraph);
    if (it != it_end)
    {
        v = boost::target(*it, *posGraph);
    }
    else
    {
        v = lastVertex;
    }

  }
}

void LimaAnalyzerPrivate::collectVertexDependencyInformations(LinguisticGraphVertex v,
                                                              std::shared_ptr<Lima::AnalysisContent> analysis)
{
    auto dcurrent = syntacticData->depVertexForTokenVertex(v);
    auto depGraph = syntacticData->dependencyGraph();
    for (auto [dit, dit_end] = boost::out_edges(dcurrent, *depGraph); dit != dit_end; dit++)
    {
        auto typeMap = get(edge_deprel_type, *depGraph);
        auto type = typeMap[*dit];
        auto syntRelName = languageData->getSyntacticRelationName(type);
        auto dest = syntacticData->tokenVertexForDepVertex(
          boost::target(*dit, *depGraph));
        if (syntRelName != "")
        {
          vertexDependencyInformations.insert(std::make_pair(v, std::make_pair(dest, syntRelName)));
        }
    }
}

QString LimaAnalyzerPrivate::getNeType(LinguisticGraphVertex posGraphVertex)
{
  // std::cerr << "LimaAnalyzerPrivate::getNeType "<< posGraphVertex << std::endl;
  auto neType = QString::fromUtf8("_") ;
  if (annotationData != nullptr)
  {
    // Check if the PosGraph vertex holds a specific entity
    // std::cerr << "LimaAnalyzerPrivate::getNeType call matches with " << "PosGraph"
    //     << " and " << posGraphVertex << std::endl;
    auto matches = annotationData->matches("PosGraph", posGraphVertex, "annot");
    // std::cerr << "LimaAnalyzerPrivate::getNeType got" << matches.size() << " matches" << std::endl;
    for (const auto& vx: matches)
    {
      // std::cerr << "LimaAnalyzerPrivate::getNeType on match " << vx << std::endl;
      if (annotationData->hasAnnotation(vx, QString::fromUtf8("SpecificEntity")))
      {
        auto se = annotationData->annotation(vx, QString::fromUtf8("SpecificEntity")).
          pointerValue<SpecificEntityAnnotation>();
        neType = MedData::single().getEntityName(se->getType());
        break;
      }
    }
    if (neType == "_")
    {
      // The PosGraph vertex did not hold a specific entity,
      // check if the AnalysisGraph vertex does
      // std::cerr << "LimaAnalyzerPrivate::getNeType checking AnalysisGraph" << std::endl;
      auto anaVertices = annotationData->matches("PosGraph", posGraphVertex, "AnalysisGraph");
      // note: anaVertices size should be 0 or 1
      for (const auto& anaVertex: anaVertices)
      {
        // std::cerr << "LimaAnalyzerPrivate::getNeType on AnalysisGraph vertex " << anaVertex << std::endl;
        auto matches = annotationData->matches("AnalysisGraph", anaVertex, "annot");
        for (const auto& vx: matches)
        {
          // std::cerr << "LimaAnalyzerPrivate::getNeType on ana match " << vx << std::endl;
          if (annotationData->hasAnnotation(vx, QString::fromUtf8("SpecificEntity")))
          {
            auto se = annotationData->annotation(vx, QString::fromUtf8("SpecificEntity"))
                .pointerValue<SpecificEntityAnnotation>();
            neType = MedData::single().getEntityName(se->getType());
            break;
          }
        }
        if (neType != "_") break;
      }
    }
  }
  // std::cerr << "LimaAnalyzerPrivate::getNeType for " << posGraphVertex << ". result = " << neType << std::endl;
  return neType;
}

std::pair<QString, int> LimaAnalyzerPrivate::getConllRelName(LinguisticGraphVertex v)
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LDEBUG << "LimaAnalyzerPrivate::getConllRelName" << v << vertexDependencyInformations.count(v);
#endif
  QString conllRelName = "_";
  int targetConllId = 0;
  if (vertexDependencyInformations.count(v) != 0)
  {
    auto target = vertexDependencyInformations.find(v)->second.first;
#ifdef DEBUG_LP
    LDEBUG << "LimaAnalyzerPrivate::getConllRelName target saved for" << v << "is" << target;
#endif
    if (vertexToToken.find(target) != vertexToToken.end())
    {
      targetConllId =  vertexToToken.find(target)->second;
    }
    else
    {
      DUMPERLOGINIT;
      LERROR << "LimaAnalyzerPrivate::getConllRelName target" << target << "not found in segmentation mapping";
    }
#ifdef DEBUG_LP
    LDEBUG << "LimaAnalyzerPrivate::getConllRelName conll target saved for " << v << " is " << targetConllId;
#endif
    auto relName = QString::fromStdString(vertexDependencyInformations.find(v)->second.second);
#ifdef DEBUG_LP
    LDEBUG << "LimaAnalyzerPrivate::getConllRelName the lima dependency tag for " << v << " is " << relName;
#endif
    if (conllLimaDepMapping.contains(relName))
    {
      conllRelName = conllLimaDepMapping[relName];
    }
    else
    {
      conllRelName = relName;
//             LERROR << "LimaAnalyzerPrivate::getConllRelName" << relName << "not found in mapping";
    }

    // There is no way for vertex to have 0 as head.
    if (conllRelName == "root")
    {
      targetConllId = 0;
    }
  }
  else
  {
#ifdef DEBUG_LP
    LDEBUG << "LimaAnalyzerPrivate::getConllRelName no target saved for" << v;
#endif
  }
#ifdef DEBUG_LP
    LDEBUG << "LimaAnalyzerPrivate::getConllRelName result for" << v << "is" << conllRelName << targetConllId;
#endif
  return { conllRelName, targetConllId };
}

const SpecificEntityAnnotation* LimaAnalyzerPrivate::getSpecificEntityAnnotation(
  LinguisticGraphVertex v) const
{
  // check only entity found in current graph (not previous graph such as AnalysisGraph)

  for (const auto& vx: annotationData->matches("PosGraph", v, "annot"))
  {
    if (annotationData->hasAnnotation(vx, QString::fromUtf8("SpecificEntity")))
    {
      //BoWToken* se = createSpecificEntity(v,*it, annotationData, anagraph, posgraph, offsetBegin);
      auto se = annotationData->annotation(
        vx,
        QString::fromUtf8("SpecificEntity")).pointerValue<SpecificEntityAnnotation>();
      if (se != nullptr)
      {
        return se;
      }
    }
  }
  return nullptr;

}

bool LimaAnalyzerPrivate::hasSpaceAfter(LinguisticGraphVertex v, LinguisticGraph* graph)
{
  auto ft = get(vertex_token, *graph, v);
  bool SpaceAfter = true;
  LinguisticGraphOutEdgeIt outIter, outIterEnd;
  for (auto [outIter, outIterEnd] = boost::out_edges(v, *graph);
        outIter != outIterEnd; outIter++)
  {
      auto next = boost::target(*outIter, *graph);
      auto nt = get(vertex_token, *graph, next);
      if( nt != nullptr && (nt->position() == ft->position()+ft->length()) )
      {
          SpaceAfter = false;
          break;
      }
  }
  return SpaceAfter;
}

QString LimaAnalyzerPrivate::getMicro(LinguisticAnalysisStructure::MorphoSyntacticData& morphoData)
{
  return QString::fromUtf8(static_cast<const LangData&>(
      MedData::single().mediaData(medId)).getPropertyCodeManager()
        .getPropertyManager("MICRO")
        .getPropertySymbolicValue(morphoData.firstValue(
          *propertyAccessor)).c_str());
}


int main(int argc, char* argv[])
{
  // LimaAnalyzer analyzer1("eng", "main", "");
  // analyzer1 = LimaAnalyzer("eng", "main", "");
  // std::cerr << analyzer1.analyzeText("Hop! Hop!") << std::endl;
  // return 0;
  LimaAnalyzer analyzer0("ud-eng", "deepud", "");
  std::cerr << analyzer0.analyzeText("Hop! Hop!") << std::endl;

  {
    LimaAnalyzer analyzer1("eng", "main", "");
    analyzer0 = LimaAnalyzer("ud-eng", "deepud", "");
    std::cerr << analyzer1.analyzeText("One, 2, three.") << std::endl;
    std::cerr << analyzer0.analyzeText("English again!") << std::endl;
  }
  LimaAnalyzer analyzer2("ud-fra", "deepud", "");
  std::cerr << analyzer2.analyzeText("Nous analysons du français pour terminer.") << std::endl;
}
