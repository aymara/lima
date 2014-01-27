/* This file is part of the KDE libraries
    Copyright (C) 1997 Martin Jones (mjones@kde.org)
    Copyright (C) 2007 Roberto Raggi (roberto@kdevelop.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
//-----------------------------------------------------------------------------
// KDE color selection dialog.
//
// 1999-09-27 Espen Sand <espensa@online.no>
// KColorDialog is now subclassed from QDialog. I have also extended
// KColorDialog::getColor() so that it contains a parent argument. This
// improves centering capability.
//
// layout management added Oct 1997 by Mario Weilguni
// <mweilguni@sime.com>
//

#include "kcolordialog.h"
#include "kcolordialog_p.h"


#include <stdio.h>
#include <stdlib.h>

#include <QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QDesktopWidget>
#include <QtGui/QRadioButton>
#include <QtGui/qdrawutil.h>
#include <QtGui/QActionEvent>
#include <QtCore/QFile>
#include <QtGui/QHeaderView>
#include <QtGui/QImage>
#include <QtGui/QItemDelegate>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QPainter>
#include <QtGui/QPushButton>
#include <QtGui/QScrollBar>
#include <QtGui/QDoubleSpinBox>
#include <QtCore/QTimer>
#include <QtGui/QDoubleValidator>
#include <QSettings>

// #include <kapplication.h>
// #include <kconfig.h>
// #include <kglobal.h>
// #include <kglobalsettings.h>
// #include <kiconloader.h>
#include <QLineEdit>
#include <QLocale>
#include <QMessageBox>
// #include <kseparator.h>

#include "kcolormimedata.h"
// #include <config.h>
#include <QDebug>

#include "kselector.h"
#include "kcolorvalueselector.h"
#include "kxyselector.h"
// #include <kconfiggroup.h>

#include <iostream>

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <QX11Info>
#endif

struct ColorCollectionNameType {
    const char* m_fileName;
    const char* m_displayName;
};

const ColorCollectionNameType colorCollectionName[] = {
    { "Recent_Colors", ("* Recent Colors *") },
    { "Custom_Colors", ("* Custom Colors *") },
    { "40.colors",     ("Forty Colors") },
    { "Oxygen.colors", ("Oxygen Colors") },
    { "Rainbow.colors", ("Rainbow Colors") },
    { "Royal.colors",  ("Royal Colors") },
    { "Web.colors",    ("Web Colors") },
    { 0, 0 } // end of data
};

static const int recentColorIndex = 0;
static const int customColorIndex = 1;
static const int fortyColorIndex = 2;


//-----------------------------------------------------------------------------

class KColorCells::KColorCellsPrivate
{
public:
    KColorCellsPrivate(KColorCells *q): q(q) {
        inMouse = false;
        selected = -1;
        shade = false;
    }

    KColorCells *q;
    QPoint mousePos;
    int selected;
    bool shade;
    bool inMouse;
};

KColorCells::KColorCells(QWidget *parent, int rows, int cols)
        : QTableWidget(parent), d(new KColorCellsPrivate(this))
{
    setItemDelegate(new QItemDelegate(this));

    setFrameShape(QFrame::NoFrame);
    d->shade = true;
    setRowCount(rows);
    setColumnCount(cols);

    verticalHeader()->hide();
    horizontalHeader()->hide();

    d->selected = 0;
    d->inMouse = false;

    // Drag'n'Drop
    setAcceptDrops(true);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    viewport()->setBackgroundRole(QPalette::Background);
    setBackgroundRole(QPalette::Background);


    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragEnabled(false);
}

KColorCells::~KColorCells()
{
    delete d;
}

QColor KColorCells::color(int index) const
{
    QTableWidgetItem * tmpItem = item(index / columnCount(), index % columnCount());

    if (tmpItem != 0)
        return tmpItem->data(Qt::BackgroundRole).value<QColor>();

    return QColor();
}

int KColorCells::count() const
{
    return rowCount() * columnCount();
}

void KColorCells::setShading(bool _shade)
{
    d->shade = _shade;
}

void KColorCells::setAcceptDrags(bool _acceptDrags)
{
    this->setDragEnabled(_acceptDrags);
}

void KColorCells::setSelected(int index)
{
    Q_ASSERT(index >= 0 && index < count());

    d->selected = index;
}

int KColorCells::selectedIndex() const
{
    return d->selected;
}

void KColorCells::setColor(int column, const QColor &color)
{
    const int tableRow = column / columnCount();
    const int tableColumn = column % columnCount();

    Q_ASSERT(tableRow >= 0 && tableRow < rowCount());
    Q_ASSERT(tableColumn >= 0 && tableColumn < columnCount());

    QTableWidgetItem * tableItem = item(tableRow, tableColumn);

    if (tableItem == 0) {
        tableItem = new QTableWidgetItem();
        setItem(tableRow, tableColumn, tableItem);
    }

    tableItem->setData(Qt::BackgroundRole , color);
}

/*void KColorCells::paintCell( QPainter *painter, int row, int col )
{
    painter->setRenderHint( QPainter::Antialiasing , true );

 QBrush brush;
 int w = 1;

 if (shade)
 {
  qDrawShadePanel( painter, 1, 1, cellWidth()-2,
       cellHeight()-2, palette(), true, 1, &brush );
  w = 2;
 }
 QColor color = colors[ row * numCols() + col ];
 if (!color.isValid())
 {
  if (!shade) return;
  color = palette().color(backgroundRole());
 }

 const QRect colorRect( w, w, cellWidth()-w*2, cellHeight()-w*2 );
 painter->fillRect( colorRect, color );

 if ( row * numCols() + col == selected ) {
  painter->setPen( qGray(color.rgb())>=127 ? Qt::black : Qt::white );
  painter->drawLine( colorRect.topLeft(), colorRect.bottomRight() );
  painter->drawLine( colorRect.topRight(), colorRect.bottomLeft() );
 }
}*/

