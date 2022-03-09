#include <string>
#include "fastText/src/fasttext.h"

#ifndef LANGDETECTOR_H
#define LANGDETECTOR_H

using namespace std;

/**
 * @brief Main class of the language detector module.
 * 
 */
class LangDetector {
    public:
        LangDetector(string modelPath);
        /*This function takes a sentence for input and return the associated language label.*/
        const string detectLang(string sentence);
        /*Used to convert a language label "__label__lang" to a printable format "lang".*/
        const string labelToPrintable(string label);
    private:
        fasttext::FastText m_fasttext;
};

#endif