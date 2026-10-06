#include "MopInstanceStrategy.h"

#include "Creature.h"
#include "Playerbots.h"
#include "PlayerbotSpec.h"
#include "Spell.h"

namespace MopInstanceBot
{
namespace
{
// MoP five-player instances.
constexpr uint32 MAP_SHADO_PAN_MONASTERY       = 959;
constexpr uint32 MAP_TEMPLE_OF_THE_JADE_SERPENT = 960;
constexpr uint32 MAP_STORMSTOUT_BREWERY        = 961;
constexpr uint32 MAP_GATE_OF_THE_SETTING_SUN   = 962;
constexpr uint32 MAP_MOGUSHAN_PALACE           = 994;
constexpr uint32 MAP_SCARLET_HALLS             = 1001;
constexpr uint32 MAP_SCARLET_MONASTERY         = 1004;
constexpr uint32 MAP_SCHOLOMANCE               = 1007;
constexpr uint32 MAP_SIEGE_OF_NIUZAO_TEMPLE    = 1011;

// MoP raids. The map ids are shared by normal and heroic difficulties.
constexpr uint32 MAP_TERRACE_OF_ENDLESS_SPRING = 996;
constexpr uint32 MAP_MOGUSHAN_VAULTS           = 1008;
constexpr uint32 MAP_HEART_OF_FEAR             = 1009;
constexpr uint32 MAP_THRONE_OF_THUNDER         = 1098;
constexpr uint32 MAP_SIEGE_OF_ORGRIMMAR        = 1136;

constexpr uint32 NPC_WISE_MARI                 = 56448;
constexpr uint32 NPC_CORRUPT_LIVING_WATER      = 56511;
constexpr uint32 SPELL_WATER_BUBBLE             = 106062;
constexpr uint32 NPC_YAN_ZHU                    = 59479;
constexpr uint32 NPC_FIZZY_BUBBLE               = 59799;
constexpr uint32 SPELL_FIZZY_BUBBLE_PLAYER      = 114459;

bool IsUsableTarget(Player* bot, Unit* target, float range)
{
    return target && target->IsAlive() && target->IsInWorld() &&
        target->GetMap() == bot->GetMap() &&
        bot->IsValidAttackTarget(target) && bot->IsWithinLOSInMap(target) &&
        bot->GetExactDist2d(target) <= range;
}

bool IsCasting(Unit* unit)
{
    return unit && (unit->GetCurrentSpell(CURRENT_GENERIC_SPELL) ||
        unit->GetCurrentSpell(CURRENT_CHANNELED_SPELL));
}

Creature* FindYanZhuFizzyBubble(Player* bot)
{
    if (!bot || bot->GetMapId() != MAP_STORMSTOUT_BREWERY ||
        !bot->IsInCombat() || bot->HasAura(SPELL_FIZZY_BUBBLE_PLAYER))
        return nullptr;

    Creature* yanZhu = bot->FindNearestCreature(NPC_YAN_ZHU, 100.0f, true);
    if (!yanZhu || !yanZhu->IsInCombat())
        return nullptr;

    return bot->FindNearestCreature(NPC_FIZZY_BUBBLE, 80.0f, true);
}

Unit* SelectCoordinatedTarget(PlayerbotAI* botAI)
{
    Player* bot = botAI->GetBot();
    if (!bot || !IsMopInstanceMap(bot->GetMapId()) || !bot->IsInCombat())
        return nullptr;

    Unit* current = botAI->GetAiObjectContext()->
        GetValue<Unit*>("current target")->Get();

    // Wise Mari is still a valid hostile unit while Water Bubble absorbs all
    // damage, so the generic attackability test alone cannot identify this
    // intermission. Send tank and damage dealers directly to the living-water
    // adds which remove the shield instead.
    if (bot->GetMapId() == MAP_TEMPLE_OF_THE_JADE_SERPENT &&
        !PlayerBotSpec::IsHeal(bot, true))
        if (Creature* wiseMari = bot->FindNearestCreature(NPC_WISE_MARI,
                120.0f, true))
            if (wiseMari->HasAura(SPELL_WATER_BUBBLE))
                if (Creature* water = bot->FindNearestCreature(
                        NPC_CORRUPT_LIVING_WATER, 120.0f, true))
                    if (IsUsableTarget(bot, water, 120.0f))
                        return water;

    // Immunity/intermission phases are common throughout MoP. If the current
    // boss cannot be attacked (Wise Mari's Water Bubble is the first dungeon
    // example), every combat role including the tank must move onto a live
    // encounter add. Prefer casters and adds already threatening the party.
    bool const protectedBoss = current && current->ToCreature() &&
        (current->ToCreature()->IsDungeonBoss() ||
         current->ToCreature()->isWorldBoss()) &&
        !IsUsableTarget(bot, current, 120.0f);
    if (!PlayerBotSpec::IsHeal(bot, true) &&
        (protectedBoss || !current || !IsUsableTarget(bot, current, 120.0f)))
    {
        Unit* best = nullptr;
        int32 bestScore = INT32_MIN;
        GuidVector const& targets = botAI->GetAiObjectContext()->
            GetValue<GuidVector>("possible targets")->Get();
        for (ObjectGuid const& guid : targets)
            if (Unit* candidate = botAI->GetUnit(guid))
            {
                Creature* creature = candidate->ToCreature();
                if (!creature || creature->IsDungeonBoss() ||
                    creature->isWorldBoss() || !creature->IsInCombat() ||
                    !IsUsableTarget(bot, creature, 120.0f))
                    continue;

                int32 score = IsCasting(creature) ? 1000 : 0;
                if (Unit* victim = creature->GetVictim())
                {
                    if (victim == bot || victim == botAI->GetMaster())
                        score += 500;
                    if (victim->ToPlayer() &&
                        PlayerBotSpec::IsHeal(victim->ToPlayer(), true))
                        score += 400;
                }

                // Wise Mari: Corrupt Living Water must be killed four times
                // before the boss bubble can break.
                if (creature->GetEntry() == NPC_CORRUPT_LIVING_WATER)
                    score += 2000;

                score -= int32(bot->GetExactDist2d(creature));
                if (score > bestScore)
                {
                    best = creature;
                    bestScore = score;
                }
            }

        if (best)
            return best;
    }

    // Healers keep their healing target and tanks keep an attackable boss.
    // Their protected-boss/add handling above remains active.
    if (!botAI->IsDps(bot) || PlayerBotSpec::IsTank(bot, true))
        return nullptr;

    // A real player's explicit target is authoritative. This makes every bot
    // engage as soon as its master starts attacking and lets the player call
    // add switches without waiting for proximity-based target discovery.
    if (Player* master = botAI->GetMaster())
        if (master->GetMap() == bot->GetMap() && master->IsAlive())
        {
            Unit* targets[] = { master->GetSelectedUnit(), master->GetVictim() };
            for (Unit* target : targets)
                if (IsUsableTarget(bot, target, 120.0f) &&
                    (master->IsInCombat() || target->IsInCombat()))
                    return target;
        }

    // When no target has been called, peel damage dealers from a boss onto an
    // active casting add. Interrupt rotations can then stop the cast. Do not
    // churn between ordinary trash mobs or pull unengaged creatures.
    if (current && current->ToCreature() &&
        (current->ToCreature()->IsDungeonBoss() ||
         current->ToCreature()->isWorldBoss()))
    {
        Unit* best = nullptr;
        float bestDistance = 70.0f;
        GuidVector const& targets = botAI->GetAiObjectContext()->
            GetValue<GuidVector>("possible targets")->Get();
        for (ObjectGuid const& guid : targets)
            if (Unit* candidate = botAI->GetUnit(guid))
            {
                Creature* creature = candidate->ToCreature();
                if (!creature || creature->IsDungeonBoss() ||
                    creature->isWorldBoss() || !creature->IsInCombat() ||
                    !IsUsableTarget(bot, creature, bestDistance))
                    continue;

                Unit* victim = creature->GetVictim();
                bool const threatensPriorityMember = victim &&
                    (victim == botAI->GetMaster() ||
                     (victim->ToPlayer() &&
                      PlayerBotSpec::IsHeal(victim->ToPlayer(), true)));
                if (!IsCasting(creature) && !threatensPriorityMember)
                    continue;

                best = creature;
                bestDistance = bot->GetExactDist2d(creature);
            }
        if (best)
            return best;
    }

    return nullptr;
}
}

bool IsMopInstanceMap(uint32 mapId)
{
    switch (mapId)
    {
        case MAP_SHADO_PAN_MONASTERY:
        case MAP_TEMPLE_OF_THE_JADE_SERPENT:
        case MAP_STORMSTOUT_BREWERY:
        case MAP_GATE_OF_THE_SETTING_SUN:
        case MAP_MOGUSHAN_PALACE:
        case MAP_SCARLET_HALLS:
        case MAP_SCARLET_MONASTERY:
        case MAP_SCHOLOMANCE:
        case MAP_SIEGE_OF_NIUZAO_TEMPLE:
        case MAP_TERRACE_OF_ENDLESS_SPRING:
        case MAP_MOGUSHAN_VAULTS:
        case MAP_HEART_OF_FEAR:
        case MAP_THRONE_OF_THUNDER:
        case MAP_SIEGE_OF_ORGRIMMAR:
            return true;
        default:
            return false;
    }
}

NextAction** MopInstanceStrategy::getDefaultActions()
{
    // These actions are map-gated internally. Loading them here guarantees
    // that hazard avoidance is active in MoP instances even when the optional
    // global AutoAvoidAoe setting is disabled.
    return NextAction::array(0,
        new NextAction("boss mechanics", ACTION_EMERGENCY + 1),
        new NextAction("avoid aoe", ACTION_EMERGENCY), nullptr);
}

void MopInstanceStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("yan-zhu fizzy bubble",
        NextAction::array(0, new NextAction("enter yan-zhu fizzy bubble",
            ACTION_EMERGENCY + 10), nullptr)));
    triggers.push_back(new TriggerNode("mop coordinated target",
        NextAction::array(0, new NextAction("mop attack coordinated target",
            ACTION_RAID + 8), nullptr)));
}

