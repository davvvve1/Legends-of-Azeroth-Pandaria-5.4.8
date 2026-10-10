#include "InstanceMechanicsAction.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

#include "Creature.h"
#include "Group.h"
#include "GroupPveCombat.h"
#include "Map.h"
#include "MotionMaster.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "PlayerbotSpec.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellMgr.h"

namespace
{
constexpr uint32 MapMogushanPalace = 994;
constexpr uint32 MapThroneOfThunder = 1098;
constexpr uint32 MapSiegeOfOrgrimmar = 1136;

constexpr uint32 NpcGekkan = 61243;
constexpr uint32 NpcGlintrokIronhide = 61337;
constexpr uint32 NpcGlintrokSkulker = 61338;
constexpr uint32 NpcGlintrokOracle = 61339;
constexpr uint32 NpcGlintrokHexxer = 61340;
constexpr uint32 NpcMuShiba = 61453;
constexpr uint32 SpellRavage = 119948;

struct AuraRule
{
    uint32 map;
    uint32 spell;
    float distance;
};

struct SwapRule
{
    uint32 spell;
    uint8 stacks;
};

// Carrier mechanics verified against the local 5.4.8 encounter scripts and
// the original encounter guides. Generic spell-shape inspection below extends
// spread handling to older instances without requiring a row for every DoT.
constexpr AuraRule SpreadAuras[] =
{
    { MapThroneOfThunder, 137194, 14.0f }, // Focused Lightning target
    { MapThroneOfThunder, 136992, 10.0f }, // Biting Cold
    { MapThroneOfThunder, 139822, 12.0f }, // Cinders
    { MapThroneOfThunder, 134626, 12.0f }, // Lingering Gaze mark
    { MapThroneOfThunder, 136425, 10.0f }, // Storm Cloud
    { MapThroneOfThunder, 138707, 10.0f }, // Anima Font
    { MapThroneOfThunder, 135991, 10.0f }, // Diffusion Chain
    { MapSiegeOfOrgrimmar, 143434, 10.0f }, // Shadow Word: Bane
    { MapSiegeOfOrgrimmar, 142913, 10.0f }, // Displaced Energy
    { MapSiegeOfOrgrimmar, 144089, 9.0f },  // Toxic Mist
    { MapSiegeOfOrgrimmar, 147068, 10.0f }  // Flames of Galakrond mark
};

constexpr AuraRule KiteAuras[] =
{
    { MapThroneOfThunder, 139857, 28.0f }, // Torrent of Ice
    { MapThroneOfThunder, 140946, 25.0f }, // Dire Fixation
    { MapSiegeOfOrgrimmar, 143445, 32.0f }, // Thok fixate
    { MapSiegeOfOrgrimmar, 148243, 25.0f }, // Galakras add fixate
    { MapSiegeOfOrgrimmar, 146581, 30.0f }, // Thok creature fixate
    { MapSiegeOfOrgrimmar, 143292, 24.0f }, // He Softfoot fixate
    { MapSiegeOfOrgrimmar, 143828, 25.0f }  // Siegecrafter Locked On
};

constexpr AuraRule StackAuras[] =
{
    { MapThroneOfThunder, 136922, 4.0f }, // Frostbite
    { MapThroneOfThunder, 135695, 4.0f }, // Static Shock
    { MapThroneOfThunder, 136295, 4.0f }, // Overcharged
    { MapSiegeOfOrgrimmar, 147068, 4.0f }, // Flames of Galakrond soak
    { MapSiegeOfOrgrimmar, 142948, 4.0f }  // Aim
};

constexpr SwapRule TankSwapAuras[] =
{
    { 138349, 2 }, // Static Wound
    { 136767, 2 }, // Triple Puncture
    { 139840, 2 }, // Rot Armor
    { 136050, 3 }, // Malformed Blood
    { 138569, 3 }, // Explosive Slam
    { 134691, 3 }, // Impale
    { 143436, 2 }, // Corrosive Blast
    { 144358, 1 }, // Wounded Pride
    { 144467, 3 }, // Ignite Armor
    { 144215, 5 }, // Froststorm Strike
    { 143494, 3 }, // Sundering Blow
    { 143385, 3 }, // Electrostatic Charge
    { 143339, 3 }, // Injection
    { 145183, 3 }  // Gripping Despair
};

constexpr uint32 StopAttackAuras[] =
{
    137149, // Kazra'jin Overload
    137166, // Kazra'jin Discharge
    143593  // General Nazgrim Defensive Stance
};

constexpr uint32 DefensiveCasts[] =
{
    136894, // Sandstorm
    134380, // Quills
    138763, // Interrupting Jolt
    136146, // Fist Smash
    137401, // Lightning Storm
    143491, // Calamity
    144400, // Swelling Pride
    147042, // Pulsing Flames
    143973, // Falling Ash
    142816, // Breath of Y'Shaarj
    143766, // Deafening Screech/Panic phase
    144969, // Hurl Corruption
    144989  // Whirling Corruption
};

// Adds which determine whether their encounters succeed. The action considers
// only alive, attackable, already-engaged units, so this list cannot start an
// unrelated pack or attack passive encounter helpers.
constexpr uint32 PriorityAdds[] =
{
    // Gekkan: Iron Protector makes nearby allies take 50% less damage, then
    // Hex of Lethargy cripples casters.  Keep Gekkan for last so Inspiring
    // Cry cannot empower surviving followers.  This ordering is shared by
    // normal, heroic and challenge mode because map and creature entries are
    // identical across those difficulties.
    NpcGlintrokIronhide, NpcGlintrokHexxer, NpcGlintrokSkulker,
    NpcGlintrokOracle,
    // Mogu'shan Palace: killing Mu'Shiba ends Ravage early.
    NpcMuShiba,
    // Throne of Thunder
    69221, 69164, 69176, 69548, 69480, 67966, 68497, 70095,
    68192, 68193, 70134, 69069, 69070, 69701, 69700, 69699,
    69702, 69957, 69958,
    // Siege of Orgrimmar
    71603, 71478, 71476, 71481, 71477, 71474, 71482, 71946,
    72958, 72945, 72947, 72050, 71773, 71626, 71644, 71397,
    71393, 71395, 71405, 71658, 71591, 71788, 71542, 71420,
    71984, 71983, 72154, 72198, 72272
};

bool HasAura(Unit const* unit, uint32 spell)
{
    return unit && unit->HasAura(spell);
}

uint8 AuraStacks(Unit const* unit, uint32 spell)
{
    Aura const* aura = unit ? unit->GetAura(spell) : nullptr;
    return aura ? aura->GetStackAmount() : 0;
}

Spell* CurrentSpell(Unit* unit)
{
    if (!unit)
        return nullptr;
    if (Spell* spell = unit->GetCurrentSpell(CURRENT_GENERIC_SPELL))
        return spell;
    return unit->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
}

bool IsHealingCast(Unit* unit)
{
    Spell* spell = CurrentSpell(unit);
    SpellInfo const* info = spell ? spell->GetSpellInfo() : nullptr;
    if (!info)
        return false;

    for (SpellEffectInfo const& effect : info->Effects)
        if (effect.IsEffect() &&
            (effect.Effect == SPELL_EFFECT_HEAL ||
             effect.Effect == SPELL_EFFECT_HEAL_MAX_HEALTH ||
             effect.Effect == SPELL_EFFECT_HEAL_MECHANICAL ||
             effect.Effect == SPELL_EFFECT_HEAL_PCT))
            return true;
    return false;
}

bool IsFrontalSpell(SpellInfo const* info)
{
    if (!info)
        return false;
    for (SpellEffectInfo const& effect : info->Effects)
        if (effect.IsEffect() &&
            (effect.TargetA.GetSelectionCategory() == TARGET_SELECT_CATEGORY_CONE ||
             effect.TargetB.GetSelectionCategory() == TARGET_SELECT_CATEGORY_CONE) &&
            (effect.TargetA.GetCheckType() == TARGET_CHECK_ENEMY ||
             effect.TargetB.GetCheckType() == TARGET_CHECK_ENEMY))
            return true;
    return false;
}

bool SpellHasHostileArea(SpellInfo const* info, uint8 depth = 0)
{
    if (!info)
        return false;
    for (SpellEffectInfo const& effect : info->Effects)
    {
        auto hostileArea = [](SpellImplicitTargetInfo const& target)
        {
            return target.IsArea() && target.GetCheckType() == TARGET_CHECK_ENEMY;
        };
        if (effect.IsEffect() &&
            (hostileArea(effect.TargetA) || hostileArea(effect.TargetB)))
            return true;
        if (depth < 2 && effect.TriggerSpell && SpellHasHostileArea(
            sSpellMgr->GetSpellInfo(effect.TriggerSpell), depth + 1))
            return true;
    }
    return false;
}

bool AuraSuggestsSpread(Aura const* aura)
{
    SpellInfo const* info = aura ? aura->GetSpellInfo() : nullptr;
    if (!info || info->IsPositive() || !SpellHasHostileArea(info))
        return false;

    // Ignore permanent encounter-wide controllers. A personal carrier aura
    // has a finite duration; persistent floor effects are handled separately
    // by AvoidAoeAction.
    return aura->GetDuration() > 0 && aura->GetMaxDuration() <= 120000;
}

bool IsListed(uint32 value, uint32 const* begin, uint32 const* end)
{
    return std::find(begin, end, value) != end;
}

Player* GroupTank(Player* bot)
{
    Group* group = bot ? bot->GetGroup(GroupSlot::Instance) : nullptr;
    if (!group && bot)
        group = bot->GetGroup();
    if (!group)
        return nullptr;
    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        if (Player* member = ref->GetSource())
            if (member->IsAlive() && member->GetMap() == bot->GetMap() &&
                PlayerBotSpec::IsMainTank(member))
                return member;
    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        if (Player* member = ref->GetSource())
            if (member->IsAlive() && member->GetMap() == bot->GetMap() &&
                PlayerBotSpec::IsTank(member, true))
                return member;
    return nullptr;
}
}

