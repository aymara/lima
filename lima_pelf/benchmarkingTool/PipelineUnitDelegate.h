// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_PIPELINEUNITDELEGATE_H
#define LIMA_PELF_PIPELINEUNITDELEGATE_H

#include <QtCore/QtDebug>
#include <QtWidgets/QItemDelegate>
#include <QtGui/QPainter>

#include "Pipeline.h"
#include "PipelineUnit.h"

namespace Lima {
namespace Pelf {

class PipelineUnitDelegate : public QItemDelegate
{

Q_OBJECT

public:

    Pipeline* pipeline;

    PipelineUnitDelegate (QObject* parent = 0, Pipeline* p = 0);
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_PIPELINEUNITDELEGATE_H
