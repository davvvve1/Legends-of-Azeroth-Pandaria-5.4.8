#include "ScriptMgr.h"
#include "Player.h"

namespace
{
void LearnRidingForLevel(Player* player)
{
    struct RidingTraining
    {
        uint8 level;
        uint32 spell;
    };

    static RidingTraining const training[] =
    {
        { 1, 33388 },   // Apprentice riding
        { 1, 33391 },   // Journeyman riding (all ordinary ground mounts)
        { 60, 34090 },  // Expert riding
        { 60, 90267 },  // Flight Master's License
        { 68, 54197 },  // Cold Weather Flying
        { 70, 34091 },  // Artisan riding
        { 80, 90265 },  // Master riding
        { 90, 115913 }, // Wisdom of the Four Winds
        { 90, 130487 } // Cloud Serpent Riding
    };

    for (RidingTraining const& skill : training)
        if (player->GetLevel() >= skill.level && !player->HasSpell(skill.spell))
            player->LearnSpell(skill.spell, false);
}
}

class player_automatic_riding : public PlayerScript
{
public:
    player_automatic_riding() : PlayerScript("player_automatic_riding") { }

    void OnLogin(Player* player) override
    {
        LearnRidingForLevel(player);
    }

    void OnLevelChanged(Player* player, uint8 /*oldLevel*/) override
    {
        LearnRidingForLevel(player);
    }
};

void AddSC_automatic_riding()
{
    new player_automatic_riding();
}
