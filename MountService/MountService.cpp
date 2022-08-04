#include "MountService.h"

#include "Ios/iphonemountcontrol.h"
#include "phoneconnectthread.h"

MountService *MountService::g_instance = nullptr;

MountService::MountService(QObject *parent)
    : QObject(parent)
{
    m_pAndroidService = new AndroidService(this);
    m_pIosService = new IosService(this);
}

MountService::~MountService()
{
    //    if (m_devCheckThread) {
    //        delete m_devCheckThread;
    //        m_devCheckThread = nullptr;
    //    }
}

void MountService::InitService(QObject *p)
{
    g_instance = new MountService(p);
}

MountService *MountService::getService()
{
    return g_instance;
}

QString MountService::getDevices(const int &iType)
{
    if (iType == Mount_Android) {
        return m_pAndroidService->getAddDevices();
    }
    if (iType == Mount_Ios) {
        return IPhoneMountControl::getMountDevice();
    }

    return "";
}

void MountService::runAdbServer()
{
    m_pAndroidService->runAdbServer();
}

QString MountService::getAdbUsbState(const QString &deviceId)
{
    return m_pAndroidService->getAdbUsbState(deviceId);
}

void MountService::execAdbForward(const QString &devId)
{
    m_pAndroidService->execAdbForward(devId);
}

QString MountService::packagePath(const QString &Id, const QString &package)
{
    return m_pAndroidService->packagePath(Id, package);
}

QString MountService::getPhoneBattery(const QString &sDeviceId)
{
    return m_pAndroidService->getPhoneBattery(sDeviceId);
}

QString MountService::uninstallPhoneAPP(const int &nType, const QString &devID, const QString &package, const int &mescs)
{
    if (nType == Mount_Android) {
        return m_pAndroidService->uninstallPhoneAPP(devID, package, mescs);
    }
    if (nType == Mount_Ios) {
        return m_pIosService->uninstallIosApp(package, mescs);
    }

    return "-1";
}

QString MountService::exportPackage(const QString &devID, const QString &path_apk, const QString &out_apk, const int &msecs)
{
    return m_pAndroidService->exportPackage(devID, path_apk, out_apk, msecs);
}

void MountService::changeInstallationPackageName(const QString &devID, const QString &oldName, const QString &newName)
{
    m_pAndroidService->changeInstallationPackageName(devID, oldName, newName);
}

int MountService::adbPull(const QString &devID, const QString &path_apk, const QString &out_apk)
{
    return m_pAndroidService->adbPull(devID, path_apk, out_apk);
}

QString MountService::getPhoneModel(const QString &sDeviceId)
{
    return m_pAndroidService->getPhoneModel(sDeviceId);
}

QString MountService::getLocaleRegion(const QString &strPhoneID)
{
    return m_pAndroidService->getLocaleRegion(strPhoneID);
}

QString MountService::getPhoneBrand(const QString &sDeviceId)
{
    return m_pAndroidService->getPhoneBrand(sDeviceId);
}

QString MountService::getPhoneVersionRelease(const QString &sDeviceId)
{
    return m_pAndroidService->getPhoneVersionRelease(sDeviceId);
}

bool MountService::startApkExe(const QString &sDeviceId)
{
    return m_pAndroidService->startApkExe(sDeviceId);
}

QString MountService::getProductType(const QString &sdevId)
{
    return IPhoneMountControl::getProductType(sdevId);
}

QString MountService::GetMountPath(const int &nType, const QString &devId)
{
    if (nType == Mount_Android) {
        return m_pAndroidService->androidDevMountPath(devId);
    }
    if (nType == Mount_Ios) {
        return IPhoneMountControl::getMountAbsolutePath(devId);
    }
    return "";
}

void MountService::getMountSize(const int &nType, const QString &strDeviceId, quint64 &nSysTotal, quint64 &nSysFree, QString &sPer)
{
    if (nType == Mount_Android) {
        m_pAndroidService->getSystemUseAndTotal(strDeviceId, nSysTotal, nSysFree, sPer);
    } else if (nType == Mount_Ios) {
        IPhoneMountControl::getMountSize(strDeviceId, nSysTotal, nSysFree, sPer);
    }
}

QList<PhoneAppInfo> MountService::getIosApps()
{
    return m_pIosService->getApps("");
}

//启动设备检测线程
void MountService::startDeviceConnectCheck()
{
    auto m_devCheckThread = new PhoneConnectThread(this);
    qRegisterMetaType<DEV_CONN_INFO>("DEV_CONN_INFO");
    connect(m_devCheckThread, &PhoneConnectThread::sigDeviceListChanged, this, &MountService::sigDeviceListChanged);
    connect(m_devCheckThread, &PhoneConnectThread::sigDeviceAuthorezeChanged, this, &MountService::sigDeviceAuthorezeChanged);
    connect(m_devCheckThread, &PhoneConnectThread::sigNoDeviceList, this, &MountService::sigNoDeviceList);
    m_devCheckThread->start();
}

QString MountService::getAdbPackage(const QString &sDeviceId)
{
    return m_pAndroidService->getAdbPackage(sDeviceId);
}

QString MountService::getAdbPackageVersionCode(const QString &sDeviceId)
{
    return m_pAndroidService->getAdbPackageVersionCode(sDeviceId);
}
