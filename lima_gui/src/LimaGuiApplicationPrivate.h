#ifndef LIMA_GUI_APPLICATION_PUBLIC_H
#define LIMA_GUI_APPLICATION_PUBLIC_H

class LimaGuiApplicationPrivate;

class LimaGuiApplication : public QObject {
  Q_OBJECT:
  
public:
  LimaGuiApplication(QObject* p = 0);
  
private:
  LimaGuiApplicationPrivate* d_ptr;
}

#endif // LIMA_GUI_APPLICATION_PUBLIC_H