void KColorCells::resizeEvent(QResizeEvent*)
{
    // According to the Qt doc:
    //   If you need to set the width of a given column to a fixed value, call
    //   QHeaderView::resizeSection() on the table's {horizontal,vertical}
    //   header.
    // Therefore we iterate over each row and column and set the header section
    // size, as the sizeHint does indeed appear to be ignored in favor of a
    // minimum size that is larger than what we want.
    for (int index = 0 ; index < columnCount() ; index++)
        horizontalHeader()->resizeSection(index, sizeHintForColumn(index));
    for (int index = 0 ; index < rowCount() ; index++)
        verticalHeader()->resizeSection(index, sizeHintForRow(index));
}

int KColorCells::sizeHintForColumn(int /*column*/) const
{
    return width() / columnCount() ;
}

int KColorCells::sizeHintForRow(int /*row*/) const
{
    return height() / rowCount() ;
}

void KColorCells::mousePressEvent(QMouseEvent *e)
{
    d->inMouse = true;
    d->mousePos = e->pos();

    QTableWidget::mousePressEvent(e);
}


int KColorCells::positionToCell(const QPoint &pos, bool ignoreBorders) const
{
    //TODO ignoreBorders not yet handled
    Q_UNUSED(ignoreBorders)

    QTableWidgetItem* tableItem = itemAt(pos);

    if (!tableItem)
        return -1;

    const int itemRow = row(tableItem);
    const int itemColumn = column(tableItem);
    int cell = itemRow * columnCount() + itemColumn;

    /*if (!ignoreBorders)
    {
       int border = 2;
       int x = pos.x() - col * cellWidth();
       int y = pos.y() - row * cellHeight();
       if ( (x < border) || (x > cellWidth()-border) ||
            (y < border) || (y > cellHeight()-border))
          return -1;
    }*/

    return cell;
}

void KColorCells::mouseMoveEvent(QMouseEvent *e)
{
    if (this->dragEnabled() || this->acceptDrops()) {
        if (!(e->buttons() & Qt::LeftButton)) return;

        if (d->inMouse) {
            int delay = 1;/*KGlobalSettings::dndEventDelay();*/
            if (e->x() > d->mousePos.x() + delay || e->x() < d->mousePos.x() - delay ||
                    e->y() > d->mousePos.y() + delay || e->y() < d->mousePos.y() - delay) {
                // Drag color object
                QTableWidgetItem * tableItem = itemAt(d->mousePos);

                if (tableItem) {
                    QVariant var = tableItem->data(Qt::BackgroundRole);
                    QColor tmpCol = var.value<QColor>();
                    if (tmpCol.isValid())
                        KColorMimeData::createDrag(tmpCol, this)->start();
                }
            }
        }
    } else
        QTableWidget::mouseMoveEvent(e);
}

void KColorCells::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "KColorCells::dragEnterEvent() acceptDrags="
    << this->dragEnabled()
    << " canDecode=" << KColorMimeData::canDecode(event->mimeData())
    << endl;
    event->setAccepted(this->dragEnabled() && KColorMimeData::canDecode(event->mimeData()));
}

// Reimplemented to override QTableWidget's override.  Else dropping doesn't work.
void KColorCells::dragMoveEvent(QDragMoveEvent *event)
{
    qDebug() << "KColorCells::dragMoveEvent() acceptDrags="
    << this->dragEnabled()
    << " canDecode=" << KColorMimeData::canDecode(event->mimeData())
    << endl;
    event->setAccepted(this->dragEnabled() && KColorMimeData::canDecode(event->mimeData()));
}

void KColorCells::dropEvent(QDropEvent *event)
{
    QColor c = KColorMimeData::fromMimeData(event->mimeData());

    qDebug() << "KColorCells::dropEvent() color.isValid=" << c.isValid();
    if (c.isValid()) {
        QTableWidgetItem * tableItem = itemAt(event->pos());

        if (tableItem)
            tableItem->setData(Qt::BackgroundRole , c);
    }
}

void KColorCells::mouseReleaseEvent(QMouseEvent *e)
{
    if (selectionMode() != QAbstractItemView::NoSelection) {
        int cell = positionToCell(d->mousePos);
        int currentCell = positionToCell(e->pos());

        // If we release the mouse in another cell and we don't have
        // a drag we should ignore this event.
        if (currentCell != cell)
            cell = -1;

        if ((cell != -1) && (d->selected != cell)) {
            d->selected = cell;

            const int newRow = cell / columnCount();
            const int newColumn = cell % columnCount();

            clearSelection(); // we do not want old violet selected cells

            item(newRow, newColumn)->setSelected(true);
        }

        d->inMouse = false;
        if (cell != -1)
            emit colorSelected(cell , color(cell));
    }

    QTableWidget::mouseReleaseEvent(e);
}

void KColorCells::mouseDoubleClickEvent(QMouseEvent * /*e*/)
{
    int cell = positionToCell(d->mousePos);

    if (cell != -1)
        emit colorDoubleClicked(cell , color(cell));
}


//-----------------------------------------------------------------------------

class KColorPatch::KColorPatchPrivate
{
public:
    KColorPatchPrivate(KColorPatch *q): q(q) {}

    KColorPatch *q;
    QColor color;
};

KColorPatch::KColorPatch(QWidget *parent) : QFrame(parent), d(new KColorPatchPrivate(this))
{
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    setAcceptDrops(true);
    setMinimumSize(12, 12);
}

KColorPatch::~KColorPatch()
{
    delete d;
}

void KColorPatch::setColor(const QColor &col)
{
    d->color.setRgb(col.rgb());

    update();
}

void KColorPatch::paintEvent(QPaintEvent* pe)
{
    QFrame::paintEvent(pe);
    QPainter painter(this);
    painter.setPen(d->color);
    painter.setBrush(QBrush(d->color));
    painter.drawRect(contentsRect());
}

void KColorPatch::mouseMoveEvent(QMouseEvent *e)
{
    // Drag color object
    if (!(e->buttons() & Qt::LeftButton))
        return;
    KColorMimeData::createDrag(d->color, this)->start();
}

