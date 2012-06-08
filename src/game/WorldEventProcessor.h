/*
 * Copyright (C) 2012 HellGround <http://www.hellground.pl/>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef WORLDEVENTPROCESSOR_H
#define WORLDEVENTPROCESSOR_H

#include <ace/Singleton.h>

class Player;

class HELLGROUND_DLL_SPEC WorldEvent
{
    public:
        WorldEvent(Player* player) : _owner(player) {}

        virtual bool Execute() = 0;

    private:
        Player* _owner;
};

class HELLGROUND_DLL_SPEC WorldEventProcessor
{
    friend class ACE_Singleton<WorldEventProcessor, ACE_Thread_Mutex>;
    WorldEventProcessor() : _killInProgress(false) {}

    typedef std::multimap<uint64, WorldEvent*> EventList;

    public:
        void ScheduleEvent(Player*, WorldEvent*);
        void DestroyEvents(uint64 = 0);

        void ExecuteEvents();

    private:
        bool _killInProgress;
        EventList _events;

        ACE_Thread_Mutex Lock;
};

#define sWorldEventProcessor (*ACE_Singleton<WorldEventProcessor, ACE_Thread_Mutex>::instance())
#endif
