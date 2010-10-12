/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

#include "precompiled.h"
#include "def_serpent_shrine.h"
#include "../../../creature/simple_ai.h"
#include "Spell.h"

#define SPELL_SPOUT         37433
#define SPELL_KNOCKBACK     19813
#define SPELL_GEYSER        37478
#define SPELL_WHIRL         37363
#define SPELL_WATERBOLT     37138
#define SPELL_SUBMERGE      37550
#define SPELL_EMERGE        20568

#define EMOTE_SPOUT "takes a deep breath."

#define SPOUT_DIST 100

#define MOB_COILFANG_GUARDIAN 21873
#define MOB_COILFANG_AMBUSHER 21865

//Ambusher spells
#define SPELL_SPREAD_SHOT   37790
#define SPELL_SHOOT         37770

//Guardian spells
#define SPELL_ARCINGSMASH   38761 // Wrong SpellId. Can't find the right one.
#define SPELL_HAMSTRING     26211

#define TRIGGER             26515

float AddPos[9][3] =
{
    {2.8553810, -459.823914, -19.182686},   //MOVE_AMBUSHER_1 X, Y, Z
    {12.400000, -466.042267, -19.182686},   //MOVE_AMBUSHER_2 X, Y, Z
    {51.366653, -460.836060, -19.182686},   //MOVE_AMBUSHER_3 X, Y, Z
    {62.597980, -457.433044, -19.182686},   //MOVE_AMBUSHER_4 X, Y, Z
    {77.607452, -384.302765, -19.182686},   //MOVE_AMBUSHER_5 X, Y, Z
    {63.897900, -378.984924, -19.182686},   //MOVE_AMBUSHER_6 X, Y, Z
    {34.447250, -387.333618, -19.182686},   //MOVE_GUARDIAN_1 X, Y, Z
    {14.388216, -423.468018, -19.625271},   //MOVE_GUARDIAN_2 X, Y, Z
    {42.471519, -445.115295, -19.769423}    //MOVE_GUARDIAN_3 X, Y, Z
};

#define ROTATE_STEP M_PI*2/40
struct Rotator
{
    Rotator(Unit *pOwner): m_owner(pOwner), m_timer(500), m_count(0), m_rotating(false), m_inlineGUID(0)
    {
    }

    void start()
    {
        m_rotateLeft = bool(urand(0, 1));
        m_rotating = true;
    }

    void end()
    {
        m_rotating = false;
        m_inlineGUID = 0;
    }

    void castspout()
    {
        Map *pMap = m_owner->GetMap();
    }

    bool update(const uint32 diff)
    {
        if(!m_rotating)
            return false;

        if(m_timer > diff)
        {
            m_timer = 500;
        }
        else
            m_timer -= diff;

        return true;
    }
      
    bool m_rotateLeft;
    bool m_rotating;

    uint32 m_timer;
    uint32 m_count;

    uint64 m_inlineGUID;

    Unit *m_owner;
};

struct TRINITY_DLL_DECL boss_the_lurker_belowAI : public Scripted_NoMovementAI
{
    boss_the_lurker_belowAI(Creature *c) : Scripted_NoMovementAI(c), Summons(m_creature)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;
    SummonList Summons;

    void Reset()
    {
        if(pInstance && GetData(DATA_THELURKERBELOWEVENT) != DONE)
        {
            pInstance->SetData(DATA_THELURKERBELOWEVENT, NOT_STARTED);
            pInstance->SetData(DATA_STRANGE_POOL, NOT_STARTED);
        }

        m_creature->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_HASTE_SPELLS, true);
        m_creature->ApplySpellImmune(1, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
    }
    void EnterCombat(Unit *who)
    {
        if (pInstance)
            pInstance->SetData(DATA_THELURKERBELOWEVENT, IN_PROGRESS);

        Scripted_NoMovementAI::AttackStart(who);
    }

    void JustDied(Unit* Killer)
    {
        if (pInstance)
            pInstance->SetData(DATA_THELURKERBELOWEVENT, DONE);

        Summons.DespawnAll();
    }

    void MoveInLineOfSight(Unit *who)
    {
    }

    void UpdateAI(const uint32 diff)
    {
    }
 };

struct TRINITY_DLL_DECL mob_coilfang_ambusherAI : public Scripted_NoMovementAI
{
    mob_coilfang_ambusherAI(Creature *c) : Scripted_NoMovementAI(c)
    {
        SpellEntry *TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_SHOOT);
        if(TempSpell)
            TempSpell->Effect[0] = 2;
    }

    uint32 MultiShotTimer;
    uint32 ShootBowTimer;

    void Reset()
    {
        MultiShotTimer = 10000;
        ShootBowTimer = 4000;
    }

    void EnterCombat(Unit *who)
    {
    }

    void MoveInLineOfSight(Unit *who)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if(MultiShotTimer < diff)
        {
        }
        else
            MultiShotTimer -= diff;

        if(ShootBowTimer < diff)
        {
        }
        else
            ShootBowTimer -= diff;
    }
};

CreatureAI* GetAI_mob_coilfang_ambusher(Creature* pCreature)
{
    return new mob_coilfang_ambusherAI (pCreature);
}

CreatureAI* GetAI_boss_the_lurker_below(Creature* pCreature)
{
    return new boss_the_lurker_belowAI (pCreature);
}

void AddSC_boss_the_lurker_below()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_the_lurker_below";
    newscript->GetAI = &GetAI_boss_the_lurker_below;
    newscript->RegisterSelf();

    /*newscript = new Script;
    newscript->Name="mob_coilfang_guardian";
    newscript->GetAI = &GetAI_mob_coilfang_guardian;
    newscript->RegisterSelf();*/

    newscript = new Script;
    newscript->Name="mob_coilfang_ambusher";
    newscript->GetAI = &GetAI_mob_coilfang_ambusher;
    newscript->RegisterSelf();
}


