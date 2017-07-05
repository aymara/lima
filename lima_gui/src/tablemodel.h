#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>

class TableModel : public QAbstractTableModel
{
  Q_OBJECT
  
public:
  explicit TableModel(QObject *parent = 0);
  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QHash<int, QByteArray> roleNames() const;
  
private:
  enum {
    RoleFoo = Qt::UserRole + 1,
    RoleBar = Qt::UserRole + 2,
    RoleBaz = Qt::UserRole + 3,
  };
};

#endif //TABLEMODEL_H
