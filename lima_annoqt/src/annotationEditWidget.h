// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Gael.de-Chalendar@cea.fr   *
 ***************************************************************************/

#ifndef ANNOTATIONEDITWIDGET_H
#define ANNOTATIONEDITWIDGET_H

#include <QTextEdit>

class Annoqt;

class AnnotationEditWidget : public QTextEdit
{
Q_OBJECT

public:
  AnnotationEditWidget(Annoqt* parent);

  virtual ~AnnotationEditWidget() {}

protected:
  void mousePressEvent ( QMouseEvent * event ) override;
  Annoqt* m_parent;
};

#endif // ANNOTATIONEDITWIDGET_H
