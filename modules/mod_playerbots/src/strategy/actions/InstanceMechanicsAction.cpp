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
constexpr uint32 MapTerraceOfEndlessSpring = 996;
constexpr uint32 MapMogushanVaults = 1008;
constexpr uint32 MapHeartOfFear = 1009;
constexpr uint32 MapShadoPanMonastery = 959;
constexpr uint32 MapTempleOfTheJadeSerpent = 960;
constexpr uint32 MapGateOfTheSettingSun = 962;
constexpr uint32 MapScarletMonastery = 1004;
constexpr uint32 MapScholomance = 1007;
constexpr uint32 MapSiegeOfNiuzaoTemple = 1011;
constexpr uint32 MapThroneOfThunder = 1098;
constexpr uint32 MapSiegeOfOrgrimmar = 1136;

constexpr uint32 NpcGekkan = 61243;
constexpr uint32 NpcGlintrokIronhide = 61337;
constexpr uint32 NpcGlintrokSkulker = 61338;
constexpr uint32 NpcGlintrokOracle = 61339;
constexpr uint32 NpcGlintrokHexxer = 61340;
constexpr uint32 NpcMuShiba = 61453;
constexpr uint32 NpcMingTheCunning = 61444;
constexpr uint32 NpcHaiyanTheUnstoppable = 61445;
constexpr uint32 NpcWhirlingDervish = 61626;
constexpr uint32 NpcWiseMari = 56448;
constexpr uint32 SpellRavage = 119948;
constexpr uint32 SpellMagneticFieldAura = 120100;
constexpr uint32 SpellWiseMariWaterBubble = 106062;
constexpr uint32 SpellWiseMariHydrolanceVisual = 106055;
constexpr uint32 SpellWiseMariWashAway = 106331;
constexpr float MingMagneticFieldClearance = 18.0f;
constexpr float MingDervishClearance = 10.0f;

struct EncounterPosition
{
    float x;
    float y;
    float z;
};

// The two raised dry platforms nearest Wise Mari's approach remain outside
// Corrupted Waters. Living Water adds path to the group there during phase 1.
constexpr EncounterPosition WiseMariDryPlatforms[] =
{
    { 1059.94f, -2581.65f, 176.143f },
    { 1023.31f, -2569.70f, 176.034f }
};

struct AuraRule
{
    uint32 map;
    uint32 spell;
    float distance;
};

struct SwapRule
{
    uint32 map;
    uint32 spell;
    uint8 stacks;
};

struct ObjectiveRule
{
    uint32 map;
    uint32 entry;
};

// Carrier mechanics verified against the local 5.4.8 encounter scripts and
// the original encounter guides. Generic spell-shape inspection below extends
// spread handling to older instances without requiring a row for every DoT.
constexpr AuraRule SpreadAuras[] =
{
    // Tier 14
    { MapMogushanVaults, 116417, 10.0f }, // Feng: Arcane Resonance
    { MapMogushanVaults, 116784, 10.0f }, // Feng: Wildfire Spark
    { MapHeartOfFear, 123180, 10.0f }, // Ta'yak: Wind Step
    { MapHeartOfFear, 124862, 10.0f }, // Shek'zeer: Visions of Demise
    { MapTerraceOfEndlessSpring, 122775, 10.0f }, // Tsulong: Nightmares
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
    { MapMogushanVaults, 118303, 24.0f }, // Spirit Kings: Undying Shadow fixate
    { MapHeartOfFear, 122835, 30.0f }, // Garalon: Pheromones
    { MapHeartOfFear, 125390, 24.0f }, // Shek'zeer: Windblade fixate
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
    { MapHeartOfFear, 123017, 4.0f }, // Ta'yak: Unseen Strike marker
    { MapThroneOfThunder, 136922, 4.0f }, // Frostbite
    { MapThroneOfThunder, 135695, 4.0f }, // Static Shock
    { MapThroneOfThunder, 136295, 4.0f }, // Overcharged
    { MapSiegeOfOrgrimmar, 147068, 4.0f }, // Flames of Galakrond soak
    { MapSiegeOfOrgrimmar, 142948, 4.0f }  // Aim
};

