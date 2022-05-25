//
// Created by tderouet on 24/05/22.
//

#include "LangDetectorCore.h"

namespace Lima::LinguisticProcessing::LDetector {
    LangDetectorCore::LangDetectorCore()
    = default;

    LangDetectorCore::~LangDetectorCore() = default;

    std::string LangDetectorCore::detectLang(const std::string& sentence) const {
        std::string lang;
        std::vector<std::pair<fasttext::real, std::string>> prediction;
        std::stringstream ioss(sentence + std::string("\n"));
        ftext.predictLine(ioss, prediction, 1, 0);
        lang = prediction.front().second;
        return lang;
    }

    std::string LangDetectorCore::labelToPrintable(const std:: string &label) {
        return label.substr(9, std::string::npos);
    }
}