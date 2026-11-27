// SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 * @file PipelineHandlers.h
 * @brief The analysis handlers needed by the dumpers of a pipeline, and their
 * creation from the configuration (aymara/lima#3)
 *
 * A dumper names in its "handler" parameter the handler it writes to, which
 * the client gives to AbstractLinguisticProcessingClient::analyze under this
 * name. The class of a handler is the one of the group of this name in the
 * "Resources" module of the media's configuration (e.g.
 * <group name="simpleStreamHandler" class="SimpleStreamHandler"/>), or, for
 * the usual handler names, a built-in one. This allows a program to create
 * the handlers of a pipeline instead of hard-coding them.
 */

#ifndef LIMA_LINGUISTICPROCESSING_PIPELINEHANDLERS_H
#define LIMA_LINGUISTICPROCESSING_PIPELINEHANDLERS_H

#include "AnalysisHandlersExport.h"
#include "AbstractTextualAnalysisHandler.h"
#include "common/LimaCommon.h"

#include <memory>
#include <string>
#include <vector>

namespace Lima
{
namespace LinguisticProcessing
{

/** A handler needed by a dumper of a pipeline */
struct LIMA_ANALYSISHANDLERS_EXPORT PipelineHandler
{
  /** The dumper: its process unit (group) name */
  std::string dumper;
  /** The handler: the "handler" parameter of the dumper */
  std::string handler;
};

/**
 * The handlers needed by the dumpers of the pipeline of the media (the units
 * with a "handler" parameter), in the order of the pipeline, its
 * sub-pipelines included. Empty if the pipeline does not exist. The units
 * added to a pipeline without being in the configuration (e.g. through
 * LimaAnalyzer::addPipelineUnit) are not seen.
 */
LIMA_ANALYSISHANDLERS_EXPORT std::vector<PipelineHandler> pipelineHandlers(
    MediaId media,
    const std::string& pipeline);

/**
 * The class of the handler named name for the media: the class of the group
 * of this name in the media's "Resources" configuration module or, if there
 * is none, the built-in class of the usual handler names (simpleStreamHandler,
 * fullXmlSimpleStreamHandler, bowTextWriter, bowTextHandler, ltrTextHandler,
 * bowDocumentHandler, metaDataHandler, eventHandler). Empty if unknown.
 */
LIMA_ANALYSISHANDLERS_EXPORT std::string analysisHandlerClass(
    MediaId media,
    const std::string& name);

/**
 * Creates the handler named name for the media (its class: see
 * analysisHandlerClass), initialized with the parameters of its group if any.
 * Returns nullptr (with an error in the log) if its class is unknown or has no
 * factory (its library not loaded, or not an AbstractTextualAnalysisHandler).
 */
LIMA_ANALYSISHANDLERS_EXPORT std::shared_ptr<AbstractTextualAnalysisHandler>
createAnalysisHandler(MediaId media, const std::string& name);

} // LinguisticProcessing
} // Lima

#endif
