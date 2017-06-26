/**
 * \author Jocelyn VERNAY
 * \file Writer.h
 * \project lima-qt-gui
 * \date 20-06-2017 
 */

#ifndef writer_h
#define writer_h

#include <QObject>
#include <QString>
#include <iostream>
#include <map>

/**
 * \brief Contains the logic of the text editor, as in save and load files features.
 */
class Writer : public QObject {
Q_OBJECT
Q_PROPERTY(QString filepath MEMBER m_file_path)
Q_PROPERTY(QString file_content MEMBER m_file_content)
public:
  Writer(QObject* q = 0);
    
  /// \brief save text content (m_file_content) to member filepath
  Q_INVOKABLE void saveFile();
  
  /// \brief load text content (m_file_content) from member filepath
  Q_INVOKABLE void loadFile();
    
private:
  QString m_file_path; ///< url of the file
  QString m_file_content; ///< text content loaded / to save
  std::map<std::string, std::string> all_files; ///< name -> real url
  
};

/*
 * FileTextEditor {
 *   id:x
 *   property string file: "fiche1.txt"
 *   
 * }
 * 
 * 
 * 
 */


#endif // writer_h
