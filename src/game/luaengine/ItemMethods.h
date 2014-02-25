/*
* Copyright (C) 2010 - 2014 Eluna Lua Engine <http://emudevs.com/>
* This program is free software licensed under GPL version 3
* Please see the included DOCS/LICENSE.TXT for more information
*/

#ifndef ITEMMETHODS_H
#define ITEMMETHODS_H

namespace LuaItem
{
    int GetItemLink(lua_State* L, Item* item) // TODO: Implement
    {
        // LOCALE_enUS = 0,
        // LOCALE_koKR = 1,
        // LOCALE_frFR = 2,
        // LOCALE_deDE = 3,
        // LOCALE_zhCN = 4,
        // LOCALE_zhTW = 5,
        // LOCALE_esES = 6,
        // LOCALE_esMX = 7,
        // LOCALE_ruRU = 8

        int loc_idx = sEluna->CHECKVAL<int>(L, 2, LOCALE_enUS);
        if (loc_idx < 0 || loc_idx >= MAX_LOCALE)
            return luaL_argerror(L, 2, "valid LocaleConstant expected");

        const ItemPrototype* temp = item->GetProto();
        std::string name = temp->Name1;

        std::ostringstream oss;
        oss << std::dec <<
            "|Hitem:" << temp->ItemId << ":" <<
            item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT) << ":" <<
            item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT) << ":" <<
            item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_2) << ":" <<
            item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_3) << ":" <<
            item->GetEnchantmentId(BONUS_ENCHANTMENT_SLOT) << ":" <<
            item->GetItemRandomPropertyId() << ":" << item->GetItemSuffixFactor() << ":" <<
            (uint32)item->GetOwner()->getLevel() << "|h[" << name << "]|h|r";

        sEluna->Push(L, oss.str());
        return 1;
    }

    int GetGUID(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetGUIDLow());
        return 1;
    }

    int GetOwnerGUID(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetOwnerGUID());
        return 1;
    }

    int GetOwner(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetOwner());
        return 1;
    }

    int SetOwner(lua_State* L, Item* item)
    {
        Player* player = sEluna->CHECKOBJ<Player>(L, 2);

        item->SetOwnerGUID(player->GetGUID());
        return 0;
    }

    int SetBinding(lua_State* L, Item* item)
    {
        bool soulbound = sEluna->CHECKVAL<bool>(L, 2);

        item->SetBinding(soulbound);
        return 0;
    }

    int IsSoulBound(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->IsSoulBound());
        return 1;
    }

    int IsBoundByEnchant(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->IsBoundByEnchant());
        return 1;
    }

    int IsNotBoundToPlayer(lua_State* L, Item* item)
    {
        Player* player = sEluna->CHECKOBJ<Player>(L, 2);

        sEluna->Push(L, item->IsBindedNotWith(player->GetGUID()));
        return 1;
    }

    int IsBag(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->IsBag());
        return 1;
    }

    int IsBroken(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->IsBroken());
        return 1;
    }

    int CanBeTraded(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->CanBeTraded());
        return 1;
    }

    int IsInTrade(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->IsInTrade());
        return 1;
    }

    int GetCount(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetCount());
        return 1;
    }

    int SetCount(lua_State* L, Item* item)
    {
        uint32 count = sEluna->CHECKVAL<uint32>(L, 2);
        item->SetCount(count);
        return 0;
    }

    int GetMaxStackCount(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetMaxStackCount());
        return 1;
    }

    int GetSlot(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetSlot());
        return 1;
    }

    int GetBagSlot(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetBagSlot());
        return 1;
    }

    int IsInBag(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->IsInBag());
        return 1;
    }

    int IsEquipped(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->IsEquipped());
        return 1;
    }

    int HasQuest(lua_State* L, Item* item)
    {
        uint32 quest = sEluna->CHECKVAL<uint32>(L, 2);
        sEluna->Push(L, item->hasQuest(quest));
        return 1;
    }

    int IsPotion(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->IsPotion());
        return 1;
    }

    int IsConjuredConsumable(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->IsConjuredConsumable());
        return 1;
    }

    int SetEnchantment(lua_State* L, Item* item)
    {
        Player* owner = item->GetOwner();
        if (!owner)
        {
            sEluna->Push(L, false);
            return 1;
        }

        uint32 enchant = sEluna->CHECKVAL<uint32>(L, 2);
        if (!sSpellItemEnchantmentStore.LookupEntry(enchant))
        {
            sEluna->Push(L, false);
            return 1;
        }

        EnchantmentSlot slot = (EnchantmentSlot)sEluna->CHECKVAL<uint32>(L, 3);
        if (slot >= MAX_INSPECTED_ENCHANTMENT_SLOT)
            return luaL_argerror(L, 2, "valid EnchantmentSlot expected");

        owner->ApplyEnchantment(item, slot, false);
        item->SetEnchantment(slot, enchant, 0, 0);
        owner->ApplyEnchantment(item, slot, true);
        sEluna->Push(L, true);
        return 1;
    }

    int ClearEnchantment(lua_State* L, Item* item)
    {
        Player* owner = item->GetOwner();
        if (!owner)
        {
            sEluna->Push(L, false);
            return 1;
        }

        EnchantmentSlot slot = (EnchantmentSlot)sEluna->CHECKVAL<uint32>(L, 2);
        if (slot >= MAX_INSPECTED_ENCHANTMENT_SLOT)
            return luaL_argerror(L, 2, "valid EnchantmentSlot expected");

        if (!item->GetEnchantmentId(slot))
        {
            sEluna->Push(L, false);
            return 1;
        }

        owner->ApplyEnchantment(item, slot, false);
        item->ClearEnchantment(slot);
        sEluna->Push(L, true);
        return 1;
    }

    int GetGUIDLow(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetGUIDLow());
        return 1;
    }

    int GetEnchantmentId(lua_State* L, Item* item)
    {
        uint32 enchant_slot = sEluna->CHECKVAL<uint32>(L, 2);

        if (enchant_slot >= MAX_INSPECTED_ENCHANTMENT_SLOT)
            return 0;

        sEluna->Push(L, item->GetEnchantmentId(EnchantmentSlot(enchant_slot)));
        return 1;
    }

    int GetSpellId(lua_State* L, Item* item)
    {
        uint32 index = sEluna->CHECKVAL<uint32>(L, 2);
        if (index >= MAX_ITEM_PROTO_SPELLS)
            return luaL_argerror(L, 2, "valid SpellIndex expected");

        sEluna->Push(L, item->GetProto()->Spells[index].SpellId);
        return 1;
    }

    int GetSpellTrigger(lua_State* L, Item* item)
    {
        uint32 index = sEluna->CHECKVAL<uint32>(L, 2);
        if (index >= MAX_ITEM_PROTO_SPELLS)
            return luaL_argerror(L, 2, "valid SpellIndex expected");

        sEluna->Push(L, item->GetProto()->Spells[index].SpellTrigger);
        return 1;
    }

    int GetClass(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetProto()->Class);
        return 1;
    }

    int GetSubClass(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetProto()->SubClass);
        return 1;
    }

    int GetName(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetProto()->Name1);
        return 1;
    }

    int GetDisplayId(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetProto()->DisplayInfoID);
        return 1;
    }

    int GetQuality(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetProto()->Quality);
        return 1;
    }

    int GetBuyCount(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetProto()->BuyCount);
        return 1;
    }

    int GetBuyPrice(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetProto()->BuyPrice);
        return 1;
    }

    int GetSellPrice(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetProto()->SellPrice);
        return 1;
    }

    int GetInventoryType(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetProto()->InventoryType);
        return 1;
    }

    int GetAllowableClass(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetProto()->AllowableClass);
        return 1;
    }

    int GetAllowableRace(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetProto()->AllowableRace);
        return 1;
    }

    int GetItemLevel(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetProto()->ItemLevel);
        return 1;
    }

    int GetRequiredLevel(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetProto()->RequiredLevel);
        return 1;
    }

    int GetRandomProperty(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetProto()->RandomProperty);
        return 1;
    }

    int GetRandomSuffix(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetProto()->RandomSuffix);
        return 1;
    }

    int GetItemSet(lua_State* L, Item* item)
    {
        sEluna->Push(L, item->GetProto()->ItemSet);
        return 1;
    }

    int GetBagSize(lua_State* L, Item* item)
    {
        if (Bag* bag = item->IsBag() ? (Bag*)item : nullptr)
            sEluna->Push(L, bag->GetBagSize());
        else
            sEluna->Push(L, 0);
        return 1;
    }

    int SaveToDB(lua_State* L, Item* item)
    {
        item->SaveToDB();
        return 0;
    }
};
#endif
