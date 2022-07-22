// Copyright (C) 1997 Martin Jones (mjones@kde.org)
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

//----------------------------------------------------------------------
// KDE color selection dialog.

// layout management added Oct 1997 by Mario Weilguni
// <mweilguni@sime.com>

#ifndef KCOLORDIALOG_H
#define KCOLORDIALOG_H

#include <QtWidgets/QDialog>
#include <QtGui/QPixmap>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QListWidget>
#include "kcolorchoosermode.h"

#include "khuesaturationselect.h"
#include "kcolorcollection.h"

class KCDPickerFilter;
class KColorValueSelector;
class QVBoxLayout;
class QGridLayout;
class QCheckBox;


/**
* A table of editable color cells.
*
* @author Martin Jones <mjones@kde.org>
*/
class  KColorCells : public QTableWidget
{
  Q_OBJECT
public:
  /** 
   * Constructs a new table of color cells, consisting of
   * @p rows * @p columns colors. 
   *
   * @param parent The parent of the new widget
   * @param rows The number of rows in the table
   * @param columns The number of columns in the table
   */
  KColorCells( QWidget *parent, int rows, int columns );
  ~KColorCells();

  /** Sets the color in the given index in the table */
  void setColor( int index, const QColor &col );
  /** Returns the color at a given index in the table */
  QColor color( int index ) const;
  /** Returns the total number of color cells in the table */
  int count() const;

  void setShading(bool shade);
  void setAcceptDrags(bool acceptDrags);

  /** Sets the currently selected cell to @p index */
  void setSelected(int index);
  /** Returns the index of the cell which is currently selected */
  int  selectedIndex() const;
  
Q_SIGNALS:
  /** Emitted when a color is selected in the table */
  void colorSelected( int index , const QColor& color );
  /** Emitted when a color in the table is double-clicked */
  void colorDoubleClicked( int index , const QColor& color );

protected:
  // the three methods below are used to ensure equal column widths and row heights
  // for all cells and to update the widths/heights when the widget is resized
  virtual int sizeHintForColumn(int column) const override;
  virtual int sizeHintForRow(int column) const override;
  virtual void resizeEvent( QResizeEvent* event ) override;

  virtual void mouseReleaseEvent( QMouseEvent * ) override;
  virtual void mousePressEvent( QMouseEvent * ) override;
  virtual void mouseMoveEvent( QMouseEvent * ) override;
  virtual void dragEnterEvent( QDragEnterEvent * ) override;
  virtual void dragMoveEvent( QDragMoveEvent * ) override;
  virtual void dropEvent( QDropEvent *) override;
  virtual void mouseDoubleClickEvent( QMouseEvent * ) override;

  int positionToCell(const QPoint &pos, bool ignoreBorders=false) const;

private:
  class KColorCellsPrivate;
  friend class KColorCellsPrivate;
  KColorCellsPrivate *const d;
  
  Q_DISABLE_COPY(KColorCells)
};

/**
* A color palette in table form.
*
* @author Waldo Bastian <bastian@kde.org>
**/
class  KColorTable : public QWidget
{
  Q_OBJECT
  public:
    explicit KColorTable( QWidget *parent, int minWidth=210, int cols = 16);
    ~KColorTable();
    
    void addToCustomColors( const QColor &);
    void addToRecentColors( const QColor &);
    
    QString name() const;
    
  public Q_SLOTS:
    void setColors(const QString &collectionName);
    
  Q_SIGNALS:
    void colorSelected( const QColor &, const QString & );
    void colorDoubleClicked( const QColor &, const QString & );
    
  private:
    Q_PRIVATE_SLOT(d, void slotColorCellSelected( int index , const QColor& ))
    Q_PRIVATE_SLOT(d, void slotColorCellDoubleClicked( int index , const QColor& ))
    Q_PRIVATE_SLOT(d, void slotColorTextSelected( const QString &colorText ))
    Q_PRIVATE_SLOT(d, void slotSetColors( const QString &_collectionName ))
    Q_PRIVATE_SLOT(d, void slotShowNamedColorReadError( void ))
    
    void readNamedColor( void );
    
  private:
    virtual void setPalette(const QPalette& p) { QWidget::setPalette(p); }
    
  private:
    class KColorTablePrivate
    {
      public:
        KColorTablePrivate(KColorTable *q): q(q) {}
        
        void slotColorCellSelected(int index , const QColor&);
        void slotColorCellDoubleClicked(int index , const QColor&);
        void slotColorTextSelected(const QString &colorText);
        void slotSetColors(const QString &_collectionName);
        void slotShowNamedColorReadError(void);
        
        KColorTable *q;
        QString i18n_namedColors;
        QComboBox *combo;
        KColorCells *cells;
        QScrollArea *sv;
        QListWidget *mNamedColorList;
        KColorCollection *mPalette;
        int mMinWidth;
        int mCols;
        QMap<QString, QColor> m_namedColorMap;
    };
    friend class KColorTablePrivate;
    KColorTablePrivate *const d;
    
