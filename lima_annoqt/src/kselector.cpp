// Copyright (C) 1997 Martin Jones (mjones@kde.org)
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "kselector.h"

#include <QImage>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QStyle>
#include <QStyleOption>

#include <stdio.h>

//-----------------------------------------------------------------------------
/*
 * 1D value selector with contents drawn by derived class.
 * See KColorDialog for example.
 */

#define ARROWSIZE 5

class KSelector::Private
{
public:
    Private()
    {
        arrowPE = QStyle::PE_IndicatorArrowLeft;
        m_indent = true;
    }

    bool m_indent;
    QStyle::PrimitiveElement arrowPE;
};

class KGradientSelector::KGradientSelectorPrivate
{
public:
  KGradientSelectorPrivate(KGradientSelector *q): q(q) {}

  void init();

  KGradientSelector *q;
  QColor color1;
  QColor color2;
  QString text1;
  QString text2;
};

KSelector::KSelector( QWidget *parent )
  : QAbstractSlider( parent )
 , d(new Private)
{
    setOrientation(Qt::Horizontal);
}

KSelector::KSelector( Qt::Orientation o, QWidget *parent )
  : QAbstractSlider( parent )
 , d(new Private)
{
    setOrientation(o);
    if(o == Qt::Horizontal)
        setArrowDirection(Qt::UpArrow);
}

KSelector::~KSelector()
{
    delete d;
}

void KSelector::setIndent( bool i )
{
    d->m_indent = i;
}

bool KSelector::indent() const
{
    return d->m_indent;
}

QRect KSelector::contentsRect() const
{
    int w = indent() ? style()->pixelMetric( QStyle::PM_DefaultFrameWidth ) : 0;
    //TODO: is the height:width ratio of an indicator arrow always 2:1? hm.
    int iw = (w < ARROWSIZE) ? ARROWSIZE : w;

    if ( orientation() == Qt::Vertical ) {
        if ( arrowDirection() == Qt::RightArrow ) {
            return QRect( w + ARROWSIZE, iw,
                          width() - w*2 - ARROWSIZE,
                          height() - iw*2 );
        } else {
            return QRect( w, iw,
                          width() - w*2 - ARROWSIZE,
                          height() - iw*2 );
        }
    } else { // Qt::Horizontal
        if ( arrowDirection() == Qt::UpArrow ) {
            return QRect( iw, w,
                          width() - 2*iw,
                          height() - w*2 - ARROWSIZE );
        } else {
            return QRect( iw, w + ARROWSIZE,
                          width() - 2*iw,
                          height() - w*2 - ARROWSIZE );
        }
    }
}

void KSelector::paintEvent( QPaintEvent * )
{
  QPainter painter;
  int w = style()->pixelMetric( QStyle::PM_DefaultFrameWidth );
  int iw = (w < ARROWSIZE) ? ARROWSIZE : w;

  painter.begin( this );

  drawContents( &painter );

  QPoint pos = calcArrowPos( value() );
  drawArrow( &painter, pos );

  if ( indent() )
  {
    QStyleOptionFrame opt;
    opt.initFrom( this );
    opt.state = QStyle::State_Sunken;
    if ( orientation() == Qt::Vertical )
      opt.rect.adjust( 0, iw - w, -5, w - iw );
    else
      opt.rect.adjust(iw - w, 0, w - iw, -5);
    style()->drawPrimitive( QStyle::PE_Frame, &opt, &painter, this );
  }


  painter.end();
}

void KSelector::mousePressEvent( QMouseEvent *e )
{
    setSliderDown(true);
    moveArrow( e->pos() );
}

void KSelector::mouseMoveEvent( QMouseEvent *e )
{
  moveArrow( e->pos() );
}

void KSelector::mouseReleaseEvent( QMouseEvent *e )
{
    moveArrow( e->pos() );
    setSliderDown(false);
}

void KSelector::wheelEvent( QWheelEvent *e )
{
    int val = value() + e->delta()/120;
    setSliderDown(true);
    setValue( val );
    setSliderDown(false);
}

void KSelector::moveArrow( const QPoint &pos )
{
    int val;
    int w = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    int iw = (w < ARROWSIZE) ? ARROWSIZE : w;

    if ( orientation() == Qt::Vertical )
        val = ( maximum() - minimum() ) * (height() - pos.y() - iw)
            / (height() - iw * 2) + minimum();
    else
        val = ( maximum() - minimum() ) * ( pos.x() - iw)
            / (width() - iw * 2) + minimum();

    setValue( val );
    update();
}

