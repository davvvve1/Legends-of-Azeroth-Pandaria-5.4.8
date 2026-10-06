/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "GenericActions.h"

#include "CreatureAI.h"
#include "Playerbots.h"
#include "SpellAuraDefines.h"
#include "PvePetSpellSafety.h"

namespace
{
enum class PlayerbotMountMode : uint8
{
    None,
    Ground,
    Flying,
    Swimming
};

SpellEffectInfo const* GetMountedEffect(SpellInfo const* spellInfo)
{
    if (!spellInfo)
        return nullptr;

    for (SpellEffectInfo const& effect : spellInfo->Effects)
        if (effect.ApplyAuraName == SPELL_AURA_MOUNTED)
            return &effect;

    return nullptr;
}

PlayerbotMountMode GetMountMode(Player* player, SpellInfo const* mountSpell)
{
    SpellEffectInfo const* mountedEffect = GetMountedEffect(mountSpell);
    if (!player || !mountedEffect)
        return PlayerbotMountMode::None;

    MountCapabilityEntry const* capability =
        player->GetMountCapability(uint32(mountedEffect->MiscValueB));
    if (capability)
    {
        SpellInfo const* speedSpell =
            sSpellMgr->GetSpellInfo(capability->SpeedModSpell);
        if (speedSpell &&
            speedSpell->HasAura(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED))
            return PlayerbotMountMode::Flying;

        if (capability->Flags & MOUNT_FLAG_CAN_SWIM)
            return PlayerbotMountMode::Swimming;
    }

    if (mountSpell->HasAura(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) ||
        mountSpell->HasAura(SPELL_AURA_FLY))
        return PlayerbotMountMode::Flying;
    if (mountSpell->HasAura(SPELL_AURA_MOD_INCREASE_SWIM_SPEED))
        return PlayerbotMountMode::Swimming;

    return PlayerbotMountMode::Ground;
}

SpellInfo const* GetActiveMountSpell(Player* player)
{
    if (!player)
        return nullptr;

    Unit::AuraEffectList const& mountAuras =
        player->GetAuraEffectsByType(SPELL_AURA_MOUNTED);
    return mountAuras.empty() ? nullptr : mountAuras.front()->GetSpellInfo();
}

std::vector<uint32> FindMatchingMountSpells(PlayerbotAI* botAI, Player* bot,
    SpellInfo const* masterMount)
{
    SpellEffectInfo const* masterEffect = GetMountedEffect(masterMount);
    if (!botAI || !bot || !masterEffect)
        return {};

    Player* master = botAI->GetMaster();
    PlayerbotMountMode const masterMode = GetMountMode(master, masterMount);
    std::vector<std::pair<uint32, uint32>> rankedSpells;

    for (auto const& spellPair : bot->GetSpellMap())
    {
        PlayerSpell const* learned = spellPair.second;
        if (!learned || learned->state == PLAYERSPELL_REMOVED || !learned->active)
            continue;

        SpellInfo const* candidate = sSpellMgr->GetSpellInfo(spellPair.first);
        SpellEffectInfo const* candidateEffect = GetMountedEffect(candidate);
        if (!candidate || candidate->IsPassive() || !candidateEffect)
            continue;

        PlayerbotMountMode const candidateMode = GetMountMode(bot, candidate);
        uint32 score = 0;
        if (candidate->Id == masterMount->Id)
            score = 3000000;
        else if (masterEffect->MiscValueB &&
            candidateEffect->MiscValueB == masterEffect->MiscValueB)
            score = 2000000;
        else if (candidateMode != PlayerbotMountMode::None &&
            candidateMode == masterMode)
            score = 1000000;
        else
            continue;

        // Prefer a newer variant when several owned mounts are otherwise an
        // equally close match. Cast validation still decides whether the
        // chosen spell is usable at the bot's current location.
        score += std::min(candidate->Id, uint32(999999));
        rankedSpells.emplace_back(score, candidate->Id);
    }

    std::sort(rankedSpells.rbegin(), rankedSpells.rend());
    std::vector<uint32> matchingSpells;
    matchingSpells.reserve(rankedSpells.size());
    for (auto const& rankedSpell : rankedSpells)
        matchingSpells.push_back(rankedSpell.second);
    return matchingSpells;
}

bool IsPlayerbotPetThreatSpell(SpellInfo const* spellInfo)
{
    if (!spellInfo)
        return false;

    for (SpellEffectInfo const& effect : spellInfo->Effects)
    {
        if (effect.Effect == SPELL_EFFECT_ATTACK_ME ||
            effect.Effect == SPELL_EFFECT_THREAT ||
            effect.Effect == SPELL_EFFECT_THREAT_ALL ||
            effect.ApplyAuraName == SPELL_AURA_MOD_TAUNT ||
            effect.ApplyAuraName == SPELL_AURA_MOD_THREAT ||
            effect.ApplyAuraName == SPELL_AURA_MOD_TOTAL_THREAT)
            return true;
    }
    return false;
}
}

bool SyncMasterMountAction::isUseful()
{
    Player* master = botAI->GetMaster();
    if (!master || !master->IsInWorld() || master->GetMap() != bot->GetMap() ||
        !bot->IsAlive() || bot->IsInCombat() || bot->IsInFlight() ||
        botAI->IsInVehicle())
        return false;

    SpellInfo const* masterMount = GetActiveMountSpell(master);
    if (!masterMount)
        return bot->IsMounted();

    std::vector<uint32> const matchingSpells =
        FindMatchingMountSpells(botAI, bot, masterMount);
    if (matchingSpells.empty())
        return false;

    SpellInfo const* botMount = GetActiveMountSpell(bot);
    if (!botMount)
        return true;

    return GetMountMode(bot, botMount) != GetMountMode(master, masterMount);
}

