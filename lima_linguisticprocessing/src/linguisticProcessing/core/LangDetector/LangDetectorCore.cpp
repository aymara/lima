//
// Created by tderouet on 24/05/22.
//

#include "LangDetectorCore.h"

namespace Lima::LinguisticProcessing::LDetector {
    class LangDetectorCorePrivate{
    public:
        fasttext::FastText ftext;
    };

    LangDetectorCore::LangDetectorCore():m_d(new LangDetectorCorePrivate()){

    }

    LangDetectorCore::~LangDetectorCore() = default;

    /*
     * Detect the language of the text.
     */

    std::string LangDetectorCore::detectLang(const std::string& text) const {
        std::string lang;
        std::vector<std::pair<fasttext::real, std::string>> prediction;
        std::stringstream ioss(text + std::string("\n"));
        m_d->ftext.predictLine(ioss, prediction, 1, 0);
        lang = prediction.front().second;
        return lang;
    }
    /*
     * Remove the "__label__" part of the language label.
     */
    std::string LangDetectorCore::labelToPrintable(const std:: string &label) {
        return label.substr(9, std::string::npos);
    }

    void LangDetectorCore::loadModel(std::string modelPath) const {
        m_d->ftext.loadModel(modelPath);
    }
}