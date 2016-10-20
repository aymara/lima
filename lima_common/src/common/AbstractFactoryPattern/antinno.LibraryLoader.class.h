
#pragma once

#include "common/AbstractFactoryPattern/AbstractFactoryPatternExport.h"

namespace Lima { namespace antinno {

class LIMA_FACTORY_EXPORT LibraryLoader
{
public:
  LibraryLoader::LibraryLoader();
  void loadFromFile(::std::string const& filePath);
};

}}