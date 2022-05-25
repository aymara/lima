#include "fastText/src/fasttext.h"
#include "LangDetectorCore.h"

using namespace Lima::LinguisticProcessing::LDetector;
int main(int argc, char *argv[]) {
    if(argc != 2) {
        std::cout << "Please execute the program with a sentence. Like this : ./LangDetector \"sentence\"\n";
        return 1;
    }
    auto ld = std::make_unique<LangDetectorCore>();
    ld->ftext.loadModel(std::string(std::getenv("LIMA_RESOURCES"))+"/LinguisticProcessings/LangDetector/lid.176.ftz");
    std::string label = ld->detectLang(std::string(argv[1]));
    std::cout << "The detected language is : " << LangDetectorCore::labelToPrintable(label) << "\n";
    return 0;
}