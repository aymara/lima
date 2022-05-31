//
// Created by tderouet on 24/05/22.
//

#ifndef LIMA_LANGDETECTORCORE_H
#define LIMA_LANGDETECTORCORE_H

#include <iostream>
#include <sstream>
#include "fasttext.h"

namespace Lima::LinguisticProcessing::LDetector {
    class LangDetectorCorePrivate;

    class LangDetectorCore
    {
    public:
        LangDetectorCore();
        virtual ~LangDetectorCore();

        void loadModel(std::string modelPath) const;

        [[nodiscard]] std::string detectLang(const std::string& text) const;
        /*Used to convert a language label "__label__lang" to a printable format "lang".*/
        [[nodiscard]] static std::string labelToPrintable(const std::string& label) ;
    private:
        LangDetectorCorePrivate* m_d;

    };
}


#endif //LIMA_LANGDETECTORCORE_H
