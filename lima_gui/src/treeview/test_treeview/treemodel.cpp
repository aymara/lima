#include "treeitem.h"
#include "treemodel.h"

#include <QStringList>

#include <iostream>

enum DEPTH {
  _0,
  _a,
  _b,
  _c,
  _d,
  _e
};

#define PUSHITEM(d, c1, c2) content.push_back(std::pair<std::pair<QString, QString>, int>(std::pair<QString, QString>(QString(c1), QString(c2)), d))

TreeModel::TreeModel(QObject* p) : TreeModel(QString(),p)
{
//  load("Connection Editing Mode                 Connecting widgets together with signals and slots\n       Connecting Objects                  Making connections in Qt Designer\n       Editing Connections                 Changing existing connections");
}

TreeModel::TreeModel(const QString &data, QObject *parent)
    : QAbstractItemModel(parent)
{
//  load(data);
  rootItem = new TreeItem(QList<QVariant>() << QString("Title") << QString("Summary"));

  std::map<int, TreeItem*> currentParents;

  currentParents[0] = rootItem;
  int depth = 0;

  std::vector <
      std::pair <
          std::pair <QString, QString>,
          int
      >
  > content;

  PUSHITEM(_a,"Connection Editing Mode", "Connecting widgets together");
  PUSHITEM(_b,"Launching Designer", "LD");
  PUSHITEM(_b,"The User Interface", "Yeah, the gui");
  PUSHITEM(_c,"Boom","Bam");
  PUSHITEM(_b,"A","A");
  PUSHITEM(_a,"ERER","ERER");

  for (auto& apair : content) {

    QList<QVariant> list;
    list << apair.first.first << apair.first.second;

    int& newdepth = apair.second;

    if (newdepth == depth) {
      TreeItem* ti = new TreeItem(list);
      currentParents[depth] = ti;
      if (depth > 0) {
        currentParents[depth - 1]->appendChild(ti);
        ti->setParent(currentParents[depth - 1]);
      }
      else {
        // replacing root
      }
    }
    else if (newdepth > depth) {
      TreeItem* ti = new TreeItem(list);
      currentParents[depth]->appendChild(ti);
      ti->setParent(currentParents[depth]);
      currentParents[depth+1] = ti;
      depth++;
    }
    else if (depth > 1 && newdepth > 0 && currentParents.find(newdepth) != currentParents.end()) {
        TreeItem* ti = new TreeItem(list);
        currentParents[newdepth] = ti;
        currentParents[newdepth-1]->appendChild(ti);
        ti->setParent(currentParents[newdepth-1]);
        depth = newdepth;
      }

  }

  rootItem->display();

}

void TreeModel::load(const QString& data)
{
  QList<QVariant> rootData;
  rootData << "Title" << "Summary";
  rootItem = new TreeItem(rootData);
  setupModelData(data.split(QString("\n")), rootItem);
}

TreeModel::~TreeModel()
{
    delete rootItem;
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Name && role != Desc)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

//    return item->data(index.column());
    return item->data(role - Qt::UserRole - 1);
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

void TreeModel::setupModelData(const QStringList &lines, TreeItem *parent)
{
    QList<TreeItem*> parents;
    QList<int> indentations;
    parents << parent;
    indentations << 0;

    int number = 0;

    while (number < lines.count()) {
        int position = 0;
        while (position < lines[number].length()) {
            if (lines[number].at(position) != ' ')
                break;
            position++;
        }

        QString lineData = lines[number].mid(position).trimmed();

        if (!lineData.isEmpty()) {
            // Read the column data from the rest of the line.
            QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
            QList<QVariant> columnData;
            for (int column = 0; column < columnStrings.count(); ++column)
                columnData << columnStrings[column];

            if (position > indentations.last()) {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.

                if (parents.last()->childCount() > 0) {
                    parents << parents.last()->child(parents.last()->childCount()-1);
                    indentations << position;
                }
            } else {
                while (position < indentations.last() && parents.count() > 0) {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            // Append a new item to the current parent's list of children.
            parents.last()->appendChild(new TreeItem(columnData, parents.last()));
        }

        ++number;
    }
}

QHash<int, QByteArray> TreeModel::roleNames() const {
  QHash<int, QByteArray> roles;

  roles[Name] = "name";
  roles[Desc] = "desc";
//  roles[Checked] = "checked";


  return roles;
}
