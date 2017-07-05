#include <QtDebug>
#include "tablemodel.h"

TableModel::TableModel(QObject *parent) :
QAbstractTableModel(parent)
{
}

int
TableModel::rowCount(const QModelIndex & /*parent*/) const
{
  qDebug() << "rowCount() called";
  return 5;
}

int
TableModel::columnCount(const QModelIndex & /* parent */) const
{
  qDebug() << "columnCount() called";
  return 3;
}

QHash<int, QByteArray>
TableModel::roleNames() const
{
  qDebug() << "roleNames() called";
  QHash<int, QByteArray> rn = QAbstractItemModel::roleNames();
  rn[RoleFoo] = "foo";
  rn[RoleBar] = "bar";
  rn[RoleBaz] = "baz";
  return rn;
}

QVariant
TableModel::data(const QModelIndex &index, int role) const
{
  QVariant ret = QVariant();
  qDebug() << "data() called" << index << role;
  if (index.isValid()) {
    switch(role) {
      case RoleFoo:
        ret.setValue(QString("Foo%1").arg(index.row()));
        break;
      case RoleBar:
        ret.setValue(QString("Bar%1").arg(index.row()));
        break;
      case RoleBaz:
        ret.setValue(QString("Baz%1").arg(index.row()));
        break;
      default:
        qDebug() << "data(Invalid Role!)" << index;
        break;
    }
  }
  return ret;
}
