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

void AddSC_vashjir()
{
    new creature_script<npc_drowning_soldier_and_warrior>("npc_drowning_soldier_and_warrior");
    new creature_script<npc_blood_and_thunder_troop_abductor>("npc_blood_and_thunder_troop_abductor");
    new creature_script<npc_blood_and_thunder_player_abductor>("npc_blood_and_thunder_player_abductor");
    new player_blood_and_thunder();
}