void KColorPatch::dragEnterEvent(QDragEnterEvent *event)
{
    event->setAccepted(KColorMimeData::canDecode(event->mimeData()));
}

void KColorPatch::dropEvent(QDropEvent *event)
{
    QColor c = KColorMimeData::fromMimeData(event->mimeData());
    if (c.isValid()) {
        setColor(c);
        emit colorChanged(c);
    }
}

KColorTable::KColorTable(QWidget *parent, int minWidth, int cols)
        : QWidget(parent), d(new KColorTablePrivate(this))
{
    d->cells = 0;
    d->mPalette = 0;
    d->mMinWidth = minWidth;
    d->mCols = cols;
    d->i18n_namedColors  = tr("Named Colors");

    QStringList diskPaletteList = KColorCollection::installedCollections();
    QStringList paletteList;

    // We must replace the untranslated file names by translate names (of course only for KDE's standard palettes)
    for (int i = 0; colorCollectionName[i].m_fileName; ++i) {
        diskPaletteList.removeAll(colorCollectionName[i].m_fileName);
        paletteList.append(tr(colorCollectionName[i].m_displayName));
    }
    paletteList += diskPaletteList;
    paletteList.append(d->i18n_namedColors);

    QVBoxLayout *layout = new QVBoxLayout(this);

    d->combo = new QComboBox(this);
    d->combo->setEditable(false);
    d->combo->addItems(paletteList);
    layout->addWidget(d->combo);

    d->sv = new QScrollArea(this);
    QSize cellSize = QSize(d->mMinWidth, 120);
    d->sv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->sv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    QSize minSize = QSize(d->sv->verticalScrollBar()->sizeHint().width(), 0);
    minSize += QSize(d->sv->frameWidth() * 2, 0);
    minSize += QSize(cellSize);
    d->sv->setFixedSize(minSize);
    layout->addWidget(d->sv);

    d->mNamedColorList = new QListWidget(this);
    d->mNamedColorList->setObjectName("namedColorList");
    d->mNamedColorList->setFixedSize(minSize);
    d->mNamedColorList->hide();
    layout->addWidget(d->mNamedColorList);
    connect(d->mNamedColorList, SIGNAL(currentTextChanged(const QString &)),
            this, SLOT(slotColorTextSelected(const QString &)));

    setFixedSize(sizeHint());
    connect(d->combo, SIGNAL(activated(const QString &)),
            this, SLOT(slotSetColors(const QString &)));
}

KColorTable::~KColorTable()
{
    delete d->mPalette;
    delete d;
}

QString
KColorTable::name() const
{
    return d->combo->currentText();
}


static const char * const *namedColorFilePath(void)
{
    //
    // 2000-02-05 Espen Sand.
    // Add missing filepaths here. Make sure the last entry is 0!
    //
    static const char * const path[] = {
#ifdef X11_RGBFILE
        X11_RGBFILE,
#endif
        "/usr/share/X11/rgb.txt",
        "/usr/X11R6/lib/X11/rgb.txt",
        "/usr/openwin/lib/X11/rgb.txt", // for Solaris.
        0
    };
    return path;
}




void KColorTable::readNamedColor(void)
{
  if (d->mNamedColorList->count() != 0) {
      return; // Strings already present
  }

  QString filename = QString(":/rgb.txt");
  if (filename.isEmpty()) return;

  QFile paletteFile(filename);

  if (!paletteFile.open(QIODevice::ReadOnly))
  {
      return;
  }

  QByteArray line;
  QStringList list;
  while (!paletteFile.atEnd())
  {
      line = paletteFile.readLine();

      int red, green, blue;
      int pos = 0;

      if (sscanf(line, "%d %d %d%n", &red, &green, &blue, &pos) == 3) {
          //
          // Remove duplicates. Every name with a space and every name
          // that start with "gray".
          //
          QString name = line.mid(pos).trimmed();
          QByteArray s1 = line.mid(pos);
          if (name.isNull() || name.indexOf(' ') != -1 ||
                  name.indexOf("gray") != -1 ||  name.indexOf("grey") != -1) {
              continue;
          }

          const QColor color(red, green, blue);
          if (color.isValid()) {
              const QString colorName(tr(name.toLatin1().data()));
              list.append(colorName);
              d->m_namedColorMap[ colorName ] = color;
          }
      }
  }

  list.sort();
  d->mNamedColorList->addItems(list);

  if (d->mNamedColorList->count() == 0) {
      //
      // Give the error dialog box a chance to center above the
      // widget (or dialog). If we had displayed it now we could get a
      // situation where the (modal) error dialog box pops up first
      // preventing the real dialog to become visible until the
      // error dialog box is removed (== bad UI).
      //
      QTimer::singleShot(10, this, SLOT(slotShowNamedColorReadError()));
  }
}


void
KColorTable::KColorTablePrivate::slotShowNamedColorReadError(void)
{
    if (mNamedColorList->count() == 0) {
        QString msg = tr(""
                           "Unable to read X11 RGB color strings. The following "
                           "file location(s) were examined:\n");

        const char * const *path = namedColorFilePath();
        for (int i = 0; path[i]; ++i) {
            msg += path[i];
            msg += '\n';
        }
        QMessageBox::warning(0,"warning", msg);
    }
}


//
// 2000-02-12 Espen Sand
// Set the color in two steps. The setColors() slot will not emit a signal
// with the current color setting. The reason is that setColors() is used
// by the color selector dialog on startup. In the color selector dialog
// we normally want to display a startup color which we specify
// when the dialog is started. The slotSetColors() slot below will
// set the palette and then use the information to emit a signal with the
// new color setting. It is only used by the combobox widget.
//
void
KColorTable::KColorTablePrivate::slotSetColors(const QString &_collectionName)
{
    q->setColors(_collectionName);
    if (mNamedColorList->count() && mNamedColorList->isVisible()) {
        int item = mNamedColorList->currentRow();
        mNamedColorList->setCurrentRow(item < 0 ? 0 : item);
        slotColorTextSelected(mNamedColorList->currentItem()->text());
    } else {
        slotColorCellSelected(0, QColor()); // FIXME: We need to save the current value!!
    }
}


