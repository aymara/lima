// Copyright (C) 1999 Steffen Hansen (hansen@kde.org)
// Copyright (C) 2005 Joseph Wenninger (jowenn@kde.org)
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef _KCOLORMIMEDATA_H
#define _KCOLORMIMEDATA_H

class QColor;
class QDrag;
class QMimeData;
class QWidget;

/**
 * Drag-and-drop and clipboard mimedata manipulation for QColor objects. The according MIME type
 * is set to application/x-color.
 *
 * See the Qt drag'n'drop documentation.
 */
namespace KColorMimeData
{
    /**
     * Sets the color and text representation fields for the specified color in the mimedata object:
     * application/x-color and text/plain types are set
     */
    void populateMimeData(QMimeData *mimeData, const QColor& color);

    /**
     * Returns true if the MIME data @p mimeData contains a color object.
     * First checks for application/x-color and if that fails, for a text/plain entry, which
     * represents a color in the format \#hexnumbers
     */
    bool canDecode(const QMimeData *mimeData);

    /**
     * Decodes the MIME data @p mimeData and returns the resulting color.
     * First tries application/x-color and if that fails, a text/plain entry, which
     * represents a color in the format \#hexnumbers. If this fails too,
     * an invalid QColor object is returned, use QColor::isValid() to test it.
     */
    QColor fromMimeData(const QMimeData *mimeData);

    /**
     * Creates a color drag object. Either you have to start this drag or delete it
     * The drag object's mime data has the application/x-color and text/plain type set and a pixmap
     * filled with the specified color, which is going to be displayed next to the mouse cursor
     */
    QDrag* createDrag(const QColor& color, QWidget *dragsource);
}


#endif // _KCOLORMIMEDATA_H
