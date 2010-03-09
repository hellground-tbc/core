#include "PlayerAI.h"
#include "Player.h"
#include "Database/DBCStores.h"

class Player;
struct SpellEntry;


SpellEntry const *PlayerAI::selectHighestRank(uint32 spell_id)
{
    SpellEntry const *spell_info   = sSpellStore.LookupEntry(spell_id);
    if (!spell_info)
        return NULL;

    PlayerSpellMap const &sp_list = me->GetSpellMap();

    SpellEntry const *highest_rank = spell_info;
    for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
    {
        if(!itr->second->active || itr->second->disabled || itr->second->state == PLAYERSPELL_REMOVED)
            continue;

        spell_info = sSpellStore.LookupEntry(itr->first);
        if (!spell_info)
            continue;

        if (highest_rank->SpellFamilyName == spell_info->SpellFamilyName && (highest_rank->SpellFamilyFlags & spell_info->SpellFamilyFlags))
        {
            if (spell_info->spellLevel > highest_rank->spellLevel)
                highest_rank = spell_info;
        }
    }
    return highest_rank;
}

// TODO: poprzenosiæ do nowych plików i oskryptowaæ? :P
struct WarriorAI: public PlayerAI
{
    WarriorAI(Player *pPlayer): PlayerAI(pPlayer)
    {
    }

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
    }
};

struct HunterAI: public PlayerAI
{
    HunterAI(Player *pPlayer): PlayerAI(pPlayer)
    {
    }

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
    }
};
struct PaladinAI: public PlayerAI
{
    PaladinAI(Player *pPlayer): PlayerAI(pPlayer)
    {
    }

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
    }
};
struct WarlockAI: public PlayerAI
{
    WarlockAI(Player *pPlayer): PlayerAI(pPlayer)
    {
    }

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
    }
};
struct DruidAI: public PlayerAI
{
    DruidAI(Player *pPlayer): PlayerAI(pPlayer)
    {
    }

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
    }
};

struct RogueAI: public PlayerAI
{
    RogueAI(Player *pPlayer): PlayerAI(pPlayer)
    {
    }

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
    }
};

struct ShamanAI: public PlayerAI
{
    ShamanAI(Player *pPlayer): PlayerAI(pPlayer)
    {
    }

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
    }
};

struct PriestAI: public PlayerAI
{
    PriestAI(Player *pPlayer): PlayerAI(pPlayer)
    {
    }

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
    }
};

struct MageAI: public PlayerAI
{
    MageAI(Player *plr): PlayerAI(plr)
    {
    }

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
    }
};