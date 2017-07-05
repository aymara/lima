#ifndef CONLL_LIST_MODEL_H
#define CONLL_LIST_MODEL_H

#include "ConllParser.h"

/// Pour afficher la sortie au format CONLL sous forme de table,
/// on peut utiliser le QML TableView. Cependant, il faut lui donner un model,
/// et il ne semble pas y avoir de maniere simple de le faire autre que d'implementer
/// une classe pour cela.
///
/// QML ne connait pas la notion de table, mais listes/roles c'est connu
/// role = column header, list = column content


#include <QObject>
#include <QAbstractTableModel>
//#include <QVariant>

//http://qiita.com/illness072/items/a6f2ce9f7a1bfff44049

// CONLL Format :
// http://universaldependencies.org/docs/format.html
// https://stackoverflow.com/questions/27416164/what-is-conll-data-format

class ConllRow : public QObject, public CONLL_Line {
  Q_OBJECT
  Q_PROPERTY(QString nid READ getId)
  Q_PROPERTY(QString token READ getToken)
  Q_PROPERTY(QString norm_token READ getNormToken)
  Q_PROPERTY(QString type READ getType)
  
public:
  ConllRow(std::string s, QObject* p = 0);
  
  QString at(unsigned int i) const { return tokens.size() && tokens.size() > i ? QString(tokens[i].c_str()) : QString("undefined"); }
  Q_INVOKABLE QString getId() const { return at(0); }
  Q_INVOKABLE QString getToken() const { return at(1); }
  Q_INVOKABLE QString getNormToken() const { return at(2); }
  Q_INVOKABLE QString getType() const { return at(3); }
};

// http://doc.qt.io/qt-5/qabstracttablemodel.html

class ConllListModel : public QAbstractTableModel {
  Q_OBJECT
public:
  ConllListModel(QObject* p = 0);
  ConllListModel(const QString&, QObject* p = 0);
  ~ConllListModel();

  /// METHODS to reimplement :
  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  int columnCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int rol = Qt::DisplayRole) const;
  QHash<int, QByteArray> roleNames() const;

  Q_INVOKABLE void fromText(const QString&);

private:
  std::vector<ConllRow*> m_data;
//  enum {
//    RoleId = Qt::UserRole +1,
//    RoleToken,
//    RoleNorm,
//    RoleWhat,
//    RoleNamedEntity,
//    Roleu1,
//    Roleu2,
//    Roleu3,
//    Roleu4,
//    RoleTarget,
//    RoleRelationship
//  };

//  ID FORM LEMMA PLEMMA POS PPOS FEAT PFEAT HEAD PHEAD DEPREL PDEPREL

//  The definition of some of these columns come from earlier shared tasks (the CoNLL-X format used in 2006 and 2007):

//      ID (index in sentence, starting at 1)
//      FORM (word form itself)
//      LEMMA (word's lemma or stem)
//      POS (part of speech)
//      FEAT (list of morphological features separated by |)
//      HEAD (index of syntactic parent, 0 for ROOT)
//      DEPREL (syntactic relationship between HEAD and this word)


  enum {
    ID = Qt::UserRole + 1,
    FORM,
    LEMMA,
    PLEMMA,
    POS,
    PPOS,
    FEAT,
    PFEAT,
    HEAD,
    PHEAD,
    DEPREL,
    PDEPREL
  };
};

//Q_DECLARE_METATYPE(ConllListModel);

#endif // CONLL_LIST_MODEL_H
