#include "fastText/src/fasttext.h"
#include "langDetector.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    LangDetector* ld = new LangDetector("../model/lid.176.ftz");
    cout << "Checking example with french: ";
    if(ld->detectLang("Je suis un éxemple.")=="__label__fr"){
        cout << "PASS\n";
    }    
    else{
        cout << "ERROR\n";
    }
    cout << "Checking counter example with french: ";
    if(ld->detectLang("I am not in french.")!="__label__fr"){
        cout << "PASS\n";
    }    
    else{
        cout << "ERROR\n";
    }
    cout << "Checking example with english: ";
    if(ld->detectLang("This is an example.")=="__label__en"){
        cout << "PASS\n";
    }    
    else{
        cout << "ERROR\n";
    }
    cout << "Checking example with chinese: ";
    if(ld->detectLang("我是一个例子")=="__label__zh"){
        cout << "PASS\n";
    }    
    else{
        cout << "ERROR\n";
    }
    cout << "Checking string conversion : ";
    if(ld->labelToPrintable("__label__fr") == "fr"){
        cout << "PASS\n";
    }    
    else{
        cout << "ERROR\n";
    }
    return 0;
}