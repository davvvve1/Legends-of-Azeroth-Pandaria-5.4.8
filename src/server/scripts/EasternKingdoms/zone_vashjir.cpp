/*
* This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "ScriptMgr.h"
#include "GameObject.h"
#include "MoveSplineInit.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "Vehicle.h"

// http://www.wowhead.com/quest=25281/pay-it-forward http://www.wowhead.com/quest=25936/pay-it-forward
enum QuestPayItForward
{
    SPELL_BLOW_BUBBLE_A  = 74151,
    SPELL_BUBBLE_SELF    = 74416,
    SPELL_BLOW_BUBBLE_H  = 77825,

    NPC_DROWNING_SOLDIER = 39663,
    NPC_DROWNING_WARRIOR = 41672
};

struct npc_drowning_soldier_and_warrior : public ScriptedAI
{
    npc_drowning_soldier_and_warrior(Creature* creature) : ScriptedAI(creature)
    {
        JustAppeared();
    }

    void JustAppeared() override
    {
        me->setRegeneratingHealth(false);
        me->SetHealth(6190);
        move_timer = 0;
    }

    void SpellHit(Unit* caster, const SpellInfo* spell) override
    {
        if (Player* player = caster->ToPlayer())
        {
            if (me->HasAura(SPELL_BUBBLE_SELF))
                return;
            if (spell->Id == SPELL_BLOW_BUBBLE_A)
                player->KilledMonsterCredit(NPC_DROWNING_SOLDIER);
            else if (spell->Id == SPELL_BLOW_BUBBLE_H)
                player->KilledMonsterCredit(NPC_DROWNING_WARRIOR);
            me->CastSpell(me, SPELL_BUBBLE_SELF, true);
            move_timer = 3000;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!move_timer)
            return;

        move_timer -= diff;
        if (move_timer <= 0)
        {
            move_timer = 0;

            // Бежать вперёд на 15 ярдов
            const float angle = (me->GetOrientation()) - static_cast<float>(M_PI / 2);
            const float x = me->GetPositionX() - 15 * std::sin(angle);
            const float y = me->GetPositionY() + 15 * std::cos(angle);
            const float z = me->GetMap()->GetHeight(x, y, me->GetPositionZ());
            me->GetMotionMaster()->MovePoint(0, x, y, z);

            me->DespawnOrUnsummon(5000);
        }
    }
private:
    int32 move_timer;
};

namespace BloodAndThunder
{
    enum Data
    {
        QUEST_BLOOD_AND_THUNDER          = 25949,

        NPC_LEGIONNAIRE_NAZGRIM          = 41793,
        NPC_HELLSCREAMS_VANGUARD_1       = 41796,
        NPC_HELLSCREAMS_VANGUARD_2       = 41797,
        NPC_HELLSCREAMS_VANGUARD_3       = 41798,
        NPC_HELLSCREAMS_VANGUARD_4       = 41799,
        NPC_HELLSCREAMS_VANGUARD_5       = 41800,
        NPC_TROOP_ABDUCTOR               = 41809,
        NPC_PLAYER_ABDUCTOR              = 41838,
        NPC_ERUNAK_RESCUE                 = 41788,
        NPC_MOANAH_RESCUE                 = 41845,
        NPC_RENDEL_RESCUE                 = 41847,
        NPC_DEFENSE_COMPLETE              = 41759,

        SPELL_SUMMON_ERUNAK_RESCUE        = 78005,
        SPELL_SUMMON_MOANAH_RESCUE        = 78007,
        SPELL_SUMMON_RENDEL_RESCUE        = 78009,

        ACTION_CAPTURE_VANGUARD           = 1
    };

    Position const RescuePath[] =
    {
        { -4625.0f, 3938.0f,  -82.0f, 0.0f },
        { -4690.0f, 3908.0f,  -95.0f, 0.0f },
        { -4760.0f, 3865.0f, -112.0f, 0.0f },
        { -4825.0f, 3815.0f, -130.0f, 0.0f },
        { -4875.0f, 3785.0f, -141.0f, 0.0f },
        { -4891.5f, 3774.0f, -145.5f, 0.0f }
    };

    Position const RescueFallbackPositions[] =
    {
        { -4887.5f, 3771.0f, -145.5f, 2.8f },
        { -4889.0f, 3778.0f, -145.5f, 3.5f },
        { -4884.0f, 3775.0f, -145.5f, 3.2f }
    };

    uint32 const VanguardEntries[] =
    {
        NPC_HELLSCREAMS_VANGUARD_1,
        NPC_HELLSCREAMS_VANGUARD_2,
        NPC_HELLSCREAMS_VANGUARD_3,
        NPC_HELLSCREAMS_VANGUARD_4,
        NPC_HELLSCREAMS_VANGUARD_5
    };

    bool IsAtImmortalCoil(Player const* player)
    {
        return player->GetMapId() == 0 && player->GetZoneId() == 4815 &&
            player->GetDistance2d(-4592.0f, 3964.0f) < 450.0f;
    }
}

// The six ambient abductors already have sniffed paths from their holding
// positions to the Immortal Coil. The missing AI only needs to board one of
// Nazgrim's troops at the inner end and release it at the outer end.
struct npc_blood_and_thunder_troop_abductor : public ScriptedAI
{
    npc_blood_and_thunder_troop_abductor(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        captureEnabled = false;
    }

    void DoAction(int32 action) override
    {
        if (action == BloodAndThunder::ACTION_CAPTURE_VANGUARD)
            captureEnabled = true;
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != WAYPOINT_MOTION_TYPE || !me->GetVehicleKit())
            return;

        if (pointId == 1)
        {
            if (Unit* passenger = me->GetVehicleKit()->GetPassenger(0))
            {
                passenger->ExitVehicle(me);
                if (Creature* vanguard = passenger->ToCreature())
                    vanguard->DespawnOrUnsummon(1000);
                captureEnabled = false;
            }
            return;
        }

        if (!captureEnabled || pointId < 6 || me->GetVehicleKit()->GetPassenger(0))
            return;

        for (uint32 entry : BloodAndThunder::VanguardEntries)
            if (Creature* vanguard = me->FindNearestCreature(entry, 22.0f, true))
                if (!vanguard->GetVehicle())
                {
                    vanguard->CombatStop(true);
                    vanguard->EnterVehicle(me, 0);
                    break;
                }
    }

private:
    bool captureEnabled = false;
};

// Personal rescue transport. Entry 41838 is the original player vehicle;
// unlike the ambient troop abductors, it was present in the client data but
// had neither a summon trigger nor movement AI in the world database.
struct npc_blood_and_thunder_player_abductor : public ScriptedAI
{
    npc_blood_and_thunder_player_abductor(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* summoner) override
    {
        Player* player = summoner->ToPlayer();
        if (!player || player->GetQuestStatus(BloodAndThunder::QUEST_BLOOD_AND_THUNDER) != QUEST_STATUS_INCOMPLETE)
        {
            me->DespawnOrUnsummon();
            return;
        }

        playerGuid = player->GetGUID();
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        me->SetCanFly(true);
        me->SetDisableGravity(true);
        me->SetSpeed(MOVE_FLIGHT, 2.0f, true);
        me->UpdateMovementFlags();

        player->CombatStop(true);
        player->ExitVehicle();
        player->EnterVehicle(me, 0);
        Talk(0, player);

        Movement::MoveSplineInit init(me);
        for (Position const& position : BloodAndThunder::RescuePath)
            init.Path().push_back(G3D::Vector3(position.GetPositionX(), position.GetPositionY(), position.GetPositionZ()));
        init.SetFly();
        init.SetUncompressed();
        init.SetSmooth();
        init.SetVelocity(14.0f);
        init.Launch();

        me->m_Events.Schedule(9 * IN_MILLISECONDS, [this]()
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, playerGuid))
                Talk(1, player);
        });

        me->m_Events.Schedule(me->GetSplineDuration() + 500, [this]()
        {
            FinishRescue();
        });
    }

private:
    void FinishRescue()
    {
        Player* player = ObjectAccessor::GetPlayer(*me, playerGuid);
        if (!player)
        {
            me->DespawnOrUnsummon();
            return;
        }

        player->CastSpell(player, BloodAndThunder::SPELL_SUMMON_ERUNAK_RESCUE, true);
        player->CastSpell(player, BloodAndThunder::SPELL_SUMMON_MOANAH_RESCUE, true);
        player->CastSpell(player, BloodAndThunder::SPELL_SUMMON_RENDEL_RESCUE, true);

        Creature* erunak = player->FindNearestCreature(BloodAndThunder::NPC_ERUNAK_RESCUE, 30.0f, true);
        if (!erunak)
            erunak = player->SummonCreature(BloodAndThunder::NPC_ERUNAK_RESCUE,
                BloodAndThunder::RescueFallbackPositions[0], TEMPSUMMON_TIMED_DESPAWN, 60 * IN_MILLISECONDS);
        if (!player->FindNearestCreature(BloodAndThunder::NPC_MOANAH_RESCUE, 30.0f, true))
            player->SummonCreature(BloodAndThunder::NPC_MOANAH_RESCUE,
                BloodAndThunder::RescueFallbackPositions[1], TEMPSUMMON_TIMED_DESPAWN, 60 * IN_MILLISECONDS);
        if (!player->FindNearestCreature(BloodAndThunder::NPC_RENDEL_RESCUE, 30.0f, true))
            player->SummonCreature(BloodAndThunder::NPC_RENDEL_RESCUE,
                BloodAndThunder::RescueFallbackPositions[2], TEMPSUMMON_TIMED_DESPAWN, 60 * IN_MILLISECONDS);

        player->ExitVehicle(me);
        player->NearTeleportTo(-4892.0f, 3773.0f, -146.5f, 5.85f, false);
        player->KilledMonsterCredit(BloodAndThunder::NPC_DEFENSE_COMPLETE);

        if (erunak)
        {
            erunak->AI()->Talk(0, player);
            erunak->Kill(me);
        }
        else
            me->DespawnOrUnsummon();
    }

    ObjectGuid playerGuid;
};

namespace BloodAndThunder
{
    class CaptureEvent final : public BasicEvent
    {
    public:
        explicit CaptureEvent(ObjectGuid guid) : playerGuid(guid) { }

        bool Execute(uint64 /*executionTime*/, uint32 /*diff*/) override
        {
            Player* player = ObjectAccessor::FindPlayer(playerGuid);
            if (!player || player->GetQuestStatus(QUEST_BLOOD_AND_THUNDER) != QUEST_STATUS_INCOMPLETE)
                return true;

            // Once the defense has started, swimming away or continuing to
            // fight must not evade the scripted defeat and abduction.
            if (!player->IsAlive() || player->GetMapId() != 0 || player->GetZoneId() != 4815)
                return true;

            std::list<Creature*> abductors;
            GetCreatureListWithEntryInGrid(abductors, player, NPC_TROOP_ABDUCTOR, 450.0f);
            for (Creature* abductor : abductors)
                abductor->AI()->DoAction(ACTION_CAPTURE_VANGUARD);

            if (Creature* nazgrim = player->FindNearestCreature(NPC_LEGIONNAIRE_NAZGRIM, 450.0f, true))
                nazgrim->AI()->Talk(0, player);

            player->SummonCreature(NPC_PLAYER_ABDUCTOR, *player, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 2 * MINUTE * IN_MILLISECONDS);
            return true;
        }

    private:
        ObjectGuid playerGuid;
    };

    void ScheduleCapture(Player* player)
    {
        if (!player || player->GetQuestStatus(QUEST_BLOOD_AND_THUNDER) != QUEST_STATUS_INCOMPLETE ||
            !IsAtImmortalCoil(player) || player->GetVehicleBase())
            return;

        if (player->m_Events.FindEvent([](BasicEvent const* event)
            {
                return dynamic_cast<CaptureEvent const*>(event) != nullptr;
            }))
            return;

        player->m_Events.Schedule(3 * MINUTE * IN_MILLISECONDS, new CaptureEvent(player->GetGUID()));
    }
}

