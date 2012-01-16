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

#include "precompiled.h"
#include "Spell.h"

bool Spell_intimidating_shout_5246(Unit* pCaster, std::list<Unit*> &unitList, SpellCastTargets const& targets, SpellEntry const *pSpell, uint32 effect_index)
{
    if (effect_index == 0)
        return true;

    if (unitList.empty())
        return true;

    // remove current target from AOE Fear, AOE Speed aura our target gets stun effect provided by 1st effect
    unitList.remove(targets.getUnitTarget());
    return true;
}

// Warrior: Deep Wounds dummyeffect implementation: 12162, 12850, 12868
bool Spell_deep_wounds(Unit *pCaster, Unit* pUnit, Item* pItem, GameObject* pGameObject, SpellEntry const *pSpell, uint32 effectIndex)
{
    // handle only dummy efect
    if (pSpell->Effect[effectIndex] != SPELL_EFFECT_DUMMY)
        return false;

    if (!pUnit || pCaster->IsFriendlyTo(pUnit))
        return true;

    Unit* pTarget = pUnit;

    float damage;
    if (pCaster->haveOffhandWeapon() && pCaster->getAttackTimer(BASE_ATTACK) > pCaster->getAttackTimer(OFF_ATTACK))
        damage = (pCaster->GetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE) + pCaster->GetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE))/2;
    else
        damage = (pCaster->GetFloatValue(UNIT_FIELD_MINDAMAGE) + pCaster->GetFloatValue(UNIT_FIELD_MAXDAMAGE))/2;

    switch (pSpell->Id)
    {
        case 12162: damage *= 0.2f; break;
        case 12850: damage *= 0.4f; break;
        case 12868: damage *= 0.6f; break;
        default:
            // not handled spell assigned
            return false;
    };

    int32 deepWoundsDotBasePoints0 = int32(damage / 4);

    if (Aura *deepWounds = pUnit->GetAuraByCasterSpell(12721, pCaster->GetGUID()))
    {
        deepWounds->SetAuraDuration(deepWounds->GetAuraMaxDuration());
        deepWounds->UpdateAuraDuration();

        Aura *bloodFrenzy = pUnit->GetAuraByCasterSpell(30070, pCaster->GetGUID());
        bloodFrenzy ? bloodFrenzy = bloodFrenzy/* do nothing */: bloodFrenzy = pUnit->GetAuraByCasterSpell(30069, pCaster->GetGUID());

        if (bloodFrenzy)
        {
            bloodFrenzy->SetAuraDuration(deepWounds->GetAuraMaxDuration());
            bloodFrenzy->UpdateAuraDuration();
        }
        return true;
    }

    pCaster->CastCustomSpell(pTarget, 12721, &deepWoundsDotBasePoints0, NULL, NULL, true, NULL);
    // we handled our effect, returning true will prevent from processing effect by core :]
    return true;
}

void AddSC_spell_scripts()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "spell_intimidating_shout";
    newscript->pSpellTargetMap = &Spell_intimidating_shout_5246;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "spell_deep_wounds";
    newscript->pSpellHandleEffect = &Spell_deep_wounds;
    newscript->RegisterSelf();
}
