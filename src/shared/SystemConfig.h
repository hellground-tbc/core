/*
 * Copyright (C) 2005-2008 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2008 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2008-2014 Hellground <http://hellground.net/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

// THIS FILE IS DEPRECATED

#ifndef HELLGROUND_SYSTEMCONFIG_H
#define HELLGROUND_SYSTEMCONFIG_H

#include "Platform/Define.h"
#include "revision.h"


#define _PACKAGENAME "HellgroundCore "
#define _CODENAME "YUME"

#if HELLGROUND_ENDIAN == HELLGROUND_BIGENDIAN
# define _ENDIAN_STRING "big-endian"
#else
# define _ENDIAN_STRING "little-endian"
#endif

#define _FULLVERSION _PACKAGENAME "Rev: " _REVISION

#define DEFAULT_PLAYER_LIMIT 100
#define DEFAULT_WORLDSERVER_PORT 8085                       //8129
#define DEFAULT_REALMSERVER_PORT 3724
#define DEFAULT_SOCKET_SELECT_TIME 10000

// The path to config files
#ifndef SYSCONFDIR
# define SYSCONFDIR ""
#endif

#define _HELLGROUND_CORE_CONFIG SYSCONFDIR "hellgroundcore.conf"
#define _HELLGROUND_REALM_CONFIG SYSCONFDIR "hellgroundrealm.conf"

// Format is YYYYMMDDRR where RR is the change in the conf file
// for that day.
#ifndef _HELLGROUND_CORE_CONFVER
# define _HELLGROUND_CORE_CONFVER 2014022101
#endif //_HELLGROUND_CORE_CONFVER

// Format is YYYYMMDDRR where RR is the change in the conf file
// for that day.
#ifndef _REALMDCONFVERSION
# define _REALMDCONFVERSION 2014021201
#endif

#endif