QPoint KSelector::calcArrowPos( int val )
{
    QPoint p;
    int w = style()->pixelMetric( QStyle::PM_DefaultFrameWidth );
    int iw = ( w < ARROWSIZE ) ? ARROWSIZE : w;

    if ( orientation() == Qt::Vertical )
    {
        p.setY( height() - iw - 1 - (height() - 2 * iw - 1) * val  / ( maximum() - minimum() ) );

        if ( d->arrowPE == QStyle::PE_IndicatorArrowRight ) {
            p.setX( 0 );
        } else {
            p.setX( width() - 5 );
        }
    }
    else
    {
        p.setX( iw + (width() - 2 * iw - 1) * val  / ( maximum() - minimum() ) );

        if ( d->arrowPE == QStyle::PE_IndicatorArrowDown ) {
            p.setY( 0 );
        } else {
            p.setY( height() - 5 );
        }
    }

    return p;
}

void KSelector::setArrowDirection( Qt::ArrowType direction )
{
    switch ( direction ) {
        case Qt::UpArrow:
            if ( orientation() == Qt::Horizontal ) {
                d->arrowPE = QStyle::PE_IndicatorArrowUp;
            } else {
                d->arrowPE = QStyle::PE_IndicatorArrowLeft;
            }
            break;
        case Qt::DownArrow:
            if ( orientation() == Qt::Horizontal ) {
                d->arrowPE = QStyle::PE_IndicatorArrowDown;
            } else {
                d->arrowPE = QStyle::PE_IndicatorArrowRight;
            }
            break;
        case Qt::LeftArrow:
            if ( orientation() == Qt::Vertical ) {
                d->arrowPE = QStyle::PE_IndicatorArrowLeft;
            } else {
                d->arrowPE = QStyle::PE_IndicatorArrowDown;
            }
            break;
        case Qt::RightArrow:
            if ( orientation() == Qt::Vertical ) {
                d->arrowPE = QStyle::PE_IndicatorArrowRight;
            } else {
                d->arrowPE = QStyle::PE_IndicatorArrowUp;
            }
            break;

        case Qt::NoArrow:
            break;
    }
}

Qt::ArrowType KSelector::arrowDirection() const
{
    switch ( d->arrowPE ) {
        case QStyle::PE_IndicatorArrowUp:
            return Qt::UpArrow;
            break;
        case QStyle::PE_IndicatorArrowDown:
            return Qt::DownArrow;
            break;
        case QStyle::PE_IndicatorArrowRight:
            return Qt::RightArrow;
            break;
        case QStyle::PE_IndicatorArrowLeft:
        default:
            return Qt::LeftArrow;
            break;
    }
}

void KSelector::drawContents( QPainter * )
{}

void KSelector::drawArrow( QPainter *painter, const QPoint &pos )
{
    painter->setPen( QPen() );
    painter->setBrush( QBrush( palette().color(QPalette::ButtonText) ) );

    QStyleOption o;

    if ( orientation() == Qt::Vertical ) {
        o.rect = QRect( pos.x(), pos.y() - ARROWSIZE / 2,
                        ARROWSIZE, ARROWSIZE );
    } else {
        o.rect = QRect( pos.x() - ARROWSIZE / 2, pos.y(),
                        ARROWSIZE, ARROWSIZE );

    }
    style()->drawPrimitive( d->arrowPE, &o, painter, this );
}

//----------------------------------------------------------------------------

KGradientSelector::KGradientSelector( QWidget *parent )
    : KSelector( parent ), d(new KGradientSelectorPrivate(this))
{
    d->init();
}


KGradientSelector::KGradientSelector( Qt::Orientation o, QWidget *parent )
    : KSelector( o, parent ), d(new KGradientSelectorPrivate(this))
{
    d->init();
}


KGradientSelector::~KGradientSelector()
{
    delete d;
}


void KGradientSelector::KGradientSelectorPrivate::init()
{
    color1.setRgb( 0, 0, 0 );
    color2.setRgb( 255, 255, 255 );

    text1 = text2 = "";
}


