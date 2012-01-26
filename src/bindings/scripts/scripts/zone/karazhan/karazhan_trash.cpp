#include "precompiled.h"
#include "def_karazhan.h"

#define SPELL_DANCE_VIBE            29521
#define SPELL_SEARING_PAIN          29492
#define SPELL_IMMOLATE              29928
#define SPELL_THROW                 29582
#define SPELL_IMPALE                29583
#define SPELL_GOBLIN_DRAGON_GUN     29513
#define SPELL_THROW_DYNAMITE        29579
#define SPELL_PUNCH                 29581
#define SPELL_CURSE_OF_AGONY        29930
#define SPELL_HEAL                  29580
#define SPELL_HOLY_NOVA             29514

#define GO_CHAIR                    183776


struct TRINITY_DLL_DECL mob_phantom_guestAI : public ScriptedAI
{
    mob_phantom_guestAI(Creature* c) : ScriptedAI(c) 
    {
        Type = urand(0, 4);   
    }

    uint32 Type;
    uint32 MainTimer;
    uint32 SecondaryTimer;

    void Reset()
    {
        me->CastSpell(me, SPELL_DANCE_VIBE, true);

        MainTimer = 0;
        SecondaryTimer = 5000;

        if(GameObject *chair = FindGameObject(GO_CHAIR, 3.0, me))
            chair->Use(me);
    }

    void AttackStart(Unit *who)
    {
        if(Type == 0 || Type == 1)
            ScriptedAI::AttackStartNoMove(who, CHECK_TYPE_CASTER);
        else
            ScriptedAI::AttackStart(who);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        if(MainTimer < diff)
        {
            switch(Type)
            {
            case 0:
                AddSpellToCast(SPELL_SEARING_PAIN, CAST_TANK);
                MainTimer = 3500;
                break;
            case 1:
                AddSpellToCast(SPELL_THROW, CAST_TANK);
                MainTimer = 2000;
                break;
            case 2:
                AddSpellToCast(SPELL_GOBLIN_DRAGON_GUN, CAST_SELF);
                MainTimer = 20000;
                break;
            case 3:
                AddSpellToCast(SPELL_PUNCH, CAST_TANK);
                MainTimer = 5000;
                break;
            case 4:
                AddSpellToCast(SPELL_HEAL, CAST_LOWEST_HP_FRIENDLY);
                MainTimer = 5000;
                break;
            }
        } 
        else
            MainTimer -= diff;

        if(SecondaryTimer < diff)
        {
            switch(Type)
            {
            case 0:
                AddSpellToCast(SPELL_IMMOLATE, CAST_RANDOM);
                SecondaryTimer = 7000;
                break;
            case 1:
                AddSpellToCast(SPELL_IMPALE, CAST_RANDOM);
                SecondaryTimer = 7000;
                break;
            case 2:
                AddSpellToCast(SPELL_THROW_DYNAMITE, CAST_RANDOM);
                SecondaryTimer = 9000;
                break;
            case 3:
                AddSpellToCast(SPELL_CURSE_OF_AGONY, CAST_RANDOM);
                SecondaryTimer = 7000;
                break;
            case 4:
                AddSpellToCast(SPELL_HOLY_NOVA, CAST_SELF);
                SecondaryTimer = 10000;
                break;
            }
        }
        else
            SecondaryTimer -= diff;

        if(Type == 0 || Type == 1)
            CheckCasterNoMovementInRange(diff, 20.0);
        CastNextSpellIfAnyAndReady(diff);
        DoMeleeAttackIfReady();
    }    
};

CreatureAI* GetAI_mob_phantom_guest(Creature *_Creature)
{
    return new mob_phantom_guestAI(_Creature);
}


bool Spell_charge(const Aura* aura, bool apply)
{
    if(!apply)
    {
        if(Unit* caster = aura->GetCaster())
            caster->CastSpell(aura->GetTarget(), 29321, true);      // trigger fear after charge
    }

    return true;
}


void AddSC_karazhan_trash()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "spell_charge_29320";
    newscript->pEffectAuraDummy = &Spell_charge;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_phantom_guest";
    newscript->GetAI = &GetAI_mob_phantom_guest;
    newscript->RegisterSelf();
}