void
KColorTable::setColors(const QString &_collectionName)
{
    QString collectionName(_collectionName);

    if (d->combo->currentText() != collectionName) {
        bool found = false;
        for (int i = 0; i < d->combo->count(); i++) {
            if (d->combo->itemText(i) == collectionName) {
                d->combo->setCurrentIndex(i);
                found = true;
                break;
            }
        }
        if (!found) {
            d->combo->addItem(collectionName);
            d->combo->setCurrentIndex(d->combo->count() - 1);
        }
    }

    // We must again find the file name of the palette from the eventual translation
    for (int i = 0; colorCollectionName[i].m_fileName; ++i) {
        if (collectionName == tr(colorCollectionName[i].m_displayName)) {
            collectionName = colorCollectionName[i].m_fileName;
            break;
        }
    }


    //
    // 2000-02-12 Espen Sand
    // The palette mode "i18n_namedColors" does not use the KColorCollection
    // class. In fact, 'mPalette' and 'cells' are 0 when in this mode. The reason
    // for this is maninly that KColorCollection reads from and writes to files
    // using "locate()". The colors used in "i18n_namedColors" mode comes from
    // the X11 diretory and is not writable. I don't think this fit in
    // KColorCollection.
    //
    if (!d->mPalette || d->mPalette->name() != collectionName) {
        if (collectionName == d->i18n_namedColors) {
            d->sv->hide();
            d->mNamedColorList->show();
            readNamedColor();

            delete d->cells; d->cells = 0;
            delete d->mPalette; d->mPalette = 0;
        } else {
            d->mNamedColorList->hide();
            d->sv->show();

            delete d->cells;
            delete d->mPalette;
            d->mPalette = new KColorCollection(collectionName);
            int rows = (d->mPalette->count() + d->mCols - 1) / d->mCols;
            if (rows < 1) rows = 1;
            d->cells = new KColorCells(d->sv->viewport(), rows, d->mCols);
            d->cells->setShading(false);
            d->cells->setAcceptDrags(false);
            QSize cellSize = QSize(d->mMinWidth, d->mMinWidth * rows / d->mCols);
            d->cells->setFixedSize(cellSize);
            for (int i = 0; i < d->mPalette->count(); i++) {
                d->cells->setColor(i, d->mPalette->color(i));
            }
            connect(d->cells, SIGNAL(colorSelected(int , const QColor&)),
                    SLOT(slotColorCellSelected(int , const QColor&)));
            connect(d->cells, SIGNAL(colorDoubleClicked(int , const QColor&)),
                    SLOT(slotColorCellDoubleClicked(int , const QColor&)));
            d->sv->setWidget(d->cells);
            d->cells->show();

            //d->sv->updateScrollBars();
        }
    }
}



void
KColorTable::KColorTablePrivate::slotColorCellSelected(int index , const QColor& /*color*/)
{
    if (!mPalette || (index >= mPalette->count()))
        return;
    emit q->colorSelected(mPalette->color(index), mPalette->name(index));
}

void
KColorTable::KColorTablePrivate::slotColorCellDoubleClicked(int index , const QColor& /*color*/)
{
    if (!mPalette || (index >= mPalette->count()))
        return;
    emit q->colorDoubleClicked(mPalette->color(index), mPalette->name(index));
}


void
KColorTable::KColorTablePrivate::slotColorTextSelected(const QString &colorText)
{
    emit q->colorSelected(m_namedColorMap[ colorText ], colorText);
}


void
KColorTable::addToCustomColors(const QColor &color)
{
    setColors(tr(colorCollectionName[customColorIndex].m_displayName));
    d->mPalette->addColor(color);
    d->mPalette->save();
    delete d->mPalette;
    d->mPalette = 0;
    setColors(tr(colorCollectionName[customColorIndex].m_displayName));
}

void
KColorTable::addToRecentColors(const QColor &color)
{
    //
    // 2000-02-12 Espen Sand.
    // The 'mPalette' is always 0 when current mode is i18n_namedColors
    //
    bool recentIsSelected = false;
    if (d->mPalette && d->mPalette->name() == colorCollectionName[ recentColorIndex ].m_fileName) {
        delete d->mPalette;
        d->mPalette = 0;
        recentIsSelected = true;
    }
    KColorCollection *recentPal = new KColorCollection(colorCollectionName[ recentColorIndex ].m_fileName);
    if (recentPal->findColor(color) == -1) {
        recentPal->addColor(color);
        recentPal->save();
    }
    delete recentPal;
    if (recentIsSelected)
        setColors(tr(colorCollectionName[ recentColorIndex ].m_displayName));
}

class KCDPickerFilter;

#ifdef Q_WS_X11
class KCDPickerFilter: public QWidget
{
public:
    KCDPickerFilter(QWidget* parent): QWidget(parent) {}

    virtual bool x11Event(XEvent* event) {
        if (event->type == ButtonRelease) {
            QMouseEvent e(QEvent::MouseButtonRelease, QPoint(),
                          QPoint(event->xmotion.x_root, event->xmotion.y_root) , Qt::NoButton, Qt::NoButton, Qt::NoModifier);
            QApplication::sendEvent(parentWidget(), &e);
            return true;
        } else return false;
    }
};

#endif


