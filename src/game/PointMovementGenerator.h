/*
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
 *
 * Copyright (C) 2008 Trinity <http://www.trinitycore.org/>
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

#ifndef TRINITY_POINTMOVEMENTGENERATOR_H
#define TRINITY_POINTMOVEMENTGENERATOR_H

#include "MovementGenerator.h"
#include "FollowerReference.h"

class Creature;

template<class T>
class TRINITY_DLL_SPEC PointMovementGenerator : public MovementGeneratorMedium< T, PointMovementGenerator<T> >
{
    public:
        PointMovementGenerator(uint32 _id, float _x, float _y, float _z, float _speed = 0.0f, bool _generatePath = false) : id(_id),
            i_x(_x), i_y(_y), i_z(_z), speed(_speed), m_generatePath(_generatePath) {}

        void Initialize(T &);
        void Finalize(T &);
        void Interrupt(T &);
        void Reset(T &);
        bool Update(T &, const uint32 &);

        void MovementInform(T &);

        MovementGeneratorType GetMovementGeneratorType() { return POINT_MOTION_TYPE; }

    private:
        float i_x,i_y,i_z;
        uint32 id;
        float speed;
        bool m_generatePath;
};

class TRINITY_DLL_SPEC AssistanceMovementGenerator : public PointMovementGenerator<Creature>
{
    public:
        AssistanceMovementGenerator(float _x, float _y, float _z) :
            PointMovementGenerator<Creature>(0, _x, _y, _z, false) {}

        MovementGeneratorType GetMovementGeneratorType() { return ASSISTANCE_MOTION_TYPE; }
        void Finalize(Unit &);
};

// Does almost nothing - just doesn't allows previous movegen interrupt current effect.
class TRINITY_DLL_SPEC EffectMovementGenerator : public MovementGenerator
{
    public:
        explicit EffectMovementGenerator(uint32 Id) : m_Id(Id) {}
        void Initialize(Unit &) {}
        void Finalize(Unit &unit);
        void Interrupt(Unit &) {}
        void Reset(Unit &) {}
        bool Update(Unit &u, const uint32 &);
        MovementGeneratorType GetMovementGeneratorType() { return EFFECT_MOTION_TYPE; }

    private:
        uint32 m_Id;
};

#endif