class player_blood_and_thunder : public PlayerScript
{
public:
    player_blood_and_thunder() : PlayerScript("player_blood_and_thunder") { }

    void OnQuestAdded(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == BloodAndThunder::QUEST_BLOOD_AND_THUNDER)
            BloodAndThunder::ScheduleCapture(player);
    }

    void OnLogin(Player* player) override
    {
        BloodAndThunder::ScheduleCapture(player);
    }

    void OnUpdate(Player* player, uint32 /*diff*/) override
    {
        BloodAndThunder::ScheduleCapture(player);
    }
};

namespace AbyssalRide
{
    enum Data
    {
        QUEST_THE_ABYSSAL_RIDE       = 25371,

        NPC_ABYSSAL_LURE             = 39942,
        NPC_ABYSSAL_SEAHORSE         = 39996,

        SPELL_LEAN_LEFT              = 87217,
        SPELL_HOLD_ON_TIGHT          = 86332,
        SPELL_LEAN_RIGHT             = 87219,

        EVENT_PROMPT                 = 1,
        EVENT_FINISH_RIDE            = 2
    };

    Position const RidePath[] =
    {
        { -4888.0f, 3796.0f, -149.0f, 0.0f },
        { -4930.0f, 3765.0f, -160.0f, 0.0f },
        { -5000.0f, 3730.0f, -175.0f, 0.0f },
        { -5080.0f, 3700.0f, -195.0f, 0.0f },
        { -5180.0f, 3675.0f, -215.0f, 0.0f },
        { -5280.0f, 3650.0f, -235.0f, 0.0f },
        { -5400.0f, 3620.0f, -250.0f, 0.0f },
        { -5520.0f, 3615.0f, -240.0f, 0.0f },
        { -5600.0f, 3650.0f, -225.0f, 0.0f },
        { -5520.0f, 3700.0f, -205.0f, 0.0f },
        { -5400.0f, 3740.0f, -190.0f, 0.0f },
        { -5250.0f, 3770.0f, -180.0f, 0.0f },
        { -5100.0f, 3800.0f, -165.0f, 0.0f },
        { -4960.0f, 3810.0f, -155.0f, 0.0f },
        { -4890.0f, 3800.0f, -149.0f, 0.0f }
    };

