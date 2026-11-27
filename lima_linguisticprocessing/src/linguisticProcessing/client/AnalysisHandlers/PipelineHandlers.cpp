// SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "PipelineHandlers.h"

#include "common/MediaProcessors/MediaProcessors.h"
#include "common/XMLConfigurationFiles/moduleConfigurationStructure.h"
#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"

#include <algorithm>
#include <map>
#include <set>

using namespace Lima::Common::XMLConfigurationFiles;

namespace Lima
{
namespace LinguisticProcessing
{

namespace
{

/** The classes of the usual handler names, for the configurations without
 * their groups (e.g. a user configuration from an older LIMA) */
const std::map<std::string, std::string>& builtinHandlerClasses()
{
  static const std::map<std::string, std::string> classes = {
    {"simpleStreamHandler", "SimpleStreamHandler"},
    {"fullXmlSimpleStreamHandler", "SimpleStreamHandler"},
    {"bowTextWriter", "BowTextWriter"},
    {"bowTextHandler", "BowTextHandler"},
    {"ltrTextHandler", "LTRTextHandler"},
    {"bowDocumentHandler", "BowDocumentHandler"},
    {"metaDataHandler", "MetaDataHandler"},
    {"eventHandler", "EventHandler"},
  };
  return classes;
}

/** Appends to result the handlers of the unit named unit and, if it is a
 * pipeline, of its units */
void collectHandlers(const ModuleConfigurationStructure& module,
                     const std::string& unit,
                     std::vector<PipelineHandler>& result,
                     std::set<std::string>& visited)
{
  // A unit appears once (and a misconfigured pipeline including itself does
  // not loop)
  if (!visited.insert(unit).second)
    return;
  auto groupIt = module.find(unit);
  if (groupIt == module.end())
    return;
  // The accessors of GroupConfigurationStructure are not const
  auto group = groupIt->second;
  try
  {
    for (const auto& sub: group.getListsValueAtKey("processUnitSequence"))
      collectHandlers(module, sub, result, visited);
  }
  catch (const NoSuchList&) {}
  try
  {
    auto handler = group.getParamsValueAtKey("handler");
    if (!handler.empty())
      result.push_back({unit, handler});
  }
  catch (const NoSuchParam&) {}
}

/** The group named name in the media's "Resources" module, nullptr if none */
GroupConfigurationStructure* resourceGroup(MediaId media, const std::string& name)
{
  try
  {
    auto& module = LinguisticResources::changeable().getModuleConfiguration(media);
    auto it = module.find(name);
    if (it != module.end())
      return &it->second;
  }
  catch (const InvalidConfiguration&) {}
  return nullptr;
}

} // namespace

std::vector<PipelineHandler> pipelineHandlers(MediaId media,
                                              const std::string& pipeline)
{
  std::vector<PipelineHandler> result;
  auto pipe = MediaProcessors::single().getPipelineForId(media, pipeline);
  if (pipe == nullptr)
    return result;
  const auto& managers = MediaProcessors::single().managers();
  auto managerIt = managers.find(media);
  if (managerIt == managers.end() || managerIt->second == nullptr)
    return result;
  std::set<std::string> visited;
  collectHandlers(managerIt->second->getModuleConfigurationStructure(),
                  pipe->getId(), result, visited);
  return result;
}

std::string analysisHandlerClass(MediaId media, const std::string& name)
{
  auto group = resourceGroup(media, name);
  if (group != nullptr)
  {
    try
    {
      auto className = group->getAttribute("class");
      if (!className.empty())
        return className;
    }
    catch (const NoSuchAttribute&) {}
  }
  auto it = builtinHandlerClasses().find(name);
  return it == builtinHandlerClasses().end() ? std::string() : it->second;
}

std::shared_ptr<AbstractTextualAnalysisHandler> createAnalysisHandler(
    MediaId media,
    const std::string& name)
{
  auto className = analysisHandlerClass(media, name);
  if (className.empty())
  {
    HANDLERLOGINIT;
    LERROR << "createAnalysisHandler: unknown handler" << name
           << ": no group of this name in the Resources configuration";
    return nullptr;
  }
  const auto& factories = AbstractResource::Factory::getRegisteredFactories();
  if (std::find(factories.begin(), factories.end(), className) == factories.end())
  {
    HANDLERLOGINIT;
    LERROR << "createAnalysisHandler: no factory for the class" << className
           << "of the handler" << name << "(its library is not loaded?)";
    return nullptr;
  }
  // The handler's own group (its parameters), or an empty one
  auto group = resourceGroup(media, name);
  GroupConfigurationStructure emptyGroup(name);
  auto& configuration = group != nullptr ? *group : emptyGroup;
  // The handlers are created for a client, not shared as the resources of
  // the Resources module (no manager: they do not use it)
  auto resource = AbstractResource::Factory::getFactory(className)->create(
    configuration, nullptr);
  auto handler = std::dynamic_pointer_cast<AbstractTextualAnalysisHandler>(resource);
  if (handler == nullptr)
  {
    HANDLERLOGINIT;
    LERROR << "createAnalysisHandler: the class" << className << "of the handler"
           << name << "is not an AbstractTextualAnalysisHandler";
  }
  return handler;
}

} // LinguisticProcessing
} // Lima
