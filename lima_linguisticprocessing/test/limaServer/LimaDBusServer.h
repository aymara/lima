#ifndef LIMADBUDSERVER_H
#define LIMADBUDSERVER_H

#include <QObject>
#include <QString>

#include <deque>

class LimaDBusServerPrivate;

class LimaDBusServer : public QObject
{
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "fr.cea.LimaDBusServer")

public:
  /**
   * @param serviceLife life of service in seconds. no limit if 0
   */
    LimaDBusServer( const std::string& limaServerConfigFile,
                const std::deque<std::string>& langs,
                const std::deque<std::string>& pipelines,
                int serviceLife = 0,
                QObject *parent = 0);
    virtual ~LimaDBusServer();

public Q_SLOTS:
    QString handleRequest(const QString& req, const QString& language, const QString& pipeline = "main");

private:
  LimaDBusServerPrivate* m_d;
};

 #endif
 
