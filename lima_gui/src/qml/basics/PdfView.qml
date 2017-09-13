/*
    Copyright 2017 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/
/*!
  @author   Jocelyn Vernay
  @date     Wed, September 06 2017
  */

import QtQuick 2.7

//! A widget meant to display PDFs

// pdf support :
// - podofo
// - xpdf : http://www.foolabs.com/xpdf/about.html
// http://www.glyphandcog.com/
// https://www.codeproject.com/Articles/7056/Code-to-extract-plain-text-from-a-PDF-file

import QtQuick 2.7
import QtWebEngine 1.0 

WebEngineView { 
  id: webview 

  property string viewer    // chemin absolu pointant vers notre fichier viewer.html dernièrement téléchargé. 
  property string pathPdf  // chemin absolu pointant vers le document pdf à afficher. 
  
  onPathPdfChanged:  {
    console.log("pathpdf=", pathPdf)
  }

  // le code ci-dessous permettra d'afficher une page blanche si au moins l'un des deux chemin n'est pas renseigné 
  url: if (viewer && pathPdf) 
  { 
    viewer + "?file=" + pathPdf 
  } 
}
