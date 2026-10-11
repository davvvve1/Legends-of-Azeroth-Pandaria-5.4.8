/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
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
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "Chat.h"
#include "DatabaseEnv.h"
#include "Spell.h"
#include "World.h"

#include <array>
#include <unordered_map>

enum eBonobosSpells
{
    SPELL_GOING_BANANAS     = 125363,
    SPELL_BANANARANG        = 125311,
    SPELL_TOSS_FILTH        = 125365,
};

enum eBonobosEvents
{
    EVENT_GOING_BANANAS         = 1,
    EVENT_BANANARANG            = 2,
    EVENT_TOSS_FILTH            = 3,
};

class npc_bonobos : public CreatureScript
{
    public:
        npc_bonobos() : CreatureScript("npc_bonobos") { }

        struct npc_bonobosAI : public ScriptedAI
        {
            npc_bonobosAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_GOING_BANANAS,       12000);
                events.ScheduleEvent(EVENT_BANANARANG,           8000);
                events.ScheduleEvent(EVENT_TOSS_FILTH,          15000);
            }

            void JustDied(Unit* /*killer*/) override { }

            void JustSummoned(Creature* summon) override
            {
                summon->DespawnOrUnsummon(12000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_GOING_BANANAS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_GOING_BANANAS, false);
                            events.ScheduleEvent(EVENT_GOING_BANANAS,      10000);
                            break;
                        case EVENT_BANANARANG:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_BANANARANG, false);
                            events.ScheduleEvent(EVENT_BANANARANG, 20000);
                            break;
                        case EVENT_TOSS_FILTH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_TOSS_FILTH, false);
                            events.ScheduleEvent(EVENT_TOSS_FILTH, 15000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_bonobosAI(creature);
        }
};

enum eSeleNaSpells
{
    SPELL_RAIN_DANCE    = 124860,
    SPELL_TORRENT       = 124935,
    SPELL_WATER_BOLT    = 124854,
};

enum eSeleNaEvents
{
    EVENT_RAIN_DANCE        = 1,
    EVENT_TORRENT           = 2,
    EVENT_WATER_BOLT        = 3,
};

class npc_sele_na : public CreatureScript
{
    public:
        npc_sele_na() : CreatureScript("npc_sele_na") { }

        struct npc_sele_naAI : public ScriptedAI
        {
            npc_sele_naAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_RAIN_DANCE,       5000);
                events.ScheduleEvent(EVENT_TORRENT,         15000);
                events.ScheduleEvent(EVENT_WATER_BOLT,      25000);
            }

            void JustDied(Unit* /*killer*/) override { }

            void JustSummoned(Creature* summon) override
            {
                summon->DespawnOrUnsummon(12000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_RAIN_DANCE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_RAIN_DANCE, false);
                            events.ScheduleEvent(EVENT_RAIN_DANCE,       5000);
                            break;
                        case EVENT_TORRENT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_TORRENT, false);
                            events.ScheduleEvent(EVENT_TORRENT, 15000);
                            break;
                        case EVENT_WATER_BOLT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_WATER_BOLT, false);
                            events.ScheduleEvent(EVENT_WATER_BOLT, 25000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sele_naAI(creature);
        }
};

enum eBlackhoofSpells
{
    SPELL_BELLOWING_RAGE    = 124297,
    SPELL_RUSHING_RAGE      = 124302,
    SPELL_YAUNGOL_STOMP     = 124289,
};

enum eBlackhoofEvents
{
    EVENT_BELLOWING_RAGE        = 1,
    EVENT_RUSHING_RAGE          = 2,
    EVENT_YAUNGOL_STOMP         = 3,
};

class npc_blackhoof : public CreatureScript
{
    public:
        npc_blackhoof() : CreatureScript("npc_blackhoof") { }

        struct npc_blackhoofAI : public ScriptedAI
        {
            npc_blackhoofAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_RUSHING_RAGE,         5000);
                events.ScheduleEvent(EVENT_YAUNGOL_STOMP,       15000);
                events.ScheduleEvent(EVENT_BELLOWING_RAGE,      25000);
            }

            void JustDied(Unit* /*killer*/) override { }

            void JustSummoned(Creature* summon) override
            {
                summon->DespawnOrUnsummon(12000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BELLOWING_RAGE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_BELLOWING_RAGE, false);
                            events.ScheduleEvent(EVENT_BELLOWING_RAGE,       25000);
                            break;
                        case EVENT_RUSHING_RAGE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_RUSHING_RAGE, false);
                            events.ScheduleEvent(EVENT_RUSHING_RAGE, 5000);
                            break;
                        case EVENT_YAUNGOL_STOMP:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_YAUNGOL_STOMP, false);
                            events.ScheduleEvent(EVENT_YAUNGOL_STOMP, 15000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_blackhoofAI(creature);
        }
};

enum eIkThikWarriorSpells
{
    SPELL_PIERCE_ARMOR          = 6016,
    SPELL_SHOCK_AND_AWE         = 118538,
};

enum eIkThikWarriorEvents
{
    EVENT_PIERCE_ARMOR          = 1,
    EVENT_SHOCK_AND_AWE         = 2,
};

class npc_ik_thik_warrior : public CreatureScript
{
    public:
        npc_ik_thik_warrior() : CreatureScript("npc_ik_thik_warrior") { }

        struct npc_ik_thik_warriorAI : public ScriptedAI
        {
            npc_ik_thik_warriorAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_PIERCE_ARMOR,         5000);
                events.ScheduleEvent(EVENT_SHOCK_AND_AWE,        15000);
            }

            void JustDied(Unit* /*killer*/) override { }

            void JustSummoned(Creature* summon) override
            {
                summon->DespawnOrUnsummon(12000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PIERCE_ARMOR:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_PIERCE_ARMOR, false);
                            events.ScheduleEvent(EVENT_PIERCE_ARMOR,       25000);
                            break;
                        case EVENT_SHOCK_AND_AWE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_SHOCK_AND_AWE, false);
                            events.ScheduleEvent(EVENT_SHOCK_AND_AWE, 40000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ik_thik_warriorAI(creature);
        }
};

// Muddy Water quest
class spell_gen_gather_muddy_water : public SpellScriptLoader
{
    public:
        spell_gen_gather_muddy_water() : SpellScriptLoader("spell_gen_gather_muddy_water") { }

        class spell_gen_gather_muddy_water_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_gather_muddy_water_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                auto target = GetHitUnit();
                if (!target || target->GetTypeId() != TYPEID_PLAYER)
                    return;

                auto player = GetCaster()->ToPlayer();
                if (player->GetQuestStatus(29951) == QUEST_STATUS_INCOMPLETE)
                {
                    if (!player->HasAura(106284))
                    {
                        BroadcastText const* bct = sObjectMgr->GetBroadcastText(55303);
                        LocaleConstant loc_idx = player->GetSession()->GetSessionDbLocaleIndex();
                        std::string baseText = "";
                        if (bct)
                            baseText = bct->GetText(loc_idx, player->GetGender());                   
                        player->TextEmote(baseText);
                        player->CastSpell(player, 106284, true);
                    }
                    else
                        player->CastSpell(player, 106294, true);

                    if (player->GetPower(POWER_ALTERNATE_POWER) == 100)
                    {
                        player->AddItem(76356, 1);
                        player->RemoveAurasDueToSpell(106284);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_gather_muddy_water_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_gen_gather_muddy_water_SpellScript();
        }
};

class spell_gen_gather_muddy_water_aura : public SpellScriptLoader
{
    public:
        spell_gen_gather_muddy_water_aura() : SpellScriptLoader("spell_gen_gather_muddy_water_aura") { }

        class spell_gen_gather_muddy_water_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_gather_muddy_water_aura_AuraScript);

            void HandlePeriodic(AuraEffect const* /*aurEff*/)
            {
                if (!GetTarget()->isMoving())
                    PreventDefaultAction();
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_gather_muddy_water_aura_AuraScript::HandlePeriodic, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gen_gather_muddy_water_aura_AuraScript();
        }
};

// Crouching Carrot, Hidden Turnip quest
class npc_orange_painted_turnip : public CreatureScript
{
    public:
        npc_orange_painted_turnip() : CreatureScript("npc_orange_painted_turnip") { }

        struct npc_orange_painted_turnipAI : public ScriptedAI
        {
            npc_orange_painted_turnipAI(Creature* creature) : ScriptedAI(creature)
            {
                creature->SetReactState(REACT_PASSIVE);
                creature->SetFaction(35);
            }

            uint32 timer;

            void Reset() override
            {
                timer = 8000;
                std::list<Creature*> clist;
                GetCreatureListWithEntryInGrid(clist, me, 56538, 10.f);
                for (auto &&c : clist)
                {
                    if (!me->IsWithinLOSInMap(c))
                        continue;

                    float x, y, z;
                    me->GetClosePoint(x, y, z, 0.f, 0.f, c->GetAngle(me));
                    c->AI()->Talk(0);
                    c->GetMotionMaster()->MoveIdle();
                    c->GetMotionMaster()->MovePoint(1, x, y, z);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (timer <= diff)
                {
                    std::list<Creature*> clist;
                    GetCreatureListWithEntryInGrid(clist, me, 56538, 10.f);
                    for (auto &&c : clist)
                    {
                        if (!me->IsWithinLOSInMap(c))
                            continue;

                        if (auto player = me->GetCharmerOrOwnerPlayerOrPlayerItself())
                            player->KilledMonsterCredit(56544);

                        if (c->AI())
                            c->AI()->Talk(1);

                        c->GetMotionMaster()->MoveFleeing(me, 4000);
                        c->DespawnOrUnsummon(4000);
                    }
                    me->DespawnOrUnsummon(5000);
                    timer = 10000;
                }
                else
                    timer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_orange_painted_turnipAI(creature);
        }
};

class DisorientedTargetSelector
{
    public:
        DisorientedTargetSelector() { }

        bool operator()(WorldObject* object)
        {
            return object && object->ToPlayer();
        }
};

// Disoriented 115226
class spell_vfw_disoriented : public SpellScriptLoader
{
    public:
        spell_vfw_disoriented() : SpellScriptLoader("spell_vfw_disoriented") { }

        class spell_vfw_disoriented_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_vfw_disoriented_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(DisorientedTargetSelector());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_vfw_disoriented_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_vfw_disoriented_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript *GetSpellScript() const override
        {
            return new spell_vfw_disoriented_SpellScript();
        }
};

enum q29982
{
    NPC_WEI_BLACKSOIL          = 57120,
    NPC_FENG_SPADEPAW          = 57121,
    NPC_SHU_LI_SPADEPAW        = 57122,
    NPC_HAIYUN_GREENTILL       = 57123,
    NPC_YING_GRENNTILL         = 57124,
    NPC_HUIZHONG_GRENNTILL     = 57125,
    NPC_ZHANG_MARLFUR          = 57126,
    NPC_MIA_MARLFUR            = 57127,
    NPC_IKTHIK_AMBUSHER        = 57134,

    NPC_WEI_BLACKSOIL_SUM      = 57155,
    NPC_HAIYUN_GREENTILL_SUM   = 57161,
    NPC_YING_GRENNTILL_SUM     = 57162,
    NPC_HUIZHONG_GRENNTILL_SUM = 57163,
    NPC_ZHANG_MARLFUR_SUM      = 57183,

    SPELL_SUMMON_WEI_BLACKSOIL = 107515,
    SPELL_SUMMON_YOON          = 107524, // npc like NPC_HAIYUN_GREENTILL
    SPELL_SUMMON_YING          = 107525,
    SPELL_SUMMON_HUIZHONG      = 107527,
    SPELL_SUMMON_ZHANG         = 107546,

    DATA_AMBUSHED              = 0,

    SPELL_SWIFT_AMBUSH         = 118355,
    SPELL_LIT_THROAT           = 118370,

    EVENT_SWIFT_AMBUSH         = 1,
    EVENT_SLIT_THROAT          = 2,
};

class npc_q29982 : public CreatureScript
{
    public:
        npc_q29982() : CreatureScript("npc_q29982") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (player->GetQuestStatus(29982) == QUEST_STATUS_INCOMPLETE)
            {
                if ((creature->GetEntry() == NPC_WEI_BLACKSOIL     && creature->AI()->GetData(DATA_AMBUSHED) == 0  && !player->GetQuestObjectiveCounter(252177)) ||
                    ((creature->GetEntry() == NPC_FENG_SPADEPAW    || creature->GetEntry() == NPC_SHU_LI_SPADEPAW) && !player->GetQuestObjectiveCounter(252178)) ||
                    ((creature->GetEntry() == NPC_HAIYUN_GREENTILL || creature->GetEntry() == NPC_YING_GRENNTILL)  && !player->GetQuestObjectiveCounter(252179)) ||
                    ((creature->GetEntry() == NPC_ZHANG_MARLFUR    || creature->GetEntry() == NPC_MIA_MARLFUR)     && !player->GetQuestObjectiveCounter(252180)))
                    player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

                if (creature->GetEntry() == NPC_WEI_BLACKSOIL && creature->AI()->GetData(DATA_AMBUSHED) == 1 && !player->GetQuestObjectiveCounter(252177))
                    player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            }