    uint32 const PromptSpells[] =
    {
        SPELL_LEAN_LEFT,
        SPELL_HOLD_ON_TIGHT,
        SPELL_LEAN_RIGHT,
        SPELL_LEAN_LEFT,
        SPELL_LEAN_RIGHT,
        SPELL_HOLD_ON_TIGHT,
        SPELL_LEAN_LEFT,
        SPELL_LEAN_RIGHT,
        SPELL_HOLD_ON_TIGHT,
        SPELL_LEAN_RIGHT,
        SPELL_LEAN_LEFT,
        SPELL_HOLD_ON_TIGHT,
        SPELL_LEAN_RIGHT,
        SPELL_LEAN_LEFT
    };
}

// 202766 - Braided Rope
class go_the_abyssal_ride_braided_rope : public GameObjectScript
{
public:
    go_the_abyssal_ride_braided_rope() : GameObjectScript("go_the_abyssal_ride_braided_rope") { }

    bool OnGossipHello(Player* player, GameObject* go) override
    {
        if (player->GetQuestStatus(AbyssalRide::QUEST_THE_ABYSSAL_RIDE) != QUEST_STATUS_INCOMPLETE ||
            player->GetVehicleBase())
            return true;

        std::list<Creature*> horses;
        GetCreatureListWithEntryInGrid(horses, player, AbyssalRide::NPC_ABYSSAL_SEAHORSE, 120.0f);
        for (Creature* horse : horses)
            if (horse->GetOwnerGUID() == player->GetGUID())
                return true;

        Position summonPosition = player->GetNearPosition(35.0f, 0.0f);
        Position arrivalPosition = player->GetNearPosition(6.0f, 0.0f);
        summonPosition.m_positionZ = player->GetPositionZ();
        arrivalPosition.m_positionZ = player->GetPositionZ();

        Creature* horse = player->SummonCreature(AbyssalRide::NPC_ABYSSAL_SEAHORSE,
            summonPosition, TEMPSUMMON_TIMED_DESPAWN, 2 * MINUTE * IN_MILLISECONDS);
        if (!horse)
            return true;

        player->KilledMonsterCredit(AbyssalRide::NPC_ABYSSAL_LURE);
        go->UseDoorOrButton();

        Movement::MoveSplineInit init(horse);
        init.MoveTo(arrivalPosition.GetPositionX(), arrivalPosition.GetPositionY(), arrivalPosition.GetPositionZ());
        init.SetFly();
        init.SetUncompressed();
        init.SetSmooth();
        init.SetVelocity(8.0f);
        init.Launch();

        horse->m_Events.Schedule(horse->GetSplineDuration(), [horse]()
        {
            if (horse->IsInWorld() && horse->IsAIEnabled)
                horse->AI()->DoAction(1);
        });

        return true;
    }
};

