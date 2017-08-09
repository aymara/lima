#ifndef LIMA_GUI_EXPORT_H
#define LIMA_GUI_EXPORT_H

#ifdef WIN32

#ifdef LIMA_GUI_EXPORTING
   #define LIMA_GUI_EXPORT    __declspec(dllexport)
#else
   #define LIMA_GUI_EXPORT    __declspec(dllimport)
#endif

#else // Not WIN32

#define LIMA_GUI_EXPORT

#endif

#endif