            if (creature->GetEntry() == NPC_WEI_BLACKSOIL && creature->AI()->GetData(DATA_AMBUSHED) == 0)
                player->SEND_GOSSIP_MENU(18832, creature->GetGUID());
            else if (creature->GetEntry() == NPC_WEI_BLACKSOIL && creature->AI()->GetData(DATA_AMBUSHED) == 1)
                player->SEND_GOSSIP_MENU(18833, creature->GetGUID());
            else
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->CLOSE_GOSSIP_MENU();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                ObjectGuid playerGuid = player->GetGUID();
                uint32 delay = 0;
                switch (creature->GetEntry())
                {
                    case NPC_WEI_BLACKSOIL:
                    {
                        Creature* wei = creature->GetEntry() == NPC_WEI_BLACKSOIL ? creature : creature->FindNearestCreature(NPC_WEI_BLACKSOIL, 10.0f, true);
                        if (!wei)
                            break;

                        wei->AI()->Talk(0);
                        wei->m_Events.Schedule(delay += 3000, [wei, playerGuid]()
                        {
                            wei->AI()->Talk(1);
                            if (Creature* creat = wei->SummonCreature(NPC_IKTHIK_AMBUSHER, -318.643f, 2377.642f, 140.410f, 2.3762f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000))
                                if (Player* player = ObjectAccessor::GetPlayer(*wei, playerGuid))
                                    creat->AI()->AttackStart(player);
                        });
                        break;
                    }
                    case NPC_FENG_SPADEPAW:
                    case NPC_SHU_LI_SPADEPAW:
                    {
                        player->KilledMonsterCredit(NPC_FENG_SPADEPAW);
                        Creature* feng = creature->GetEntry() == NPC_FENG_SPADEPAW ? creature : creature->FindNearestCreature(NPC_FENG_SPADEPAW, 10.0f, true);
                        if (!feng)
                            break;
                        Creature* shuLi = creature->GetEntry() == NPC_SHU_LI_SPADEPAW ? creature : creature->FindNearestCreature(NPC_SHU_LI_SPADEPAW, 10.0f, true);
                        if (!shuLi)
                            break;

                        shuLi->AI()->Talk(0);
                        shuLi->m_Events.Schedule(delay += 5000, [shuLi]() { shuLi->AI()->Talk(1); });
                        feng->m_Events.Schedule(delay += 5000, [feng, playerGuid]() { if (Player* player = ObjectAccessor::GetPlayer(*feng, playerGuid)) feng->AI()->Talk(0, player); });
                        break;
                    }
                    case NPC_HAIYUN_GREENTILL:
                    case NPC_YING_GRENNTILL:
                    {
                        player->KilledMonsterCredit(NPC_HAIYUN_GREENTILL);
                        Creature* haiyun = creature->GetEntry() == NPC_HAIYUN_GREENTILL ? creature : creature->FindNearestCreature(NPC_HAIYUN_GREENTILL, 10.0f, true);
                        if (!haiyun)
                            break;
                        Creature* ying = creature->GetEntry() == NPC_YING_GRENNTILL ? creature : creature->FindNearestCreature(NPC_YING_GRENNTILL, 10.0f, true);
                        if (!ying)
                            break;
                        Creature* huizhong = creature->FindNearestCreature(NPC_HUIZHONG_GRENNTILL, 20.0f, true);
                        if (!huizhong)
                            break;

                        ying->CastSpell(ying->GetPosition(), SPELL_SUMMON_YING);
                        haiyun->CastSpell(haiyun->GetPosition(), SPELL_SUMMON_YOON);
                        huizhong->CastSpell(huizhong->GetPosition(), SPELL_SUMMON_HUIZHONG);
                        ying->SetVisible(false);
                        haiyun->SetVisible(false);
                        huizhong->SetVisible(false);

                        //Creature* yingSum = ying->FindNearestCreature(NPC_YING_GRENNTILL_SUM, 10.0f, true);
                        Creature* yingSum = ying->SummonCreature(NPC_YING_GRENNTILL_SUM, ying->GetPosition());
                        if (!yingSum)
                            break;
                        Creature* haiyunSum = haiyun->SummonCreature(NPC_HAIYUN_GREENTILL_SUM, haiyun->GetPosition());
                        //Creature* haiyunSum = haiyun->FindNearestCreature(NPC_HAIYUN_GREENTILL_SUM, 10.0f, true);
                        if (!haiyunSum)
                            break;
                        Creature* huizhongSum = huizhong->SummonCreature(NPC_HUIZHONG_GRENNTILL_SUM, huizhong->GetPosition());
                        //Creature* huizhongSum = huizhong->FindNearestCreature(NPC_HUIZHONG_GRENNTILL_SUM, 10.0f, true);
                        if (!huizhongSum)
                            break;

                        haiyunSum->SetFacingToObject(player);
                        haiyunSum->AI()->Talk(0);

                        ObjectGuid haiyunGuid = haiyun->GetGUID();
                        ObjectGuid yingGuid = ying->GetGUID();
                        ObjectGuid huizhongGuid = huizhong->GetGUID();
                        ObjectGuid yingSumGuid = yingSum->GetGUID();
                        ObjectGuid haiyunSumGuid = haiyunSum->GetGUID();
                        ObjectGuid huizhongSumGuid = huizhongSum->GetGUID();

                        haiyunSum->m_Events.Schedule(delay += 3000, [haiyunSum, huizhongSumGuid, yingSumGuid]()
                        {
                            haiyunSum->AI()->Talk(1);
                            if (Creature* yingSum = ObjectAccessor::GetCreature(*haiyunSum, yingSumGuid))
                                haiyunSum->SetFacingToObject(yingSum);
                            if (Creature* huizhongSum = ObjectAccessor::GetCreature(*haiyunSum, huizhongSumGuid))
                                huizhongSum->GetMotionMaster()->MovePoint(0, -374.923f, 2051.632f, 126.114f);
                        });
                        yingSum->m_Events.Schedule(delay += 3000, [yingSum]()
                        {
                            yingSum->AI()->Talk(0);
                        });
                        huizhongSum->m_Events.Schedule(delay += 3000, [huizhongSum, playerGuid]()
                        {
                            if (Player* player = ObjectAccessor::GetPlayer(*huizhongSum, playerGuid))
                                huizhongSum->AI()->Talk(0, player);
                        });
                        haiyunSum->m_Events.Schedule(delay += 3000, [haiyunSum]()
                        {
                            haiyunSum->AI()->Talk(2);
                        });
                        haiyunSum->m_Events.Schedule(delay += 3000, [haiyunSum, playerGuid]()
                        {
                            if (Player* player = ObjectAccessor::GetPlayer(*haiyunSum, playerGuid))
                                haiyunSum->SetFacingToObject(player);
                            haiyunSum->AI()->Talk(3);
                        });
                        haiyunSum->m_Events.Schedule(delay += 3000, [haiyunSum, yingSumGuid]()
                        {
                            haiyunSum->GetMotionMaster()->MovePoint(0, -369.885f, 2013.500f, 125.777f);
                            if (Creature* yingSum = ObjectAccessor::GetCreature(*haiyunSum, yingSumGuid))
                                yingSum->GetMotionMaster()->MovePoint(0, -369.963f, 2007.283f, 125.334f);
                        });
                        huizhongSum->m_Events.Schedule(delay += 1000, [huizhongSum]()
                        {
                            huizhongSum->GetMotionMaster()->MovePoint(0, -366.897f, 1996.729f, 125.218f);
                        });
                        huizhongSum->m_Events.Schedule(delay += 10000, [haiyunSumGuid, yingSumGuid, huizhongSum, yingGuid, haiyunGuid, huizhongGuid]()
                        {
                            if (Creature* ying = ObjectAccessor::GetCreature(*huizhongSum, yingGuid))
                                ying->SetVisible(true);
                            if (Creature* haiyun = ObjectAccessor::GetCreature(*huizhongSum, haiyunGuid))
                                haiyun->SetVisible(true);
                            if (Creature* huizhong = ObjectAccessor::GetCreature(*huizhongSum, huizhongGuid))
                                huizhong->SetVisible(true);
                            if (Creature* haiyunSum = ObjectAccessor::GetCreature(*huizhongSum, haiyunSumGuid))
                                haiyunSum->DespawnOrUnsummon();
                            if (Creature* yingSum = ObjectAccessor::GetCreature(*huizhongSum, yingSumGuid))
                                yingSum->DespawnOrUnsummon();
                            huizhongSum->DespawnOrUnsummon();
                        });
                        break;
                    }
                    case NPC_ZHANG_MARLFUR:
                    case NPC_MIA_MARLFUR:
                    {
                        Creature* zhang = creature->GetEntry() == NPC_ZHANG_MARLFUR ? creature : creature->FindNearestCreature(NPC_ZHANG_MARLFUR, 10.0f, true);
                        if (!zhang)
                            break;

                        player->CastSpell(zhang, SPELL_SUMMON_ZHANG);
                        zhang->SetVisible(false);

                        Creature* zhangSum = zhang->FindNearestCreature(NPC_ZHANG_MARLFUR_SUM, 10.0f, true);
                        if (!zhangSum)
                            break;

                        zhangSum->AI()->Talk(0);
                        zhangSum->m_Events.Schedule(delay += 5000, [zhangSum, playerGuid]()
                        {
                            if (Player* player = ObjectAccessor::GetPlayer(*zhangSum, playerGuid))
                            {
                                zhangSum->SetFacingToObject(player);
                                zhangSum->AI()->Talk(1, player);
                            }
                        });
                        zhangSum->m_Events.Schedule(delay += 3000, [zhangSum]()
                        {
                            zhangSum->GetMotionMaster()->MovePoint(0, -335.536f, 2069.130f, 119.780f);
                            zhangSum->AI()->Talk(2);
                        });
                        zhangSum->m_Events.Schedule(delay += 3000, [zhangSum, playerGuid]()
                        {
                            zhangSum->AI()->Talk(3);
                            if (Player* player = ObjectAccessor::GetPlayer(*zhangSum, playerGuid))
                                zhangSum->SetFacingToObject(player);
                        });
                        zhangSum->m_Events.Schedule(delay += 3000, [zhangSum]()
                        {
                            zhangSum->AI()->Talk(4);
                        });
                        zhangSum->m_Events.Schedule(delay += 1000, [zhangSum, zhang]()
                        {
                            zhangSum->GetMotionMaster()->MovePoint(0, zhang->GetPosition());
                        });
                        zhangSum->m_Events.Schedule(delay += 2000, [zhangSum, zhang]()
                        {
                            zhang->SetVisible(true);
                            zhangSum->DespawnOrUnsummon();
                        });
                        break;
                    }
                }
            }
            else if (action == GOSSIP_ACTION_INFO_DEF + 2)
            {
                player->KilledMonsterCredit(NPC_WEI_BLACKSOIL);

                creature->CastSpell(creature, SPELL_SUMMON_WEI_BLACKSOIL);
                creature->SetVisible(false);

                //Creature* weiSum = creature->FindNearestCreature(NPC_WEI_BLACKSOIL_SUM, 10.0f, true);
                Creature* weiSum = creature->SummonCreature(NPC_WEI_BLACKSOIL_SUM, creature->GetPosition());
                if (!weiSum)
                    return false;

                weiSum->SetFacingToObject(player);
                weiSum->AI()->Talk(0);

                uint32 delay = 0;
                weiSum->m_Events.Schedule(delay += 3000, [weiSum]() { weiSum->GetMotionMaster()->MovePoint(0, -326.190f, 2393.927f, 140.406f); });
                weiSum->m_Events.Schedule(delay += 3000, [weiSum]() { weiSum->GetMotionMaster()->MovePoint(0, -309.222f, 2400.113f, 140.223f); });
                weiSum->m_Events.Schedule(delay += 3000, [weiSum]() { weiSum->DespawnOrUnsummon(); });
                creature->m_Events.Schedule(delay += 10000, [creature]() { creature->SetVisible(true); creature->AI()->SetData(DATA_AMBUSHED, 0); });
            }

            return false;
        }

        struct npc_q29982AI : public ScriptedAI
        {
            npc_q29982AI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                if (me->GetEntry() == NPC_IKTHIK_AMBUSHER)
                {
                    events.ScheduleEvent(EVENT_SWIFT_AMBUSH, 2000);
                    events.ScheduleEvent(EVENT_SLIT_THROAT, 7000);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (me->GetEntry() == NPC_IKTHIK_AMBUSHER)
                {
                    if (Creature* wey = me->FindNearestCreature(NPC_WEI_BLACKSOIL, 30.0f, true))
                        wey->AI()->SetData(DATA_AMBUSHED, 1);
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                _ambushed = data;
                if (type == DATA_AMBUSHED && data == 1)
                {
                    if (me->GetEntry() == NPC_WEI_BLACKSOIL)
                        Talk(2);
                }
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_AMBUSHED)
                    return _ambushed;

                return 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SWIFT_AMBUSH:
                            me->CastSpell(me->GetVictim(), SPELL_SWIFT_AMBUSH, false);
                            break;
                        case EVENT_SLIT_THROAT:
                             me->CastSpell(me->GetVictim(), SPELL_LIT_THROAT, false);
                            events.ScheduleEvent(EVENT_SLIT_THROAT, urand(10000, 12000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            bool _ambushed = false;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_q29982AI(creature);
        }
};

enum q31314
{
    QUEST_OLD_MAN_THISTLES_TREASURE = 31314,

    NPC_FIND_CAVE_ENTRANCE_CREDIT   = 64328,
    NPC_FIND_TREASURE_CREDIT        = 64329,
    NPC_FISH_FELLREED               = 64347,

    OBJ_FIND_CAVE_ENTRANCE          = 268518,
    OBJ_FIND_TREASURE               = 268532,

    AT_CAVE_ENTRANCE                = 8262,
    AT_CAVE_TREASURE                = 8263,
};

class AreaTrigger_cavern_of_endless_echoes : public AreaTriggerScript
{
    public:
        AreaTrigger_cavern_of_endless_echoes() : AreaTriggerScript("AreaTrigger_cavern_of_endless_echoes") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (player->GetQuestStatus(QUEST_OLD_MAN_THISTLES_TREASURE) != QUEST_STATUS_INCOMPLETE)
                return false;

            std::list<TempSummon*> summons;
            player->GetSummons(summons, NPC_FISH_FELLREED);

            Creature* fish = nullptr;
            if (!summons.empty())
                fish = summons.back();

            switch (trigger->ID)
            {
                case AT_CAVE_ENTRANCE:
                    if (!player->GetQuestObjectiveCounter(OBJ_FIND_CAVE_ENTRANCE))
                    {
                        player->KilledMonsterCredit(NPC_FIND_CAVE_ENTRANCE_CREDIT);
                        if (fish)
                            fish->AI()->Talk(0);
                    }
                    break;
                case AT_CAVE_TREASURE:
                    if (!player->GetQuestObjectiveCounter(OBJ_FIND_TREASURE))
                    {
                        player->KilledMonsterCredit(NPC_FIND_TREASURE_CREDIT);
                        if (fish)
                            fish->AI()->Talk(1);
                    }
                    break;
            }

            return true;
        }
};

struct npc_fish_fellreed : public ScriptedAI
{
    npc_fish_fellreed(Creature* creature) : ScriptedAI(creature) { }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (me->GetAreaId() != 6466)
            me->DespawnOrUnsummon();
    }
};

enum
{
    QUEST_A_NEIGHBORS_DURY    = 29915,
    SPELL_SUMMON_WAYWARD_LAMB = 118394,
    NPC_WAYWARD_LAMB          = 61076
};

struct npc_liang_thunderfoot : public ScriptedAI
{
    npc_liang_thunderfoot(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_A_NEIGHBORS_DURY)
        {
            std::list<Creature*> mobs;
            GetCreatureListWithEntryInGrid(mobs, me, NPC_WAYWARD_LAMB, 100.0f);
            for (auto&& mob : mobs)
                if (mob->GetOwnerGUID() == player->GetGUID())
                    mob->DespawnOrUnsummon();

            playerGUID = player->GetGUID();
            me->CastSpell(470.4836f, -268.4369f, 210.8644f, SPELL_SUMMON_WAYWARD_LAMB, true);
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        ObjectGuid guid = playerGUID;
        uint32 delay = 0;
        summoned->m_Events.Schedule(delay += 1000, [this, summoned, guid]()
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, guid))
            {
                summoned->SetOwnerGUID(player->GetGUID());
                summoned->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, M_PI);
            }
        });
    }

