/* DUKTO - A simple, fast and multi-platform file transfer tool for LAN users
 * Copyright (C) 2009 Emanuele Colombo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "oslib.h"

#include <QHostInfo>
#include <QSystemDeviceInfo>
#include <QSettings>

QTM_USE_NAMESPACE

#if defined(Q_WS_WIN)
#define PLATFORM "Windows"
#elif defined(Q_WS_MAC)
#define PLATFORM "Apple Mac"
#elif defined(Q_WS_X11)
#define PLATFORM "Linux"
#elif defined(Q_WS_S60)
#define PLATFORM "Symbian"
#else
#define PLATFORM "Unknown"
#endif

QString OsLib::removeUrlPrefix(QString url)
{
#if defined(Q_WS_WIN)
    url.replace("file:///", "");
#else
    url.replace("file://", "");
#endif
    return url;
}

QString OsLib::retrieveSystemName()
{
#if defined(Q_WS_S60)
    QString systemName = "";
    QSystemDeviceInfo info;
    QSettings settings("msec.it", "Dukto");
    QString uname = settings.value("dukto/username", "User").toString();
    systemName = uname + " at " + info.model() + " (" + PLATFORM + ")";
#else
    static QString systemName = "";
    if (systemName != "") return systemName;
    QString uname(getenv("USERNAME"));
    if (uname == "") uname = getenv("USER");
    if (uname == "") uname = "Unknown";
    systemName = uname + " at " + QHostInfo::localHostName() + " (" + PLATFORM + ")";
#endif
    return systemName;
}
