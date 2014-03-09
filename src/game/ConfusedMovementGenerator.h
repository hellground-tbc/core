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

#ifndef HELLGROUND_CONFUSEDMOVEMENTGENERATOR_H
#define HELLGROUND_CONFUSEDMOVEMENTGENERATOR_H

#include "MovementGenerator.h"

#include "Object.h"
#include "Timer.h"

#define WANDER_DISTANCE    2.5f
#define MAX_RANDOM_POINTS  6

template<class UNIT>
class HELLGROUND_EXPORT ConfusedMovementGenerator : public MovementGeneratorMedium< UNIT, ConfusedMovementGenerator<UNIT> >
{
    public:
        explicit ConfusedMovementGenerator() : _nextMoveTime(0) {}

        void Initialize(UNIT &);
        void Finalize(UNIT &);
        void Interrupt(UNIT &);
        void Reset(UNIT &u);
        bool Update(UNIT &, const uint32 &);

        const char* Name() const { return "<Confused>"; }
        MovementGeneratorType GetMovementGeneratorType() const { return CONFUSED_MOTION_TYPE; }

    private:
        void _generateMovement(UNIT &unit);

        TimeTrackerSmall _nextMoveTime;

        Position _randomPosition[MAX_RANDOM_POINTS+1];
};

#endif
