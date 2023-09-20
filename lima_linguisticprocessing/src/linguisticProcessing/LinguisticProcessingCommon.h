// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2020 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/

#include <common/LimaCommon.h>

#ifndef LIMA_LINGUISTICPROCESSING_COMMON_H
#define LIMA_LINGUISTICPROCESSING_COMMON_H

#ifdef WIN32

#undef min
#undef max

#ifdef LIMA_ANALYSISDICT_EXPORTING
   #define LIMA_ANALYSISDICT_EXPORT    __declspec(dllexport)
#else
   #define LIMA_ANALYSISDICT_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_BOW_EXPORTING
   #define LIMA_BOW_EXPORT    __declspec(dllexport)
#else
   #define LIMA_BOW_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_PROPERTYCODE_EXPORTING
   #define LIMA_PROPERTYCODE_EXPORT    __declspec(dllexport)
#else
   #define LIMA_PROPERTYCODE_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_LPMISC_EXPORTING
   #define LIMA_LPMISC_EXPORT    __declspec(dllexport)
#else
   #define LIMA_LPMISC_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_ANNOTATIONGRAPH_EXPORTING
   #define LIMA_ANNOTATIONGRAPH_EXPORT    __declspec(dllexport)
#else
   #define LIMA_ANNOTATIONGRAPH_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_LINGUISTICDATA_EXPORTING
   #define LIMA_LINGUISTICDATA_EXPORT    __declspec(dllexport)
#else
   #define LIMA_LINGUISTICDATA_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_LINEARTEXTREPRESENTATION_EXPORTING
   #define LIMA_LINEARTEXTREPRESENTATION_EXPORT    __declspec(dllexport)
#else
   #define LIMA_LINEARTEXTREPRESENTATION_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_TGV_EXPORTING
   #define LIMA_TGV_EXPORT    __declspec(dllexport)
#else
   #define LIMA_TGV_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_LP_HELPERS_EXPORTING
   #define LIMA_LP_HELPERS_EXPORT    __declspec(dllexport)
#else
   #define LIMA_LP_HELPERS_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define LIMA_DATA_EXPORT
#define LIMA_ANALYSISDICT_EXPORT
#define LIMA_BOW_EXPORT
#define LIMA_PROPERTYCODE_EXPORT
#define LIMA_LPMISC_EXPORT
#define LIMA_ANNOTATIONGRAPH_EXPORT
#define LIMA_LINGUISTICDATA_EXPORT
#define LIMA_LINEARTEXTREPRESENTATION_EXPORT
#define LIMA_TGV_EXPORT
#define LIMA_LP_HELPERS_EXPORT

#endif

#define LPCLIENTFACTORYLOGINIT LOGINIT("LP::ClientFactory");
#define CORECLIENTLOGINIT LOGINIT("LP::CoreClient");
#define RESOURCESLOGINIT LOGINIT("LP::Resources")
#define DICTIONARYLOGINIT LOGINIT("LP::Dictionary")
#define LASLOGINIT LOGINIT("LP::LAS")
// #define PROCESSORSLOGINIT LOGINIT("LP::Processors") // dans Common désormais
#define TOKENIZERLOGINIT LOGINIT("LP::Tokenizer")
#define TOKENIZERLOADERLOGINIT LOGINIT("LP::Tokenizer::Loader")
#define COREFSOLVERLOGINIT LOGINIT("LP::CorefSolver")
#define EVENTANALYZERLOGINIT LOGINIT("LP::EventAnalyzer")
#define EVENTANALYSISLOGINIT LOGINIT("LP::EventAnalysis")
#define COMPACTDICTLOGINIT  LOGINIT("LP::CompactDict")
#define MORPHOLOGINIT  LOGINIT("LP::MorphologicAnalysis")
#define TFSELOGINIT  LOGINIT("LP::TensorflowSpecificEntities")
#define AULOGINIT  LOGINIT("LP::Automaton")
#define PTLOGINIT  LOGINIT("LP::PosTagger")
#define SELOGINIT  LOGINIT("LP::SpecificEntities")
#define SALOGINIT  LOGINIT("LP::SyntacticAnalysis")
#define COMPOUNDSLOGINIT LOGINIT("LP::Compounds")
#define DUMPERLOGINIT LOGINIT("LP::Dumper")
#define DESAGGLOGINIT LOGINIT("LP::Desagglutination");
#define HWLOGINIT LOGINIT("LP::HyperWordStemmer");
#define ANALYSISDICTLOGINIT LOGINIT("LP::AnalysisDict");
#define SENTBOUNDLOGINIT LOGINIT("LP::SentenceBounds");
#define SEGMENTATIONLOGINIT LOGINIT("LP::Segmentation");
#define GEOENTITIESTAGGERLOGINIT LOGINIT("LP::GeoEntities");
#define SEMANTICANALYSISLOGINIT LOGINIT("LP::SemanticAnalysis");
#define LIMASERVERLOGINIT LOGINIT("LP::LimaServer");
#define TENSORFLOWMORPHOSYNTAXLOGINIT LOGINIT("LP::TensorFlowMorphoSyntax");
#define TENSORFLOWLEMMATIZERLOGINIT LOGINIT("LP::TensorFlowLemmatizer");
#define SENTENCEBOUNDARIESUPDATERLOGINIT LOGINIT("LP::SentenceBoundariesUpdater");
#define LANGDETECTORLOGINIT LOGINIT("LP::LangDetector")
#define LEMMALOGINIT LOGINIT("LP::Lemmatizer")


#endif
