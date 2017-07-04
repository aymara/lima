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
#include <QAbstractListModel>

class CONLL_Line;

class ConllRow : public QObject, public CONLL_Line {
  Q_OBJECT
  Q_PROPERTY(int nid READ getId)
  Q_PROPERTY(Qstring token READ getToken)
  Q_PROPERTY(QString norm_token READ getNormToken)
  Q_PROPERTY(QString type READ getType)
  
public:
  ConllRow(std::string s, QObject* p = 0);
  
  int getId();
  Q_INVOKABLE QString getToken();
  Q_INVOKABLE QString getNormToken();
  Q_INVOKABLE QString getType();
};

// http://doc.qt.io/qt-5/qabstracttablemodel.html

class ConllListModel : public QAbstractListModel {
  Q_OBJECT
public:
  ConllListModel();

  /// METHODS to reimplement :
  Q_INVOKABLE rowCount(const QModelIndex& parent = QModelIndex()) const;
  Q_INVOKABLE columnCount(const QModelIndex& parent = QModelIndex()) const;
  Q_INVOKABLE data(const QModelIndex& index, int rol = QT::DisplayRole) const;
};

#endif CONLL_LIST_MODEL_H
