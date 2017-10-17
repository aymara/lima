/*
    Copyright 2017 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/
/**
 * \file    ConllListModel.h
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 * 
 */

#ifndef CONLL_LIST_MODEL_H
#define CONLL_LIST_MODEL_H

#include "LimaGuiExport.h"
#include "ConllParser.h"

#include <QObject>
#include <QAbstractTableModel>

namespace Lima 
{
namespace Gui 
{

/// \class ConllRow
/// \brief This is an extension of the CONLL_Line structure to Qt
class LIMA_GUI_EXPORT ConllRow : public QObject, public CONLL_Line 
{
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
class LIMA_GUI_EXPORT ConllListModel : public QAbstractTableModel 
{
  Q_OBJECT
public:
  ConllListModel(QObject* p = 0);
  ConllListModel(const QString&, QObject* p = 0);
  ~ConllListModel();

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int rol = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

  ///
  /// \brief convert the raw conll output into a data model suitable for Qt use
  /// \param raw is the conll output in raw text format
  Q_INVOKABLE void fromText(const QString& raw);

private:
  std::vector<ConllRow*> m_data;

  enum 
  {
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

} // END namespace Gui
} // END namespace Lima

#endif // CONLL_LIST_MODEL_H