bool InstanceMechanics::ShouldTankSwap(Player* bot, Unit* boss)
{
    if (!bot || !boss || !PlayerBotSpec::IsTank(bot, true) ||
        boss->GetVictim() == bot)
        return false;
    Player* currentTank = boss->GetVictim() ? boss->GetVictim()->ToPlayer() : nullptr;
    if (!currentTank || !PlayerBotSpec::IsTank(currentTank, true))
        return false;
    for (SwapRule const& rule : TankSwapAuras)
        if (AuraStacks(currentTank, rule.spell) >= rule.stacks)
            return true;
    return false;
}

Unit* InstanceMechanicsAction::FindEncounterBoss() const
{
    Unit* current = context->GetValue<Unit*>("current target")->Get();
    if (current && current->IsAlive() && current->ToCreature() &&
        (current->ToCreature()->IsDungeonBoss() || current->ToCreature()->isWorldBoss()))
        return current;

    Group* group = bot->GetGroup(GroupSlot::Instance);
    if (!group)
        group = bot->GetGroup();
    if (group)
        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
            if (Player* member = ref->GetSource())
                for (Unit* attacker : member->getAttackers())
                    if (Creature* creature = attacker ? attacker->ToCreature() : nullptr)
                        if (creature->IsAlive() &&
                            (creature->IsDungeonBoss() || creature->isWorldBoss()))
                            return creature;

    GuidVector const& targets = context->GetValue<GuidVector>(
        "possible targets")->Get();
    for (ObjectGuid const& guid : targets)
        if (Creature* creature = botAI->GetCreature(guid))
            if (creature->IsAlive() && creature->IsInCombat() &&
                (creature->IsDungeonBoss() || creature->isWorldBoss()))
                return creature;
    return nullptr;
}

