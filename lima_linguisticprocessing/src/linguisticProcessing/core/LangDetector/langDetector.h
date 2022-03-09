#include <string>
#include "fastText/src/fasttext.h"

#ifndef LANGDETECTOR_H
#define LANGDETECTOR_H

using namespace std;

class LangDetector {
    public:
        LangDetector(string modelPath);
        const string detectLang(string sentence);
    private:
        fasttext::FastText m_fasttext;
};

#endif