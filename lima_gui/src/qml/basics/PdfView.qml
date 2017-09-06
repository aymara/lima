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
