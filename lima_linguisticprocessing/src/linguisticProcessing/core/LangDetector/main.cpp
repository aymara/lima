#include "fastText/src/fasttext.h"
#include "langDetector.h"

int main(int argc, char *argv[]) {
    if(argc != 2) {
        cout << "Please execute the program with a sentence. Like this : ./LangDetector \"sentence\"\n";
        return 1;
    }
    LangDetector* ld = new LangDetector("../model/lid.176.ftz");
    string label = ld->detectLang(argv[1]);
    cout << "The detected language is : " << ld->labelToPrintable(label) << "\n";
    return 0;
}