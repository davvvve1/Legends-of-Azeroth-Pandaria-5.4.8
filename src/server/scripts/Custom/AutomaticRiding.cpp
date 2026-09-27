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
        { 1, 34090 },  // Expert riding
        { 1, 90267 },  // Flight Master's License
        { 1, 54197 },  // Cold Weather Flying
        { 1, 34091 },  // Artisan riding
        { 1, 90265 },  // Master riding
        { 1, 115913 }, // Wisdom of the Four Winds
        { 1, 130487 } // Cloud Serpent Riding
    };

    for (RidingTraining const& skill : training)
        // LearnSpell also restores ranks previously hidden by superseding.
        if (player->GetLevel() >= skill.level && !player->HasActiveSpell(skill.spell))
            player->LearnSpell(skill.spell, false);

    // Mount capability selection uses the riding skill value, not just known spells.
    // Repair missing or lower saved skill values without waiting for a trainer.
    if (player->GetPureSkillValue(SKILL_RIDING) < 375 || player->GetPureMaxSkillValue(SKILL_RIDING) < 375)
        player->SetSkill(SKILL_RIDING, 5, 375, 375);
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
