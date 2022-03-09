#include "langDetector.h"
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

LangDetector::LangDetector(string modelDir){
    m_fasttext.loadModel(modelDir);
}

const string LangDetector::detectLang(string sentence){
    string lang;
    std::vector<std::pair<fasttext::real, std::string>> prediction;
    std::stringstream ioss(sentence + std::string("\n"));
    m_fasttext.predictLine(ioss,prediction,1,0);
    lang = prediction.front().second;
    return lang;
}

const string LangDetector::labelToPrintable(string label){
    return label.substr(9,string::npos);
}