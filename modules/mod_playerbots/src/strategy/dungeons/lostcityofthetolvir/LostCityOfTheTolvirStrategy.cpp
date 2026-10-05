/* Playerbot strategy for Lost City of the Tol'vir (normal and heroic). */
#include "LostCityOfTheTolvirStrategy.h"

#include "Creature.h"
#include "ObjectAccessor.h"
#include "Playerbots.h"

namespace LostCityOfTheTolvirBot
{
namespace
{
Position const WindTunnelRecoveryPosition =
    { -10887.7f, -1447.7f, 2.25f, 4.75f };
Position const SiamatPlatformSafePosition =
    { -10928.0f, -1400.0f, 37.25f, 0.0f };

Creature* FindCreature(Player* bot, uint32 entry, float range)
{
    Creature* creature = bot->FindNearestCreature(entry, range, true);
    return creature && creature->IsAlive() ? creature : nullptr;
}

Creature* FindUsableWindTunnel(Player* bot)
{
    Creature* tunnel = FindCreature(bot, NPC_WIND_TUNNEL, 220.0f);
    return tunnel && tunnel->IsVisible() &&
        tunnel->HasFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK) ?
        tunnel : nullptr;
}

bool IsWindTunnelVehicle(Player* bot)
{
    Unit* vehicle = bot->GetVehicleBase();
    return vehicle && (vehicle->GetEntry() == NPC_WIND_TUNNEL ||
        vehicle->GetEntry() == NPC_WIND_TUNNEL_LANDING);
}

bool NeedsSiamatLift(PlayerbotAI* botAI)
{
    Player* bot = botAI->GetBot();
    if (bot->GetMapId() != MAP_LOST_CITY_OF_THE_TOLVIR || !bot->IsAlive() ||
        IsWindTunnelVehicle(bot) || bot->GetPositionZ() > 27.0f)
        return false;

    Player* master = botAI->GetMaster();
    bool const masterOnPlatform = master &&
        master->GetMapId() == MAP_LOST_CITY_OF_THE_TOLVIR &&
        master->GetPositionZ() > 27.0f;
    Creature* siamat = FindCreature(bot, NPC_SIAMAT, 350.0f);
    bool const encounterStarted = siamat && siamat->IsInCombat();
    if (!masterOnPlatform && !encounterStarted)
        return false;

    // A bot below the map cannot find or path back to a tunnel. The action
    // first returns it to a real tunnel on the lower floor, then clicks it.
    return bot->GetPositionZ() < -40.0f || FindUsableWindTunnel(bot);
}

Unit* SelectPriorityTarget(PlayerbotAI* botAI)
{
    Player* bot = botAI->GetBot();
    if (bot->GetMapId() != MAP_LOST_CITY_OF_THE_TOLVIR || !bot->IsInCombat() || !botAI->IsDps(bot))
        return nullptr;

    uint32 const priority[] =
    {
        NPC_SERVANT_OF_SIAMAT,
        NPC_HARBINGER_OF_DARKNESS,
        NPC_SOUL_FRAGMENT,
        NPC_FRENZIED_CROCOLISK,
        NPC_BLAZE_OF_THE_HEAVENS
    };
    GuidVector const& targets = botAI->GetAiObjectContext()->GetValue<GuidVector>("possible targets")->Get();
    for (uint32 entry : priority)
        for (ObjectGuid const& guid : targets)
            if (Unit* unit = botAI->GetUnit(guid))
                if (unit->GetEntry() == entry && unit->IsAlive() && bot->IsValidAttackTarget(unit) &&
                    bot->GetExactDist2d(unit) < 70.0f)
                    return unit;
    return nullptr;
}

Creature* FindNearbyHazard(Player* bot)
{
    struct Hazard { uint32 entry; float radius; };
    Hazard const hazards[] =
    {
        { NPC_MYSTIC_TRAP_TARGET, 7.0f },
        { NPC_TEMPEST_STORM, 8.0f },
        { NPC_CLOUD_BURST, 7.0f }
    };
    for (Hazard const& hazard : hazards)
        if (Creature* creature = FindCreature(bot, hazard.entry, hazard.radius))
            return creature;
    return nullptr;
}
}

void LostCityOfTheTolvirStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("lct use wind tunnel",
        NextAction::array(0, new NextAction("lct use wind tunnel", ACTION_MOVE + 20), nullptr)));
    triggers.push_back(new TriggerNode("lct avoid hazard",
        NextAction::array(0, new NextAction("lct avoid hazard", ACTION_MOVE + 10), nullptr)));
    triggers.push_back(new TriggerNode("lct priority target",
        NextAction::array(0, new NextAction("lct attack priority target", ACTION_RAID + 5), nullptr)));
}

void LostCityOfTheTolvirStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    multipliers.push_back(new LostCityOfTheTolvirMultiplier(botAI));
}

bool WindTunnelTrigger::IsActive()
{
    if (bot->GetMapId() == MAP_LOST_CITY_OF_THE_TOLVIR &&
        bot->GetPositionZ() > 27.0f)
        if (Pet* pet = bot->GetPet())
            if (pet->GetPositionZ() < 27.0f)
                return true;

    return NeedsSiamatLift(botAI);
}

bool PriorityTargetTrigger::IsActive()
{
    Unit* target = SelectPriorityTarget(botAI);
    return target && target != AI_VALUE(Unit*, "current target");
}