Unit* InstanceMechanicsAction::FindPriorityAdd(Unit* boss) const
{
    GuidVector const& targets = context->GetValue<GuidVector>(
        "possible targets")->Get();
    Unit* best = nullptr;
    size_t bestRank = std::size(PriorityAdds) + 1;
    for (ObjectGuid const& guid : targets)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || unit == boss || !unit->IsAlive() || !unit->IsInCombat() ||
            !bot->IsValidAttackTarget(unit) || !bot->IsWithinLOSInMap(unit))
            continue;
        auto const found = std::find(std::begin(PriorityAdds),
            std::end(PriorityAdds), unit->GetEntry());
        // Known encounter-critical targets retain their documented ordering.
        // In encounters which do not yet have a dedicated row, an engaged
        // add currently casting a heal is still a safer switch than tunneling
        // the boss.  This generic fallback applies to every dungeon/raid and
        // never pulls an idle pack.
        bool const genericHealer = found == std::end(PriorityAdds) &&
            IsHealingCast(unit);
        if (found == std::end(PriorityAdds) && !genericHealer)
            continue;
        size_t const rank = genericHealer ? std::size(PriorityAdds) :
            size_t(std::distance(std::begin(PriorityAdds), found));
        if (!best || rank < bestRank ||
            (rank == bestRank && bot->GetDistance(unit) < bot->GetDistance(best)))
        {
            best = unit;
            bestRank = rank;
        }
    }
    return best;
}

