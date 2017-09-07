#include <QStringList>

#include "treeitem.h"

#include <iostream>

TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent)
{
    m_parentItem = parent;
    m_itemData = data;
}

TreeItem::~TreeItem()
{
    qDeleteAll(m_childItems);
}

void TreeItem::appendChild(TreeItem *item)
{
    m_childItems.append(item);
}

TreeItem *TreeItem::child(int row)
{
    return m_childItems.value(row);
}

int TreeItem::childCount() const
{
    return m_childItems.count();
}

int TreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant TreeItem::data(int column) const
{
    return m_itemData.value(column);
}

TreeItem *TreeItem::parentItem()
{
    return m_parentItem;
}

int TreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

void TreeItem::display(int depth) {
  for (int i=0;i<depth;i++) std::cout << "#\t";
  std::cout<< *this << std::endl;
  for (auto& child : m_childItems) {
    if  (child) {
      child->display(depth + 1);
    }
  }
}

std::ostream& operator<<(std::ostream& o, const TreeItem& ti) {
  std::cout << "[";
  for (auto& var : ti.m_itemData) {
    std::cout << var.toString().toStdString() << ":";
  }
  std::cout << "](" << ti.m_childItems.size() << ")";
  return o;
}