bool HazardTrigger::IsActive()
{
    return bot->GetMapId() == MAP_LOST_CITY_OF_THE_TOLVIR && bot->IsInCombat() && FindNearbyHazard(bot);
}

bool AttackPriorityTargetAction::Execute(Event /*event*/)
{
    Unit* target = SelectPriorityTarget(botAI);
    return target && Attack(target);
}

bool AvoidHazardAction::Execute(Event /*event*/)
{
    Creature* hazard = FindNearbyHazard(bot);
    return hazard && MoveAway(hazard, 12.0f);
}

bool UseWindTunnelAction::Execute(Event /*event*/)
{
    // Pets do not board the tunnel vehicle. Keep recovering them beside the
    // owner if their follow movement placed them below the platform collision.
    if (bot->GetPositionZ() > 27.0f)
        if (Pet* pet = bot->GetPet())
            if (pet->GetPositionZ() < 27.0f)
            {
                pet->GetMotionMaster()->Clear();
                pet->NearTeleportTo(bot->GetPositionX(), bot->GetPositionY(),
                    bot->GetPositionZ() + 1.0f, bot->GetOrientation());
                return true;
            }

    if (bot->GetPositionZ() < -40.0f)
    {
        bot->CombatStopWithPets(true);
        bot->AttackStop();
        return bot->TeleportTo(MAP_LOST_CITY_OF_THE_TOLVIR,
            WindTunnelRecoveryPosition.GetPositionX(),
            WindTunnelRecoveryPosition.GetPositionY(),
            WindTunnelRecoveryPosition.GetPositionZ(),
            WindTunnelRecoveryPosition.GetOrientation());
    }

    Creature* tunnel = FindUsableWindTunnel(bot);
    if (!tunnel)
        return false;

    if (bot->GetDistance(tunnel) > INTERACTION_DISTANCE)
        return MoveTo(tunnel, 2.0f, MovementPriority::MOVEMENT_FORCED);

    botAI->RemoveShapeshift();
    bot->AttackStop();
    bot->GetMotionMaster()->Clear();
    bot->StopMoving();
    bot->SetFacingToObject(tunnel);
    if (!tunnel->HandleSpellClick(bot))
        return false;

    // The landing vehicle is spawned almost exactly in the platform's
    // collision plane (z=35.44). After both vehicle auras have expired, place
    // the bot and its pet safely above that plane so neither falls through.
    ObjectGuid const botGuid = bot->GetGUID();
    ObjectGuid const masterGuid = GetMaster() ? GetMaster()->GetGUID() :
        ObjectGuid::Empty;
    botAI->AddTimedEvent([botGuid, masterGuid]()
    {
        Player* player = ObjectAccessor::FindPlayer(botGuid);
        if (!player || player->GetMapId() != MAP_LOST_CITY_OF_THE_TOLVIR)
            return;

        float x = SiamatPlatformSafePosition.GetPositionX();
        float y = SiamatPlatformSafePosition.GetPositionY();
        float z = SiamatPlatformSafePosition.GetPositionZ();
        float orientation = player->GetOrientation();
        if (Player* master = ObjectAccessor::FindPlayer(masterGuid))
            if (master->GetMapId() == MAP_LOST_CITY_OF_THE_TOLVIR &&
                master->GetPositionZ() > 27.0f)
            {
                master->GetClosePoint(x, y, z, player->GetObjectSize(),
                    2.0f, static_cast<float>(M_PI));
                z = std::max(z + 1.0f, SiamatPlatformSafePosition.GetPositionZ());
                orientation = master->GetOrientation();
            }

        player->ExitVehicle();
        player->GetMotionMaster()->Clear();
        player->NearTeleportTo(x, y, z, orientation);
        if (Pet* pet = player->GetPet())
        {
            pet->GetMotionMaster()->Clear();
            pet->NearTeleportTo(x, y, z + 0.75f, orientation);
        }
    }, 4000);

    return true;
}

float LostCityOfTheTolvirMultiplier::GetValue(Action* action)
{
    if (bot->GetMapId() != MAP_LOST_CITY_OF_THE_TOLVIR)
        return 1.0f;

    if (IsWindTunnelVehicle(bot))
        return dynamic_cast<MovementAction*>(action) ? 0.0f : 1.0f;

    if (!NeedsSiamatLift(botAI) || dynamic_cast<UseWindTunnelAction*>(action))
        return 1.0f;

    return dynamic_cast<MovementAction*>(action) ? 0.0f : 1.0f;
}

LostCityOfTheTolvirTriggerContext::LostCityOfTheTolvirTriggerContext()
{
    creators["lct use wind tunnel"] = [](PlayerbotAI* ai) -> Trigger* { return new WindTunnelTrigger(ai); };
    creators["lct priority target"] = [](PlayerbotAI* ai) -> Trigger* { return new PriorityTargetTrigger(ai); };
    creators["lct avoid hazard"] = [](PlayerbotAI* ai) -> Trigger* { return new HazardTrigger(ai); };
}

LostCityOfTheTolvirActionContext::LostCityOfTheTolvirActionContext()
{
    creators["lct use wind tunnel"] = [](PlayerbotAI* ai) -> Action* { return new UseWindTunnelAction(ai); };
    creators["lct attack priority target"] = [](PlayerbotAI* ai) -> Action* { return new AttackPriorityTargetAction(ai); };
    creators["lct avoid hazard"] = [](PlayerbotAI* ai) -> Action* { return new AvoidHazardAction(ai); };
}
}
