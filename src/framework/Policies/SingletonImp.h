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

#ifndef HELLGROUND_SINGLETONIMPL_H
#define HELLGROUND_SINGLETONIMPL_H

#include "Singleton.h"

// avoid the using namespace here cuz
// its a .h file afterall

template
<
typename T,
class ThreadingModel,
class CreatePolicy,
class LifeTimePolicy
>
T&
Hellground::Singleton<T, ThreadingModel, CreatePolicy, LifeTimePolicy >::Instance()
{
    if( !si_instance )
    {
        // double-checked Locking pattern
        Guard();
        if( !si_instance )
        {
            if( si_destroyed )
            {
                si_destroyed = false;
                LifeTimePolicy::OnDeadReference();
            }
            si_instance = CreatePolicy::Create();
            LifeTimePolicy::ScheduleCall(&DestroySingleton);
        }
    }

    return *si_instance;
}

template
<
typename T,
class ThreadingModel,
class CreatePolicy,
class LifeTimePolicy
>
void
Hellground::Singleton<T, ThreadingModel, CreatePolicy, LifeTimePolicy>::DestroySingleton()
{
    CreatePolicy::Destroy(si_instance);
    si_instance = NULL;
    si_destroyed = true;
}

#define INSTANTIATE_SINGLETON_1(TYPE) \
    template class HELLGROUND_DLL_DECL Hellground::Singleton<TYPE, Hellground::SingleThreaded<TYPE>, Hellground::OperatorNew<TYPE>, Hellground::ObjectLifeTime<TYPE> >; \
    template<> TYPE* Hellground::Singleton<TYPE, Hellground::SingleThreaded<TYPE>, Hellground::OperatorNew<TYPE>, Hellground::ObjectLifeTime<TYPE> >::si_instance = 0; \
    template<> bool Hellground::Singleton<TYPE, Hellground::SingleThreaded<TYPE>, Hellground::OperatorNew<TYPE>, Hellground::ObjectLifeTime<TYPE> >::si_destroyed = false

#define INSTANTIATE_SINGLETON_2(TYPE, THREADINGMODEL) \
    template class HELLGROUND_DLL_DECL Hellground::Singleton<TYPE, THREADINGMODEL, Hellground::OperatorNew<TYPE>, Hellground::ObjectLifeTime<TYPE> >; \
    template<> TYPE* Hellground::Singleton<TYPE, THREADINGMODEL, Hellground::OperatorNew<TYPE>, Hellground::ObjectLifeTime<TYPE> >::si_instance = 0; \
    template<> bool Hellground::Singleton<TYPE, THREADINGMODEL, Hellground::OperatorNew<TYPE>, Hellground::ObjectLifeTime<TYPE> >::si_destroyed = false

#define INSTANTIATE_SINGLETON_3(TYPE, THREADINGMODEL, CREATIONPOLICY ) \
    template class HELLGROUND_DLL_DECL Hellground::Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, Hellground::ObjectLifeTime<TYPE> >; \
    template<> TYPE* Hellground::Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, Hellground::ObjectLifeTime<TYPE> >::si_instance = 0; \
    template<> bool Hellground::Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, Hellground::ObjectLifeType<TYPE> >::si_destroyed = false

#define INSTANTIATE_SINGLETON_4(TYPE, THREADINGMODEL, CREATIONPOLICY, OBJECTLIFETIME) \
    template class HELLGROUND_DLL_DECL Hellground::Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, OBJECTLIFETIME >; \
    template<> TYPE* Hellground::Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, OBJECTLIFETIME >::si_instance = 0; \
    template<> bool Hellground::Singleton<TYPE, THREADINGMODEL, CREATIONPOLICY, OBJECTLIFETIME >::si_destroyed = false
#endif