// 39996 - Abyssal Seahorse
struct npc_the_abyssal_ride_seahorse : public ScriptedAI
{
    npc_the_abyssal_ride_seahorse(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* summoner) override
    {
        Player* player = summoner->ToPlayer();
        if (!player || player->GetQuestStatus(AbyssalRide::QUEST_THE_ABYSSAL_RIDE) != QUEST_STATUS_INCOMPLETE)
        {
            me->DespawnOrUnsummon();
            return;
        }

        playerGuid = player->GetGUID();
        me->SetReactState(REACT_PASSIVE);
        me->SetCanFly(true);
        me->SetDisableGravity(true);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        me->UpdateMovementFlags();
    }

    void DoAction(int32 action) override
    {
        if (action != 1 || ready || rideStarted)
            return;

        ready = true;
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    }

    void OnSpellClick(Unit* clicker, bool& result) override
    {
        Player* player = clicker ? clicker->ToPlayer() : nullptr;
        if (!ready || rideStarted || !player || player->GetGUID() != playerGuid ||
            player->GetQuestStatus(AbyssalRide::QUEST_THE_ABYSSAL_RIDE) != QUEST_STATUS_INCOMPLETE ||
            player->GetVehicleBase())
        {
            result = false;
            return;
        }

        result = true;
        player->EnterVehicle(me, 0);
        StartRide(player);
    }

