// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Gael.de-Chalendar@cea.fr   *
 ***************************************************************************/

#include "annotationEditWidget.h"
#include "annoqt.h"

#include <QDebug>

AnnotationEditWidget::AnnotationEditWidget(Annoqt* parent) :
  QTextEdit(parent),
  m_parent(parent)
{
}

void AnnotationEditWidget::mousePressEvent ( QMouseEvent * event )
{
  QTextEdit::mousePressEvent(event);
  qDebug() << "AnnotationEditWidget::mousePressEvent cursor position: " << textCursor().position();
  if (event->button() ==  Qt::LeftButton
    && event->modifiers() != Qt::ControlModifier)
  {
    m_parent->selectEventAt(textCursor().position(), event->globalPos());
  }
}