constexpr SwapRule TankSwapAuras[] =
{
    // Tier 14. These are the tank-facing debuffs used by the local 5.4.8
    // scripts; normal and heroic share the same aura ids.
    { MapMogushanVaults, 131788, 2 }, // Feng: Lightning Lash
    { MapMogushanVaults, 131790, 2 }, // Feng: Arcane Shock
    { MapMogushanVaults, 131792, 2 }, // Feng: Shadowburn
    { MapHeartOfFear, 123474, 2 }, // Ta'yak: Overwhelming Assault
    { MapHeartOfFear, 123707, 4 }, // Shek'zeer: Eyes of the Empress
    { MapTerraceOfEndlessSpring, 122752, 2 }, // Tsulong: Shadow Breath
    { MapTerraceOfEndlessSpring, 123121, 8 }, // Lei Shi: Spray
    // Throne of Thunder
    { MapThroneOfThunder, 138349, 2 }, // Static Wound
    { MapThroneOfThunder, 136767, 2 }, // Triple Puncture
    { MapThroneOfThunder, 139840, 2 }, // Rot Armor
    { MapThroneOfThunder, 136050, 3 }, // Malformed Blood
    { MapThroneOfThunder, 138569, 3 }, // Explosive Slam
    { MapThroneOfThunder, 134691, 3 }, // Impale
    // Siege of Orgrimmar
    { MapSiegeOfOrgrimmar, 143436, 2 }, // Corrosive Blast
    { MapSiegeOfOrgrimmar, 144358, 1 }, // Wounded Pride
    { MapSiegeOfOrgrimmar, 144467, 3 }, // Ignite Armor
    { MapSiegeOfOrgrimmar, 144215, 5 }, // Froststorm Strike
    { MapSiegeOfOrgrimmar, 143494, 3 }, // Sundering Blow
    { MapSiegeOfOrgrimmar, 143385, 3 }, // Electrostatic Charge
    { MapSiegeOfOrgrimmar, 143339, 3 }, // Injection
    { MapSiegeOfOrgrimmar, 145183, 3 }  // Gripping Despair
};

constexpr uint32 StopAttackAuras[] =
{
    137149, // Kazra'jin Overload
    137166, // Kazra'jin Discharge
    143593  // General Nazgrim Defensive Stance
};

