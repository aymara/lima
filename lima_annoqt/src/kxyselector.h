// Copyright (C) 1997 Martin Jones (mjones@kde.org)
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef KXYSELECTOR_H
#define KXYSELECTOR_H

#include <QtWidgets/QWidget>
#include <QtGui/QPixmap>

/**
 * KXYSelector is the base class for other widgets which
 * provides the ability to choose from a two-dimensional
 * range of values. The currently chosen value is indicated
 * by a cross. An example is the KHSSelector which
 * allows to choose from a range of colors, and which is
 * used in KColorDialog.
 *
 * A custom drawing routine for the widget surface has
 * to be provided by the subclass.
 */
class KXYSelector : public QWidget
{
  Q_OBJECT
  Q_PROPERTY( int xValue READ xValue WRITE setXValue )
  Q_PROPERTY( int yValue READ yValue WRITE setYValue )

public:
  /**
   * Constructs a two-dimensional selector widget which
   * has a value range of [0..100] in both directions.
   */
  explicit KXYSelector( QWidget *parent=0 );
  /**
   * Destructs the widget.
   */
  ~KXYSelector();

  /**
   * Sets the current values in horizontal and
   * vertical direction.
   * @param xPos the horizontal value
   * @param yPos the vertical value
   */
  void setValues( int xPos, int yPos );

  /**
   * Sets the current horizontal value
   * @param xPos the horizontal value
   */
  void setXValue( int xPos );

  /**
   * Sets the current vertical value
   * @param yPos the vertical value
   */
  void setYValue( int yPos );

  /**
   * Sets the range of possible values.
   */
  void setRange( int minX, int minY, int maxX, int maxY );

  /**
    * Sets the color used to draw the marker
   * @param col the color
    */
  void setMarkerColor( const QColor &col );

  /**
   * @return the current value in horizontal direction.
   */
  int xValue() const;
  /**
   * @return the current value in vertical direction.
   */
  int yValue() const;

  /**
   * @return the rectangle on which subclasses should draw.
   */
  QRect contentsRect() const;

  /**
   * Reimplemented to give the widget a minimum size
   */
  virtual QSize minimumSizeHint() const override;

Q_SIGNALS:
  /**
   * This signal is emitted whenever the user chooses a value,
   * e.g. by clicking with the mouse on the widget.
   */
  void valueChanged( int x, int y );

protected:
  /**
   * Override this function to draw the contents of the widget.
   * The default implementation does nothing.
   *
   * Draw within contentsRect() only.
   */
  virtual void drawContents( QPainter * );

  /**
   * Override this function to draw the marker which
   * indicates the currently selected value pair.
   */
  virtual void drawMarker( QPainter *p, int xp, int yp );

  virtual void paintEvent( QPaintEvent *e ) override;
  virtual void mousePressEvent( QMouseEvent *e ) override;
  virtual void mouseMoveEvent( QMouseEvent *e ) override;
  virtual void wheelEvent( QWheelEvent * ) override;

  /**
   * Converts a pixel position to its corresponding values.
   */
  void valuesFromPosition( int x, int y, int& xVal, int& yVal ) const;

private:
  void setPosition( int xp, int yp );

private:
  class Private;
  friend class Private;
  Private * const d;
  
  Q_DISABLE_COPY(KXYSelector)
};

#endif /* KXYSELECTOR_H */
