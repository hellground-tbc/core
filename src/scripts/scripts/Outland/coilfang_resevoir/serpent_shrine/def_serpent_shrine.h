/* 
 * Copyright (C) 2006-2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

#ifndef SC_DEF_SERPENT_SHRINE_H
#define SC_DEF_SERPENT_SHRINE_H

enum LurkerEventState
{
    LURKER_NOT_STARTED = 0,
    LURKER_FISHING     = 1,
    LURKER_HOOKED      = 2
};
enum WaterEventState
{
    WATERSTATE_NONE     = 0,
    WATERSTATE_FRENZY   = 1,
    WATERSTATE_SCALDING = 2
};

#define DATA_CANSTARTPHASE3 1
#define DATA_CARIBDIS 2
#define DATA_HYDROSSTHEUNSTABLEEVENT 3
#define DATA_KARATHRESS 4
#define DATA_KARATHRESSEVENT 5
#define DATA_KARATHRESSEVENT_STARTER 6
#define DATA_LADYVASHJ 7
#define DATA_LADYVASHJEVENT 8
#define DATA_LEOTHERASTHEBLINDEVENT 9
#define DATA_MOROGRIMTIDEWALKEREVENT 10
#define DATA_SHARKKIS 11
#define DATA_SHIELDGENERATOR1 12
#define DATA_SHIELDGENERATOR2 13
#define DATA_SHIELDGENERATOR3 14
#define DATA_SHIELDGENERATOR4 15
#define DATA_THELURKERBELOW 16
#define DATA_THELURKERBELOWEVENT 17
#define DATA_TIDALVESS 18
#define DATA_LEOTHERAS 19
#define DATA_LEOTHERAS_EVENT_STARTER 20
#define DATA_CONTROL_CONSOLE 21
#define DATA_STRANGE_POOL 22
#define DATA_WATER 23
#define DATA_SHARKKIS_PET 24

#define WATER_Z -21.57f
#define VASHJ_WATER_Z 20.27f
#endif