constexpr uint32 DefensiveCasts[] =
{
    122713, // Zor'lok: Force and Verve
    122949, // Ta'yak: Unseen Strike
    122774, // Garalon: Crush
    124845, // Shek'zeer: Calamity
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
    // Trial of the King: an active Mu'Shiba must be killed before the active
    // boss because killing it immediately ends Ravage. Haiyan is the next
    // explicit Trial target when his scripted turn has made him attackable.
    // Both entries are checked for active/aggressive state below, so the bot
    // never attacks the passive yellow versions waiting beside the arena.
    NpcMuShiba, NpcHaiyanTheUnstoppable,
    // Gekkan: Iron Protector makes nearby allies take 50% less damage, then
    // Hex of Lethargy cripples casters.  Keep Gekkan for last so Inspiring
    // Cry cannot empower surviving followers.  This ordering is shared by
    // normal, heroic and challenge mode because map and creature entries are
    // identical across those difficulties.
    NpcGlintrokIronhide, NpcGlintrokHexxer, NpcGlintrokSkulker,
    NpcGlintrokOracle,
    // MoP dungeons
    56511, // Jade Temple: Corrupt Living Water
    58856, 58865, 59555, // Jade Temple: Haunting Sha variants
    56762, // Jade Temple: Yu'lon manifestation
    56792, // Jade Temple: Figment of Doubt
    56754, // Shado-Pan: Azure Serpent
    56713, // Shado-Pan: Snowdrift clone
    66652, // Shado-Pan: Lesser Volatile Energy
    56895, // Setting Sun: Raigonn Weak Spot
    59794, // Setting Sun: Krik'thik Disruptor
    60447, // Setting Sun: Krik'thik Saboteur
    61623, // Niuzao: Sappling
    61484, // Niuzao: Amber Sapper
    61670, // Niuzao: Sik'thik Demolisher
    59893, // Scarlet Monastery: Empowering Spirit
    59930, // Scarlet Monastery: Empowered Zombie
    58664, // Scholomance: Chillheart's Phylactery
    58791, // Scholomance: Lilian's Soul
    59099, // Scholomance: Fresh Test Subject
    // Mogu'shan Vaults. Encounter objects which must die before damage can
    // return to the boss are ordered before ordinary spawned damage adds.
    60958, // Pinning Arrow
    60913, // Elegon Energy Charge
    60776, // Empyreal Focus
    60793, // Celestial Protector
    60398, // Emperor's Courage
    60397, // Emperor's Strength
    60396, // Emperor's Rage
    60480, // Titan Spark
    60731, // Undying Shadow
    60240, // Gara'jal Spirit Totem
    // Heart of Fear
    62531, // Amber Prison
    65498, // Zarthik Battle-Mender
    65499, // Sra'thik Amber-Trapper
    65500, // Kor'thik Elite Blademaster
    63053, // Garalon's Leg
    62711, // Amber Monstrosity
    62691, // Living Amber
    64453, // Set'thik Windblade
    63591, // Kor'thik Reaver
    // Terrace of Endless Spring
    60886, // Coalesced Corruption
    62969, // Embodied Terror
    62977, // Fright Spawn
    62995, // Animated Protector
    61034, // Terror Spawn
    61038, 61046, 61042, // Sha of Fear outer-platform bowmen
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

// Some phase objectives never acquire a victim and therefore never enter the
// ordinary attacker list. They are safe to select while the party is already
// in combat because the encounter scripts expose them as attackable only in
// the phase where killing them is required.
constexpr ObjectiveRule EncounterObjectives[] =
{
    { MapTempleOfTheJadeSerpent, 56511 }, // Corrupt Living Water
    { MapTempleOfTheJadeSerpent, 56762 }, // Yu'lon
    { MapTempleOfTheJadeSerpent, 56792 }, // Figment of Doubt
    { MapShadoPanMonastery, 56754 }, // Azure Serpent
    { MapShadoPanMonastery, 56713 }, // Snowdrift clone
    { MapGateOfTheSettingSun, 56895 }, // Raigonn Weak Spot
    { MapSiegeOfNiuzaoTemple, 61623 }, // Sappling
    { MapScarletMonastery, 59893 }, // Empowering Spirit
    { MapScholomance, 58664 }, // Chillheart's Phylactery
    { MapScholomance, 58791 }, // Lilian's Soul
    { MapMogushanVaults, 60958 }, // Pinning Arrow
    { MapMogushanVaults, 60913 }, // Energy Charge
    { MapMogushanVaults, 60776 }, // Empyreal Focus
    { MapMogushanVaults, 60240 }, // Spirit Totem
    { MapHeartOfFear, 62531 }, // Amber Prison
    { MapHeartOfFear, 63053 }, // Garalon's Leg
    { MapTerraceOfEndlessSpring, 62995 } // Animated Protector
};

bool IsEncounterObjective(uint32 map, uint32 entry)
{
    return std::any_of(std::begin(EncounterObjectives),
        std::end(EncounterObjectives), [map, entry](ObjectiveRule const& rule)
        {
            return rule.map == map && rule.entry == entry;
        });
}

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
        if (rule.map == bot->GetMapId() &&
            AuraStacks(currentTank, rule.spell) >= rule.stacks)
            return true;
    return false;
}

