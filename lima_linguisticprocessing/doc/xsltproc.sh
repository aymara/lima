# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

FILENAME=`basename $1 .xml`
#xsltproc --stringparam paper.type A4 /usr/share/sgml/docbook/xsl-stylesheets-1.78.1/fo/docbook.xsl $FILENAME.xml > $FILENAME.fo
xsltproc --stringparam paper.type A4 `locate fo/docbook.xsl | grep "/usr" | head -n 1` $FILENAME.xml > $FILENAME.fo
fop $FILENAME.fo -pdf $FILENAME.pdf