private:
    ObjectGuid playerGUID;
};

struct npc_francis_the_shepherd_boy : public ScriptedAI
{
    npc_francis_the_shepherd_boy(Creature* creature) : ScriptedAI(creature) { }

    void OnQuestReward(Player* player, Quest const* quest, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_A_NEIGHBORS_DURY)
        {
            std::list<Creature*> mobs;
            GetCreatureListWithEntryInGrid(mobs, me, NPC_WAYWARD_LAMB, 100.0f);
            for (auto&& mob : mobs)
                if (mob->GetOwnerGUID() == player->GetGUID())
                    mob->DespawnOrUnsummon();
        }
    }
};

// Suspicious Footprint Identified 114457
class spell_suspicious_footprint_identified : public SpellScript
{
    PrepareSpellScript(spell_suspicious_footprint_identified);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Player* target = GetHitPlayer())
                target->KilledMonsterCredit(caster->GetEntry());

            caster->AI()->Talk(0);
            caster->DespawnOrUnsummon();
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_suspicious_footprint_identified::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

const std::map<uint32, uint32> invChickenType=
{
    { 1, 114652 },
    { 2, 114672 },
    { 3, 114677 },
    /*{ 4, 114670 },*/
    { 5, 114678 },
};

// Pick Up Chicken 114668
class spell_four_wind_pick_up_chicken : public SpellScript
{
    PrepareSpellScript(spell_four_wind_pick_up_chicken);

    void HandleBeforeCast()
    {
        // Announce
        if (Creature* chicken = GetCaster()->FindNearestCreature(58918, 20.0f, true))
            chicken->AI()->Talk(0);
    }

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        std::vector<uint32> chickenType = { 1, 2, 3, 5 };
        uint32 chickenPrize = Trinity::Containers::SelectRandomContainerElement(chickenType);

        if (Unit* caster = GetCaster())
        {
            caster->CastSpell(caster, invChickenType.find(chickenPrize)->second, true);

            // Announce
            if (Creature* chicken = GetCaster()->FindNearestCreature(58918, 20.0f, true))
                chicken->AI()->Talk(chickenPrize);
        }
    }

    void Register() override
    {
        BeforeCast += SpellCastFn(spell_four_wind_pick_up_chicken::HandleBeforeCast);
        OnEffectHitTarget += SpellEffectFn(spell_four_wind_pick_up_chicken::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

enum to_be_a_master
{
    QUEST_TO_BE_A_MASTER         = 31521,

    NPC_BLACK_PEPPER_RIBS_CREDIT = 64930,
    NPC_SEA_MIST_RICE_CREDIT     = 64931,
    NPC_MOGU_FISH_STEW_CREDIT    = 64932,
    NPC_STEAMED_CRAB_CREDIT      = 64933,
    NPC_CHUN_TIAN_ROLLS_CREDIT   = 64934,
    NPC_MAD_BREAKFAST_CREDIT     = 64935,

    SPELL_BLACK_PEPPER_RIBS      = 104300,
    SPELL_SEA_MIST_RICE          = 104303,
    SPELL_MOGU_FISH_STEW         = 104306,
    SPELL_STEAMED_CRAB           = 104309,
    SPELL_CHUN_TIAN_ROLLS        = 104312,
    SPELL_MAD_BREAKFAST          = 124054,
};

const std::map<uint32, uint32> invCookingMasterType =
{
    { SPELL_BLACK_PEPPER_RIBS, NPC_BLACK_PEPPER_RIBS_CREDIT },
    { SPELL_SEA_MIST_RICE,        NPC_SEA_MIST_RICE_CREDIT, },
    { SPELL_MOGU_FISH_STEW,      NPC_MOGU_FISH_STEW_CREDIT, },
    { SPELL_STEAMED_CRAB,          NPC_STEAMED_CRAB_CREDIT, },
    { SPELL_CHUN_TIAN_ROLLS,    NPC_CHUN_TIAN_ROLLS_CREDIT, },
    { SPELL_MAD_BREAKFAST,        NPC_MAD_BREAKFAST_CREDIT, },
};

// To be a Master Food Coocking 104300, 104303, 104306, 104309, 104312, 124054
class spell_to_be_a_master_food_coocking : public SpellScript
{
    PrepareSpellScript(spell_to_be_a_master_food_coocking);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Player* caster = GetCaster()->ToPlayer())
            if (caster->GetQuestStatus(QUEST_TO_BE_A_MASTER) == QUEST_STATUS_INCOMPLETE)
                caster->KilledMonsterCredit(invCookingMasterType.find(GetSpellInfo()->Id)->second);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_to_be_a_master_food_coocking::HandleEffectHit, EFFECT_0, SPELL_EFFECT_CREATE_ITEM);
    }
};

struct npc_just_a_folk_story_quest : public ScriptedAI
{
    npc_just_a_folk_story_quest(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipSelect(Player* player, uint32 /*sender*/, uint32 /*action*/) override
    {
        player->CLOSE_GOSSIP_MENU();
        player->KilledMonsterCredit(me->GetEntry());
        Talk(0, player);
        return true;
    }
};

enum GranaryVandal
{
    EVENT_THROW_GRAIN = 1,

    SPELL_SPIRITELY   = 130181,
    SPELL_THROW_GRAIN = 130208,
};

struct npc_granary_vandal : public ScriptedAI
{
    npc_granary_vandal(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    Position pos;

    void Reset() override
    {
        events.Reset();
        events.ScheduleEvent(EVENT_THROW_GRAIN, urand(2, 5) * IN_MILLISECONDS);
        me->SetReactState(REACT_DEFENSIVE);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.Reset();
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->CastSpell(me, SPELL_SPIRITELY, true);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_THROW_GRAIN:
                    pos = me->GetRandomNearPosition(15.0f);
                    me->CastSpell(pos, SPELL_THROW_GRAIN);
                    events.ScheduleEvent(EVENT_THROW_GRAIN, urand(7, 10) * IN_MILLISECONDS);
                    break;
                default:
                    break;
            }
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

std::vector<uint32> grainSpells = { 146357, 147336, 147337, 147338, 147339 };

struct npc_catch_and_carry : public ScriptedAI
{
    npc_catch_and_carry(Creature* creature) : ScriptedAI(creature) { }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who->GetDistance2d(me) > 10.0f)
            return;

        if (Player* player = who->ToPlayer())
        {
            for (auto&& spell : grainSpells)
            {
                if (player->HasAura(spell))
                {
                    player->RemoveAura(spell);
                    player->KilledMonsterCredit(72900);
                }
            }
        }
    }
};

// Throw Grain - 130212
class spell_throw_grain : public SpellScript
{
    PrepareSpellScript(spell_throw_grain);

    void HandleScript()
    {
        if (Player* target = GetHitPlayer())
        {
            if (target->GetQuestStatus(33022) == QUEST_STATUS_INCOMPLETE)
            {
                PreventHitDefaultEffect(EFFECT_0);
                PreventHitDamage();
                SetEffectValue(0);

                if (target->HasAura(146349))
                {
                    for (auto&& spell : grainSpells)
                    {
                        if (!target->HasAura(spell))
                        {
                            target->CastSpell(target, spell, true);
                            return;
                        }
                    }
                }
            }
        }
    }

    void Register() override
    {
        BeforeHit += SpellHitFn(spell_throw_grain::HandleScript);
    }
};

// Catch Grain - 146349
class spell_catch_grain : public AuraScript
{
    PrepareAuraScript(spell_catch_grain);