void KGradientSelector::drawContents( QPainter *painter )
{
  QImage image( contentsRect().width(), contentsRect().height(), QImage::Format_RGB32 );

  QColor col;
  float scale;

  int redDiff   = d->color2.red() - d->color1.red();
  int greenDiff = d->color2.green() - d->color1.green();
  int blueDiff  = d->color2.blue() - d->color1.blue();

  if ( orientation() == Qt::Vertical )
  {
    for ( int y = 0; y < image.height(); y++ )
    {
      scale = 1.0 * y / image.height();
      col.setRgb( d->color1.red() + int(redDiff*scale),
            d->color1.green() + int(greenDiff*scale),
            d->color1.blue() + int(blueDiff*scale) );

      unsigned int *p = (uint *) image.scanLine( y );
      for ( int x = 0; x < image.width(); x++ )
        *p++ = col.rgb();
    }
  }
  else
  {
    unsigned int *p = (uint *) image.scanLine( 0 );

    for ( int x = 0; x < image.width(); x++ )
    {
      scale = 1.0 * x / image.width();
      col.setRgb( d->color1.red() + int(redDiff*scale),
            d->color1.green() + int(greenDiff*scale),
            d->color1.blue() + int(blueDiff*scale) );
      *p++ = col.rgb();
    }

    for ( int y = 1; y < image.height(); y++ )
      memcpy( image.scanLine( y ), image.scanLine( y - 1),
         sizeof( unsigned int ) * image.width() );
  }

  /*
  QColor ditherPalette[8];

  for ( int s = 0; s < 8; s++ )
    ditherPalette[s].setRgb( d->color1.red() + redDiff * s / 8,
                d->color1.green() + greenDiff * s / 8,
                d->color1.blue() + blueDiff * s / 8 );

  KImageEffect::dither( image, ditherPalette, 8 );
  */

  QPixmap p = QPixmap::fromImage(image);

  painter->drawPixmap( contentsRect().x(), contentsRect().y(), p );

  if ( orientation() == Qt::Vertical )
  {
    int yPos = contentsRect().top() + painter->fontMetrics().ascent() + 2;
    int xPos = contentsRect().left() + (contentsRect().width() -
       painter->fontMetrics().width( d->text2 )) / 2;
    QPen pen( d->color2 );
    painter->setPen( pen );
    painter->drawText( xPos, yPos, d->text2 );

    yPos = contentsRect().bottom() - painter->fontMetrics().descent() - 2;
    xPos = contentsRect().left() + (contentsRect().width() -
      painter->fontMetrics().width( d->text1 )) / 2;
    pen.setColor( d->color1 );
    painter->setPen( pen );
    painter->drawText( xPos, yPos, d->text1 );
  }
  else
  {
    int yPos = contentsRect().bottom()-painter->fontMetrics().descent()-2;

    QPen pen( d->color2 );
    painter->setPen( pen );
    painter->drawText( contentsRect().left() + 2, yPos, d->text1 );

    pen.setColor( d->color1 );
    painter->setPen( pen );
    painter->drawText( contentsRect().right() -
       painter->fontMetrics().width( d->text2 ) - 2, yPos, d->text2 );
  }
}

QSize KGradientSelector::minimumSize() const
{
    return sizeHint();
}

void KGradientSelector::setColors( const QColor &col1, const QColor &col2 )
{
  d->color1 = col1;
  d->color2 = col2;
  update();
}

void KGradientSelector::setText( const QString &t1, const QString &t2 )
{
  d->text1 = t1;
  d->text2 = t2;
  update();
}

void KGradientSelector::setFirstColor( const QColor &col )
{
  d->color1 = col;
  update();
}

void KGradientSelector::setSecondColor( const QColor &col )
{
  d->color2 = col;
  update();
}

void KGradientSelector::setFirstText( const QString &t )
{
  d->text1 = t;
  update();
}

void KGradientSelector::setSecondText( const QString &t )
{
  d->text2 = t;
  update();
}

QColor KGradientSelector::firstColor() const
{
  return d->color1;
}

QColor KGradientSelector::secondColor() const
{
  return d->color2;
}

QString KGradientSelector::firstText() const
{
  return d->text1;
}

QString KGradientSelector::secondText() const
{
  return d->text2;
}

// #include "kselector.moc"
