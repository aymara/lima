#ifndef conll_analyzer_h
#define conll_analyzer_h

struct CONLL_Line {
    std::string raw;
    std::vector<std::string> tokens;
    
    CONLL_Line(std::string str);
};

typedef std::vector<CONLL_Line*> CONLL_List;

/// Free all pointers in the list
void freeConllList(CONLL_List&);

std::vector<std::string> split(std::string str, char delimiter);

namespace ConllParser {
  
  
  
/// A line of the CONLL format.
/// raw : is the raw text of the line.
/// tokens : list obtained by splitting the text.

std::string parse_conll(std::string filepath);
std::vector<std::string> into_lines(std::string str);




/// Returns the whole CONLL data bunch
/// Pointers should be deleted when no more used
/// You can use the method freeConlllist for that
CONLL_List getConllData(std::string filepath);

void show_dependencies(CONLL_List& lines);
void displayAsColumns(CONLL_List& lines);

} // namespace ConllParser

#endif // conll_analyzer_h
