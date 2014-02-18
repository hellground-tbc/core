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

#ifndef SC_DEF_MECHANAR_H
#define SC_DEF_MECHANAR_H

#define DATA_NETHERMANCER_EVENT     1
#define DATA_IRONHAND_EVENT         2
#define DATA_GYROKILL_EVENT         3
#define DATA_CACHE_OF_LEGION_EVENT  4
#define DATA_MECHANO_LORD_EVENT     5
#define DATA_BRIDGE_EVENT           6

#define GET_CHARGE_ID(p) ( (p)->HasAura(39088, 0) ? (1) : ( (p)->HasAura(39091, 0) ? (-1) : (0) ))

#endif