Unit* InstanceMechanicsAction::FindMindControlledMember() const
{
    if (bot->GetMapId() != MapSiegeOfOrgrimmar)
        return nullptr;

    Group* group = bot->GetGroup(GroupSlot::Instance);
    if (!group)
        group = bot->GetGroup();
    if (!group)
        return nullptr;

    // Garrosh's Touch of Y'Shaarj is removed by damaging the controlled
    // player. Both normal and empowered variants have separate player auras.
    constexpr uint32 touchAuras[] = { 145071, 145175, 145599 };
    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        if (Player* member = ref->GetSource())
            if (member != bot && member->IsAlive() &&
                member->GetMap() == bot->GetMap() &&
                bot->IsValidAttackTarget(member))
                for (uint32 spell : touchAuras)
                    if (member->HasAura(spell))
                        return member;
    return nullptr;
}

Unit* InstanceMechanicsAction::FindFriendlyEncounterUnit() const
{
    if (!PlayerBotSpec::IsHeal(bot, true))
        return nullptr;

    // Ravage deals physical damage every second for ten seconds.  Keep its
    // victim alive while the damage dealers kill Mu'Shiba to end it early.
    if (bot->GetMapId() == MapMogushanPalace)
    {
        Group* group = bot->GetGroup(GroupSlot::Instance);
        if (!group)
            group = bot->GetGroup();
        if (group)
            for (GroupReference* ref = group->GetFirstMember(); ref;
                ref = ref->next())
                if (Player* member = ref->GetSource())
                    if (member->IsAlive() && member->GetMap() == bot->GetMap() &&
                        member->HasAura(SpellRavage))
                        return member;
    }

    // Immerseus contaminated puddles and Tsulong's day phase are the two MoP
    // raid objectives whose progress explicitly requires healing an NPC.
    uint32 const entries[] = { 71604u, 73260u, 62442u };
    for (uint32 entry : entries)
        if (Creature* target = bot->FindNearestCreature(entry, 120.0f, true))
            if (target->IsFriendlyTo(bot) && target->GetHealthPct() < 98.0f &&
                bot->IsWithinLOSInMap(target))
                return target;
    return nullptr;
}