    Q_DISABLE_COPY(KColorTable)
};

/**
 * @short A color displayer.
 *
 * The KColorPatch widget is a (usually small) widget showing
 * a selected color e. g. in the KColorDialog. It
 * automatically handles drag and drop from and on the widget.
 *
 */
class  KColorPatch : public QFrame
{
  Q_OBJECT
public:
  KColorPatch( QWidget *parent );
  virtual ~KColorPatch();

  void setColor( const QColor &col );

Q_SIGNALS:
  void colorChanged( const QColor&);

protected:
  virtual void paintEvent    ( QPaintEvent * pe ) override;
  virtual void mouseMoveEvent( QMouseEvent * ) override;
  virtual void dragEnterEvent( QDragEnterEvent *) override;
  virtual void dropEvent( QDropEvent *) override;

private:
  class KColorPatchPrivate;
  friend class KColorCellsPrivate;
  KColorPatchPrivate *const d;
  
  Q_DISABLE_COPY(KColorPatch)
};

class KColorSpinBox : public QSpinBox
{
  Q_OBJECT
  public:
    KColorSpinBox(int minValue, int maxValue, int step, QWidget* parent)
    : QSpinBox(parent) {
      setRange(minValue, maxValue); setSingleStep(step);
    }
    
    
    // Override Qt's braindead auto-selection.
    //XXX KDE4 : check this is no more necessary , was disabled to port to Qt4 //mikmak
    /*
    virtual void valueChange()
    {
      updateDisplay();
      Q_EMIT valueChanged( value() );
      Q_EMIT valueChanged( currentValueText() );
}*/

};

/**
 * @short A color selection dialog.
 *
 * <b>Features:</b>\n
 *
 * @li Color selection from a wide range of palettes.
 * @li Color selection from a palette of H vs S and V selectors.
 * @li Direct input of HSV or RGB values.
 * @li Saving of custom colors
 *
 * In most cases, you will want to use the static method KColorDialog::getColor().
 * This pops up the dialog (with an initial selection provided by you), lets the
 * user choose a color, and returns.
 *
 * Example:
 *
 * \code
 * 	QColor myColor;
 * 	int result = KColorDialog::getColor( myColor );
 *         if ( result == KColorDialog::Accepted )
 *            ...
 * \endcode
 *
 * @image html kcolordialog.png "KDE Color Dialog"
 *
 * The color dialog is really a collection of several widgets which can
 * you can also use separately: the quadratic plane in the top left of
 * the dialog is a KXYSelector. Right next to it is a KHSSelector
 * for choosing hue/saturation.
 *
 * On the right side of the dialog you see a KColorTable showing
 * a number of colors with a combo box which offers several predefined
 * palettes or a palette configured by the user. The small field showing
 * the currently selected color is a KColorPatch.
 *
 **/
class KColorDialog : public QWidget
{
  Q_OBJECT

  public:
    /**
     * Constructs a color selection dialog.
     */
    explicit KColorDialog( QWidget *parent = 0L );
    /**
     * Destroys the color selection dialog.
     */
    ~KColorDialog();

    /**
     * Returns the currently selected color.
     **/
    QColor color() const;

    /**
     * Creates a modal color dialog, let the user choose a
     * color, and returns when the dialog is closed.
     *
     * The selected color is returned in the argument @p theColor.
     *
     * @param theColor if valid, specifies the color to be initially selected. On
     *                 return, holds the selected color.
     * @returns QDialog::result().
     */
//     static int getColor( QColor &theColor, QWidget *parent=0L );

    /**
     * Creates a modal color dialog, lets the user choose a
     * color, and returns when the dialog is closed.
     *
     * The selected color is returned in the argument @p theColor.
     *
     * This version takes a @p defaultColor argument, which sets the color
     * selected by the "default color" checkbox. When this checkbox is checked,
     * the invalid color (QColor()) is returned into @p theColor.
     *
     * @param theColor if valid, specifies the color to be initially selected. On
     *                 return, holds the selected color.
     * @param defaultColor color selected by the "default color" checkbox
     * @returns QDialog::result().
     */
//     static int getColor( QColor &theColor, const QColor& defaultColor, QWidget *parent=0L );

    /**
     * Gets the color from the pixel at point p on the screen.
     */
    static QColor grabColor(const QPoint &p);

    /**
     * Call this to make the dialog show a "Default Color" checkbox.
     * If this checkbox is selected, the dialog will return an "invalid" color (QColor()).
     * This can be used to mean "the default text color", for instance,
     * the one with the KDE text color on screen, but black when printing.
     */
    void setDefaultColor( const QColor& defaultCol );

    /**
     * @return the value passed to setDefaultColor
     */
    QColor defaultColor() const;

  public Q_SLOTS:
    /**
     * Preselects a color.
     */
    void setColor( const QColor &col );

