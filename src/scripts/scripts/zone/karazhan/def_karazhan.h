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

#ifndef SC_DEF_KARAZHAN_H
#define SC_DEF_KARAZHAN_H

#define DATA_ATTUMEN_EVENT               1
#define DATA_MOROES_EVENT                2
#define DATA_MAIDENOFVIRTUE_EVENT        3
#define DATA_OPTIONAL_BOSS_EVENT         4
#define DATA_OPERA_EVENT                 5
#define DATA_CURATOR_EVENT               6
#define DATA_SHADEOFARAN_EVENT           7
#define DATA_TERESTIAN_EVENT             8
#define DATA_NETHERSPITE_EVENT           9
#define DATA_CHESS_EVENT                10
#define DATA_MALCHEZZAR_EVENT           11
#define DATA_NIGHTBANE_EVENT            12
#define DATA_OPERA_PERFORMANCE          13
#define DATA_OPERA_OZ_DEATHCOUNT        14
#define DATA_KILREK                     15
#define DATA_TERESTIAN                  16
#define DATA_MOROES                     17
#define DATA_GAMEOBJECT_CURTAINS        18
#define DATA_GAMEOBJECT_STAGEDOORLEFT   19
#define DATA_GAMEOBJECT_STAGEDOORRIGHT  20
#define DATA_GAMEOBJECT_LIBRARY_DOOR    21
#define DATA_GAMEOBJECT_MASSIVE_DOOR    22
#define DATA_GAMEOBJECT_NETHER_DOOR     23
#define DATA_GAMEOBJECT_GAME_DOOR       24
#define DATA_GAMEOBJECT_GAME_EXIT_DOOR  25
#define DATA_IMAGE_OF_MEDIVH            26
#define DATA_MASTERS_TERRACE_DOOR_1     27
#define DATA_MASTERS_TERRACE_DOOR_2     28
#define DATA_ARAN                       29
#define DATA_NIGHTBANE                  30
#define DATA_BLIZZARD                   31
#define DATA_BARNES                     32
#define CHESS_EVENT_TEAM                33
#define DATA_CHESS_ECHO_OF_MEDIVH       34
#define DATA_DUST_COVERED_CHEST         35
#define DATA_CHESS_DAMAGE               36

#define SPELL_BROKEN_PACT               30065

// Opera Performances
#define EVENT_OZ        1
#define EVENT_HOOD      2
#define EVENT_RAJ       3

#define ERROR_INST_DATA(a)     error_log("TSCR: Instance Data for Karazhan not set properly. Encounter for Creature Entry %u may not work properly.", a->GetEntry());
#endif