KColorDialog::KColorDialog(QWidget *parent)
        : QWidget(parent), d(new KColorDialogPrivate(this))
{
//     setCaption(tr("Select Color"));
//     setButtons(modal ? Ok | Cancel : Close);
//     showButtonSeparator(true);
//     setModal(modal);
    d->bRecursion = true;
    d->bColorPicking = false;
#ifdef Q_WS_X11
    d->filter = 0;
#endif
    d->cbDefaultColor = 0L;
    d->_mode = ChooserClassic;

    QLabel *label;

    QGridLayout *tl_layout = new QGridLayout(this);
    tl_layout->setMargin(0);
    tl_layout->setSpacing(1);
    d->tl_layout = tl_layout;
    tl_layout->addItem(new QSpacerItem(1*2, 0), 0, 1);

    //
    // the more complicated part: the left side
    // add a V-box
    //
    QVBoxLayout *l_left = new QVBoxLayout();
    tl_layout->addLayout(l_left, 0, 0);

    //
    // add a H-Box for the XY-Selector and a grid for the
    // entry fields
    //
    QHBoxLayout *l_ltop = new QHBoxLayout();
    l_left->addLayout(l_ltop);

    // a little space between
    l_left->addSpacing(10);

    QGridLayout *l_lbot = new QGridLayout();
    l_left->addLayout(l_lbot);

    //
    // the palette and value selector go into the H-box
    //
    d->hsSelector = new KHueSaturationSelector(this);
    d->hsSelector->setMinimumSize(256, 256);
    l_ltop->addWidget(d->hsSelector, 8);
    connect(d->hsSelector, SIGNAL(valueChanged(int, int)),
            SLOT(slotHSChanged(int, int)));

    d->valuePal = new KColorValueSelector(this);
    d->valuePal->setMinimumSize(26, 70);
    d->valuePal->setIndent(false);
    d->valuePal->setArrowDirection(Qt::RightArrow);
    l_ltop->addWidget(d->valuePal, 1);
    connect(d->valuePal, SIGNAL(valueChanged(int)),
            SLOT(slotVChanged(int)));

    //
    // add the HSV fields
    //
    l_lbot->setColumnStretch(2, 10);

    d->hmode = new QRadioButton(tr("Hue:"), this);
    l_lbot->addWidget(d->hmode, 0, 0);

    d->hedit = new KColorSpinBox(0, 359, 1, this);
    l_lbot->addWidget(d->hedit, 0, 1);
    connect(d->hedit, SIGNAL(valueChanged(int)),
            SLOT(slotHSVChanged()));
    connect(d->hmode, SIGNAL(clicked()),
            SLOT(setHMode()));

    d->smode = new QRadioButton(tr("Saturation:"), this);
    l_lbot->addWidget(d->smode, 1, 0);

    d->sedit = new KColorSpinBox(0, 255, 1, this);
    l_lbot->addWidget(d->sedit, 1, 1);
    connect(d->sedit, SIGNAL(valueChanged(int)),
            SLOT(slotHSVChanged()));
    connect(d->smode, SIGNAL(clicked()),
            SLOT(setSMode()));

    d->vmode = new QRadioButton(tr("Value:"), this);
    l_lbot->addWidget(d->vmode, 2, 0);

    d->vedit = new KColorSpinBox(0, 255, 1, this);
    l_lbot->addWidget(d->vedit, 2, 1);
    connect(d->vedit, SIGNAL(valueChanged(int)),
            SLOT(slotHSVChanged()));
    connect(d->vmode, SIGNAL(clicked()),
            SLOT(setVMode()));


    //
    // add the RGB fields
    //
    d->rmode = new QRadioButton(tr("Red:"), this);
    l_lbot->addWidget(d->rmode, 0, 3);
    d->redit = new KColorSpinBox(0, 255, 1, this);
    l_lbot->addWidget(d->redit, 0, 4);
    connect(d->redit, SIGNAL(valueChanged(int)),
            SLOT(slotRGBChanged()));
    connect(d->rmode, SIGNAL(clicked()),
            SLOT(setRMode()));

            d->gmode = new QRadioButton(tr("Green:"), this);
    l_lbot->addWidget(d->gmode, 1, 3);

    d->gedit = new KColorSpinBox(0, 255, 1, this);
    l_lbot->addWidget(d->gedit, 1, 4);
    connect(d->gedit, SIGNAL(valueChanged(int)),
            SLOT(slotRGBChanged()));
    connect(d->gmode, SIGNAL(clicked()),
            SLOT(setGMode()));

            d->bmode = new QRadioButton(tr("Blue:"), this);
    l_lbot->addWidget(d->bmode, 2, 3);

    d->bedit = new KColorSpinBox(0, 255, 1, this);
    l_lbot->addWidget(d->bedit, 2, 4);
    connect(d->bedit, SIGNAL(valueChanged(int)),
            SLOT(slotRGBChanged()));
    connect(d->bmode, SIGNAL(clicked()),
            SLOT(setBMode()));

    //
    // the entry fields should be wide enough to hold 8888888
    //
    int w = d->hedit->fontMetrics().width("8888888");
    d->hedit->setFixedWidth(w);
    d->sedit->setFixedWidth(w);
    d->vedit->setFixedWidth(w);

    d->redit->setFixedWidth(w);
    d->gedit->setFixedWidth(w);
    d->bedit->setFixedWidth(w);

    //
    // add a layout for the right side
    //
    d->l_right = new QVBoxLayout;
    tl_layout->addLayout(d->l_right, 0, 2);

    //
    // Add the palette table
    //
    d->table = new KColorTable(this);
    d->l_right->addWidget(d->table, 10);

    connect(d->table, SIGNAL(colorSelected(const QColor &, const QString &)),
            SLOT(slotColorSelected(const QColor &, const QString &)));

    connect(
        d->table,
        SIGNAL(colorDoubleClicked(const QColor &, const QString &)),
        SLOT(slotColorDoubleClicked(const QColor &, const QString &))
    );
    // Store the default value for saving time.
    d->originalPalette = d->table->name();

    //
    // a little space between
    //
    d->l_right->addSpacing(10);

    QHBoxLayout *l_hbox = new QHBoxLayout();
    d->l_right->addItem(l_hbox);

    //
    // The add to custom colors button
    //
    QPushButton *addButton = new QPushButton(this);
    addButton->setText(tr("&Add to Custom Colors"));
    l_hbox->addWidget(addButton, 0, Qt::AlignLeft);
    connect(addButton, SIGNAL(clicked()), SLOT(slotAddToCustomColors()));

    //
    // The color picker button
    //
    QPushButton* button = new QPushButton(this);
    button->setIcon(QIcon("color-picker"));
    int commonHeight = addButton->sizeHint().height();
    button->setFixedSize(commonHeight, commonHeight);
    l_hbox->addWidget(button, 0, Qt::AlignHCenter);
    connect(button, SIGNAL(clicked()), SLOT(slotColorPicker()));

    //
    // a little space between
    //
    d->l_right->addSpacing(10);

    //
    // and now the entry fields and the patch (=colored box)
    //
    QGridLayout *l_grid = new QGridLayout();
    d->l_right->addLayout(l_grid);

    l_grid->setColumnStretch(2, 1);

    label = new QLabel(this);
    label->setText(tr("Name:"));
    l_grid->addWidget(label, 0, 1, Qt::AlignLeft);

    d->colorName = new QLabel(this);
    l_grid->addWidget(d->colorName, 0, 2, Qt::AlignLeft);

    label = new QLabel(this);
    label->setText(tr("HTML:"));
    l_grid->addWidget(label, 1, 1, Qt::AlignLeft);

    d->htmlName = new QLineEdit(this);
    d->htmlName->setMaxLength(13);   // Qt's QColor allows 12 hexa-digits
    d->htmlName->setText("#FFFFFF"); // But HTML uses only 6, so do not worry about the size
    w = d->htmlName->fontMetrics().width(QLatin1String("#DDDDDDD"));
    d->htmlName->setFixedWidth(w);
    l_grid->addWidget(d->htmlName, 1, 2, Qt::AlignLeft);

    connect(d->htmlName, SIGNAL(textChanged(const QString &)),
            SLOT(slotHtmlChanged()));

            d->patch = new KColorPatch(this);
    d->patch->setFixedSize(48, 48);
    l_grid->addWidget(d->patch, 0, 0, 2, 1, Qt::AlignHCenter | Qt::AlignVCenter);
    connect(d->patch, SIGNAL(colorChanged(const QColor&)),
            SLOT(setColor(const QColor&)));

    tl_layout->activate();

    readSettings();
    d->bRecursion = false;
    d->bEditHsv = false;
    d->bEditRgb = false;
    d->bEditHtml = false;

    setFixedSize(sizeHint());
    QColor col;
    col.setHsv(0, 0, 255);
    d->_setColor(col);

// FIXME: with enabled event filters, it crashes after ever enter of a drag.
// better disable drag and drop than crashing it...
//   d->htmlName->installEventFilter(this);
//   d->hsSelector->installEventFilter(this);
    d->hsSelector->setAcceptDrops(true);

    d->setVMode();
}

