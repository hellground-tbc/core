/*
* Copyright (C) 2010 - 2014 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef SPELLMETHODS_H
#define SPELLMETHODS_H

namespace LuaSpell
{
    int GetCaster(lua_State* L, Spell* spell)
    {
        sEluna->Push(L, spell->GetCaster());
        return 1;
    }

    int GetCastTime(lua_State* L, Spell* spell)
    {
        sEluna->Push(L, spell->GetCastTime());
        return 1;
    }

    int GetId(lua_State* L, Spell* spell)
    {
        sEluna->Push(L, spell->GetSpellEntry()->Id);
        return 1;
    }

    int GetPowerCost(lua_State* L, Spell* spell)
    {
        sEluna->Push(L, spell->GetPowerCost());
        return 1;
    }

    int Cast(lua_State* L, Spell* spell)
    {
        bool skipCheck = sEluna->CHECKVAL<bool>(L, 2);
        spell->cast(skipCheck);
        return 0;
    }

    int IsAutoRepeat(lua_State* L, Spell* spell)
    {
        sEluna->Push(L, spell->IsAutoRepeat());
        return 1;
    }

    int SetAutoRepeat(lua_State* L, Spell* spell)
    {
        bool repeat = sEluna->CHECKVAL<bool>(L, 2);
        spell->SetAutoRepeat(repeat);
        return 0;
    }

    int cancel(lua_State* L, Spell* spell)
    {
        spell->cancel();
        return 0;
    }

    // Finish()
    int Finish(lua_State* L, Spell* spell)
    {
        spell->finish();
        return 0;
    }

    int GetTargetDest(lua_State* L, Spell* spell)
    {

        if (!spell->m_targets.HasDst())
            return 0;

        sEluna->Push(L, spell->m_targets.m_destX);
        sEluna->Push(L, spell->m_targets.m_destY);
        sEluna->Push(L, spell->m_targets.m_destZ);
        return 3;
    }

    int GetTarget(lua_State* L, Spell* spell)
    {

        if (GameObject* target = spell->m_targets.getGOTarget())
            sEluna->Push(sEluna->L, target);
        else if (Item* target = spell->m_targets.getItemTarget())
            sEluna->Push(sEluna->L, target);
        else if (Corpse* target = spell->m_targets.getCorpseTarget())
            sEluna->Push(sEluna->L, target);
        else if (Unit* target = spell->m_targets.getUnitTarget())
            sEluna->Push(sEluna->L, target);
        else
            sEluna->Push(sEluna->L);

        return 1;
    }
};
#endif
