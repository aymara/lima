/**
 * \file    ConllParser.h
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 * 
 */

#ifndef conll_analyzer_h
#define conll_analyzer_h

#include "LimaGuiExport.h"

#include <string>
#include <vector>
#include <map>
#include <iostream>

namespace Lima {
namespace Gui {

/// \brief A line of the CONLL format.
struct LIMA_GUI_EXPORT CONLL_Line 
{

    std::string raw; ///< raw text of the line.
    std::vector<std::string> tokens; ///< list obtained by splitting the text.
    
    CONLL_Line(const std::string& str);

    const std::string& operator[](unsigned int) const;

    std::string at(unsigned int i) {
      if (i >= tokens.size()) {
        std::cout << "size=" << tokens.size() << ",i=" << i << std::endl;
        return "";
      }
      else
      return tokens[i];
    }
};

typedef std::vector<CONLL_Line*> CONLL_List;

LIMA_GUI_EXPORT 
CONLL_List conllRawToLines(const std::string& conll);

LIMA_GUI_EXPORT
/// Free all pointers in the list
void freeConllList(CONLL_List&);

///////////////////////////////////////////////////////////////

LIMA_GUI_EXPORT
std::vector<std::string> parseFile(const std::string& filepath);

LIMA_GUI_EXPORT
std::vector<std::string> split(const std::string& str, const char& delimiter);

LIMA_GUI_EXPORT
std::string parse_conll(const std::string& filepath);

LIMA_GUI_EXPORT
std::vector<std::string> into_lines(const std::string& str);

LIMA_GUI_EXPORT
std::vector<CONLL_Line*> textToConll(const std::string& text);


LIMA_GUI_EXPORT
/// Returns the whole CONLL data bunch
/// Pointers should be deleted when no more used
/// You can use the method freeConlllist for that
CONLL_List getConllData(const std::string& filepath);

LIMA_GUI_EXPORT
void show_dependencies(const CONLL_List& lines);

LIMA_GUI_EXPORT
void displayAsColumns(const CONLL_List& lines);

LIMA_GUI_EXPORT
std::map<std::string, std::vector<std::string> > getNamedEntitiesFromConll(const std::string& text);

LIMA_GUI_EXPORT
std::string markup(const std::string& content, const std::string& markup, const std::string& style);

LIMA_GUI_EXPORT
///// \return the text with html highlighted named entities
///// \param text : the raw text
///// \param types is the map obtained by the above function
///// \param is the colors : <named_entity_type_name>:<color>
std::string highlightNamedEntities(
    const std::string& raw,
    std::map<std::string, std::vector<std::string>>& types,
    std::map<std::string, std::string>& colors);


} // END namespace Gui
} // END namespace Lima


#endif // conll_analyzer_h