KColorDialog::~KColorDialog()
{
/*#ifdef Q_WS_X11
    if (d->bColorPicking && kapp)
        kapp->removeX11EventFilter(d->filter);
#endif*/
    delete d;
}

bool
KColorDialog::eventFilter(QObject *obj, QEvent *ev)
{
    if ((obj == d->htmlName) || (obj == d->hsSelector))
        switch (ev->type()) {
        case QEvent::DragEnter:
        case QEvent::DragMove:
        case QEvent::DragLeave:
        case QEvent::Drop:
        case QEvent::DragResponse:
            qApp->sendEvent(d->patch, ev);
            return true;
        default:
            break;
        }
    return QWidget::eventFilter(obj, ev);
}

void
KColorDialog::setDefaultColor(const QColor& col)
{
    if (!d->cbDefaultColor) {
        //
        // a little space between
        //
        d->l_right->addSpacing(10);

        //
        // and the "default color" checkbox, under all items on the right side
        //
        d->cbDefaultColor = new QCheckBox(tr("Default color"), this);

        d->l_right->addWidget(d->cbDefaultColor);

        d->tl_layout->activate();

        connect(d->cbDefaultColor, SIGNAL(clicked()), SLOT(slotDefaultColorClicked()));
    }

    d->defaultColor = col;

    d->slotDefaultColorClicked();
}

QColor KColorDialog::defaultColor() const
{
    return d->defaultColor;
}


void KColorDialog::KColorDialogPrivate::setChooserMode(KColorChooserMode c)
{
    _mode = c;
    hsSelector->setChooserMode(c);
    valuePal->setChooserMode(c);

    updateModeButtons();
    valuePal->updateContents();
    hsSelector->updateContents();
    valuePal->repaint();
    hsSelector->repaint();
    slotHSVChanged();
}



void
KColorDialog::KColorDialogPrivate::updateModeButtons()
{
    hmode->setChecked(false);
    smode->setChecked(false);
    vmode->setChecked(false);
    rmode->setChecked(false);
    gmode->setChecked(false);
    bmode->setChecked(false);

    switch (valuePal->chooserMode()) {
    case ChooserHue:
        hmode->setChecked(true);
        break;
    case ChooserSaturation:
        smode->setChecked(true);
        break;
    case ChooserRed:
        rmode->setChecked(true);
        break;
    case ChooserGreen:
        gmode->setChecked(true);
        break;
    case ChooserBlue:
        bmode->setChecked(true);
        break;
    case ChooserValue:
    default:
        vmode->setChecked(true);
        break;
    }
}

KColorChooserMode KColorDialog::KColorDialogPrivate::chooserMode()
{
    return _mode;
}

void KColorDialog::KColorDialogPrivate::slotDefaultColorClicked()
{
    if (cbDefaultColor->isChecked()) {
        selColor = defaultColor;
        showColor(selColor, tr("-default-"));
    } else {
        showColor(selColor, QString());
    }
    emit q->colorSelected(selColor);
}

void
KColorDialog::KColorDialogPrivate::setHMode()
{
    setChooserMode(ChooserHue);
}

void
KColorDialog::KColorDialogPrivate::setSMode()
{
    setChooserMode(ChooserSaturation);
}

void
KColorDialog::KColorDialogPrivate::setVMode()
{
    setChooserMode(ChooserValue);
}

void
KColorDialog::KColorDialogPrivate::setRMode()
{
    setChooserMode(ChooserRed);
}

void
KColorDialog::KColorDialogPrivate::setGMode()
{
    setChooserMode(ChooserGreen);

}

void
KColorDialog::KColorDialogPrivate::setBMode()
{
    setChooserMode(ChooserBlue);
}

