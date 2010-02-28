/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Boss_Warlord_Najentus
SD%Complete: 95
SDComment:
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "def_black_temple.h"

#define SAY_AGGRO                       -1564000
#define SAY_NEEDLE1                     -1564001
#define SAY_NEEDLE2                     -1564002
#define SAY_SLAY1                       -1564003
#define SAY_SLAY2                       -1564004
#define SAY_SPECIAL1                    -1564005
#define SAY_SPECIAL2                    -1564006
#define SAY_ENRAGE1                     -1564007            //is this text actually in use?
#define SAY_ENRAGE2                     -1564008
#define SAY_DEATH                       -1564009

//Spells
#define SPELL_NEEDLE_SPINE             39835
#define SPELL_TIDAL_BURST              39878
#define SPELL_TIDAL_SHIELD             39872
#define SPELL_IMPALING_SPINE           39837
#define SPELL_CREATE_NAJENTUS_SPINE    39956
#define SPELL_HURL_SPINE               39948
#define SPELL_BERSERK                  45078

#define GOBJECT_SPINE                  185584

struct TRINITY_DLL_DECL boss_najentusAI : public ScriptedAI
{
    boss_najentusAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        m_creature->GetPosition(wLoc);
    }

    ScriptedInstance* pInstance;

    uint32 NeedleSpineTimer;
    uint32 EnrageTimer;
    uint32 SpecialYellTimer;
    uint32 TidalShieldTimer;
    uint32 ImpalingSpineTimer;
    uint32 CheckTimer;

    uint8 plToNeedle;

    bool checkAura;

    WorldLocation wLoc;

    //       go_guid, target_guid
    std::map<uint64 , uint64> SpineTargetMap;

    void Reset()
    {
        EnrageTimer = 480000;
        SpecialYellTimer = 45000 + (rand()%76)*1000;

        ImpalingSpineTimer = 20000;
        NeedleSpineTimer   = 2000 + rand()%2000;
        TidalShieldTimer   = 60000;

        CheckTimer = 3000;

        plToNeedle = 3;
        checkAura = false;

        std::map<uint64, uint64>::iterator spineTarget = SpineTargetMap.begin();
        for(;spineTarget != SpineTargetMap.end(); ++spineTarget)
            if(GameObject *go = GameObject::GetGameObject(*m_creature, spineTarget->first))
            {
                go->SetLootState(GO_JUST_DEACTIVATED);
                go->SetRespawnTime(0);
            }

        SpineTargetMap.clear();

        if(pInstance)
            pInstance->SetData(DATA_HIGHWARLORDNAJENTUSEVENT, NOT_STARTED);
    }

    void KilledUnit(Unit *victim)
    {
        switch(rand()%2)
        {
        case 0: DoScriptText(SAY_SLAY1, m_creature); break;
        case 1: DoScriptText(SAY_SLAY2, m_creature); break;
        }
    }

    void JustDied(Unit *victim)
    {
        if(pInstance)
            pInstance->SetData(DATA_HIGHWARLORDNAJENTUSEVENT, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if(spell->Id == SPELL_HURL_SPINE && m_creature->HasAura(SPELL_TIDAL_SHIELD, 0))
        {
            m_creature->RemoveAurasDueToSpell(SPELL_TIDAL_SHIELD);
            m_creature->CastSpell(m_creature, SPELL_TIDAL_BURST, true);
        }
    }

    void Aggro(Unit *who)
    {
        if(pInstance)
            pInstance->SetData(DATA_HIGHWARLORDNAJENTUSEVENT, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
        DoZoneInCombat();
    }

    bool RemoveImpalingSpine(uint64 go_guid)
    {
        if(SpineTargetMap.empty())
            return false;

        std::map<uint64, uint64>::iterator spineTarget = SpineTargetMap.find(go_guid);
        if(spineTarget == SpineTargetMap.end())
            return false;

        Unit *target = Unit::GetUnit(*m_creature, spineTarget->second);
        if(target && target->HasAura(SPELL_IMPALING_SPINE, 1))
            target->RemoveAurasDueToSpell(SPELL_IMPALING_SPINE);
        SpineTargetMap.erase(spineTarget);
        return true;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (CheckTimer < diff)
        {
            if (m_creature->GetDistance(wLoc.x, wLoc.y, wLoc.z) > 105)
                EnterEvadeMode();
            else
                DoZoneInCombat();
            CheckTimer = 3000;
        }
        else
            CheckTimer -= diff;

        if (checkAura)
        {
            if(m_creature->HasAura(SPELL_TIDAL_SHIELD, 0))
                NeedleSpineTimer = 10000;
            else
                checkAura = false;
        }

        if(TidalShieldTimer < diff)
        {
            m_creature->CastSpell(m_creature, SPELL_TIDAL_SHIELD, true);

            ImpalingSpineTimer = 20000;

            TidalShieldTimer = 60000;
            checkAura = true;
        }
        else
            TidalShieldTimer -= diff;

        if(EnrageTimer < diff)
        {
            DoScriptText(SAY_ENRAGE2, m_creature);
            m_creature->CastSpell(m_creature, SPELL_BERSERK, true);
            EnrageTimer = 600000;
        }
        else
            EnrageTimer -= diff;

        if(NeedleSpineTimer < diff)
        {
            /*plToNeedle = 3;
            Map::PlayerList const &pList = m_creature->GetMap()->GetPlayers();
            std::list<Unit*> uList;

            //for (Map::PlayerList::iterator itr = pList.begin(); itr != pList.end(); itr = itr->next())
            for(Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
                if (!itr->getSource()->isGameMaster() && itr->getSource()->isAlive())
                    uList.push_back((Unit*)(itr->getSource()));

            if (!uList.empty())
            {
                if (uList.size() < 3)
                    plToNeedle = uList.size();

                Unit* target;
                for (int8 i = 0; i < plToNeedle; i++)
                {
                    int8 temp = rand()%(uList.size());
                    std::list<Unit*>::iterator itr = uList.begin();
                    int tmpi = 0;

                    while ( tmpi < temp)
                    {
                        itr++;
                        tmpi++;
                    }
                    target = (*itr);
                    uList.erase(itr);

                    m_creature->CastSpell(target, SPELL_NEEDLE_SPINE, false);
                }
            }*/
            m_creature->CastSpell(m_creature->getVictim(), 39992u, true);
            /*
            Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 1);
            if(!target)
                target = m_creature->getVictim();

            if(target)
                m_creature->CastSpell(target, SPELL_NEEDLE_SPINE, true);
            */
            NeedleSpineTimer = 2000 + rand()%2000;
        }
        else
            NeedleSpineTimer -= diff;

        if(SpecialYellTimer < diff)
        {
            switch(rand()%2)
            {
            case 0: DoScriptText(SAY_SPECIAL1, m_creature); break;
            case 1: DoScriptText(SAY_SPECIAL2, m_creature); break;
            }

            SpecialYellTimer = 25000 + (rand()%76)*1000;
        }
        else
            SpecialYellTimer -= diff;

        if(ImpalingSpineTimer < diff)
        {
            Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 1, 150, true);

            if(!target)
                target = m_creature->getVictim();

            if(target)
            {
                m_creature->CastSpell(target, SPELL_IMPALING_SPINE, true);

                //must let target summon, otherwise you cannot click the spine
                GameObject *_go = target->SummonGameObject(GOBJECT_SPINE, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), m_creature->GetOrientation(), 0, 0, 0, 0, 30);

                if(_go)
                {
                    std::pair<uint64, uint64> spineTarget(_go->GetGUID(), target->GetGUID());
                    SpineTargetMap.insert(spineTarget);
                }

                switch(rand()%2)
                {
                    case 0: DoScriptText(SAY_NEEDLE1, m_creature); break;
                    case 1: DoScriptText(SAY_NEEDLE2, m_creature); break;
                }
            }
            ImpalingSpineTimer = 21000;
        }
        else
            ImpalingSpineTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

bool GOHello_go_najentus_spine(Player *player, GameObject* _GO)
{
    if(ScriptedInstance* pInstance = (ScriptedInstance*)_GO->GetInstanceData())
        if(Creature* Najentus = Unit::GetCreature(*_GO, pInstance->GetData64(DATA_HIGHWARLORDNAJENTUS)))
            if(((boss_najentusAI*)Najentus->AI())->RemoveImpalingSpine(_GO->GetGUID()))
            {
                player->CastSpell(player, SPELL_CREATE_NAJENTUS_SPINE, true);
                _GO->SetLootState(GO_JUST_DEACTIVATED);
                _GO->SetRespawnTime(0);
            }
    return true;
}

CreatureAI* GetAI_boss_najentus(Creature *_Creature)
{
    return new boss_najentusAI (_Creature);
}

void AddSC_boss_najentus()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_najentus";
    newscript->GetAI = &GetAI_boss_najentus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_najentus_spine";
    newscript->pGOHello = &GOHello_go_najentus_spine;
    newscript->RegisterSelf();
}