bool InstanceMechanics::IsActiveMogushanTrialTarget(Player* bot,
    Creature* creature)
{
    if (!bot || !creature || bot->GetMapId() != MapMogushanPalace ||
        creature->GetMap() != bot->GetMap() || !creature->IsAlive() ||
        !creature->IsInWorld() || !bot->IsValidAttackTarget(creature) ||
        (creature->GetEntry() != NpcMuShiba &&
            creature->GetEntry() != NpcHaiyanTheUnstoppable))
        return false;

    return creature->GetReactState() != REACT_PASSIVE &&
        !creature->HasFlag(UNIT_FIELD_FLAGS,
            UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NON_ATTACKABLE_2 |
            UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC |
            UNIT_FLAG_PACIFIED) &&
        bot->GetDistance(creature) <= 150.0f;
}

Unit* InstanceMechanics::PriorityTarget(PlayerbotAI* botAI, Player* bot,
    Unit* boss)
{
    if (!botAI || !bot || !bot->IsAlive() || !bot->IsInWorld())
        return nullptr;

    std::vector<Unit*> targets;
    auto addTarget = [&](Unit* unit)
    {
        if (unit && std::find(targets.begin(), targets.end(), unit) ==
            targets.end())
            targets.push_back(unit);
    };

    for (ObjectGuid const& guid : botAI->GetAiObjectContext()
        ->GetValue<GuidVector>("possible targets")->Get())
        addTarget(botAI->GetUnit(guid));

    // The normal possible-target cache is intentionally shorter ranged and
    // can update one tick behind scripted activations. Trial of the King's
    // actors are already spawned in the arena, so scan the two ordered
    // targets directly and validate their active state below.
    if (bot->GetMapId() == MapMogushanPalace)
    {
        addTarget(bot->FindNearestCreature(NpcMuShiba, 150.0f, true));
        addTarget(bot->FindNearestCreature(NpcHaiyanTheUnstoppable,
            150.0f, true));
    }

    // Phase objects often have no victim and can be absent from the regular
    // possible-target cache. Discover only the objective entries belonging
    // to this map; attackability and active-combat checks below still gate
    // whether they can own skull.
    for (ObjectiveRule const& rule : EncounterObjectives)
        if (rule.map == bot->GetMapId())
            addTarget(bot->FindNearestCreature(rule.entry, 150.0f, true));

    Group* group = bot->GetGroup(GroupSlot::Instance);
    if (!group)
        group = bot->GetGroup();
    if (group)
        for (GroupReference* ref = group->GetFirstMember(); ref;
            ref = ref->next())
            if (Player* member = ref->GetSource())
                if (member->IsAlive() && member->IsInWorld() &&
                    member->GetMap() == bot->GetMap())
                    for (Unit* attacker : member->getAttackers())
                        addTarget(attacker);

    Unit* best = nullptr;
    size_t bestRank = std::size(PriorityAdds) + 1;
    for (Unit* unit : targets)
    {
        if (!unit || unit == boss || !unit->IsAlive() ||
            unit->GetMap() != bot->GetMap() ||
            !bot->IsValidAttackTarget(unit) ||
            !bot->IsWithinLOSInMap(unit))
            continue;

        Creature* creature = unit->ToCreature();
        bool const trialPriority = creature &&
            bot->GetMapId() == MapMogushanPalace &&
            (unit->GetEntry() == NpcMuShiba ||
                unit->GetEntry() == NpcHaiyanTheUnstoppable);
        bool const activeTrialTarget = trialPriority &&
            IsActiveMogushanTrialTarget(bot, creature);
        bool const encounterObjective = creature && bot->IsInCombat() &&
            IsEncounterObjective(bot->GetMapId(), unit->GetEntry()) &&
            bot->GetDistance(unit) <= 150.0f;
        if (trialPriority && !activeTrialTarget)
            continue;
        if (!trialPriority && !encounterObjective &&
            (!unit->IsInCombat() || !GroupPveCombat::IsEngaged(bot, unit)))
            continue;

        auto const found = std::find(std::begin(PriorityAdds),
            std::end(PriorityAdds), unit->GetEntry());
        // Known encounter-critical targets retain their documented ordering.
        // In encounters which do not yet have a dedicated row, an engaged
        // add currently casting a heal is still a safer switch than tunneling
        // the boss. This generic fallback applies to every dungeon/raid and
        // never pulls an idle pack.
        bool const genericHealer = found == std::end(PriorityAdds) &&
            IsHealingCast(unit);
        if (found == std::end(PriorityAdds) && !genericHealer)
            continue;

        size_t const rank = genericHealer ? std::size(PriorityAdds) :
            size_t(std::distance(std::begin(PriorityAdds), found));
        if (!best || rank < bestRank ||
            (rank == bestRank &&
                bot->GetDistance(unit) < bot->GetDistance(best)))
        {
            best = unit;
            bestRank = rank;
        }
    }
    return best;
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
    return InstanceMechanics::PriorityTarget(botAI, bot, boss);
}