void
KColorDialog::readSettings()
{
  QSettings settings( "LIMA", "Annotation Configuration Tool" );
//   KConfigGroup group(KGlobal::config(), "Colors");

  QString collectionName = settings.value( "CurrentPalette", "Forty Colors").toString();
//   group.readEntry("CurrentPalette");
    if (collectionName.isEmpty())
        collectionName = tr(colorCollectionName[fortyColorIndex].m_displayName);

    d->table->setColors(collectionName);
}

void
KColorDialog::KColorDialogPrivate::slotWriteSettings()
{
  QSettings settings( "LIMA", "Annotation Configuration Tool" );
  
//   KConfigGroup group(KGlobal::config(), "Colors");

    QString collectionName = table->name();
    settings.setValue( "CurrentPalette", collectionName );
}

QColor
KColorDialog::color() const
{
    if (d->cbDefaultColor && d->cbDefaultColor->isChecked())
        return QColor();
    if (d->selColor.isValid())
        d->table->addToRecentColors(d->selColor);
    return d->selColor;
}

void KColorDialog::setColor(const QColor &col)
{
    d->_setColor(col);
}

//
// static function to display dialog and return color
//
// int KColorDialog::getColor(QColor &theColor, QWidget *parent)
// {
//     KColorDialog dlg(parent, true);
//     dlg.setObjectName("Color Selector");
//     if (theColor.isValid())
//         dlg.setColor(theColor);
//     int result = dlg.exec();
// 
//     if (result == Accepted) {
//         theColor = dlg.color();
//     }
// 
//     return result;
// }

//
// static function to display dialog and return color
//
// int KColorDialog::getColor(QColor &theColor, const QColor& defaultCol, QWidget *parent)
// {
//     KColorDialog dlg(parent, true);
//     dlg.setObjectName("Color Selector");
//     dlg.setDefaultColor(defaultCol);
//     dlg.setColor(theColor);
//     int result = dlg.exec();
// 
//     if (result == Accepted)
//         theColor = dlg.color();
// 
//     return result;
// }

void KColorDialog::KColorDialogPrivate::slotRGBChanged(void)
{
    if (bRecursion) return;
    int red = redit->value();
    int grn = gedit->value();
    int blu = bedit->value();

    if (red > 255 || red < 0) return;
    if (grn > 255 || grn < 0) return;
    if (blu > 255 || blu < 0) return;

    QColor col;
    col.setRgb(red, grn, blu);
    bEditRgb = true;
    _setColor(col);
    bEditRgb = false;
}

void KColorDialog::KColorDialogPrivate::slotHtmlChanged(void)
{
    if (bRecursion || htmlName->text().isEmpty()) return;

    QString strColor(htmlName->text());

    // Assume that a user does not want to type the # all the time
    if (strColor[0] != '#') {
        bool signalsblocked = htmlName->blockSignals(true);
        strColor.prepend("#");
        htmlName->setText(strColor);
        htmlName->blockSignals(signalsblocked);
    }

    const QColor color(strColor);

    if (color.isValid()) {
        QColor col(color);
        bEditHtml = true;
        _setColor(col);
        bEditHtml = false;
    }
}

void KColorDialog::KColorDialogPrivate::slotHSVChanged(void)
{
    if (bRecursion) return;
    int hue = hedit->value();
    int sat = sedit->value();
    int val = vedit->value();

    if (hue > 359 || hue < 0) return;
    if (sat > 255 || sat < 0) return;
    if (val > 255 || val < 0) return;

    QColor col;
    col.setHsv(hue, sat, val);
    bEditHsv = true;
    _setColor(col);
    bEditHsv = false;
}

void KColorDialog::KColorDialogPrivate::slotHSChanged(int x, int y)
{
    int _h, _s, _v, _r, _g, _b;

    _h = selColor.hue();
    _s = selColor.saturation();
    _v = selColor.value();
    _r = selColor.red();
    _g = selColor.green();
    _b = selColor.blue();

    QColor col;

    switch (chooserMode()) {
    case ChooserRed:
        col.setRgb(_r, x, y);
        break;
    case ChooserGreen:
        col.setRgb(x, _g, y);
        break;
    case ChooserBlue:
        col.setRgb(y, x, _b);
        break;
    case ChooserHue:
        col.setHsv(_h, x, y);
        break;
    case ChooserSaturation:
        col.setHsv(x, _s, y);
        break;
    case ChooserValue:
    default:
        col.setHsv(x, y, _v);
        break;
    }
    _setColor(col);
}

void KColorDialog::KColorDialogPrivate::slotVChanged(int v)
{
    int _h, _s, _v, _r, _g, _b;

    _h = selColor.hue();
    _s = selColor.saturation();
    _v = selColor.value();
    _r = selColor.red();
    _g = selColor.green();
    _b = selColor.blue();


    QColor col;

    switch (chooserMode()) {
    case ChooserHue:
        col.setHsv(v, _s, _v);
        break;
    case ChooserSaturation:
        col.setHsv(_h, v, _v);
        break;
    case ChooserRed:
        col.setRgb(v, _g, _b);
        break;
    case ChooserGreen:
        col.setRgb(_r, v, _b);
        break;
    case ChooserBlue:
        col.setRgb(_r, _g, v);
        break;
    case ChooserValue:
    default:
        col.setHsv(_h, _s, v);
        break;
    }

    _setColor(col);
}

void KColorDialog::KColorDialogPrivate::slotColorSelected(const QColor &color)
{
    _setColor(color);
}

void KColorDialog::KColorDialogPrivate::slotAddToCustomColors()
{
    table->addToCustomColors(selColor);
}

void KColorDialog::KColorDialogPrivate::slotColorSelected(const QColor &color, const QString &name)
{
    _setColor(color, name);
}

void KColorDialog::KColorDialogPrivate::slotColorDoubleClicked
(
    const QColor  & color,
    const QString & name
)
{
    _setColor(color, name);
//     q->accept();
}

