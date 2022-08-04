#ifndef MOUNTSERVICE_H
#define MOUNTSERVICE_H

#include "mountservice_global.h"

/**
 * @brief 挂在服务 入口
 */

#include <QObject>

#include "defines.h"
#include "Ios/IosService.h"
#include "Android/AndroidService.h"

class PhoneConnectThread;

class MOUNTSERVICESHARED_EXPORT MountService : public QObject
{
    Q_OBJECT
private:
    MountService(QObject *parent);

public:
    ~MountService() override;

    static void InitService(QObject *p);
    static MountService *getService();

signals:
    void sigDeviceListChanged(const DEV_CONN_INFO &devInfo);
    void sigDeviceAuthorezeChanged(const DEV_CONN_INFO devInfo);
    void sigNoDeviceList();

public:
    void startDeviceConnectCheck();

    QString getAdbPackage(const QString &sDeviceId);
    QString getAdbPackageVersionCode(const QString &sDeviceId);

    QString getDevices(const int &);
    void runAdbServer();

    QString getAdbUsbState(const QString &deviceId);

    void execAdbForward(const QString &devId);

    QString packagePath(const QString &Id, const QString &package);

    QString getPhoneBattery(const QString &sDeviceId);

    QString uninstallPhoneAPP(const int &, const QString &devID, const QString &package, const int &mescs);

    QString exportPackage(const QString &devID, const QString &path_apk, const QString &out_apk, const int &msecs);

    void changeInstallationPackageName(const QString &devID, const QString &oldName, const QString &newName);

    int adbPull(const QString &devID, const QString &path_apk, const QString &out_apk);

    QString getPhoneModel(const QString &sDeviceId);
    QString getPhoneBrand(const QString &sDeviceId);
    QString getPhoneVersionRelease(const QString &sDeviceId);
    QString getLocaleRegion(const QString &strPhoneID);

    bool startApkExe(const QString &sDeviceId);

    QString getProductType(const QString &);

    QString GetMountPath(const int &, const QString &devId);
    void getMountSize(const int &, const QString &strDeviceId, quint64 &nSysTotal, quint64 &nSysFree, QString &sPer);

public:
    QList<PhoneAppInfo> getIosApps();

private:
    static MountService *g_instance;
    AndroidService *m_pAndroidService = nullptr;
    IosService *m_pIosService = nullptr;

    //    PhoneConnectThread *m_devCheckThread = nullptr;
};

#endif // IOSSERVICE_H
