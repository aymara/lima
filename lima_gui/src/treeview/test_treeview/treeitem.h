// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>

#include <iostream>

class TreeItem
{
public:
    explicit TreeItem(const QList<QVariant> &data, TreeItem *parentItem = 0);
    ~TreeItem();

    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    TreeItem *parentItem();

    void setParent(TreeItem* t) {
      m_parentItem = t;
    }

    void display(int depth = 0);

    friend std::ostream& operator<<(std::ostream& o, const TreeItem&);

private:
    QList<TreeItem*> m_childItems;
    QList<QVariant> m_itemData;
    TreeItem *m_parentItem;
};

#endif // TREEITEM_H