void KColorDialog::KColorDialogPrivate::_setColor(const QColor &color, const QString &name)
{
    if (color.isValid()) {
        if (cbDefaultColor && cbDefaultColor->isChecked())
            cbDefaultColor->setChecked(false);
        selColor = color;
    } else {
        if (cbDefaultColor && cbDefaultColor->isChecked())
            cbDefaultColor->setChecked(true);
        selColor = defaultColor;
    }

    showColor(selColor, name);

    emit q->colorSelected(selColor);
}

// show but don't set into selColor, nor emit colorSelected
void KColorDialog::KColorDialogPrivate::showColor(const QColor &color, const QString &name)
{
    bRecursion = true;

    if (name.isEmpty())
        colorName->setText(tr("-unnamed-"));
    else
        colorName->setText(name);

    patch->setColor(color);

    setRgbEdit(color);
    setHsvEdit(color);
    setHtmlEdit(color);


    switch (chooserMode()) {
    case ChooserSaturation:
        hsSelector->setValues(color.hue(), color.value());
        valuePal->setValue(color.saturation());
        break;
    case ChooserValue:
        hsSelector->setValues(color.hue(), color.saturation());
        valuePal->setValue(color.value());
        break;
    case ChooserRed:
        hsSelector->setValues(color.green(), color.blue());
        valuePal->setValue(color.red());
        break;
    case ChooserGreen:
        hsSelector->setValues(color.red(), color.blue());
        valuePal->setValue(color.green());
        break;
    case ChooserBlue:
        hsSelector->setValues(color.green(), color.red());
        valuePal->setValue(color.blue());
        break;
    case ChooserHue:
    default:
        hsSelector->setValues(color.saturation(), color.value());
        valuePal->setValue(color.hue());
        break;

    }

    bool blocked = valuePal->blockSignals(true);

    valuePal->setHue(color.hue());
    valuePal->setSaturation(color.saturation());
    valuePal->setColorValue(color.value());
    valuePal->updateContents();
    valuePal->blockSignals(blocked);
    valuePal->repaint();

    blocked = hsSelector->blockSignals(true);

    hsSelector->setHue(color.hue());
    hsSelector->setSaturation(color.saturation());
    hsSelector->setColorValue(color.value());
    hsSelector->updateContents();
    hsSelector->blockSignals(blocked);
    hsSelector->repaint();

    bRecursion = false;
}



void
KColorDialog::KColorDialogPrivate::slotColorPicker()
{
    bColorPicking = true;
/*#ifdef Q_WS_X11
    filter = new KCDPickerFilter(q);
    kapp->installX11EventFilter(filter);
#endif*/
    q->grabMouse(Qt::CrossCursor);
    q->grabKeyboard();
}

void
KColorDialog::mouseMoveEvent(QMouseEvent *e)
{
    if (d->bColorPicking) {
        d->_setColor(grabColor(e->globalPos()));
        return;
    }

    QWidget::mouseMoveEvent(e);
}

void
KColorDialog::mouseReleaseEvent(QMouseEvent *e)
{
    if (d->bColorPicking) {
        d->bColorPicking = false;
/*#ifdef Q_WS_X11
        kapp->removeX11EventFilter(d->filter);
        delete d->filter; d->filter = 0;
#endif*/
        releaseMouse();
        releaseKeyboard();
        d->_setColor(grabColor(e->globalPos()));
        return;
    }
    QWidget::mouseReleaseEvent(e);
}

QColor
KColorDialog::grabColor(const QPoint &p)
{
#ifdef Q_WS_X11
    // we use the X11 API directly in this case as we are not getting back a valid
    // return from QPixmap::grabWindow in the case where the application is using
    // an argb visual
    Window root = RootWindow(QX11Info::display(), QX11Info::appScreen());
//     XImage *ximg = XGetImage(QX11Info::display(), root, p.x(), p.y(), 1, 1, -1, ZPixmap);
//     unsigned long xpixel = XGetPixel(ximg, 0, 0);
//     XDestroyImage(ximg);
    XColor xcol;
//     xcol.pixel = xpixel;
    xcol.flags = DoRed | DoGreen | DoBlue;
//     XQueryColor(QX11Info::display(),
//                 DefaultColormap(QX11Info::display(), QX11Info::appScreen()),
//                 &xcol);
    return QColor::fromRgbF(xcol.red / 65535.0, xcol.green / 65535.0, xcol.blue / 65535.0);
#else
    QWidget *desktop = QApplication::desktop();
    QPixmap pm = QPixmap::grabWindow(desktop->winId(), p.x(), p.y(), 1, 1);
    QImage i = pm.toImage();
    return i.pixel(0, 0);
#endif
}

void
KColorDialog::keyPressEvent(QKeyEvent *e)
{
    if (d->bColorPicking) {
        if (e->key() == Qt::Key_Escape) {
            d->bColorPicking = false;
/*#ifdef Q_WS_X11
            kapp->removeX11EventFilter(d->filter);
            delete d->filter; d->filter = 0;
#endif*/
            releaseMouse();
            releaseKeyboard();
        }
        e->accept();
        return;
    }
    QWidget::keyPressEvent(e);
}

void KColorDialog::KColorDialogPrivate::setRgbEdit(const QColor &col)
{
    if (bEditRgb) return;
    int r, g, b;
    col.getRgb(&r, &g, &b);

    redit->setValue(r);
    gedit->setValue(g);
    bedit->setValue(b);
}

void KColorDialog::KColorDialogPrivate::setHtmlEdit(const QColor &col)
{
    if (bEditHtml) return;
    int r, g, b;
    col.getRgb(&r, &g, &b);
    QString num;

    num.sprintf("#%02X%02X%02X", r, g, b);
    htmlName->setText(num);
}


void KColorDialog::KColorDialogPrivate::setHsvEdit(const QColor &col)
{
    if (bEditHsv) return;
    int h, s, v;
    col.getHsv(&h, &s, &v);

    hedit->setValue(h);
    sedit->setValue(s);
    vedit->setValue(v);
}

// #include "kcolordialog.moc"
// #include "kcolordialog_p.moc"
//#endif
