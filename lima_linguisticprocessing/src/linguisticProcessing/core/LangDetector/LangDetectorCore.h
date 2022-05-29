//
// Created by tderouet on 24/05/22.
//

#ifndef LIMA_LANGDETECTORCORE_H
#define LIMA_LANGDETECTORCORE_H

#include <iostream>
#include <sstream>
#include "fasttext/fasttext.h"

namespace Lima::LinguisticProcessing::LDetector {

    class LangDetectorCore
    {
    public:
        LangDetectorCore();
        virtual ~LangDetectorCore();

        [[nodiscard]] std::string detectLang(const std::string& sentence) const;
        /*Used to convert a language label "__label__lang" to a printable format "lang".*/
        [[nodiscard]] static std::string labelToPrintable(const std::string& label) ;

        fasttext::FastText ftext;

    };
}


#endif //LIMA_LANGDETECTORCORE_H
