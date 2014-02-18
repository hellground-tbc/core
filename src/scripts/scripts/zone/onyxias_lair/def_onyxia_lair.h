/* 
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

#ifndef SC_DEF_ONYXIA_LAIR_H
#define SC_DEF_ONYXIA_LAIR_H

struct cPosition
{
    float x;
    float y;
    float z;
    float o;
};

enum SpawnDefinitions
{
    LEFT                 = 0,
    RIGHT                = 1,
    MAX                  = 2,
};

static cPosition spawnEntrancePoints[MAX] =
{
    {-35.8699, -161.351, -91.1765},
    {-35.813, -268.307, -91.1498}
};

enum OnyxiaData
{
    DATA_ONYXIA     = 1,
    DATA_HATCH_EGGS = 2,
    DATA_ERUPT      = 3
};

#endif