  Q_SIGNALS:
    /**
     * Emitted when a color is selected.
     * Connect to this to monitor the color as it as selected if you are
     * not running modal.
     */
    void colorSelected( const QColor &col );

  private:
    Q_PRIVATE_SLOT(d, void slotRGBChanged( void ))
    Q_PRIVATE_SLOT(d, void slotHSVChanged( void ))
    Q_PRIVATE_SLOT(d, void slotHtmlChanged( void ))
    Q_PRIVATE_SLOT(d, void slotHSChanged( int, int ))
    Q_PRIVATE_SLOT(d, void slotVChanged( int ))
    Q_PRIVATE_SLOT(d, void slotColorSelected( const QColor &col ))
    Q_PRIVATE_SLOT(d, void slotColorSelected( const QColor &col, const QString &name ))
    Q_PRIVATE_SLOT(d, void slotColorDoubleClicked( const QColor &col, const QString &name ))
    Q_PRIVATE_SLOT(d, void slotColorPicker())
    Q_PRIVATE_SLOT(d, void slotAddToCustomColors())
    Q_PRIVATE_SLOT(d, void slotDefaultColorClicked())
    Q_PRIVATE_SLOT(d, void setHMode ( void ))
    Q_PRIVATE_SLOT(d, void setSMode ( void ))
    Q_PRIVATE_SLOT(d, void setVMode ( void ))
    Q_PRIVATE_SLOT(d, void setRMode ( void ))
    Q_PRIVATE_SLOT(d, void setGMode ( void ))
    Q_PRIVATE_SLOT(d, void setBMode ( void ))

    /**
     * Write the settings of the dialog to config file.
     **/
    Q_PRIVATE_SLOT(d, void slotWriteSettings())

  private:
    /**
     * Read the settings for the dialog from config file.
     **/
    void readSettings();

  protected:
    virtual void mouseMoveEvent( QMouseEvent * ) override;
    virtual void mouseReleaseEvent( QMouseEvent * ) override;
    virtual void keyPressEvent( QKeyEvent * ) override;
    virtual bool eventFilter( QObject *obj, QEvent *ev ) override;

  private:
    class KColorDialogPrivate
    {
      public:
        KColorDialogPrivate(KColorDialog *q): q(q) {}
        
        void setRgbEdit(const QColor &col);
        void setHsvEdit(const QColor &col);
        void setHtmlEdit(const QColor &col);
        void _setColor(const QColor &col, const QString &name = QString());
        void showColor(const QColor &color, const QString &name);
        
        void slotRGBChanged(void);
        void slotHSVChanged(void);
        void slotHtmlChanged(void);
        void slotHSChanged(int, int);
        void slotVChanged(int);
        
        void setHMode();
        void setSMode();
        void setVMode();
        void setRMode();
        void setGMode();
        void setBMode();
        
        void updateModeButtons();
        
        void slotColorSelected(const QColor &col);
        void slotColorSelected(const QColor &col, const QString &name);
        void slotColorDoubleClicked(const QColor &col, const QString &name);
        void slotColorPicker();
        void slotAddToCustomColors();
        void slotDefaultColorClicked();
        /**
        * Write the settings of the dialog to config file.
        **/
    void slotWriteSettings();
    
    /**
    * Returns the mode.
    */
    KColorChooserMode chooserMode();
    
    /**
    * Sets a mode. Updates the color picker and the color bar.
    */
    void setChooserMode(KColorChooserMode c);
    
    KColorDialog *q;
    KColorTable *table;
    QString originalPalette;
    bool bRecursion;
    bool bEditRgb;
    bool bEditHsv;
    bool bEditHtml;
    bool bColorPicking;
    QLabel *colorName;
    QLineEdit *htmlName;
    KColorSpinBox *hedit;
    KColorSpinBox *sedit;
    KColorSpinBox *vedit;
    KColorSpinBox *redit;
    KColorSpinBox *gedit;
    KColorSpinBox *bedit;
    QRadioButton *hmode;
    QRadioButton *smode;
    QRadioButton *vmode;
    QRadioButton *rmode;
    QRadioButton *gmode;
    QRadioButton *bmode;
    
    KColorPatch *patch;
    KColorPatch *comparePatch;
    
    KColorChooserMode _mode;
    
    KHueSaturationSelector *hsSelector;
    KColorCollection *palette;
    KColorValueSelector *valuePal;
    QVBoxLayout* l_right;
    QGridLayout* tl_layout;
    QCheckBox *cbDefaultColor;
    QColor defaultColor;
    QColor selColor;
    #ifdef Q_WS_X11
    KCDPickerFilter* filter;
    #endif
    };
    
    friend class KColorPatchPrivate;
    KColorDialogPrivate *const d;
    
    Q_DISABLE_COPY(KColorDialog)
};

#endif		// KCOLORDIALOG_H
