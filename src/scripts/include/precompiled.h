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

#ifndef SC_PRECOMPILED_H
#define SC_PRECOMPILED_H

#include "DBCStores.h"

#include "../ScriptMgr.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Unit.h"
#include "GameObject.h"
#include "SpellMgr.h"
#include "sc_creature.h"
#include "sc_gossip.h"
#include "sc_instance.h"
#include "CombatAI.h"
#include "PassiveAI.h"

#ifdef WIN32
#include <windows.h>
BOOL APIENTRY DllMain( HANDLE hModule,
DWORD  ul_reason_for_call,
LPVOID lpReserved
)
{
    return true;
}
#endif

#endif