bool YanZhuFizzyBubbleTrigger::IsActive()
{
    return FindYanZhuFizzyBubble(bot) != nullptr;
}

bool CoordinatedTargetTrigger::IsActive()
{
    Unit* target = SelectCoordinatedTarget(botAI);
    return target && target != AI_VALUE(Unit*, "current target");
}

bool AttackCoordinatedTargetAction::Execute(Event /*event*/)
{
    Unit* target = SelectCoordinatedTarget(botAI);
    return target && Attack(target);
}

bool EnterYanZhuFizzyBubbleAction::Execute(Event /*event*/)
{
    Creature* bubble = FindYanZhuFizzyBubble(bot);
    if (!bubble)
        return false;

    if (bot->GetDistance(bubble) > INTERACTION_DISTANCE)
        return MoveTo(bubble, INTERACTION_DISTANCE - 0.5f,
            MovementPriority::MOVEMENT_FORCED);

    botAI->RemoveShapeshift();
    bot->AttackStop();
    bot->GetMotionMaster()->Clear(false);
    bot->StopMoving();
    bot->SetFacingToObject(bubble);
    return bubble->HandleSpellClick(bot);
}

MopInstanceTriggerContext::MopInstanceTriggerContext()
{
    creators["yan-zhu fizzy bubble"] = [](PlayerbotAI* ai) -> Trigger*
    {
        return new YanZhuFizzyBubbleTrigger(ai);
    };
    creators["mop coordinated target"] = [](PlayerbotAI* ai) -> Trigger*
    {
        return new CoordinatedTargetTrigger(ai);
    };
}

MopInstanceActionContext::MopInstanceActionContext()
{
    creators["enter yan-zhu fizzy bubble"] = [](PlayerbotAI* ai) -> Action*
    {
        return new EnterYanZhuFizzyBubbleAction(ai);
    };
    creators["mop attack coordinated target"] = [](PlayerbotAI* ai) -> Action*
    {
        return new AttackCoordinatedTargetAction(ai);
    };
}
}
