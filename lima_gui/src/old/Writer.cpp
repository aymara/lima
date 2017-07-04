#include "Writer.h"
#include <fstream>
#include <sstream>
#include "TextAnalyzer.h"

#include <iostream>
#include <QTextStream>
#include <QFile>

// std::vector<std::string> split(std::string text, char delimiter) {
//         std::vector<std::string> content;
//         std::stringstream tstream(text);
//         std::string word;
//         while(std::getline(tstream,word,delimiter)) {
//             content.push_back(word);
//         }
//         return content;
// }

Writer::Writer(QObject* parent) : QObject(parent) {
  
}

void Writer::loadFile() {
  QFile file(QString(split(m_file_path.toStdString(),':')[1].c_str()));
  if (file.open(QFile::ReadOnly)) {
    QTextStream qts(&file);
    m_file_content = qts.readAll();
    file.close();
  }
  else {
    std::cout << "didn't open : " << m_file_path.toStdString() << std::endl;
    std::cout << "Error opening file: " << strerror(errno) << std::endl;
  }
}

void Writer::saveFile() {
//   QFile file(QString(split(m_file_path.toStdString(),':')[1].c_str()));
  QFile file(QString("test.html"));
  if (file.open(QFile::WriteOnly | QFile::Text)) {
    QTextStream qts(&file);
    qts << m_file_content;
    file.close();
  }
  else {
    std::cout << "didn't open : " << m_file_path.toStdString() << std::endl;
    std::cout << "Error opening file: " << strerror(errno) << std::endl;
  }
}


// QString Writer::loadFile(QString path) {
//     std::cout << "loading file " << path.toStdString() << std::endl;
//     std::ifstream myfile(path.toStdString());
//     std::string line;
//     std::string content = "";
//     if (myfile.is_open()) {
//         while (std::getline(myfile,line)) {
//             content += line;
//         }
//         
//         myfile.close();
//     }
//     else {
//         std::cout << "Couldn't open file" << std::endl;
//     }
//     std::cout << "content=" << content << std::endl;
//     return QString(content.c_str());
// }
// 
// void Writer::saveAs(QString path, QString content) {
//     std::ofstream myfile;
//     myfile.open(path.toStdString());
//     if (myfile.is_open()) {
//         myfile << content.toStdString();
//         myfile.close();
//     }
//     else {
//         std::cout << "Couldn't save at " << path.toStdString() << std::endl;
//     }
// }
// 
// void Writer::save() {
//     if (filepath != "") {
//         saveAs(QString(filepath.c_str()), QString("Oyez oyez !"));
//     }
// }
