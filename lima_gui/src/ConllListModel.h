#ifndef CONLL_LIST_MODEL_H
#define CONLL_LIST_MODEL_H

#include "ConllParser.h"

#include <QObject>
#include <QAbstractTableModel>
//#include <QVariant>

//http://qiita.com/illness072/items/a6f2ce9f7a1bfff44049

// CONLL Format :
// http://universaldependencies.org/docs/format.html
// https://stackoverflow.com/questions/27416164/what-is-conll-data-format

// actually, there is an entry for it in the lima wiki

/// \class ConllRow
/// \brief This is an extension of the CONLL_Line structure to Qt
class ConllRow : public QObject, public CONLL_Line {
  Q_OBJECT
  Q_PROPERTY(QString nid READ getId)
  Q_PROPERTY(QString token READ getToken)
  Q_PROPERTY(QString norm_token READ getNormToken)
  Q_PROPERTY(QString type READ getType)
  
public:
  ConllRow(const std::string& s, QObject* p = 0);
  
  QString at(unsigned int i) const { return tokens.size() && tokens.size() > i ? QString(tokens[i].c_str()) : QString("undefined"); }
  Q_INVOKABLE QString getId() const { return at(0); }
  Q_INVOKABLE QString getToken() const { return at(1); }
  Q_INVOKABLE QString getNormToken() const { return at(2); }
  Q_INVOKABLE QString getType() const { return at(3); }
};

// http://doc.qt.io/qt-5/qabstracttablemodel.html

/// \class ConllListModel
/// \brief conll qt data model
class ConllListModel : public QAbstractTableModel {
  Q_OBJECT
public:
  ConllListModel(QObject* p = 0);
  ConllListModel(const QString&, QObject* p = 0);
  ~ConllListModel();

  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  int columnCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int rol = Qt::DisplayRole) const;
  QHash<int, QByteArray> roleNames() const;

  ///
  /// \brief convert the raw conll output into a data model suitable for Qt use
  /// \param raw is the conll output in raw text format
  Q_INVOKABLE void fromText(const QString& raw);

private:
  std::vector<ConllRow*> m_data;

  enum {
    ID = Qt::UserRole + 1,
    FORM,
    LEMMA,
    CPOSTAG,
    POSTAG,
    NE,
    FEATS,
    HEAD,
    DEPREL,
    PHEAD,
    PDEPREL
  };
};

#endif // CONLL_LIST_MODEL_H