InstanceMechanicsAction::Plan InstanceMechanicsAction::BuildPlan() const
{
    Plan plan;
    if (!bot || !bot->IsAlive() || !bot->IsInCombat() || !bot->GetMap() ||
        !bot->GetMap()->IsDungeon())
        return plan;

    if (Unit* friendly = FindFriendlyEncounterUnit())
        return { Reaction::HealEncounterUnit, nullptr, friendly, 0.0f };

    if (Unit* controlled = FindMindControlledMember())
        return { Reaction::BreakMindControl, nullptr, controlled, 0.0f };

    Unit* boss = FindEncounterBoss();
    if (!boss)
        return plan;

    if (InstanceMechanics::ShouldTankSwap(bot, boss))
        return { Reaction::TankSwap, boss, boss, 0.0f };

    uint32 const map = bot->GetMapId();
    for (AuraRule const& rule : KiteAuras)
        if (rule.map == map && HasAura(bot, rule.spell))
            return { Reaction::Kite, boss, nullptr, rule.distance };

    for (AuraRule const& rule : SpreadAuras)
        if (rule.map == map && HasAura(bot, rule.spell))
            return { Reaction::Spread, nullptr, nullptr, rule.distance };

    for (auto const& auraPair : bot->GetAppliedAuras())
        if (AuraApplication const* application = auraPair.second)
            if (AuraSuggestsSpread(application->GetBase()))
                return { Reaction::Spread, nullptr, nullptr, 9.0f };

    Group* group = bot->GetGroup(GroupSlot::Instance);
    if (!group)
        group = bot->GetGroup();
    if (group)
        for (AuraRule const& rule : StackAuras)
            if (rule.map == map)
                for (GroupReference* ref = group->GetFirstMember(); ref;
                    ref = ref->next())
                    if (Player* member = ref->GetSource())
                        if (member->IsAlive() && member->GetMap() == bot->GetMap() &&
                            HasAura(member, rule.spell) && member != bot &&
                            boss->GetVictim() != bot)
                            return { Reaction::Stack, member, nullptr, rule.distance };

    if (Creature* creature = boss->ToCreature())
        for (uint32 spell : StopAttackAuras)
            if (creature->HasAura(spell))
            {
                if (Unit* add = FindPriorityAdd(boss))
                    return { Reaction::FocusAdd, nullptr, add, 0.0f };
                return { Reaction::StopAttack, boss, nullptr, 0.0f };
            }

    bool const bossUnavailable = !bot->IsValidAttackTarget(boss) ||
        boss->HasUnitFlag(UNIT_FLAG_NON_ATTACKABLE) ||
        boss->HasUnitFlag(UNIT_FLAG_NOT_SELECTABLE);
    if (Unit* add = FindPriorityAdd(boss))
    {
        bool const gekkanEntourage = map == MapMogushanPalace &&
            boss->GetEntry() == NpcGekkan;
        // Gekkan's stationary casters and melee followers must be gathered by
        // the tank as well as focused by damage dealers.  Healers retain their
        // healing target while the encounter layer coordinates the rest.
        if (bossUnavailable || PlayerBotSpec::IsDps(bot, true) ||
            (gekkanEntourage && !PlayerBotSpec::IsHeal(bot, true)))
            return { Reaction::FocusAdd, nullptr, add, 0.0f };
    }

    if (Spell* cast = CurrentSpell(boss))
    {
        SpellInfo const* info = cast->GetSpellInfo();
        if (info && IsListed(info->Id, std::begin(DefensiveCasts),
            std::end(DefensiveCasts)))
            return { Reaction::Defensive, boss, nullptr, 0.0f };

        bool explicitFrontal = info &&
            (info->Id == 136741 || info->Id == 133776 ||
             info->Id == 145226 || info->Id == 142815 ||
             info->Id == 147688 || info->Id == 144316);
        bool const activeTank = boss->GetVictim() == bot &&
            PlayerBotSpec::IsTank(bot, true);
        if (!activeTank && info && (explicitFrontal || IsFrontalSpell(info)) &&
            boss->HasInArc(float(M_PI) * 0.65f, bot))
            return { Reaction::AvoidFrontal, boss, nullptr, 0.0f };
    }

    return plan;
}

