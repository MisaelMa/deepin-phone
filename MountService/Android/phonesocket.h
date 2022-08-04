#ifndef PHONESOCKET_H
#define PHONESOCKET_H

#include <QObject>

#include "defines.h"

class QTcpSocket;

class PhoneSocket : public QObject
{
    Q_OBJECT
public:
    explicit PhoneSocket(QObject *parent = nullptr);

    void parseApps(const QString &result);

public:
    void sendCommand(const QString &devId);

signals:
    void sigAppInfoList(const QString &, const QList<PhoneAppInfo> &);

private:
    void recvSocketData(int flag, const QString &result);
    void parseAppsEmpty();

private:
    QString m_strDeviceID = "";
    bool m_bIsSendTwice = false;

    QTcpSocket *tcp = nullptr;
};

#endif // PHONESOCKETTHREAD_H