    void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            for (auto&& spell : grainSpells)
                if (owner->HasAura(spell))
                    owner->RemoveAura(spell);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_catch_grain::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

enum MiscCreatureSpells
{
    SPELL_BELLY_FLOP           = 125384,
    SPELL_BUBBLE_BLAST         = 114965,
    SPELL_BUBBLE_BLAST_EFF     = 114966,
    SPELL_FEARSOME_HOWL        = 129502,
    SPELL_POUNCED              = 129497,
    SPELL_PREY_POUNCE          = 129496,
    SPELL_CHUCK_WATERMELON     = 118657,
    SPELL_WIGGLE_FLIP          = 118663,
    SPELL_FRENZIED_HOP         = 118749,
    SPELL_FRENZIED_HOP_EFF     = 118748,
    SPELL_DASH                 = 36589,
    SPELL_RAZOR_BEAK           = 109088,
    SPELL_DIVE                 = 115132,
    SPELL_HAWK_REND            = 114881,
    SPELL_GRAIN_SACK           = 113671,
    SPELL_GNASH                = 118576,
    SPELL_GNASH_EFF            = 118573,
    SPELL_SUNDER_ARMOR         = 13444,
    SPELL_HEALING_WAVE         = 119575,
    SPELL_WRATH                = 119577,
    SPELL_THRASH               = 131657,
    SPELL_THRASH_EFF           = 131659,
    SPELL_BLUDGEON             = 117586,
    SPELL_GROUND_AND_POUND_EFF = 128965,
    SPELL_GROUND_AND_POUND     = 128963,
    SPELL_GROUND_AND_POUND_AT  = 128966,
};

enum MiscCreatureEvents
{
    EVENT_BELLY_FLOP = 1,
    EVENT_BUBBLE_BLAST,
    EVENT_FEARSOME_HOWL,
    EVENT_PREY_POUNCE,
    EVENT_CHUCK_WATERMELON,
    EVENT_WIGGLE_FLIP,
    EVENT_FRENZIED_HOP,
    EVENT_DASH,
    EVENT_RAZOR_BEAK,
    EVENT_DIVE,
    EVENT_HAWK_REND,
    EVENT_GNASH,
    EVENT_SUNDER_ARMOR,
    EVENT_HEALING_WAVE,
    EVENT_WRATH,
    EVENT_THRASH,
    EVENT_BLUDGEON,
    EVENT_GROUND_AND_POUND,
};

// Adolescent Mushan 56239
struct npc_vfw_adolescent_mushan : public customCreatureAI
{
    npc_vfw_adolescent_mushan(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_BELLY_FLOP, 4 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_BELLY_FLOP, urand(8 * IN_MILLISECONDS, 10 * IN_MILLISECONDS), EVENT_BELLY_FLOP, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Wyrmhorn Turtle 56256
struct npc_vfw_wyrmhorn_turtle : public customCreatureAI
{
    npc_vfw_wyrmhorn_turtle(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_BUBBLE_BLAST, urand(2 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_BUBBLE_BLAST, 13.5 * IN_MILLISECONDS, EVENT_BUBBLE_BLAST, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Lupello 56357
struct npc_vfw_lupello : public customCreatureAI
{
    npc_vfw_lupello(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_FEARSOME_HOWL, 10 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_PREY_POUNCE, 5 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_FEARSOME_HOWL, 14 * IN_MILLISECONDS, EVENT_FEARSOME_HOWL, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_PREY_POUNCE, 10.5 * IN_MILLISECONDS, EVENT_PREY_POUNCE, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Snagtooth Hooligan 56462
struct npc_vfw_snagtooth_hooligan : public customCreatureAI
{
    npc_vfw_snagtooth_hooligan(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_CHUCK_WATERMELON, 1.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_WIGGLE_FLIP, 5 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_CHUCK_WATERMELON, 7 * IN_MILLISECONDS, EVENT_CHUCK_WATERMELON, eventId);
            ExecuteTargetEvent(SPELL_WIGGLE_FLIP, 10 * IN_MILLISECONDS, EVENT_WIGGLE_FLIP, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Frenzyhop 56514
struct npc_vfw_frenzyhop : public customCreatureAI
{
    npc_vfw_frenzyhop(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_FRENZIED_HOP, 6 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_FRENZIED_HOP, 14 * IN_MILLISECONDS, EVENT_FRENZIED_HOP, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Hornbill Strider 56753
struct npc_vfw_hornbill_strider : public customCreatureAI
{
    npc_vfw_hornbill_strider(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCast(me, SPELL_DASH);
        events.ScheduleEvent(EVENT_DASH, 18 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_RAZOR_BEAK, 4.5 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_RAZOR_BEAK, 7 * IN_MILLISECONDS, EVENT_RAZOR_BEAK, eventId);
            ExecuteTargetEvent(SPELL_DASH, 18 * IN_MILLISECONDS, EVENT_DASH, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Grainhunter Hawk 57215, Soaring Hunter 57216
struct npc_vfw_grainhunter_hawk : public customCreatureAI
{
    npc_vfw_grainhunter_hawk(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        if (me->GetEntry() == 57216) // visual sack
            DoCast(me, SPELL_GRAIN_SACK);

        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_DIVE, 2.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_HAWK_REND, 5 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_HAWK_REND, 12.5 * IN_MILLISECONDS, EVENT_HAWK_REND, eventId);
            ExecuteTargetEvent(SPELL_DIVE, 8 * IN_MILLISECONDS, EVENT_DIVE, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Springtail Gnasher 57413
struct npc_vfw_springtail_gnasher : public customCreatureAI
{
    npc_vfw_springtail_gnasher(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_GNASH, 8 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_WIGGLE_FLIP, 5 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_GNASH, 12 * IN_MILLISECONDS, EVENT_GNASH, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_WIGGLE_FLIP, 10 * IN_MILLISECONDS, EVENT_WIGGLE_FLIP, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Mothfighter 57648
struct npc_vfw_mothfighter : public customCreatureAI
{
    npc_vfw_mothfighter(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SUNDER_ARMOR, 4 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_SUNDER_ARMOR, 9 * IN_MILLISECONDS, EVENT_SUNDER_ARMOR, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Wildscale Herbalist 58216
struct npc_vfw_wildscale_herbalist : public customCreatureAI
{
    npc_vfw_wildscale_herbalist(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_WRATH, 1 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_HEALING_WAVE, 12 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_WRATH, 2.5 * IN_MILLISECONDS, EVENT_WRATH, eventId);
            ExecuteTargetEvent(SPELL_HEALING_WAVE, 12 * IN_MILLISECONDS, EVENT_HEALING_WAVE, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Ik`thik Vanguard 58368
struct npc_vfw_ikthik_vanguard : public customCreatureAI
{
    npc_vfw_ikthik_vanguard(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_THRASH, urand(5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_THRASH, 12 * IN_MILLISECONDS, EVENT_THRASH, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Darkhide 58435
struct npc_vfw_darkhide : public customCreatureAI
{
    npc_vfw_darkhide(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        me->RemoveAllAreasTrigger();
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_BLUDGEON, 3.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_GROUND_AND_POUND, 10 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override 
    {
        me->RemoveAllAreasTrigger();
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_BLUDGEON, 8.5 * IN_MILLISECONDS, EVENT_BLUDGEON, eventId);
            ExecuteTargetEvent(SPELL_GROUND_AND_POUND, 16.5 * IN_MILLISECONDS, EVENT_GROUND_AND_POUND, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

enum fireworkBarrelsType
{
    GO_STOLEN_GRAIN_1  = 211086,
    GO_STOLEN_GRAIN_2  = 211077,
    GO_STOLEN_GRAIN_3  = 211081,
    GO_STOLEN_WEAPON_1 = 211057,
    GO_STOLEN_WEAPON_2 = 211051,
    GO_STOLEN_TOOLS_1  = 211031,
    GO_STOLEN_TOOLS_2  = 211026,
    GO_STOLEN_TOOLS_3  = 211037,
    GO_STOLEN_BEER_1   = 211046,
    GO_STOLEN_BEER_2   = 211065,
    GO_STOLEN_BEER_3   = 211068,

    NPC_WEAPONS_CREDIT = 59276,
    NPC_TOOLS_CREDIT   = 59278,
    NPC_BEER_CREDIT    = 59279,
    NPC_GRAIN_CREDIT   = 59280,

    SPELL_EXPLOSION_VISUAL = 71495,
};

const std::map<uint32, uint32> stolneItemsType =
{
    { GO_STOLEN_GRAIN_1, NPC_GRAIN_CREDIT },
    { GO_STOLEN_GRAIN_2, NPC_GRAIN_CREDIT },
    { GO_STOLEN_GRAIN_3, NPC_GRAIN_CREDIT },
    { GO_STOLEN_WEAPON_1, NPC_WEAPONS_CREDIT },
    { GO_STOLEN_WEAPON_2, NPC_WEAPONS_CREDIT },
    { GO_STOLEN_TOOLS_1, NPC_TOOLS_CREDIT },
    { GO_STOLEN_TOOLS_2, NPC_TOOLS_CREDIT },
    { GO_STOLEN_TOOLS_3, NPC_TOOLS_CREDIT },
    { GO_STOLEN_BEER_1, NPC_BEER_CREDIT },
    { GO_STOLEN_BEER_2, NPC_BEER_CREDIT },
    { GO_STOLEN_BEER_3, NPC_BEER_CREDIT },
};

// Barrel of Fireworks 59284
struct npc_vfw_barrel_of_fireworks : public ScriptedAI
{
    npc_vfw_barrel_of_fireworks(Creature* creature) : ScriptedAI(creature) { }
    
    TaskScheduler scheduler;
    ObjectGuid summonerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        summonerGUID = summoner->GetGUID();
        scheduler
            .Schedule(Milliseconds(3000), [this](TaskContext context)
        {
            DoCast(me, SPELL_EXPLOSION_VISUAL);

            if (uint32 creditEntry = getStolenCredit())
            {
                if (Player* target = ObjectAccessor::GetPlayer(*me, summonerGUID))
                    target->KilledMonsterCredit(creditEntry);
            }

            me->DespawnOrUnsummon(500);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }

    uint32 getStolenCredit()
    {
        std::list<GameObject*> stolenItems;

        for (auto&& itr : stolneItemsType)
            GetGameObjectListWithEntryInGrid(stolenItems, me, itr.first, 6.0f);

        if (stolenItems.empty())
            return 0;

        uint32 nearGOEntry = Trinity::Containers::SelectRandomContainerElement(stolenItems)->GetEntry();
        return stolneItemsType.find(nearGOEntry)->second;
    }
};

enum MantidoteType
{
    SPELL_SUMMON_MANIFESTATION_OF_FEAR = 115094,
    SPELL_LASH_OUT                     = 118547,
    SPELL_PANICKED_STRIKE              = 118544,
    SPELL_FAR_REACHING_FEAR            = 129499,
    SPELL_WHAT_LURK_BENEATH            = 129504,

    ACTION_SUMMON_MANIFESTATION        = 0,

    EVENT_LASH_OUT                     = 1,
    EVENT_PANICKED_STRIKE,
    EVENT_FAR_REACHING_FEAR,
    EVENT_WHAT_LURK_BENEATH,

    TALK_SWARM_KEEP                    = 0,

    NPC_MANTIDOTE_CREDIT               = 59875,
};

// Ik`thik Wing Commander 56723
struct npc_vfw_ik_thik_wing_commander : public customCreatureAI
{
    npc_vfw_ik_thik_wing_commander(Creature* creature) : customCreatureAI(creature), summons(me) { }

    SummonList summons;
    ObjectGuid maskApplyGUID;

    void Reset() override
    {
        events.Reset();
        me->SetReactState(REACT_AGGRESSIVE);
        maskApplyGUID = ObjectGuid::Empty;
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        maskApplyGUID = guid;
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        if (Player* target = ObjectAccessor::GetPlayer(*me, maskApplyGUID))
            summon->AI()->AttackStart(target);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_PANICKED_STRIKE, 3.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_LASH_OUT, 6 * IN_MILLISECONDS);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_SUMMON_MANIFESTATION)
        {
            events.Reset();
            me->PrepareChanneledCast(me->GetOrientation());
            DoCast(me, SPELL_SUMMON_MANIFESTATION_OF_FEAR);
        }
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        Talk(TALK_SWARM_KEEP);

        if (Player* target = ObjectAccessor::GetPlayer(*me, maskApplyGUID))
            target->KilledMonsterCredit(NPC_MANTIDOTE_CREDIT);

        // Fly away!
        float x, y;
        GetPositionWithDistInOrientation(me, 25.0f, frand(0.0f, 2 * M_PI), x, y);
        me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ() + 20.0f);
        me->DespawnOrUnsummon(me->GetSplineDuration());
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_PANICKED_STRIKE, 8.5 * IN_MILLISECONDS, EVENT_PANICKED_STRIKE, eventId);
            ExecuteTargetEvent(SPELL_LASH_OUT, 10.5 * IN_MILLISECONDS, EVENT_LASH_OUT, eventId, PRIORITY_CHANNELED);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Manifestation of Fear 59874
struct npc_vfw_manifestation_of_fear : public customCreatureAI
{
    npc_vfw_manifestation_of_fear(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_FAR_REACHING_FEAR, 2.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_WHAT_LURK_BENEATH, 8 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_FAR_REACHING_FEAR, 9.5 * IN_MILLISECONDS, EVENT_FAR_REACHING_FEAR, eventId, PRIORITY_CHANNELED);
            ExecuteTargetEvent(SPELL_WHAT_LURK_BENEATH, 15 * IN_MILLISECONDS, EVENT_WHAT_LURK_BENEATH, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

enum SheepsClothingType
{
    NPC_KRUNGKO_FINGERLICKER = 64389,
    NPC_KUNZEN_HUNTER        = 59121,

    SPELL_HOZEN_STRIKE       = 115041,
    SPELL_HOZEN_RAGE         = 115006,
    SPELL_SUMMON_KRUNGKO     = 125752,

    EVENT_HOZEN_STRIKE = 1,
    EVENT_HOZEN_RAGE,

    TALK_FOUND_SHEEP = 0,
    TALK_SHEEP_TASTE,
};

// Krungko Fingerlicker 64389
struct npc_vfw_krungko_fingerlicker : public customCreatureAI
{
    npc_vfw_krungko_fingerlicker(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid summonerGUID;
    ObjectGuid helperGUID;
    bool hasTriggered;

    void IsSummonedBy(Unit* summoner) override
    {
        summonerGUID = summoner->GetGUID();
        
        if (Creature* hozenHunter = me->SummonCreature(NPC_KUNZEN_HUNTER, *me, TEMPSUMMON_TIMED_DESPAWN, 300 * IN_MILLISECONDS))
            helperGUID = hozenHunter->GetGUID();

        if (Player* target = ObjectAccessor::GetPlayer(*me, summonerGUID))
        {
            me->SetWalk(true);
            me->GetMotionMaster()->MovePoint(0, *target);
        }

        uint32 delay = 2000;
        scheduler
            .Schedule(Milliseconds(delay), [this](TaskContext context)
        {
            Talk(TALK_FOUND_SHEEP);

            if (Creature* hozenHunter = ObjectAccessor::GetCreature(*me, helperGUID))
            {
                hozenHunter->SetWalk(true);

                if (Player* target = ObjectAccessor::GetPlayer(*me, summonerGUID))
                    hozenHunter->GetMotionMaster()->MovePoint(0, *target);
            }
        });

        scheduler
            .Schedule(Milliseconds(delay += 6000), [this](TaskContext context)
        {
            Talk(TALK_SHEEP_TASTE);

            if (Player* target = ObjectAccessor::GetPlayer(*me, summonerGUID))
            {
                AttackStart(target);

                if (Creature* hozenHunter = ObjectAccessor::GetCreature(*me, helperGUID))
                    hozenHunter->AI()->AttackStart(target);
            }
        });
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();

        if (Creature* hozenHunter = ObjectAccessor::GetCreature(*me, helperGUID))
            hozenHunter->DespawnOrUnsummon();

        me->DespawnOrUnsummon();
    }

    void Reset() override
    {
        events.Reset();
        hasTriggered = false;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_HOZEN_STRIKE, 2.5 * IN_MILLISECONDS);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(40) && !hasTriggered)
        {
            hasTriggered = true;
            events.ScheduleEvent(EVENT_HOZEN_RAGE, 0.5 * IN_MILLISECONDS);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_HOZEN_STRIKE, 7 * IN_MILLISECONDS, EVENT_HOZEN_STRIKE, eventId);
            ExecuteTargetEvent(SPELL_HOZEN_RAGE, 0, EVENT_HOZEN_RAGE, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

enum BarrelsManType
{
    SPELL_BREAKING_BARREL     = 108817,
    SPELL_BREAKING_BARREL_EFF = 108816,

    NPC_UNBARRELED_PANDAREN   = 57681,

    QUEST_BARRELS_MAN         = 30077,

    TALK_UNBARRELED           = 0,
};

// Unbarreled Pandaren 57681
struct npc_vfw_unbarreled_pandaren : public customCreatureAI
{
    npc_vfw_unbarreled_pandaren(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid summonerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        me->ToTempSummon()->SetPrivateObjectOwner(summoner->GetGUID());
        summonerGUID = summoner->GetGUID();

        uint32 delay = 2000;
        scheduler
            .Schedule(Milliseconds(delay), [this](TaskContext context)
        {
            if (Player* target = ObjectAccessor::GetPlayer(*me, summonerGUID))
                me->SetFacingToObject(target);

            Talk(TALK_UNBARRELED);
        });

        scheduler
            .Schedule(Milliseconds(delay += 3000), [this](TaskContext context)
        {
            // Move away!
            Position pos = me->GetNearPosition(15.0f, frand(0.0f, 2 * M_PI));
            me->GetMotionMaster()->MovePoint(0, pos);
            me->DespawnOrUnsummon(me->GetSplineDuration());
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Prey Pounce 129496
class spell_vfw_prey_pounce : public SpellScript
{
    PrepareSpellScript(spell_vfw_prey_pounce);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_POUNCED, true);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_vfw_prey_pounce::HandleEffectHit, EFFECT_0, SPELL_EFFECT_JUMP_DEST);
    }
};

// Krungko Timer 125753
class spell_vfw_krungko_timer : public AuraScript
{
    PrepareAuraScript(spell_vfw_krungko_timer);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Player* owner = GetOwner()->ToPlayer())
        {
            //owner->CastSpell(owner, SPELL_SUMMON_KRUNGKO, true);
            Position pos = owner->GetNearPosition(frand(40.0f, 45.0f), frand(0.0f, 2 * M_PI));
            float z = owner->GetPositionZ();
            owner->UpdateAllowedPositionZ(pos.GetPositionX(), pos.GetPositionY(), z);
            owner->SummonCreature(NPC_KRUNGKO_FINGERLICKER, pos.GetPositionX(), pos.GetPositionY(), z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 300 * IN_MILLISECONDS);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_vfw_krungko_timer::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Apply Mask 115092
class spell_vfw_apply_mask : public SpellScript
{
    PrepareSpellScript(spell_vfw_apply_mask);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Creature* target = GetHitCreature())
        {
            target->AI()->SetGUID(GetCaster()->GetGUID());
            target->AI()->DoAction(ACTION_SUMMON_MANIFESTATION);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_vfw_apply_mask::HandleEffectHit, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 426. Summoned by 128966 - Ground and Pound
class sat_vfw_ground_and_pound : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* caster = GetCaster())
            if (Player* target = triggering->ToPlayer())
                caster->CastSpell(target, SPELL_GROUND_AND_POUND_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_GROUND_AND_POUND_EFF);
    }
};

// Breaking Barrel 108817
class spell_vfw_breaking_barrel : public AuraScript
{
    PrepareAuraScript(spell_vfw_breaking_barrel);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (aurEff->GetBase()->GetDuration() > 0) // not fully break this
            return;

        if (Creature* barrel = GetOwner()->ToCreature())
        {
            if (GetCaster() && GetCaster()->ToPlayer() && GetCaster()->ToPlayer()->GetQuestStatus(QUEST_BARRELS_MAN) == QUEST_STATUS_INCOMPLETE)
            {
                GetCaster()->ToPlayer()->KilledMonsterCredit(barrel->GetEntry());
                GetCaster()->ToPlayer()->SummonCreature(NPC_UNBARRELED_PANDAREN, *barrel, TEMPSUMMON_MANUAL_DESPAWN);
                barrel->DespawnOrUnsummon();
            }
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_vfw_breaking_barrel::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

enum SaviorOfStoneplow
{
    QUEST_THE_SAVIOR_OF_STONEPLOW = 30627,
    NPC_IKTHIK_COLOSSUS            = 56703,

    GOSSIP_ACTION_START_FANNY      = GOSSIP_ACTION_INFO_DEF + 1,
};

// Miss Fanny 59857 - The Savior of Stoneplow 30627
class npc_vfw_miss_fanny : public CreatureScript
{
public:
    npc_vfw_miss_fanny() : CreatureScript("npc_vfw_miss_fanny") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (player->GetQuestStatus(QUEST_THE_SAVIOR_OF_STONEPLOW) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let's go, Miss Fanny.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_START_FANNY);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 sender, uint32 action) override
    {
        player->CLOSE_GOSSIP_MENU();

        if (sender != GOSSIP_SENDER_MAIN || action != GOSSIP_ACTION_START_FANNY)
            return true;

        // The retail sequence used a latency-sensitive vehicle ride and an
        // internal vehicle fight. That data is absent here, so award the exact
        // kill objective instead of leaving the player trapped in a dead event.
        if (player->GetQuestStatus(QUEST_THE_SAVIOR_OF_STONEPLOW) == QUEST_STATUS_INCOMPLETE)
            player->KilledMonsterCredit(NPC_IKTHIK_COLOSSUS);

        return true;
    }
};

namespace AshyosVision
{
    constexpr uint32 QuestId = 29577;
    constexpr uint32 ObjectiveId = 259400;
    constexpr uint32 PoolsOfPurityArea = 5973;
    constexpr uint32 GossipMenu = 29577;
    constexpr uint32 GossipActionBeginRitual = GOSSIP_ACTION_INFO_DEF + 1;
    constexpr uint32 SpellAshyoKneeling = 107838;
    constexpr uint32 SpellGoldenDreamVisual = 131525;
}

// Clever Ashyo 56113 - Ashyo's Vision 29577
class npc_vfw_clever_ashyo : public CreatureScript
{
public:
    npc_vfw_clever_ashyo() : CreatureScript("npc_vfw_clever_ashyo") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (creature->HasFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->GetAreaId() == AshyosVision::PoolsOfPurityArea &&
            player->GetQuestStatus(AshyosVision::QuestId) == QUEST_STATUS_INCOMPLETE &&
            !player->GetQuestObjectiveCounter(AshyosVision::ObjectiveId))
        {
            player->ADD_GOSSIP_ITEM_DB(AshyosVision::GossipMenu, 0, GOSSIP_SENDER_MAIN,
                AshyosVision::GossipActionBeginRitual);
            player->SEND_GOSSIP_MENU(AshyosVision::GossipMenu, creature->GetGUID());
            return true;
        }

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        player->CLOSE_GOSSIP_MENU();

        if (sender != GOSSIP_SENDER_MAIN || action != AshyosVision::GossipActionBeginRitual ||
            creature->GetAreaId() != AshyosVision::PoolsOfPurityArea ||
            player->GetQuestStatus(AshyosVision::QuestId) != QUEST_STATUS_INCOMPLETE ||
            player->GetQuestObjectiveCounter(AshyosVision::ObjectiveId))
            return true;

        // Credit immediately so another player restarting the shared visual
        // sequence cannot interrupt this player's quest progression.
        player->KilledMonsterCredit(creature->GetEntry());

        creature->AI()->Talk(0, player);
        creature->CastSpell(creature, AshyosVision::SpellAshyoKneeling, true);
        creature->CastSpell(creature, AshyosVision::SpellGoldenDreamVisual, true);

        ObjectGuid playerGuid = player->GetGUID();
        creature->m_Events.Schedule(6 * IN_MILLISECONDS, [creature, playerGuid]()
        {
            if (Player* eventPlayer = ObjectAccessor::GetPlayer(*creature, playerGuid))
                creature->AI()->Talk(1, eventPlayer);
        });
        creature->m_Events.Schedule(12 * IN_MILLISECONDS, [creature]()
        {
            creature->RemoveAurasDueToSpell(AshyosVision::SpellAshyoKneeling);
        });

        return true;
    }
};

namespace SunsongRanch
{
    constexpr uint32 MapId = 870;
    constexpr uint32 AreaId = 6039;
    constexpr uint32 TillersFaction = 1272;
    constexpr uint8 BasePlotCount = 4;
    constexpr uint8 ExaltedPlotCount = 8;

    constexpr uint32 UntilledSoil = 58562;
    constexpr uint32 TilledSoil = 58563;
    constexpr uint32 StubbornWeed = 60153;
    constexpr uint32 EncroachingWeed = 60185;
    constexpr uint32 SnagtoothHooligan = 56462;
    constexpr uint32 SwoopingPlainshawk = 60072;

    constexpr uint32 AlluringAura = 115588;
    constexpr uint32 InfestedAura = 115483;
    constexpr uint32 WigglingAura = 115489;
    constexpr uint32 SmotheredAura = 115977;
    constexpr uint32 StubbornAura = 115894;
    constexpr uint32 TanglevineBeam = 116236;
    constexpr uint32 SolidStone = 115745;
    constexpr uint32 Pull = 115857;
    constexpr uint32 VintageBugSprayer = 115481;
    constexpr uint32 HarvestReputation = 131047;

    enum PlotState : uint8
    {
        PlotUntilled,
        PlotTilled,
        PlotGrowing,
        PlotRipe,
        PlotEncounter
    };

    enum Encounter : uint8
    {
        EncounterNone,
        EncounterInfested,
        EncounterSnagtooth,
        EncounterPlainshawk,
        EncounterVine,
        EncounterStones
    };

    struct CropData
    {
        uint32 PlantSpell;
        uint32 HarvestItem;
        uint32 GrowingEntry;
        uint32 RipeEntry;
        uint32 InfestedEntry;
        uint32 WigglingEntry;
        uint32 AlluringEntry;
        uint32 SmotheredEntry;
    };

    // The first ten crops are cooking ingredients.  The last six are the
    // profession plots sold by Merchant Greenfield at higher Tillers ranks.
    std::array<CropData, 16> const Crops =
    {{
        { 111102, 74840, 58566, 58567, 60026, 60029, 60070, 60181 },
        { 123361, 74841, 63153, 63154, 63145, 63146, 63147, 63148 },
        { 123388, 74843, 63161, 63165, 63162, 63169, 63159, 63167 },
        { 123485, 74842, 63181, 63185, 63182, 63189, 63178, 63187 },
        { 123535, 74844, 63224, 63229, 63226, 63233, 63222, 63231 },
        { 123565, 74849, 63246, 63250, 63247, 63254, 63243, 63252 },
        { 123568, 74850, 63261, 63265, 63262, 63270, 63259, 63268 },
        { 129974, 74846, 66081, 66085, 66082, 66089, 66079, 66087 },
        { 129976, 74847, 66109, 66113, 66110, 66117, 66107, 66115 },
        { 129978, 74848, 66124, 66129, 66125, 66133, 66122, 66131 },
        { 123773, 72092, 65965, 65973, 65966, 65971, 65964, 65969 },
        { 123774,     0, 65916, 65933, 65918, 65924, 65913, 65921 },
        { 123775, 74249, 65986, 65989, 65987, 65993, 65985, 65991 },
        { 129623, 72988, 66003, 66006, 66004, 66010, 66002, 66008 },
        { 129628, 72120, 66013, 66016, 66014, 66020, 66012, 66018 },
        { 129863, 89112, 66040, 66043, 66041, 66047, 66039, 66045 }
    }};

    std::array<Position, ExaltedPlotCount> const PlotPositions =
    {{
        Position(-171.727f, 642.276f, 165.493f, 6.22907f),
        Position(-166.521f, 641.210f, 165.493f, 6.22907f),
        Position(-172.071f, 646.583f, 165.493f, 6.22907f),
        Position(-166.536f, 645.502f, 165.493f, 6.22907f),
        Position(-171.543f, 637.844f, 165.493f, 6.22907f),
        Position(-166.279f, 636.965f, 165.493f, 0.06996f),
        Position(-172.089f, 650.839f, 165.493f, 6.22907f),
        Position(-166.623f, 649.691f, 165.493f, 6.22907f)
    }};

    struct PlotRecord
    {
        uint8 Crop = 0;
        PlotState State = PlotUntilled;
        uint32 ReadyTime = 0;
        Encounter Event = EncounterNone;
        uint8 Progress = 0;
    };

    struct PullState
    {
        uint8 Plot = 0;
        Encounter Event = EncounterNone;
        uint32 Timer = 0;
    };

    std::unordered_map<uint32, PullState> ActivePulls;

    uint8 GetUnlockedPlotCount(Player const* player)
    {
        return player->GetReputationRank(TillersFaction) >= REP_EXALTED ? ExaltedPlotCount : BasePlotCount;
    }

    bool IsAtFarm(Player const* player)
    {
        return player && player->IsInWorld() && player->GetMapId() == MapId && player->GetAreaId() == AreaId;
    }

    int8 FindPlot(Position const& position)
    {
        for (uint8 i = 0; i < PlotPositions.size(); ++i)
            if (position.GetExactDist2d(PlotPositions[i]) < 2.0f)
                return i;

        return -1;
    }

    CropData const* FindCropBySpell(uint32 spellId, uint8* cropIndex = nullptr)
    {
        for (uint8 i = 0; i < Crops.size(); ++i)
        {
            if (Crops[i].PlantSpell != spellId)
                continue;

            if (cropIndex)
                *cropIndex = i;
            return &Crops[i];
        }

        return nullptr;
    }

    bool IsFarmEntry(uint32 entry)
    {
        if (entry == UntilledSoil || entry == TilledSoil || entry == StubbornWeed ||
            entry == EncroachingWeed || entry == SnagtoothHooligan || entry == SwoopingPlainshawk)
            return true;

        for (CropData const& crop : Crops)
            if (entry == crop.GrowingEntry || entry == crop.RipeEntry || entry == crop.InfestedEntry ||
                entry == crop.WigglingEntry || entry == crop.AlluringEntry || entry == crop.SmotheredEntry)
                return true;

        return false;
    }

    void EnsurePlotRows(Player* player)
    {
        uint32 guid = player->GetGUID().GetCounter();
        CharacterDatabase.DirectPExecute(
            "INSERT IGNORE INTO character_sunsong_farm "
            "(guid, plot, crop, state, ready_time, encounter, progress) VALUES "
            "(%u,0,0,0,0,0,0),(%u,1,0,0,0,0,0),(%u,2,0,0,0,0,0),(%u,3,0,0,0,0,0),"
            "(%u,4,0,0,0,0,0),(%u,5,0,0,0,0,0),(%u,6,0,0,0,0,0),(%u,7,0,0,0,0,0)",
            guid, guid, guid, guid, guid, guid, guid, guid);
    }

    bool LoadPlot(Player const* player, uint8 plot, PlotRecord& record)
    {
        QueryResult result = CharacterDatabase.PQuery(
            "SELECT crop, state, ready_time, encounter, progress FROM character_sunsong_farm "
            "WHERE guid = %u AND plot = %u", player->GetGUID().GetCounter(), uint32(plot));
        if (!result)
            return false;

        Field* fields = result->Fetch();
        record.Crop = fields[0].GetUInt8();
        record.State = PlotState(fields[1].GetUInt8());
        record.ReadyTime = fields[2].GetUInt32();
        record.Event = Encounter(fields[3].GetUInt8());
        record.Progress = fields[4].GetUInt8();

        if (record.Crop >= Crops.size() || record.State > PlotEncounter || record.Event > EncounterStones)
        {
            record = PlotRecord();
            return false;
        }

        return true;
    }

    void SavePlot(Player const* player, uint8 plot, PlotRecord const& record)
    {
        CharacterDatabase.DirectPExecute(
            "REPLACE INTO character_sunsong_farm "
            "(guid, plot, crop, state, ready_time, encounter, progress) "
            "VALUES (%u, %u, %u, %u, %u, %u, %u)",
            player->GetGUID().GetCounter(), uint32(plot), uint32(record.Crop), uint32(record.State),
            record.ReadyTime, uint32(record.Event), uint32(record.Progress));
    }

    uint32 GetPlotEntry(PlotRecord const& record)
    {
        CropData const& crop = Crops[record.Crop];
        switch (record.State)
        {
            case PlotUntilled: return UntilledSoil;
            case PlotTilled:   return TilledSoil;
            case PlotGrowing:  return crop.GrowingEntry;
            case PlotRipe:     return crop.RipeEntry;
            case PlotEncounter:
                switch (record.Event)
                {
                    case EncounterInfested:   return crop.InfestedEntry;
                    case EncounterSnagtooth:  return crop.WigglingEntry;
                    case EncounterPlainshawk: return crop.AlluringEntry;
                    case EncounterVine:       return crop.SmotheredEntry;
                    case EncounterStones:     return StubbornWeed;
                    default:                  return crop.RipeEntry;
                }
        }

        return UntilledSoil;
    }

    void DespawnPlotSummons(Player* player, uint8 plot)
    {
        std::vector<TempSummon*> summons;
        for (TempSummon* summon : player->GetSummons())
            if (summon && summon->GetPrivateObjectOwner() == player->GetGUID() && IsFarmEntry(summon->GetEntry()) &&
                summon->GetHomePosition().GetExactDist2d(PlotPositions[plot]) < 2.0f)
                summons.push_back(summon);

        for (TempSummon* summon : summons)
            summon->DespawnOrUnsummon();
    }

    TempSummon* SummonPlot(Player* player, uint8 plot, PlotRecord const& record)
    {
        uint32 entry = GetPlotEntry(record);
        TempSummon* crop = player->SummonCreature(entry, PlotPositions[plot], TEMPSUMMON_MANUAL_DESPAWN, 0, 0, player->GetGUID());
        if (!crop)
            return nullptr;

        crop->SetPrivateObjectOwner(player->GetGUID());
        crop->SetReactState(REACT_PASSIVE);
        crop->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (record.State == PlotGrowing || record.State == PlotTilled)
            crop->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        else
            crop->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

        if (record.State == PlotEncounter)
        {
            switch (record.Event)
            {
                case EncounterInfested:   crop->CastSpell(crop, InfestedAura, true); break;
                case EncounterSnagtooth:  crop->CastSpell(crop, WigglingAura, true); break;
                case EncounterPlainshawk: crop->CastSpell(crop, AlluringAura, true); break;
                case EncounterVine:       crop->CastSpell(crop, SmotheredAura, true); break;
                case EncounterStones:     crop->CastSpell(crop, StubbornAura, true); break;
                default: break;
            }
        }

        return crop;
    }

    void RefreshPlot(Player* player, uint8 plot, PlotRecord const& record)
    {
        DespawnPlotSummons(player, plot);
        SummonPlot(player, plot, record);
    }

    void ClearPullState(Player* player)
    {
        ActivePulls.erase(player->GetGUID().GetCounter());
        player->RemoveAurasDueToSpell(SolidStone);
        player->RemoveAurasDueToSpell(TanglevineBeam);
    }

    void CompleteEncounter(Player* player, uint8 plot)
    {
        PlotRecord record;
        if (!LoadPlot(player, plot, record) || record.State != PlotEncounter)
            return;

        record.State = PlotRipe;
        record.ReadyTime = 0;
        record.Event = EncounterNone;
        record.Progress = 0;
        SavePlot(player, plot, record);
        ClearPullState(player);
        RefreshPlot(player, plot, record);
    }

    void RefreshFarm(Player* player)
    {
        if (!IsAtFarm(player))
            return;

        EnsurePlotRows(player);
        uint8 plotCount = GetUnlockedPlotCount(player);
        for (uint8 plot = 0; plot < plotCount; ++plot)
        {
            PlotRecord record;
            if (!LoadPlot(player, plot, record))
            {
                record = PlotRecord();
                SavePlot(player, plot, record);
            }

            if (record.State == PlotGrowing && record.ReadyTime && record.ReadyTime <= uint32(time(nullptr)))
            {
                record.State = PlotEncounter;
                record.ReadyTime = 0;
                record.Event = Encounter(urand(EncounterInfested, EncounterStones));
                record.Progress = 0;
                SavePlot(player, plot, record);
            }

            RefreshPlot(player, plot, record);
        }
    }

    bool ValidatePlantTarget(Player* player, Creature* target, uint32 spellId, uint8* plotOut = nullptr)
    {
        if (!IsAtFarm(player) || !target || target->GetEntry() != TilledSoil ||
            target->GetPrivateObjectOwner() != player->GetGUID() || !FindCropBySpell(spellId))
            return false;

        int8 plot = FindPlot(*target);
        if (plot < 0 || uint8(plot) >= GetUnlockedPlotCount(player))
            return false;

        PlotRecord record;
        if (!LoadPlot(player, plot, record) || record.State != PlotTilled)
            return false;

        if (plotOut)
            *plotOut = plot;
        return true;
    }

    void Plant(Player* player, Creature* target, uint32 spellId)
    {
        uint8 plot = 0;
        uint8 cropIndex = 0;
        if (!ValidatePlantTarget(player, target, spellId, &plot) || !FindCropBySpell(spellId, &cropIndex))
            return;

        PlotRecord record;
        record.Crop = cropIndex;
        record.State = PlotGrowing;
        record.ReadyTime = uint32(sWorld->GetNextDailyQuestsResetTime());
        SavePlot(player, plot, record);
        RefreshPlot(player, plot, record);
    }

    bool GiveHarvest(Player* player, uint8 cropIndex)
    {
        if (cropIndex == 11)
        {
            std::array<uint32, 6> const herbs = {{ 72234, 72235, 72237, 72238, 79010, 79011 }};
            return player->AddItem(herbs[urand(0, herbs.size() - 1)], 5);
        }

        uint32 item = Crops[cropIndex].HarvestItem;
        uint32 count = cropIndex >= 10 ? ((cropIndex == 14 || cropIndex == 15) ? 1 : 5) : 5;
        return item && player->AddItem(item, count);
    }

    void Harvest(Player* player, uint8 plot, PlotRecord& record)
    {
        if (!GiveHarvest(player, record.Crop))
            return;

        player->CastSpell(player, HarvestReputation, true);
        record.State = PlotTilled;
        record.ReadyTime = 0;
        record.Event = EncounterNone;
        record.Progress = 0;
        SavePlot(player, plot, record);
        RefreshPlot(player, plot, record);
    }

    bool HasLivingCombatSummon(Player* player, uint8 plot, uint32 entry)
    {
        for (TempSummon* summon : player->GetSummons())
            if (summon && summon->IsAlive() && summon->GetEntry() == entry &&
                summon->GetPrivateObjectOwner() == player->GetGUID() &&
                summon->GetHomePosition().GetExactDist2d(PlotPositions[plot]) < 2.0f)
                return true;

        return false;
    }

    void StartCombatEncounter(Player* player, Creature* crop, uint8 plot, Encounter event)
    {
        uint32 entry = event == EncounterPlainshawk ? SwoopingPlainshawk : SnagtoothHooligan;
        if (HasLivingCombatSummon(player, plot, entry))
            return;

        Position spawn = PlotPositions[plot];
        if (event == EncounterPlainshawk)
            spawn.m_positionZ += 12.0f;

        TempSummon* enemy = player->SummonCreature(entry, spawn, TEMPSUMMON_DEAD_DESPAWN, 0, 0, player->GetGUID());
        if (!enemy)
            return;

        enemy->SetPrivateObjectOwner(player->GetGUID());
        enemy->SelectLevel(enemy->GetCreatureTemplate(), player->GetLevel());
        CreatureBaseStats const* stats = sObjectMgr->GetCreatureBaseStats(
            enemy->GetLevel(), enemy->GetCreatureTemplate()->unit_class);
        enemy->SetModifierValue(UNIT_MOD_ARMOR, BASE_VALUE, stats->GenerateArmor(enemy->GetCreatureTemplate()));
        enemy->UpdateAllStats();
        enemy->SetFaction(14);
        enemy->SetHomePosition(spawn);
        if (event == EncounterPlainshawk)
        {
            enemy->SetCanFly(true);
            enemy->SetDisableGravity(true);
        }

        crop->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        enemy->SetReactState(REACT_AGGRESSIVE);
        enemy->AI()->AttackStart(player);
    }

    void StartPullEncounter(Player* player, Creature* crop, uint8 plot, Encounter event)
    {
        auto active = ActivePulls.find(player->GetGUID().GetCounter());
        if (active != ActivePulls.end() && active->second.Plot != plot)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Finish clearing the other farm plot first.");
            return;
        }

        bool newlyStarted = active == ActivePulls.end();
        ActivePulls[player->GetGUID().GetCounter()] = { plot, event, 0 };

        if (event == EncounterStones)
        {
            if (newlyStarted)
            {
                player->CastSpell(player, SolidStone, true);
                ChatHandler(player->GetSession()).PSendSysMessage("Use Pull! five times to clear the stubborn weed.");
            }
            return;
        }

        crop->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        if (TempSummon* weed = player->SummonCreature(EncroachingWeed, PlotPositions[plot],
            TEMPSUMMON_MANUAL_DESPAWN, 0, 0, player->GetGUID()))
        {
            weed->SetPrivateObjectOwner(player->GetGUID());
            weed->SetReactState(REACT_PASSIVE);
            weed->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            weed->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            weed->CastSpell(player, TanglevineBeam, true);
        }

        ChatHandler(player->GetSession()).PSendSysMessage("Move 20 yards away from the crop to tear out the vine.");
    }

    void PullStubbornWeed(Player* player);

    void HandleCropClick(Player* player, Creature* crop)
    {
        if (!IsAtFarm(player) || crop->GetPrivateObjectOwner() != player->GetGUID())
            return;

        int8 plot = FindPlot(*crop);
        if (plot < 0 || uint8(plot) >= GetUnlockedPlotCount(player))
            return;

        PlotRecord record;
        if (!LoadPlot(player, plot, record))
            return;

        switch (record.State)
        {
            case PlotUntilled:
                record.State = PlotTilled;
                SavePlot(player, plot, record);
                RefreshPlot(player, plot, record);
                break;
            case PlotRipe:
                Harvest(player, plot, record);
                break;
            case PlotEncounter:
                switch (record.Event)
                {
                    case EncounterInfested:
                        ChatHandler(player->GetSession()).PSendSysMessage("Use your Vintage Bug Sprayer on this crop.");
                        break;
                    case EncounterSnagtooth:
                    case EncounterPlainshawk:
                        StartCombatEncounter(player, crop, plot, record.Event);
                        break;
                    case EncounterVine:
                        StartPullEncounter(player, crop, plot, record.Event);
                        break;
                    case EncounterStones:
                        StartPullEncounter(player, crop, plot, record.Event);
                        PullStubbornWeed(player);
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    void SprayInfestedCrop(Player* player)
    {
        if (!IsAtFarm(player))
            return;

        uint8 nearestPlot = 0;
        float nearestDistance = 20.0f;
        bool found = false;
        for (uint8 plot = 0; plot < GetUnlockedPlotCount(player); ++plot)
        {
            PlotRecord record;
            float distance = player->GetExactDist2d(PlotPositions[plot]);
            if (distance >= nearestDistance || !LoadPlot(player, plot, record) ||
                record.State != PlotEncounter || record.Event != EncounterInfested)
                continue;

            nearestDistance = distance;
            nearestPlot = plot;
            found = true;
        }

        if (found)
            CompleteEncounter(player, nearestPlot);
    }

    void PullStubbornWeed(Player* player)
    {
        auto active = ActivePulls.find(player->GetGUID().GetCounter());
        if (active == ActivePulls.end() || active->second.Event != EncounterStones)
            return;

        uint8 plot = active->second.Plot;
        PlotRecord record;
        if (!LoadPlot(player, plot, record) || record.State != PlotEncounter || record.Event != EncounterStones)
        {
            ClearPullState(player);
            return;
        }

        if (++record.Progress >= 5)
        {
            CompleteEncounter(player, plot);
            return;
        }

        SavePlot(player, plot, record);
        ChatHandler(player->GetSession()).PSendSysMessage("Pull! (%u/5)", uint32(record.Progress));
    }
}

struct npc_sunsong_farm_crop : public ScriptedAI
{
    npc_sunsong_farm_crop(Creature* creature) : ScriptedAI(creature) { }

    void OnSpellClick(Unit* clicker, bool& result) override
    {
        if (Player* player = clicker->ToPlayer())
        {
            result = true;
            SunsongRanch::HandleCropClick(player, me);
        }
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

class spell_sunsong_plant_seed : public SpellScript
{
    PrepareSpellScript(spell_sunsong_plant_seed);

    SpellCastResult CheckCast()
    {
        Player* player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
        Creature* target = GetExplTargetUnit() ? GetExplTargetUnit()->ToCreature() : nullptr;
        if (!player || !SunsongRanch::IsAtFarm(player))
            return SPELL_FAILED_NOT_HERE;

        if (!SunsongRanch::ValidatePlantTarget(player, target, GetSpellInfo()->Id))
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }

    void HandlePlant(SpellEffIndex /*effIndex*/)
    {
        Player* player = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
        Creature* target = GetHitCreature();
        if (player && target)
            SunsongRanch::Plant(player, target, GetSpellInfo()->Id);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_sunsong_plant_seed::CheckCast);
        OnEffectHitTarget += SpellEffectFn(spell_sunsong_plant_seed::HandlePlant, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

class player_sunsong_ranch : public PlayerScript
{
public:
    player_sunsong_ranch() : PlayerScript("player_sunsong_ranch") { }

    void OnLogin(Player* player) override
    {
        ObjectGuid playerGuid = player->GetGUID();
        player->m_Events.Schedule(1500, [playerGuid]()
        {
            if (Player* onlinePlayer = ObjectAccessor::FindPlayer(playerGuid))
                SunsongRanch::RefreshFarm(onlinePlayer);
        });
    }

    void OnLogout(Player* player) override
    {
        SunsongRanch::ClearPullState(player);
    }

    void OnUpdateZone(Player* player, uint32 /*newZone*/, uint32 newArea) override
    {
        if (newArea == SunsongRanch::AreaId)
            SunsongRanch::RefreshFarm(player);
        else
            SunsongRanch::ClearPullState(player);
    }

    void OnMapChanged(Player* player) override
    {
        SunsongRanch::RefreshFarm(player);
    }

    void OnCreatureKill(Player* player, Creature* killed) override
    {
        uint32 entry = killed->GetEntry();
        if ((entry != SunsongRanch::SnagtoothHooligan && entry != SunsongRanch::SwoopingPlainshawk) ||
            killed->GetPrivateObjectOwner() != player->GetGUID())
            return;

        int8 plot = SunsongRanch::FindPlot(killed->GetHomePosition());
        if (plot < 0)
            return;

        SunsongRanch::PlotRecord record;
        if (!SunsongRanch::LoadPlot(player, plot, record) || record.State != SunsongRanch::PlotEncounter)
            return;

        if ((entry == SunsongRanch::SnagtoothHooligan && record.Event == SunsongRanch::EncounterSnagtooth) ||
            (entry == SunsongRanch::SwoopingPlainshawk && record.Event == SunsongRanch::EncounterPlainshawk))
            SunsongRanch::CompleteEncounter(player, plot);
    }

    void OnSpellCast(Player* player, Spell* spell, bool /*skipCheck*/) override
    {
        if (!spell || !SunsongRanch::IsAtFarm(player))
            return;

        switch (spell->GetSpellInfo()->Id)
        {
            case SunsongRanch::VintageBugSprayer:
                SunsongRanch::SprayInfestedCrop(player);
                break;
            case SunsongRanch::Pull:
                SunsongRanch::PullStubbornWeed(player);
                break;
            default:
                break;
        }
    }

    void OnUpdate(Player* player, uint32 diff) override
    {
        auto active = SunsongRanch::ActivePulls.find(player->GetGUID().GetCounter());
        if (active == SunsongRanch::ActivePulls.end())
            return;

        if (!SunsongRanch::IsAtFarm(player))
        {
            SunsongRanch::ClearPullState(player);
            return;
        }

        if (active->second.Timer > diff)
        {
            active->second.Timer -= diff;
            return;
        }

        active->second.Timer = 250;
        if (active->second.Event == SunsongRanch::EncounterVine &&
            player->GetExactDist2d(SunsongRanch::PlotPositions[active->second.Plot]) >= 20.0f)
        {
            uint8 plot = active->second.Plot;
            SunsongRanch::CompleteEncounter(player, plot);
        }
    }
};

namespace LessonInBravery
{
    constexpr uint32 Quest = 29918;
    constexpr uint32 GreatWhitePlainshawk = 56171;
    constexpr uint32 ValleyOfTheFourWinds = 5805;

    void EnsurePlainshawk(Player* player)
    {
        if (!player || !player->IsInWorld() || player->GetMapId() != 870 ||
            player->GetZoneId() != ValleyOfTheFourWinds ||
            player->GetQuestStatus(Quest) != QUEST_STATUS_INCOMPLETE)
            return;

        std::list<Creature*> plainshawks;
        GetCreatureListWithEntryInGrid(plainshawks, player, GreatWhitePlainshawk, 120.0f);
        for (Creature* plainshawk : plainshawks)
            if (plainshawk->IsAlive())
                return;

        Position spawn = player->GetNearPosition(35.0f, 0.0f);
        spawn.m_positionZ += 20.0f;

        if (TempSummon* plainshawk = player->SummonCreature(GreatWhitePlainshawk, spawn,
            TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 15 * MINUTE * IN_MILLISECONDS, 0, player->GetGUID()))
        {
            plainshawk->SetCanFly(true);
            plainshawk->SetDisableGravity(true);
            plainshawk->SetReactState(REACT_AGGRESSIVE);
        }
    }
}

class player_lesson_in_bravery : public PlayerScript
{
public:
    player_lesson_in_bravery() : PlayerScript("player_lesson_in_bravery") { }

    void OnQuestAdded(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == LessonInBravery::Quest)
            LessonInBravery::EnsurePlainshawk(player);
    }

    void OnLogin(Player* player) override
    {
        ObjectGuid playerGuid = player->GetGUID();
        player->m_Events.Schedule(1000, [playerGuid]()
        {
            if (Player* onlinePlayer = ObjectAccessor::FindPlayer(playerGuid))
                LessonInBravery::EnsurePlainshawk(onlinePlayer);
        });
    }

    void OnUpdateZone(Player* player, uint32 newZone, uint32 /*newArea*/) override
    {
        if (newZone == LessonInBravery::ValleyOfTheFourWinds)
            LessonInBravery::EnsurePlainshawk(player);
    }
};

namespace HopHunting
{
    constexpr uint32 ChensResolution = 30046;
    constexpr uint32 QuestId = 30053;
    constexpr uint32 StormstoutsHops = 30055;
    constexpr uint32 GardenerFran = 62377;
    constexpr uint32 GardenerFranObjective = 251646;
    constexpr uint32 GardenerFranGossipMenu = 13850;
    constexpr uint32 GardenerFranGossipAction = GOSSIP_ACTION_INFO_DEF + 1;

    void EnsureQuest(Player* player)
    {
        if (!player || !player->IsInWorld() || player->GetMapId() != 870 ||
            player->GetZoneId() != LessonInBravery::ValleyOfTheFourWinds ||
            !player->GetQuestRewardStatus(ChensResolution) ||
            player->GetQuestStatus(QuestId) != QUEST_STATUS_NONE ||
            player->GetQuestRewardStatus(QuestId) ||
            player->GetQuestStatus(StormstoutsHops) != QUEST_STATUS_NONE ||
            player->GetQuestRewardStatus(StormstoutsHops))
            return;

        if (Quest const* quest = sObjectMgr->GetQuestTemplate(QuestId))
            if (player->CanTakeQuest(quest, false) && player->CanAddQuest(quest, true))
                player->AddQuestAndCheckCompletion(quest, nullptr);
    }
}

class npc_gardener_fran_hop_hunting : public CreatureScript
{
public:
    npc_gardener_fran_hop_hunting() : CreatureScript("npc_gardener_fran_hop_hunting") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        player->PlayerTalkClass->ClearMenus();
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(HopHunting::QuestId) == QUEST_STATUS_INCOMPLETE &&
            !player->GetQuestObjectiveCounter(HopHunting::GardenerFranObjective))
        {
            player->ADD_GOSSIP_ITEM_DB(HopHunting::GardenerFranGossipMenu, 0,
                GOSSIP_SENDER_MAIN, HopHunting::GardenerFranGossipAction);
        }

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        player->CLOSE_GOSSIP_MENU();
        if (sender != GOSSIP_SENDER_MAIN || action != HopHunting::GardenerFranGossipAction ||
            !player->IsWithinDistInMap(creature, INTERACTION_DISTANCE) ||
            player->GetQuestStatus(HopHunting::QuestId) != QUEST_STATUS_INCOMPLETE ||
            player->GetQuestObjectiveCounter(HopHunting::GardenerFranObjective))
            return true;

        creature->AI()->Talk(0, player);
        player->KilledMonsterCredit(HopHunting::GardenerFran);
        return true;
    }
};

class player_hop_hunting_recovery : public PlayerScript
{
public:
    player_hop_hunting_recovery() : PlayerScript("player_hop_hunting_recovery") { }

    void OnQuestRewarded(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == HopHunting::ChensResolution)
            HopHunting::EnsureQuest(player);
    }

    void OnLogin(Player* player) override
    {
        ObjectGuid playerGuid = player->GetGUID();
        player->m_Events.Schedule(1500, [playerGuid]()
        {
            if (Player* onlinePlayer = ObjectAccessor::FindPlayer(playerGuid))
                HopHunting::EnsureQuest(onlinePlayer);
        });
    }

    void OnUpdateZone(Player* player, uint32 newZone, uint32 /*newArea*/) override
    {
        if (newZone == LessonInBravery::ValleyOfTheFourWinds)
            HopHunting::EnsureQuest(player);
    }
};

namespace BarrelingAlong
{
    constexpr uint32 QuestId = 30172;
    constexpr uint32 MudmugEscort = 58341;
    constexpr uint32 MapId = 870;
    constexpr uint32 GossipActionResume = GOSSIP_ACTION_INFO_DEF + 1;
    constexpr float ArrivalX = -233.034f;
    constexpr float ArrivalY = 509.152f;
    constexpr float ArrivalRadius = 35.0f;

    void EnsureEscort(Player* player)
    {
        if (!player || !player->IsInWorld() || player->GetMapId() != MapId ||
            player->GetQuestStatus(QuestId) != QUEST_STATUS_INCOMPLETE)
            return;

        std::list<TempSummon*> mudmugs;
        player->GetSummons(mudmugs, MudmugEscort);
        if (!mudmugs.empty())
            return;

        player->SummonCreature(MudmugEscort, player->GetPosition(),
            TEMPSUMMON_MANUAL_DESPAWN, 0, 0, player->GetGUID());
    }
}

class npc_mudmug_barreling_along_questgiver : public CreatureScript
{
public:
    npc_mudmug_barreling_along_questgiver() : CreatureScript("npc_mudmug_barreling_along_questgiver") { }

    bool OnQuestAccept(Player* player, Creature* /*creature*/, Quest const* quest) override
    {
        if (quest->GetQuestId() == BarrelingAlong::QuestId)
            BarrelingAlong::EnsureEscort(player);
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        player->PlayerTalkClass->ClearMenus();
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(BarrelingAlong::QuestId) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let's head back to Halfhill, Mudmug.",
                GOSSIP_SENDER_MAIN, BarrelingAlong::GossipActionResume);

        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        player->CLOSE_GOSSIP_MENU();
        if (sender == GOSSIP_SENDER_MAIN && action == BarrelingAlong::GossipActionResume &&
            player->IsWithinDistInMap(creature, INTERACTION_DISTANCE))
            BarrelingAlong::EnsureEscort(player);
        return true;
    }
};

class npc_mudmug_barreling_along_escort : public CreatureScript
{
public:
    npc_mudmug_barreling_along_escort() : CreatureScript("npc_mudmug_barreling_along_escort") { }

    struct npc_mudmug_barreling_along_escortAI : public ScriptedAI
    {
        npc_mudmug_barreling_along_escortAI(Creature* creature) : ScriptedAI(creature) { }

        void IsSummonedBy(Unit* summoner) override
        {
            Player* player = summoner ? summoner->ToPlayer() : nullptr;
            if (!player || player->GetQuestStatus(BarrelingAlong::QuestId) != QUEST_STATUS_INCOMPLETE)
            {
                me->DespawnOrUnsummon();
                return;
            }

            playerGuid = player->GetGUID();
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            me->GetMotionMaster()->MoveFollow(player, 3.0f, M_PI);
        }

        void UpdateAI(uint32 diff) override
        {
            if (completed)
                return;

            if (checkTimer > diff)
            {
                checkTimer -= diff;
                return;
            }
            checkTimer = 1000;

            Player* player = ObjectAccessor::GetPlayer(*me, playerGuid);
            if (!player || player->GetQuestStatus(BarrelingAlong::QuestId) != QUEST_STATUS_INCOMPLETE)
            {
                me->DespawnOrUnsummon();
                return;
            }

            if (player->GetMapId() != BarrelingAlong::MapId)
            {
                me->DespawnOrUnsummon();
                return;
            }

            if (player->GetExactDist2d(BarrelingAlong::ArrivalX, BarrelingAlong::ArrivalY) <=
                BarrelingAlong::ArrivalRadius)
            {
                completed = true;
                player->KilledMonsterCredit(BarrelingAlong::MudmugEscort, me->GetGUID());
                me->DespawnOrUnsummon(5 * IN_MILLISECONDS);
                return;
            }

            if (!me->IsWithinDistInMap(player, 60.0f))
            {
                me->NearTeleportTo(player->GetPositionX(), player->GetPositionY(),
                    player->GetPositionZ(), player->GetOrientation());
                me->GetMotionMaster()->MoveFollow(player, 3.0f, M_PI);
            }
        }

    private:
        ObjectGuid playerGuid;
        uint32 checkTimer = 1000;
        bool completed = false;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_mudmug_barreling_along_escortAI(creature);
    }
};

class player_barreling_along_recovery : public PlayerScript
{
public:
    player_barreling_along_recovery() : PlayerScript("player_barreling_along_recovery") { }

    void OnQuestAdded(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == BarrelingAlong::QuestId)
            ScheduleRecovery(player);
    }

    void OnLogin(Player* player) override
    {
        ScheduleRecovery(player);
    }

    void OnMapChanged(Player* player) override
    {
        ScheduleRecovery(player);
    }

private:
    static void ScheduleRecovery(Player* player)
    {
        ObjectGuid playerGuid = player->GetGUID();
        player->m_Events.Schedule(1000, [playerGuid]()
        {
            if (Player* onlinePlayer = ObjectAccessor::FindPlayer(playerGuid))
                BarrelingAlong::EnsureEscort(onlinePlayer);
        });
    }
};

namespace StoneplowThirsts
{
    constexpr uint32 QuestId = 30117;
    constexpr uint32 LoonMai = 56720;
    constexpr uint32 MapId = 870;
    constexpr float LoonMaiX = -392.342f;
    constexpr float LoonMaiY = 1911.94f;
    constexpr float LoonMaiZ = 134.199f;
    constexpr float LoonMaiO = 3.05433f;

    void EnsureLoonMai(Player* player)
    {
        if (!player || !player->IsInWorld() || player->GetMapId() != MapId ||
            player->GetQuestStatus(QuestId) != QUEST_STATUS_INCOMPLETE ||
            player->GetExactDist2d(LoonMaiX, LoonMaiY) > 120.0f ||
            player->FindNearestCreature(LoonMai, 120.0f, true))
            return;

        Position position = { LoonMaiX, LoonMaiY, LoonMaiZ, LoonMaiO };
        if (TempSummon* loonMai = player->SummonCreature(LoonMai, position,
            TEMPSUMMON_TIMED_DESPAWN, 30 * MINUTE * IN_MILLISECONDS, 0, player->GetGUID()))
        {
            loonMai->SetPhaseMask(player->GetPhaseMask(), true);
            loonMai->SetPrivateObjectOwner(player->GetGUID());
        }
    }
}

class player_stoneplow_thirsts_recovery : public PlayerScript
{
public:
    player_stoneplow_thirsts_recovery() : PlayerScript("player_stoneplow_thirsts_recovery") { }

    void OnQuestAdded(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == StoneplowThirsts::QuestId)
            ScheduleRecovery(player);
    }

    void OnLogin(Player* player) override
    {
        ScheduleRecovery(player);
    }

    void OnMapChanged(Player* player) override
    {
        ScheduleRecovery(player);
    }

    void OnUpdateZone(Player* player, uint32 /*newZone*/, uint32 /*newArea*/) override
    {
        StoneplowThirsts::EnsureLoonMai(player);
    }

private:
    static void ScheduleRecovery(Player* player)
    {
        ObjectGuid playerGuid = player->GetGUID();
        player->m_Events.Schedule(1000, [playerGuid]()
        {
            if (Player* onlinePlayer = ObjectAccessor::FindPlayer(playerGuid))
                StoneplowThirsts::EnsureLoonMai(onlinePlayer);
        });
    }
};

namespace ChenAndLiLi
{
    constexpr uint32 QuestId = 29907;
    constexpr uint32 Chen = 56343;
    constexpr uint32 LiLi = 56344;
    constexpr uint32 SummonSpell = 105835;
    constexpr uint32 PangsSteadArea = 5936;
    constexpr uint32 StartEscortEvent = 1;
    constexpr uint32 EscortArrivalPoint = 1;
    constexpr float StartX = 517.318f;
    constexpr float StartY = -693.748f;
    constexpr float PangX = 545.832f;
    constexpr float PangY = -606.115f;
    constexpr float PangZ = 263.454f;

    void CreditArrival(Player* player)
    {
        if (!player || !player->IsInWorld() || player->GetMapId() != 870 ||
            player->GetQuestStatus(QuestId) != QUEST_STATUS_INCOMPLETE ||
            player->GetExactDist2d(PangX, PangY) > 80.0f)
            return;

        player->KilledMonsterCredit(Chen);
    }

    void EnsureEscort(Player* player)
    {
        if (!player || !player->IsInWorld() || player->GetMapId() != 870 ||
            player->GetQuestStatus(QuestId) != QUEST_STATUS_INCOMPLETE ||
            player->GetExactDist2d(StartX, StartY) > 120.0f)
            return;

        std::list<TempSummon*> chens;
        player->GetSummons(chens, Chen);
        if (chens.empty())
            player->CastSpell(player, SummonSpell, true);
    }
}

class npc_chen_and_li_li_escort : public CreatureScript
{
public:
    npc_chen_and_li_li_escort() : CreatureScript("npc_chen_and_li_li_escort") { }

    struct npc_chen_and_li_li_escortAI : public ScriptedAI
    {
        npc_chen_and_li_li_escortAI(Creature* creature) : ScriptedAI(creature) { }

        void IsSummonedBy(Unit* summoner) override
        {
            Player* player = summoner ? summoner->ToPlayer() : nullptr;
            if (!player || player->GetQuestStatus(ChenAndLiLi::QuestId) != QUEST_STATUS_INCOMPLETE)
            {
                me->DespawnOrUnsummon();
                return;
            }

            playerGuid = player->GetGUID();
            me->SetReactState(REACT_PASSIVE);
            me->SetWalk(true);
            events.ScheduleEvent(ChenAndLiLi::StartEscortEvent, 500);
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE || id != ChenAndLiLi::EscortArrivalPoint)
                return;

            if (Player* player = ObjectAccessor::GetPlayer(*me, playerGuid))
                ChenAndLiLi::CreditArrival(player);

            if (Creature* liLi = ObjectAccessor::GetCreature(*me, liLiGuid))
                liLi->DespawnOrUnsummon(10 * IN_MILLISECONDS);
            me->DespawnOrUnsummon(10 * IN_MILLISECONDS);
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);
            if (events.ExecuteEvent() != ChenAndLiLi::StartEscortEvent)
                return;

            Position liLiPosition = me->GetNearPosition(2.0f, M_PI / 2.0f);
            if (TempSummon* liLi = me->SummonCreature(ChenAndLiLi::LiLi, liLiPosition,
                TEMPSUMMON_TIMED_DESPAWN, 2 * MINUTE * IN_MILLISECONDS, 0, playerGuid))
            {
                liLiGuid = liLi->GetGUID();
                liLi->SetReactState(REACT_PASSIVE);
                liLi->SetWalk(true);
                liLi->GetMotionMaster()->MoveFollow(me, 2.0f, M_PI);
            }

            me->GetMotionMaster()->MovePoint(ChenAndLiLi::EscortArrivalPoint,
                ChenAndLiLi::PangX, ChenAndLiLi::PangY, ChenAndLiLi::PangZ, true);
        }

    private:
        EventMap events;
        ObjectGuid playerGuid;
        ObjectGuid liLiGuid;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_chen_and_li_li_escortAI(creature);
    }
};

class player_chen_and_li_li_recovery : public PlayerScript
{
public:
    player_chen_and_li_li_recovery() : PlayerScript("player_chen_and_li_li_recovery") { }

    void OnQuestAdded(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() == ChenAndLiLi::QuestId)
        {
            ObjectGuid playerGuid = player->GetGUID();
            player->m_Events.Schedule(1000, [playerGuid]()
            {
                if (Player* onlinePlayer = ObjectAccessor::FindPlayer(playerGuid))
                    ChenAndLiLi::EnsureEscort(onlinePlayer);
            });
        }
    }

    void OnLogin(Player* player) override
    {
        ObjectGuid playerGuid = player->GetGUID();
        player->m_Events.Schedule(1000, [playerGuid]()
        {
            if (Player* onlinePlayer = ObjectAccessor::FindPlayer(playerGuid))
            {
                ChenAndLiLi::CreditArrival(onlinePlayer);
                ChenAndLiLi::EnsureEscort(onlinePlayer);
            }
        });
    }

    void OnUpdate(Player* player, uint32 /*diff*/) override
    {
        if (player->GetAreaId() == ChenAndLiLi::PangsSteadArea)
            ChenAndLiLi::CreditArrival(player);
    }
};

namespace LiLisDayOff
{
    constexpr uint32 QuestId = 29950;
    constexpr uint32 Companion = 56549;
    constexpr uint32 MapId = 870;

    struct ExplorationLocation
    {
        uint32 ObjectiveId;
        uint32 CreditEntry;
        uint32 AreaId;
        float X;
        float Y;
        float Radius;
    };

    std::array<ExplorationLocation, 3> const Locations =
    {{
        { 258091, 56546, 5986, -485.0f,  232.0f, 55.0f }, // Silken Fields
        { 258092, 56547, 5973,  198.0f,  279.0f, 55.0f }, // New Cifera
        { 258093, 56548, 6021,  -18.0f, -301.0f, 55.0f }  // Huangtze Falls
    }};

    void CreditExploration(Player* player, Creature* liLi)
    {
        if (!player || !liLi || !player->IsInWorld() || player->GetMapId() != MapId ||
            player->GetQuestStatus(QuestId) != QUEST_STATUS_INCOMPLETE ||
            !liLi->IsAlive() || !liLi->IsWithinDistInMap(player, 40.0f))
            return;

        for (ExplorationLocation const& location : Locations)
        {
            if (player->GetQuestObjectiveCounter(location.ObjectiveId) ||
                (player->GetAreaId() != location.AreaId &&
                 player->GetExactDist2d(location.X, location.Y) > location.Radius))
                continue;

            player->KilledMonsterCredit(location.CreditEntry);
        }
    }
}

class npc_li_li_day_off_companion : public CreatureScript
{
public:
    npc_li_li_day_off_companion() : CreatureScript("npc_li_li_day_off_companion") { }

    struct npc_li_li_day_off_companionAI : public ScriptedAI
    {
        npc_li_li_day_off_companionAI(Creature* creature) : ScriptedAI(creature) { }

        void IsSummonedBy(Unit* summoner) override
        {
            Player* player = summoner ? summoner->ToPlayer() : nullptr;
            if (!player || player->GetQuestStatus(LiLisDayOff::QuestId) != QUEST_STATUS_INCOMPLETE)
            {
                me->DespawnOrUnsummon();
                return;
            }

            playerGuid = player->GetGUID();

            // Reusing the Wishing-Stone replaces a lost/stuck companion rather
            // than leaving several private copies following the player.
            std::list<TempSummon*> companions;
            player->GetSummons(companions, LiLisDayOff::Companion);
            for (TempSummon* companion : companions)
                if (companion != me)
                    companion->DespawnOrUnsummon();

            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS,
                UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            me->GetMotionMaster()->MoveFollow(player, 2.5f, M_PI);

            // Also handles using the stone while already standing inside an
            // exploration area, which otherwise misses the area transition.
            LiLisDayOff::CreditExploration(player, me);
        }

        void UpdateAI(uint32 diff) override
        {
            if (checkTimer > diff)
            {
                checkTimer -= diff;
                return;
            }
            checkTimer = 500;

            Player* player = ObjectAccessor::GetPlayer(*me, playerGuid);
            if (!player || player->GetMapId() != LiLisDayOff::MapId ||
                player->GetQuestStatus(LiLisDayOff::QuestId) != QUEST_STATUS_INCOMPLETE)
            {
                me->DespawnOrUnsummon();
                return;
            }

            if (!me->IsWithinDistInMap(player, 45.0f))
            {
                me->NearTeleportTo(player->GetPositionX(), player->GetPositionY(),
                    player->GetPositionZ(), player->GetOrientation());
                me->GetMotionMaster()->MoveFollow(player, 2.5f, M_PI);
            }

            LiLisDayOff::CreditExploration(player, me);
        }

    private:
        ObjectGuid playerGuid;
        uint32 checkTimer = 500;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_li_li_day_off_companionAI(creature);
    }
};

namespace WeedWar
{
    constexpr uint32 Quest = 30052;
    constexpr uint32 DailyQuest = 30321;
    constexpr uint32 Aura = 114494;
    constexpr uint32 NastyWeed = 57308;
    constexpr uint32 UglyWeed = 57306;
    constexpr uint32 QuestCredit = 57358;
    constexpr uint32 DailyQuestCredit = 59524;
    constexpr uint32 MapId = 870;
    // Gai Lan stands west of the actual field.  Center the event on the crop
    // rows (the live field spans roughly x -150..25, y 1090..1215), otherwise
    // valid players on the eastern half never receive a single summon.
    constexpr float FarmX = -65.0f;
    constexpr float FarmY = 1152.0f;
    constexpr float FarmRadius = 125.0f;

    bool HasActiveQuest(Player* player)
    {
        return player && (player->GetQuestStatus(Quest) == QUEST_STATUS_INCOMPLETE ||
            player->GetQuestStatus(DailyQuest) == QUEST_STATUS_INCOMPLETE);
    }
}

// The retail event is driven by Weed War's one-second periodic dummy aura.
// Its missing script left the player with the aura but no weeds and therefore
// no possible objective progress.  Keep a bounded set of personal summons
// around the player for the full three-minute event.
class spell_vfw_weed_war : public AuraScript
{
    PrepareAuraScript(spell_vfw_weed_war);

    void HandlePeriodic(AuraEffect const* /*auraEffect*/)
    {
        Player* player = GetOwner() ? GetOwner()->ToPlayer() : nullptr;
        if (!WeedWar::HasActiveQuest(player) ||
            player->GetMapId() != WeedWar::MapId ||
            player->GetExactDist2d(WeedWar::FarmX, WeedWar::FarmY) >
                WeedWar::FarmRadius)
            return;

        std::list<TempSummon*> weeds;
        player->GetSummons(weeds, WeedWar::NastyWeed);
        std::list<TempSummon*> uglyWeeds;
        player->GetSummons(uglyWeeds, WeedWar::UglyWeed);
        weeds.splice(weeds.end(), uglyWeeds);

        uint32 const outstanding = uint32(weeds.size());
        uint32 const toSummon = outstanding < 16 ?
            std::min<uint32>(4, 16 - outstanding) : 0;
        for (uint32 index = 0; index < toSummon; ++index)
        {
            Position position = player->GetNearPosition(frand(4.0f, 16.0f),
                frand(0.0f, 2.0f * float(M_PI)));
            player->UpdateGroundPositionZ(position.m_positionX,
                position.m_positionY, position.m_positionZ);
            uint32 const entry = (outstanding + index) % 3 ?
                WeedWar::NastyWeed : WeedWar::UglyWeed;
            player->SummonCreature(entry, position,
                TEMPSUMMON_TIMED_DESPAWN, 30 * IN_MILLISECONDS, 0,
                player->GetGUID());
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(
            spell_vfw_weed_war::HandlePeriodic, EFFECT_0,
            SPELL_AURA_PERIODIC_DUMMY);
    }
};

struct npc_vfw_weed_war_weed : public ScriptedAI
{
    npc_vfw_weed_war_weed(Creature* creature) : ScriptedAI(creature)
    {
        // Weed War is completed by pulling weeds, but keeping the creatures on
        // their friendly/neutral template factions makes them look unusable to
        // the client and prevents players who attack them from making progress.
        // Support both the retail spell-click interaction and ordinary damage.
        me->SetFaction(14);
        me->SetReactState(REACT_PASSIVE);
        me->RemoveFlag(UNIT_FIELD_FLAGS,
            UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC |
            UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    }

    void AwardCredit(Player* player)
    {
        if (!WeedWar::HasActiveQuest(player))
            return;

        if (player->GetQuestStatus(WeedWar::Quest) == QUEST_STATUS_INCOMPLETE)
            player->KilledMonsterCredit(WeedWar::QuestCredit);
        if (player->GetQuestStatus(WeedWar::DailyQuest) == QUEST_STATUS_INCOMPLETE)
            player->KilledMonsterCredit(WeedWar::DailyQuestCredit);

        if (!WeedWar::HasActiveQuest(player))
            player->RemoveAurasDueToSpell(WeedWar::Aura);
    }

    void OnSpellClick(Unit* clicker, bool& result) override
    {
        Player* player = clicker ? clicker->ToPlayer() : nullptr;
        TempSummon* summon = me->ToTempSummon();
        if (!WeedWar::HasActiveQuest(player) || !summon ||
            summon->GetSummonerGUID() != player->GetGUID())
            return;

        result = true;
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        AwardCredit(player);
        me->DespawnOrUnsummon(250);
    }

    void JustDied(Unit* /*killer*/) override
    {
        TempSummon* summon = me->ToTempSummon();
        Player* owner = summon && summon->GetSummoner() ?
            summon->GetSummoner()->ToPlayer() : nullptr;
        AwardCredit(owner);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

void AddSC_valley_of_the_four_winds()
{
    // Rare Mobs
    new npc_bonobos();
    new npc_sele_na();
    new npc_blackhoof();

    // Standard Mobs
    new npc_ik_thik_warrior();

    // Quests
    new spell_gen_gather_muddy_water();
    new spell_gen_gather_muddy_water_aura();
    new npc_orange_painted_turnip();
    new spell_vfw_disoriented();
    new npc_q29982();
    new AreaTrigger_cavern_of_endless_echoes();
    new creature_script<npc_fish_fellreed>("npc_fish_fellreed");
    new creature_script<npc_liang_thunderfoot>("npc_liang_thunderfoot");
    new creature_script<npc_francis_the_shepherd_boy>("npc_francis_the_shepherd_boy");
    new spell_script<spell_suspicious_footprint_identified>("spell_suspicious_footprint_identified");
    new spell_script<spell_four_wind_pick_up_chicken>("spell_four_wind_pick_up_chicken");
    new spell_script<spell_to_be_a_master_food_coocking>("spell_to_be_a_master_food_coocking");
    new creature_script<npc_just_a_folk_story_quest>("npc_just_a_folk_story_quest");
    new creature_script<npc_granary_vandal>("npc_granary_vandal");
    new creature_script<npc_catch_and_carry>("npc_catch_and_carry");
    new spell_script<spell_throw_grain>("spell_throw_grain");
    new aura_script<spell_catch_grain>("spell_catch_grain");
    new creature_script<npc_vfw_adolescent_mushan>("npc_vfw_adolescent_mushan");
    new creature_script<npc_vfw_wyrmhorn_turtle>("npc_vfw_wyrmhorn_turtle");
    new creature_script<npc_vfw_lupello>("npc_vfw_lupello");
    new creature_script<npc_vfw_snagtooth_hooligan>("npc_vfw_snagtooth_hooligan");
    new creature_script<npc_vfw_frenzyhop>("npc_vfw_frenzyhop");
    new creature_script<npc_vfw_hornbill_strider>("npc_vfw_hornbill_strider");
    new creature_script<npc_vfw_grainhunter_hawk>("npc_vfw_grainhunter_hawk");
    new creature_script<npc_vfw_springtail_gnasher>("npc_vfw_springtail_gnasher");
    new creature_script<npc_vfw_mothfighter>("npc_vfw_mothfighter");
    new creature_script<npc_vfw_wildscale_herbalist>("npc_vfw_wildscale_herbalist");
    new creature_script<npc_vfw_ikthik_vanguard>("npc_vfw_ikthik_vanguard");
    new creature_script<npc_vfw_darkhide>("npc_vfw_darkhide");
    new creature_script<npc_vfw_barrel_of_fireworks>("npc_vfw_barrel_of_fireworks");
    new creature_script<npc_vfw_ik_thik_wing_commander>("npc_vfw_ik_thik_wing_commander");
    new creature_script<npc_vfw_manifestation_of_fear>("npc_vfw_manifestation_of_fear");
    new creature_script<npc_vfw_krungko_fingerlicker>("npc_vfw_krungko_fingerlicker");
    new creature_script<npc_vfw_unbarreled_pandaren>("npc_vfw_unbarreled_pandaren");
    new spell_script<spell_vfw_prey_pounce>("spell_vfw_prey_pounce");
    new spell_script<spell_vfw_apply_mask>("spell_vfw_apply_mask");
    new aura_script<spell_vfw_krungko_timer>("spell_vfw_krungko_timer");
    new atrigger_script<sat_vfw_ground_and_pound>("sat_vfw_ground_and_pound");
    new aura_script<spell_vfw_breaking_barrel>("spell_vfw_breaking_barrel");
    new npc_vfw_miss_fanny();
    new npc_vfw_clever_ashyo();
    new creature_script<npc_sunsong_farm_crop>("npc_sunsong_farm_crop");
    new spell_script<spell_sunsong_plant_seed>("spell_sunsong_plant_seed");
    new player_sunsong_ranch();
    new player_lesson_in_bravery();
    new npc_gardener_fran_hop_hunting();
    new player_hop_hunting_recovery();
    new npc_mudmug_barreling_along_questgiver();
    new npc_mudmug_barreling_along_escort();
    new player_barreling_along_recovery();
    new player_stoneplow_thirsts_recovery();
    new npc_chen_and_li_li_escort();
    new player_chen_and_li_li_recovery();
    new npc_li_li_day_off_companion();
    new aura_script<spell_vfw_weed_war>("spell_vfw_weed_war");
    new creature_script<npc_vfw_weed_war_weed>("npc_vfw_weed_war_weed");
}
