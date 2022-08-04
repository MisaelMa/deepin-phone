/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co., Ltd.
 *
 * Author:     yelei <yelei@uniontech.com>
 * Maintainer: yelei <yelei@uniontech.com>
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef PHONETHREAD_H
#define PHONETHREAD_H

#include "defines.h"
#include <QThread>

class PhoneThread : public QThread
{
    Q_OBJECT
public:
    PhoneThread(QObject *parent = nullptr);
    virtual ~PhoneThread();

    //获取手机信息
    bool readPhoneInfo(QString strPhoneID, DEVICE_TYPE type);

    void setStrApkVersion(const QString &strApkVersion);
    bool findPhoneInfo();

signals:
    void sigFindPhoneInfo(const PhoneInfo &info);
    void error(int nErrorCode, QString strErrorMsg);

protected:
    void run();

private:
    bool apkInstall(const QString &sDeviceId, PhoneInfo info);

    bool installNewApkInDevice(const QString &sDeviceId);

private:
    QString m_strPhoneID;
    DEVICE_TYPE m_type;

    bool m_bInstalFailed = false;

    QString m_strApkVersion = "";
    const QString s_apk_package = "com.ut.utassistant";
    const QString s_apk_class_name = s_apk_package + ".MainActivity";
};

#endif // PHONETHREAD_H