bool SyncMasterMountAction::Execute(Event /*event*/)
{
    Player* master = botAI->GetMaster();
    if (!master || !master->IsInWorld() || master->GetMap() != bot->GetMap())
        return false;

    SpellInfo const* masterMount = GetActiveMountSpell(master);
    if (!masterMount)
    {
        if (!bot->IsMounted())
            return false;

        bot->RemoveAurasByType(SPELL_AURA_MOUNTED);
        if (bot->IsMounted())
            bot->Dismount();
        return true;
    }

    std::vector<uint32> const matchingSpells =
        FindMatchingMountSpells(botAI, bot, masterMount);
    if (matchingSpells.empty())
        return false;

    if (bot->IsMounted())
    {
        bot->RemoveAurasByType(SPELL_AURA_MOUNTED);
        if (bot->IsMounted())
            bot->Dismount();
        return true;
    }

    bot->StopMoving();
    botAI->RemoveShapeshift();
    for (uint32 matchingSpell : matchingSpells)
    {
        if (!botAI->CanCastSpell(matchingSpell, bot) ||
            !botAI->CastSpell(matchingSpell, bot))
            continue;

        TC_LOG_DEBUG("playerbots",
            "Synced master mount: bot=%s master=%s masterSpell=%u botSpell=%u",
            bot->GetName().c_str(), master->GetName().c_str(), masterMount->Id,
            matchingSpell);
        return true;
    }

    return false;
}

bool MeleeAction::isUseful()
{
    // do not allow if can't attack from vehicle
    if (botAI->IsInVehicle() && !botAI->IsInVehicle(false, false, true))
        return false;

    return true;
}

bool TogglePetSpellAutoCastAction::Execute(Event event)
{
    Pet* pet = bot->GetPet();
    if (!pet)
    {
        return false;
    }
    pet->SetReactState(REACT_PASSIVE);
    // hack on high level spell after low level initialization
    std::vector<unsigned int> shouldRemove;
    for (unsigned int& m_autospell : pet->m_autospells)
    {
        if (!pet->HasSpell(m_autospell))
        {
            shouldRemove.push_back(m_autospell);
        }
    }
    for (unsigned int spellId : shouldRemove)
    {
        auto autospellItr = std::find(pet->m_autospells.begin(), pet->m_autospells.end(), spellId);
        if (autospellItr != pet->m_autospells.end())
            pet->m_autospells.erase(autospellItr);
    }
    bool toggled = false;
    for (PetSpellMap::const_iterator itr = pet->m_spells.begin(); itr != pet->m_spells.end(); ++itr)
    {
        if (itr->second.state == PETSPELL_REMOVED)
            continue;

        uint32 spellId = itr->first;
        const SpellInfo* spellInfo = sSpellMgr->GetSpellInfo(spellId);
        if (!spellInfo || !spellInfo->IsAutocastable())
            continue;

        bool shouldApply = true;
        // spellId == 4511 || spellId == 54424 || spellId == 57564 || spellId == 57565 ||
        // spellId == 57566 || spellId == 57567 ||
        // cat stealth, prowl
        if (spellId == 1742 || spellId == 24450)
        {
            shouldApply = false;
        }
        // This maintenance action used to re-enable Growl/Suffering and undo
        // the PvE preparation performed by BotFactory. Keep taunt and direct
        // threat effects disabled for every Playerbot-controlled pet class.
        if (IsPlayerbotPetThreatSpell(spellInfo))
            shouldApply = false;
        if (botAI->IsGroupPveActivity() && IsPvePetRushSpell(spellInfo))
            shouldApply = false;
        bool isAutoCast = false;
        for (unsigned int& m_autospell : pet->m_autospells)
        {
            if (m_autospell == spellId)
            {
                isAutoCast = true;
                break;
            }
        }
        if (shouldApply != isAutoCast)
        {
            pet->ToggleAutocast(spellInfo, shouldApply);
            toggled = true;
        }
    }
    return toggled;
}

bool PetAttackAction::Execute(Event event)
{
    Guardian* pet = bot->GetGuardianPet();
    if (!pet || !pet->GetCharmInfo() || !pet->ToCreature()->IsAIEnabled)
    {
        return false;
    }

    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target)
    {
        return false;
    }

    if (!bot->IsValidAttackTarget(target))
    {
        return false;
    }

    // Recheck at execution time: the trigger and action can run on different
    // updates, and the pet must not inherit a target selected by somebody else.
    if (!botAI->CanPetEngageTarget(target))
    {
        return false;
    }

    pet->SetReactState(REACT_PASSIVE);
    pet->ClearUnitState(UNIT_STATE_FOLLOW);
    pet->AttackStop();
    pet->SetTarget(target->GetGUID());

    pet->GetCharmInfo()->SetIsCommandAttack(true);
    pet->GetCharmInfo()->SetIsAtStay(false);
    pet->GetCharmInfo()->SetIsFollowing(false);
    pet->GetCharmInfo()->SetIsCommandFollow(false);
    pet->GetCharmInfo()->SetIsReturning(false);

    pet->ToCreature()->AI()->AttackStart(target);
    return true;
}