    void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply) override
    {
        Player* player = passenger ? passenger->ToPlayer() : nullptr;
        if (!player || player->GetGUID() != playerGuid)
            return;

        if (apply)
        {
            StartRide(player);
            return;
        }

        if (!finished)
            AbortRide(player);
    }

    void SpellHitTarget(Unit* target, SpellInfo const* spell) override
    {
        if (!rideStarted || finished || !target || target->GetGUID() != playerGuid ||
            !spell || spell->Id != expectedSpell)
            return;

        if (pressesNeeded > 1)
        {
            --pressesNeeded;
            return;
        }

        pressesNeeded = 0;
        expectedSpell = 0;
        Talk(3, target);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case AbyssalRide::EVENT_PROMPT:
                    GivePrompt();
                    break;
                case AbyssalRide::EVENT_FINISH_RIDE:
                    FinishRide();
                    break;
                default:
                    break;
            }
        }
    }

private:
    void StartRide(Player* player)
    {
        if (rideStarted || !player || player->GetVehicleBase() != me)
            return;

        rideStarted = true;
        ready = false;
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        player->VehicleSpellInitialize();

        Movement::MoveSplineInit init(me);
        for (Position const& position : AbyssalRide::RidePath)
            init.Path().push_back(G3D::Vector3(position.GetPositionX(), position.GetPositionY(), position.GetPositionZ()));
        init.SetFly();
        init.SetUncompressed();
        init.SetSmooth();
        init.SetVelocity(20.0f);
        init.Launch();

        events.ScheduleEvent(AbyssalRide::EVENT_PROMPT, 3 * IN_MILLISECONDS);
        events.ScheduleEvent(AbyssalRide::EVENT_FINISH_RIDE, me->GetSplineDuration() + 250);
    }

    void GivePrompt()
    {
        Player* player = ObjectAccessor::GetPlayer(*me, playerGuid);
        if (!player || player->GetVehicleBase() != me || promptIndex >= std::size(AbyssalRide::PromptSpells))
            return;

        expectedSpell = AbyssalRide::PromptSpells[promptIndex++];
        pressesNeeded = expectedSpell == AbyssalRide::SPELL_HOLD_ON_TIGHT ? 3 : 1;

        if (expectedSpell == AbyssalRide::SPELL_LEAN_LEFT)
            Talk(0, player);
        else if (expectedSpell == AbyssalRide::SPELL_HOLD_ON_TIGHT)
            Talk(1, player);
        else
            Talk(2, player);

        if (promptIndex < std::size(AbyssalRide::PromptSpells))
            events.ScheduleEvent(AbyssalRide::EVENT_PROMPT, 5 * IN_MILLISECONDS);
    }

    void RemoveRideAuras(Player* player)
    {
        player->RemoveAurasDueToSpell(AbyssalRide::SPELL_LEAN_LEFT);
        player->RemoveAurasDueToSpell(AbyssalRide::SPELL_HOLD_ON_TIGHT);
        player->RemoveAurasDueToSpell(AbyssalRide::SPELL_LEAN_RIGHT);
    }

    void FinishRide()
    {
        if (finished)
            return;

        finished = true;
        events.Reset();

        Player* player = ObjectAccessor::GetPlayer(*me, playerGuid);
        if (player)
        {
            RemoveRideAuras(player);
            if (player->GetVehicleBase() == me)
            {
                Talk(4, player);
                player->KilledMonsterCredit(AbyssalRide::NPC_ABYSSAL_SEAHORSE);
                player->ExitVehicle(me);
            }
        }

        me->DespawnOrUnsummon(1000);
    }

    void AbortRide(Player* player)
    {
        finished = true;
        events.Reset();
        RemoveRideAuras(player);
        player->NearTeleportTo(-4889.92f, 3799.13f, -149.06f, 3.80f, false);
        me->DespawnOrUnsummon();
    }

    EventMap events;
    ObjectGuid playerGuid;
    uint32 expectedSpell = 0;
    uint8 promptIndex = 0;
    uint8 pressesNeeded = 0;
    bool ready = false;
    bool rideStarted = false;
    bool finished = false;
};

