
#include "ConllParser.h"

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <cstdio>
#include <iomanip>

/// This need cleaning up
///
/// Contains all kinds of function to parse text and files

CONLL_Line::CONLL_Line(std::string str) {
  raw = str;
  tokens = split(str, '\t');
}

void freeConllList(CONLL_List& cl) {
  for (unsigned int i=0;i<cl.size();i++) {
    delete(cl[i]);
  }
}

/// splits a string according to the parameter delimiter
/// \return the list of the resulting tokens
std::vector<std::string> split(std::string line, char delimiter) {
  std::vector<std::string> elements;
  std::stringstream ss(line);
  std::string word;
  while (std::getline(ss,word,delimiter)) {
    elements.push_back(word);
  }
  return elements;
}

/// parse a file and return its content
std::vector<std::string> parseFile(std::string filepath) {
  std::ifstream myfile;
  myfile.open(filepath.c_str(), std::ifstream::in);
  std::vector<std::string> content;
  std::string line;
  while (!myfile.eof() && std::getline(myfile,line)) {
    content.push_back(line);
  }
  myfile.close();
  return content;
}

namespace ConllParser {

std::vector<CONLL_Line*> getConllData(std::string filepath) {
    
    std::vector<CONLL_Line*> conll_lines;
    
    // retrieve the file's content
    std::vector<std::string> data = into_lines(parse_conll(filepath));
    
    // create structures
    for (unsigned int i=0; i<data.size(); i++) {
        if (!data[i].empty())
        conll_lines.push_back(new CONLL_Line(data[i]));
    }
    
    return conll_lines;
}

/// removes the unneeded text at the start of the output
void clean_up(std::string& str) {
    str.erase(0,str.find('\'')+1);
    str.erase(0,str.find('\'')+1);
}

/// runs LIMA's analyzeText on parameter file and returns the output
/// \return the raw text content of the file
std::string parse_conll(std::string file) {
    std::string unit = "analyzeText";
    std::string unit_path = "~/Lima/Dist/lima-gui/debug/bin/";
//     chdir(unit_path.c_str());
    std::string cmd = unit + " -l fre " + file;
    //cmd = "cat " + file;
    std::array<char, 128> buffer;
    std::string result;
    
    // open pipe to run another process
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) throw std::runtime_error("Couldn't open a pipe.");
    
    while (!feof(pipe)) {
        if (fgets(buffer.data(), 128, pipe) != NULL) {
            result += buffer.data();
        }
    }
    
    int return_value = pclose(pipe);
    if (return_value) std::cout << "pipe returned " << return_value << std::endl;
    
    clean_up(result);
    return result;
}

/// gets lines from raw text
/// \return the list of the resulting lines
std::vector<std::string> into_lines(std::string str) {
    std::stringstream ss(str);
    std::string line;
    std::vector<std::string> lines;
    
    while (std::getline(ss,line,'\n')) {
        lines.push_back(line);
    }
    
    return lines;    
}

/// displays the expected content : Dependance(mot source, mot cible)
/// words with no dependency are not displayed by default
void show_dependencies(CONLL_List& lines) {
    
    // show if there's no dependency
    bool showna = false;
    std::string na = "{n/a}";
    
    std::string crt_token;
    std::string src_token;
    std::string dependency;
    
    for (unsigned int i=0; i<lines.size(); i++) {
        crt_token = na;
        src_token = na;
        dependency = na;
    
        // get current line word
        crt_token = lines[i]->tokens[1];
        
        // get dependency
        if (lines[i]->tokens[8] != "_")
            dependency = lines[i]->tokens[8];
            
        // get src word
        if (lines[i]->tokens[7] != "_") {
            unsigned int src_ind = std::stoi(lines[i]->tokens[7])-1;
            if (src_ind >= lines.size()) {
                std::cout << "Line " << src_ind << " doesn't exist." << std::endl;
                break;
            }
            CONLL_Line* src_line = lines[src_ind];
            if (src_line) src_token = (src_line->tokens[1]);
        }
        
        // display
        if (crt_token == na || src_token == na || dependency == na) {
            if (showna) std::cout << na << ":" << crt_token << std::endl;}
        else 
            std::cout << dependency << "(" << src_token << ", " << crt_token << ")" << std::endl;   
    }
}

/// display the conll file content
void displayAsColumns(CONLL_List& lines) {
    
    std::vector<std::string> headers;
    headers.push_back("token_num");
    headers.push_back("token");
    headers.push_back("norm_token");
    headers.push_back("token_type");
    headers.push_back("token_what");
    headers.push_back("named_entity");
    headers.push_back("?");
    headers.push_back("target_token");
    headers.push_back("relationship");
    
    int offset = 20;
    for (unsigned int i=0;i<headers.size();i++) {
        std::cout << std::setw(offset) << headers[i];
    }
    std::cout << std::endl;
    for (unsigned int i=0;i<lines.size();i++) for (int j=0;j<offset;j++) std::cout << "-";
    std::cout << std::endl;
    for (unsigned int i=0;i<lines.size();i++) {
        
        for (unsigned int j=0;j<lines[i]->tokens.size();j++) {
            std::cout << std::setw(offset) << (lines[i]->tokens[j] != "_"?lines[i]->tokens[j]:" ");
        }
        
        std::cout << std::endl;
    }
}

} // namespace ConllParser

/*
 Sujet :
 
 Transforme la sortie du format CONLL au format suivant :
 1 ligne par relation de dépendance: dépendance(mot src, mot dest)
 
 Par exemple : Il ne pense pas lui parler ce soir du cas Paul Durand.
 On aura entre autres :
 PronSujVerbe(penser,il)
 
*/
