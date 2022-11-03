// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 * \file    ConllListModel.cpp
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 *
 */

#include "ConllListModel.h"

#include "LimaGuiCommon.h"

#include <iostream>


namespace Lima
{
namespace Gui
{

ConllRow::ConllRow(const std::string& s, QObject* p) : QObject(p), CONLL_Line(s)
{

}

ConllListModel::ConllListModel(QObject *p) : QAbstractTableModel(p)
{

}

ConllListModel::ConllListModel(const QString& content, QObject* p) : QAbstractTableModel(p)
{
  fromText(content);
}

ConllListModel::~ConllListModel()
{
  for (auto& d : m_data) {
    delete d;
  }
}

void ConllListModel::fromText(const QString& text)
{
  CONLLLOGINIT;
  LDEBUG << "ConllListModel::fromText" << text;

  beginResetModel();
  for (auto& d : m_data)
  {
    delete d;
  }
  m_data.clear();
  std::vector<std::string> data = into_lines(text.toStdString());

  for (unsigned int i=0; i<data.size(); i++)
  {
    if (!data[i].empty()) {
      m_data.push_back(new ConllRow(data[i]));
    }
  }
  endResetModel();
}

int ConllListModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_data.size();
}

int ConllListModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 3;
}

QVariant ConllListModel::data(const QModelIndex &index, int rol) const
{
  if ( rol > Qt::UserRole)
  {
    return m_data[index.row()]->at(rol - 1 - Qt::UserRole);
  }
  else
  {
    return QVariant();
  }
}

QHash<int, QByteArray> ConllListModel::roleNames() const
{
  QHash<int, QByteArray> rn = QAbstractItemModel::roleNames();
  rn[ID] = "id"; // Those strings are direclty related to the 'TableViewColumn' elements role property
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

} // Gui
} // Lima