namespace HonorAndPrivilege
{
    enum : uint32
    {
        QUEST_HONOR_AND_PRIVILEGE_ALLIANCE = 25898,
        QUEST_HONOR_AND_PRIVILEGE_HORDE    = 25972,
        NPC_RESCUE_BALLOON_CREDIT          = 41572
    };
}

// 77741 - Rescue Flare
// The client spell only contains a dummy effect. Retail awards the shared
// Rescue Balloon objective when the flare is fired at the surface.
class spell_honor_and_privilege_rescue_flare : public SpellScript
{
    PrepareSpellScript(spell_honor_and_privilege_rescue_flare);

    void HandleAfterCast()
    {
        Player* player = GetCaster()->ToPlayer();
        if (!player || player->GetMapId() != 0 || player->GetZoneId() != 5144 || player->GetPositionZ() < -30.0f)
            return;

        if (player->GetQuestStatus(HonorAndPrivilege::QUEST_HONOR_AND_PRIVILEGE_ALLIANCE) == QUEST_STATUS_INCOMPLETE ||
            player->GetQuestStatus(HonorAndPrivilege::QUEST_HONOR_AND_PRIVILEGE_HORDE) == QUEST_STATUS_INCOMPLETE)
            player->KilledMonsterCredit(HonorAndPrivilege::NPC_RESCUE_BALLOON_CREDIT);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_honor_and_privilege_rescue_flare::HandleAfterCast);
    }
};

void AddSC_vashjir()
{
    new creature_script<npc_drowning_soldier_and_warrior>("npc_drowning_soldier_and_warrior");
    new creature_script<npc_blood_and_thunder_troop_abductor>("npc_blood_and_thunder_troop_abductor");
    new creature_script<npc_blood_and_thunder_player_abductor>("npc_blood_and_thunder_player_abductor");
    new player_blood_and_thunder();
    new go_the_abyssal_ride_braided_rope();
    new creature_script<npc_the_abyssal_ride_seahorse>("npc_the_abyssal_ride_seahorse");
    new spell_script<spell_honor_and_privilege_rescue_flare>("spell_honor_and_privilege_rescue_flare");
}
