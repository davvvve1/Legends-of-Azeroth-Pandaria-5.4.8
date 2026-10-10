#!/usr/bin/env python3
"""Compile production dungeon release eligibility against a simulated party."""
from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parents[2]
source=(root/'modules/mod_playerbots/src/strategy/Dead/ReleaseSpiritAction.cpp').read_text()
resurrect_header=(root/'modules/mod_playerbots/src/strategy/value/PartyMemberToResurectValue.h').read_text()
resurrect_source=(root/'modules/mod_playerbots/src/strategy/value/PartyMemberToResurectValue.cpp').read_text()
reach_header=(root/'modules/mod_playerbots/src/strategy/actions/ReachTargetActions.h').read_text()
reach_source=(root/'modules/mod_playerbots/src/strategy/actions/ReachTargetActions.cpp').read_text()
def block(marker):
    a=source.index(marker);b=source.index('{',a)+1;depth=1
    while depth:
        depth+=(source[b]=='{')-(source[b]=='}');b+=1
    return source[a:b]
fixture=r'''
#include <cassert>
#include <iostream>
using uint32=unsigned;
enum {PLAYER_FLAGS_GHOST};
struct Map {bool dungeon=true; bool IsDungeon(){return dungeon;}};
struct InstanceScript {bool combat=false;bool IsEncounterInProgress(){return combat;}};
struct Player;
struct PlayerbotAI;
struct GroupReference {
    Player* member=nullptr;GroupReference* following=nullptr;
    Player* GetSource(){return member;} GroupReference* next(){return following;}
};
struct Group {GroupReference* first;GroupReference* GetFirstMember(){return first;}};
struct Player {
    Map* map;InstanceScript* instance;Group* group=nullptr;
    bool alive=false,combat=false,world=true,ghost=false,request=false,bg=false,arena=false;
    bool healer=false;PlayerbotAI* ai=nullptr;
    bool IsAlive(){return alive;} bool isDead(){return !alive;} bool IsInCombat(){return combat;}
    bool IsInWorld(){return world;} Map* GetMap(){return map;} Group* GetGroup(){return group;}
    bool InSamePhase(Player*){return true;}
    InstanceScript* GetInstanceScript(){return instance;}
    bool HasPlayerFlag(int){return ghost;} bool IsRessurectRequested(){return request;}
    bool InArena(){return arena;} bool InBattleground(){return bg;}
};
struct PlayerbotAI {
    Player* master=nullptr;bool real=false;
    Player* GetGroupMaster(){return master;} bool HasActivePlayerMaster(){return true;}
    bool IsRealPlayer(){return real;}
};
#define GET_PLAYERBOT_AI(player) ((player)->ai)
struct PlayerBotSpec {static bool IsHeal(Player* player,bool){return player->healer;}};
HELPER
bool IsActivePandariaWorldBossFight(Player*){return false;}
struct Config {float sightDistance=90;} config;
auto sPlayerbotAIConfig=&config;
struct Facade {bool IsDistanceGreaterThan(float a,float b){return a>b;}} facade;
auto sServerFacade=&facade;
#define AI_VALUE2(type,name,qualifier) 0.0f
struct AutoReleaseSpiritAction {Player* bot;PlayerbotAI* botAI;bool isUseful();};
USEFUL
int main(){
    Map map,other;InstanceScript instance;
    Player bot{&map,&instance},master{&map,&instance},survivor{&map,&instance};
    PlayerbotAI healerAI;survivor.ai=&healerAI;
    GroupReference refs[]={{&bot,nullptr},{&master,nullptr},{&survivor,nullptr}};
    refs[0].following=&refs[1];refs[1].following=&refs[2];Group group{refs};
    bot.group=master.group=survivor.group=&group;
    PlayerbotAI ai{&master}; AutoReleaseSpiritAction action{&bot,&ai};
    assert(action.isUseful()); // Whole group dead, encounter reset.
    master.alive=true;
    assert(action.isUseful()); // Master already revived nearby: previous failure.
    survivor.alive=true;survivor.combat=true;
    assert(!action.isUseful()); // Trash still fighting a survivor.
    survivor.combat=false;instance.combat=true;
    assert(!action.isUseful()); // Boss has not reset yet.
    bot.request=true;assert(action.isUseful()); // Accept legitimate battle res.
    bot.request=false;instance.combat=false;
    assert(action.isUseful()); // A living damage bot cannot resurrect.
    survivor.healer=true;
    assert(!action.isUseful()); // A living healer bot owns post-combat recovery.
    survivor.healer=false;
    bot.ghost=true;assert(!action.isUseful()); // No repeated release packets.
    bot.ghost=false;bot.alive=true;assert(!action.isUseful());bot.alive=false;
    survivor.combat=true;survivor.map=&other;
    assert(action.isUseful()); // Other instances do not block recovery.
    survivor.map=&map;survivor.world=false;
    assert(action.isUseful());
    map.dungeon=false;survivor.world=true;survivor.combat=false;
    assert(!action.isUseful()); // Outdoor bot still waits for nearby living master.
    master.alive=false;assert(action.isUseful());
    bot.arena=true;assert(!action.isUseful());bot.arena=false;
    bot.bg=true;assert(action.isUseful());
    std::cout<<"Dungeon wipe release: master recovery, combat barriers, resurrection and ghost checks passed\n";
}
'''
fixture=fixture.replace('HELPER',block('bool CanReleaseDungeonSpirit(')).replace('USEFUL',block('bool AutoReleaseSpiritAction::isUseful()'))
with tempfile.TemporaryDirectory(prefix='bot-wipe-') as directory:
    cpp=Path(directory)/'test.cpp';binary=Path(directory)/'test';cpp.write_text(fixture)
    subprocess.run(['c++','-std=c++17','-Wall','-Wextra','-Werror',str(cpp),'-o',str(binary)],check=True)
    subprocess.run([str(binary)],check=True)

assert 'bool Check(Unit* player) override' in resurrect_header
assert 'player->GetMap() == bot->GetMap()' in resurrect_source
assert 'spellDistance * 2' not in resurrect_source
assert 'groupSupportDistance' in resurrect_source
assert 'bool Execute(Event event) override' in reach_header
assert 'bool isUseful() override' in reach_header
assert 'ReachPartyMemberToResurrectAction::Execute' in reach_source
assert '!bot->IsWithinLOSInMap(target)' in reach_source
assert 'MovementPriority::MOVEMENT_NORMAL' in reach_source
heal_source=(root/'modules/mod_playerbots/src/strategy/value/PartyMemberToHealValue.cpp').read_text()
config_source=(root/'modules/mod_playerbots/src/Utils/PlayerbotAIConfig.cpp').read_text()
assert 'AiPlayerbot.GroupSupportDistance", 150.0f' in config_source
assert 'groupSupportDistance' in heal_source
assert 'healDistance * 2' not in heal_source
print('Healing/resurrection targeting: 150-yard discovery and reach movement passed')
