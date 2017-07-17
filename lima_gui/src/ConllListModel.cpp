#include "ConllListModel.h"
#include <iostream>
#include <QtDebug>

ConllRow::ConllRow(const std::string& s, QObject* p) : QObject(p), CONLL_Line(s) {

}

////////////////////////////////
/// \brief ConllListModel::ConllListModel
/// \param content
/// \param p
ConllListModel::ConllListModel(QObject *p) : QAbstractTableModel(p) {

}

ConllListModel::ConllListModel(const QString& content, QObject* p) : QAbstractTableModel(p) {
  fromText(content);
}

void ConllListModel::fromText(const QString& text) {

  std::vector<std::string> data = ConllParser::into_lines(text.toStdString());

  for (unsigned int i=0; i<data.size(); i++) {
      if (!data[i].empty()) {
        m_data.push_back(new ConllRow(data[i]));
      }
  }
}

int ConllListModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return m_data.size();
}

int ConllListModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 3;
}

QVariant ConllListModel::data(const QModelIndex &index, int rol) const {
  if ( rol > Qt::UserRole) {
    return m_data[index.row()]->at(rol - 1 - Qt::UserRole);
  }
  else {
    return QVariant();
  }
}

QHash<int, QByteArray> ConllListModel::roleNames() const {
  QHash<int, QByteArray> rn = QAbstractItemModel::roleNames();
  rn[ID] = "id";
  rn[FORM] = "form";
  rn[LEMMA] = "lemma";
  rn[CPOSTAG] = "cpostag";
  rn[POSTAG] = "postag";
  rn[NE] = "ne";
  rn[FEATS] = "feats";
  rn[HEAD] = "head";
  rn[DEPREL] = "deprel";
  rn[PHEAD] = "phead";
  rn[PDEPREL] = "pdeprel";
  return rn;
}

ConllListModel::~ConllListModel() {
  for (auto& d : m_data) {
    delete d;
  }
}
