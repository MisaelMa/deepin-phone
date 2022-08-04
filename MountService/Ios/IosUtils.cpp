/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     wangzhixuan <wangzhixuan@uniontech.com>
*
* Maintainer: wangzhixuan <wangzhixuan@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "IosUtils.h"

IosUtils::IosUtils(QObject *parent)
    : QObject(parent)
{
}

QString IosUtils::transIPhoneDevName(const QString &strDevName)
{
    if (strDevName == "iPhone1,1")
        return "iPhone 2G";
    if (strDevName == "iPhone1,2")
        return "iPhone 3G";
    if (strDevName == "iPhone2,1")
        return "iPhone 3GS";
    if (strDevName == "iPhone3,1")
        return "iPhone 4";
    if (strDevName == "iPhone3,2")
        return "iPhone 4";
    if (strDevName == "iPhone3,3")
        return "iPhone 4";
    if (strDevName == "iPhone4,1")
        return "iPhone 4S";
    if (strDevName == "iPhone5,1")
        return "iPhone 5";
    if (strDevName == "iPhone5,2")
        return "iPhone 5";
    if (strDevName == "iPhone5,3")
        return "iPhone 5c";
    if (strDevName == "iPhone5,4")
        return "iPhone 5c";
    if (strDevName == "iPhone6,1")
        return "iPhone 5s";
    if (strDevName == "iPhone6,2")
        return "iPhone 5s";
    if (strDevName == "iPhone7,1")
        return "iPhone 6 Plus";
    if (strDevName == "iPhone7,2")
        return "iPhone 6";

    if (strDevName == "iPhone8,1")
        return "iPhone 6s";
    if (strDevName == "iPhone8,2")
        return "iPhone 6s Plus";
    if (strDevName == "iPhone9,1")
        return "iPhone 7";
    if (strDevName == "iPhone9,3")
        return "iPhone 7";
    if (strDevName == "iPhone9,2")
        return "iPhone 7 Plus";
    if (strDevName == "iPhone9,4")
        return "iPhone 7 Plus";
    if (strDevName == "iPhone10,1")
        return "iPhone 8";
    if (strDevName == "iPhone10,4")
        return "iPhone 8";
    if (strDevName == "iPhone10,2")
        return "iPhone 8 Plus";
    if (strDevName == "iPhone10,5")
        return "iPhone 8 Plus";
    if (strDevName == "iPhone10,3")
        return "iPhone X";
    if (strDevName == "iPhone10,6")
        return "iPhone X";

    if (strDevName == "iPhone11,8")
        return "iPhone XR";
    if (strDevName == "iPhone11,2")
        return "iPhone XS";
    if (strDevName == "iPhone11,4")
        return "iPhone XS Max";
    if (strDevName == "iPhone11,6")
        return "iPhone XS Max";
    if (strDevName == "iPhone12,1")
        return "iPhone 11";
    if (strDevName == "iPhone12,3")
        return "iPhone 11 Pro";
    if (strDevName == "iPhone12,5")
        return "iPhone 11 Pro Max";

    return strDevName;
}