Unit* InstanceMechanicsAction::FindMindControlledMember() const
{
    Group* group = bot->GetGroup(GroupSlot::Instance);
    if (!group)
        group = bot->GetGroup();
    if (!group)
        return nullptr;

    // These scripted controls are removed by damaging the affected player.
    // IsCharmed covers the same mechanic in older encounters, while the
    // explicit auras cover scripts which only temporarily change faction.
    constexpr uint32 breakableControlAuras[] =
    {
        117708, // Spirit Kings: Maddening Shout
        122740, // Zor'lok: Convert
        123713, // Shek'zeer: Servant of the Empress
        145071, 145175, 145599 // Garrosh: Touch of Y'Shaarj variants
    };
    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        if (Player* member = ref->GetSource())
            if (member != bot && member->IsAlive() &&
                member->GetMap() == bot->GetMap() &&
                bot->IsValidAttackTarget(member))
            {
                if (member->IsCharmed())
                    return member;
                for (uint32 spell : breakableControlAuras)
                    if (member->HasAura(spell))
                        return member;
            }
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

    // Magnetic Field is an aura on Ming himself, not a DynamicObject,
    // AreaTrigger, or passive emitter. The generic ground-effect scanner
    // therefore cannot discover it. Whirling Dervish is likewise a moving
    // creature hazard. Both mechanics are lethal positional checks and must
    // preempt healing, targeting, and tank combat movement for every role.
    if (bot->GetMapId() == MapMogushanPalace)
    {
        Creature* ming = bot->FindNearestCreature(NpcMingTheCunning,
            150.0f, true);
        if (ming && ming->IsInCombat() &&
            ming->HasAura(SpellMagneticFieldAura) &&
            bot->GetExactDist2d(ming) < MingMagneticFieldClearance)
            return { Reaction::AvoidUnitHazard, ming, nullptr,
                MingMagneticFieldClearance };

        Creature* dervish = bot->FindNearestCreature(NpcWhirlingDervish,
            40.0f, true);
        if (dervish && dervish->IsInWorld() &&
            bot->GetExactDist2d(dervish) < MingDervishClearance)
            return { Reaction::AvoidUnitHazard, dervish, nullptr,
                MingDervishClearance };
    }

    if (bot->GetMapId() == MapTempleOfTheJadeSerpent)
    {
        Creature* wiseMari = bot->FindNearestCreature(NpcWiseMari,
            150.0f, true);
        if (wiseMari && wiseMari->IsInCombat())
        {
            // During phase one, fight the four Living Waters from a raised
            // platform instead of following targets into Corrupted Waters.
            if (wiseMari->HasAura(SpellWiseMariWaterBubble) &&
                bot->GetPositionZ() <= 174.7f)
                return { Reaction::WiseMariDryPlatform, wiseMari, nullptr,
                    0.0f };

            Spell* cast = CurrentSpell(wiseMari);
            uint32 const castId = cast && cast->GetSpellInfo() ?
                cast->GetSpellInfo()->Id : 0;

            // Wash Away is a rotating cone. Keep every role, including the
            // tank (the phase is not tankable), one quarter-turn ahead of it.
            if (wiseMari->HasAura(SpellWiseMariWashAway) ||
                castId == SpellWiseMariWashAway)
                return { Reaction::CircleWiseMari, wiseMari, nullptr, 20.0f };

            // Hydrolance telegraphs the active fountain sector through Wise
            // Mari's facing. The normal frontal escape is correct here, but
            // must also apply to the active tank because the boss is passive.
            if ((wiseMari->HasAura(SpellWiseMariHydrolanceVisual) ||
                 castId == SpellWiseMariHydrolanceVisual) &&
                wiseMari->HasInArc(float(M_PI) * 0.65f, bot))
                return { Reaction::AvoidFrontal, wiseMari, nullptr, 0.0f };
        }
    }

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
        bool const muShiba = map == MapMogushanPalace &&
            add->GetEntry() == NpcMuShiba;
        // Gekkan's stationary casters and melee followers must be gathered by
        // the tank as well as focused by damage dealers. Mu'Shiba is also the
        // explicit kill target during Ravage, so the elected leader switches
        // with the group after publishing skull. Healers retain their healing
        // target while the encounter layer coordinates the rest.
        if (bossUnavailable || PlayerBotSpec::IsDps(bot, true) ||
            (gekkanEntourage && !PlayerBotSpec::IsHeal(bot, true)) ||
            (muShiba && botAI->IsInstanceTankLeader()))
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
        case Reaction::AvoidUnitHazard:
            if (!plan.anchor || !plan.anchor->IsInWorld() ||
                plan.anchor->GetMap() != bot->GetMap())
                return false;
            if (bot->GetExactDist2d(plan.anchor) >= plan.distance)
                return true;
            if (bot->IsNonMeleeSpellCasted(true))
                botAI->InterruptSpell();
            return FleePosition(plan.anchor->GetPosition(),
                plan.distance, 250, MovementPriority::MOVEMENT_HAZARD);
        case Reaction::WiseMariDryPlatform:
        {
            EncounterPosition const* best = nullptr;
            float bestDistance = FLT_MAX;
            for (EncounterPosition const& platform : WiseMariDryPlatforms)
            {
                float const dx = platform.x - bot->GetPositionX();
                float const dy = platform.y - bot->GetPositionY();
                float const distance = dx * dx + dy * dy;
                if (distance < bestDistance)
                {
                    best = &platform;
                    bestDistance = distance;
                }
            }
            if (!best)
                return false;
            if (bot->IsNonMeleeSpellCasted(true))
                botAI->InterruptSpell();
            return MoveTo(bot->GetMapId(), best->x, best->y, best->z,
                false, false, true, true,
                MovementPriority::MOVEMENT_HAZARD, true);
        }
        case Reaction::CircleWiseMari:
        {
            if (!plan.anchor)
                return false;
            if (bot->IsNonMeleeSpellCasted(true))
                botAI->InterruptSpell();

            // The local encounter script rotates Wash Away by increasing the
            // boss orientation. Staying 90 degrees ahead follows the safe side
            // around the inner dry ring without ever crossing the beam.
            float const angle = Position::NormalizeOrientation(
                plan.anchor->GetOrientation() + float(M_PI_2));
            float x = plan.anchor->GetPositionX() +
                std::cos(angle) * plan.distance;
            float y = plan.anchor->GetPositionY() +
                std::sin(angle) * plan.distance;
            float z = std::max(175.0f, bot->GetPositionZ());
            if (!bot->GetMap()->CheckCollisionAndGetValidCoords(bot,
                bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(),
                x, y, z))
                return Flee(plan.anchor);
            return MoveTo(bot->GetMapId(), x, y, z, false, false, true, true,
                MovementPriority::MOVEMENT_FORCED, true);
        }
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
