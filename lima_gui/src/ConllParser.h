#ifndef conll_analyzer_h
#define conll_analyzer_h

#include <string>
#include <vector>

/// A line of the CONLL format.
/// raw : is the raw text of the line.
/// tokens : list obtained by splitting the text.
struct CONLL_Line {
    std::string raw;
    std::vector<std::string> tokens;
    
    CONLL_Line(const std::string& str);
    const std::string& operator[](unsigned int) const;
};

typedef std::vector<CONLL_Line*> CONLL_List;

/// Free all pointers in the list
void freeConllList(CONLL_List&);

///////////////////////////////////////////////////////////////

std::vector<std::string> parseFile(const std::string& filepath);

std::vector<std::string> split(const std::string& str, const char& delimiter);

namespace ConllParser {

std::string parse_conll(const std::string& filepath);

std::vector<std::string> into_lines(const std::string& str);

std::vector<CONLL_Line*> textToConll(const std::string& text);


/// Returns the whole CONLL data bunch
/// Pointers should be deleted when no more used
/// You can use the method freeConlllist for that
CONLL_List getConllData(const std::string& filepath);

void show_dependencies(const CONLL_List& lines);

void displayAsColumns(const CONLL_List& lines);

} // namespace ConllParser

//std::map<std::string, std::vector<std::string> > getNamedEntitiesFromConll(const std::string& text);


//std::string markup(const std::string& content, const std::string& markup, const std::string& style);

///// \return the text with html highlighted named entities
///// \param text : the raw text
///// \param types is the map obtained by the above function
///// \param is the colors : <named_entity_type_name>:<color>
//std::string highlightNamedEntities(
//    const std::string& raw,
//    std::map<std::string, std::vector<std::string>>& types,
//    std::map<std::string, std::string>& colors);





#endif // conll_analyzer_h