bool InstanceMechanicsAction::UseDefensive()
{
    constexpr uint32 defensiveSpells[] =
    {
        871, 118038, 498, 642, 19263, 31224, 47585, 48707, 48792,
        108271, 30823, 11426, 104773, 108416, 122783, 115203, 22812,
        61336, 55233, 86659
    };
    for (uint32 spell : defensiveSpells)
        if (bot->HasAura(spell))
            return true;
    for (uint32 spell : defensiveSpells)
        if (bot->HasSpell(spell) && botAI->CastSpell(spell, bot))
            return true;
    return false;
}

bool InstanceMechanicsAction::HealEncounterUnit(Unit* target)
{
    if (!target || !PlayerBotSpec::IsHeal(bot, true))
        return false;
    for (char const* spell : { "flash heal", "healing touch", "healing wave",
        "holy light", "surging mist", "regrowth", "greater heal" })
        if (botAI->CastSpell(spell, target))
            return true;
    return false;
}

bool InstanceMechanicsAction::isUseful()
{
    return BuildPlan().reaction != Reaction::None;
}

bool InstanceMechanicsAction::Execute(Event /*event*/)
{
    Plan const plan = BuildPlan();
    switch (plan.reaction)
    {
        case Reaction::Spread:
            if (bot->IsNonMeleeSpellCasted(true))
                botAI->InterruptSpell();
            return MoveFromGroup(plan.distance, MovementPriority::MOVEMENT_HAZARD);
        case Reaction::Stack:
            if (!plan.anchor || bot->GetDistance(plan.anchor) <= plan.distance)
                return false;
            return MoveTo(plan.anchor, plan.distance,
                MovementPriority::MOVEMENT_HAZARD);
        case Reaction::Kite:
            if (!plan.anchor)
                return false;
            if (bot->IsNonMeleeSpellCasted(true))
                botAI->InterruptSpell();
            return MoveAway(plan.anchor, plan.distance);
        case Reaction::AvoidFrontal:
        {
            if (!plan.anchor)
                return false;
            if (bot->IsNonMeleeSpellCasted(true))
                botAI->InterruptSpell();
            float const distance = std::max(4.0f,
                plan.anchor->GetCombatReach() + bot->GetCombatReach() + 2.0f);
            float const angle = plan.anchor->GetOrientation() + float(M_PI);
            float x = plan.anchor->GetPositionX() + std::cos(angle) * distance;
            float y = plan.anchor->GetPositionY() + std::sin(angle) * distance;
            float z = plan.anchor->GetPositionZ();
            if (!bot->GetMap()->CheckCollisionAndGetValidCoords(bot,
                bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(),
                x, y, z))
                return Flee(plan.anchor);
            return MoveTo(bot->GetMapId(), x, y, z, false, false, true, true,
                MovementPriority::MOVEMENT_HAZARD);
        }
        case Reaction::FocusAdd:
            return plan.target && Attack(plan.target);
        case Reaction::BreakMindControl:
            // AttackAction also updates the shared combat target. Avoid using
            // burst-specific spell IDs here so every class can participate.
            return plan.target && Attack(plan.target);
        case Reaction::StopAttack:
            bot->AttackStop();
            if (Pet* pet = bot->GetPet())
                pet->AttackStop();
            return true;
        case Reaction::TankSwap:
            if (!plan.target)
                return false;
            context->GetValue<Unit*>("current target")->Set(plan.target);
            bot->SetTarget(plan.target->GetGUID());
            return botAI->CastSpell(GroupPveCombat::TauntSpell(bot), plan.target);
        case Reaction::Defensive:
            return UseDefensive();
        case Reaction::HealEncounterUnit:
            return HealEncounterUnit(plan.target);
        case Reaction::None:
            return false;
    }
    return false;
}
