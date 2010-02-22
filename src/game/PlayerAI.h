#ifndef TRINITY_PLAYERAI_H
#define TRINITY_PLAYERAI_H

#include "CreatureAI.h"
#include "Unit.h"

class Unit;

class TRINITY_DLL_SPEC PlayerAI : public UnitAI
{
    protected:
        Unit *me;

        void OnCharmed(bool apply){}
        bool UpdateVictim();
public:
        PlayerAI(Player *p) : UnitAI((Unit*)p), me((Unit*)p) {}
};


struct TRINITY_DLL_DECL MageAI : public PlayerAI
{
    MageAI(Player *p) : PlayerAI(p){}

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL RogueAI : public PlayerAI
{
    RogueAI(Player *p) : PlayerAI(p){}

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL WarriorAI : public PlayerAI
{
    WarriorAI(Player *p) : PlayerAI(p){}

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL PaladinAI : public PlayerAI
{
    PaladinAI(Player *p) : PlayerAI(p){}

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL ShamanAI : public PlayerAI
{
    ShamanAI(Player *p) : PlayerAI(p){}

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL DruidAI : public PlayerAI
{
    DruidAI(Player *p) : PlayerAI(p){}

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL WarlockAI : public PlayerAI
{
    WarlockAI(Player *p) : PlayerAI(p){}

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL PriestAI : public PlayerAI
{
    PriestAI(Player *p) : PlayerAI(p){}

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL HunterAI : public PlayerAI
{
    HunterAI(Player *p) : PlayerAI(p){}

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if(!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

#endif