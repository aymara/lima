#ifndef FILETEXTEXTRACTOR_H
#define FILETEXTEXTRACTOR_H

#include <string>
#include <vector>

namespace Lima {
namespace Gui {
namespace Tools {

/// \brief To handle different types of files, this class sets  the base for
/// file text extraction.
/// You can inherit from this class for each extension (pdf, docx) to create
/// a new handler for a type of file
/// You'll always need to modify the code of LimaGuiApplication::openFile, though ...
/// So it's not ideal

std::string extractTextFromFile(const std::string& path, const std::string& extension = "");

} // Tools
} // Gui
} // Lima

#endif // FILETEXTEXTRACTOR_H
