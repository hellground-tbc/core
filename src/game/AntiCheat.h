/*
 * Copyright (C) 2008-2014 HellGround <http://www.hellground.pl/>
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

#ifndef HELLGROUND_ANTICHEAT_H
#define HELLGROUND_ANTICHEAT_H

#include <ace/Method_Request.h>

#include "Player.h"

class ACRequest : public ACE_Method_Request
{
    public:
        ACRequest(Player *player, MovementInfo lastPacket, MovementInfo newPacket)
            : m_ownerGUID(player->GetGUID()), m_oldMovement(lastPacket), m_newMovement(newPacket) {}

        virtual int call();

        bool DetectWaterWalkHack(Player *);
        bool DetectTeleportToPlane(Player*);
        bool DetectFlyHack(Player*);
        bool DetectSpeedHack(Player*);

        MovementInfo &GetLastMovementInfo() { return m_oldMovement; }
        MovementInfo &GetNewMovementInfo() { return m_newMovement; }

    private:
        uint64 m_ownerGUID;

        MovementInfo m_oldMovement;
        MovementInfo m_newMovement;
};

#endif
