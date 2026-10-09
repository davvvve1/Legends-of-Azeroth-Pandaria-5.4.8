-- Restore the private Sunsong Ranch crop state machine.  These templates are
-- personal summons; the script makes only the owner's current plot clickable.
UPDATE `creature_template`
SET `ScriptName` = 'npc_sunsong_farm_crop'
WHERE `entry` IN (
    58562,58563,58566,58567,60026,60029,60070,60153,60181,60185,
    63145,63146,63147,63148,63153,63154,63159,63161,63162,63165,
    63167,63169,63178,63181,63182,63185,63187,63189,63222,63224,
    63226,63229,63231,63233,63243,63246,63247,63250,63252,63254,
    63259,63261,63262,63265,63268,63270,65913,65916,65918,65921,
    65924,65933,65964,65965,65966,65969,65971,65973,65985,65986,
    65987,65989,65991,65993,66002,66003,66004,66006,66008,66010,
    66012,66013,66014,66016,66018,66020,66039,66040,66041,66043,
    66045,66047,66079,66081,66082,66085,
    66087,66089,66107,66109,66110,66113,66115,66117,66122,66124,
    66125,66129,66131,66133
);

-- Tilling is handled by the private plot script.  The old generic vehicle
-- click attempted to mount the soil and could display a spurious cast error.
DELETE FROM `npc_spellclick_spells`
WHERE `npc_entry` = 58562
  AND `spell_id` = 46598;

DELETE FROM `spell_script_names`
WHERE `ScriptName` = 'spell_sunsong_plant_seed';

INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
    (111102, 'spell_sunsong_plant_seed'), -- Green Cabbage
    (123361, 'spell_sunsong_plant_seed'), -- Juicycrunch Carrot
    (123388, 'spell_sunsong_plant_seed'), -- Scallions
    (123485, 'spell_sunsong_plant_seed'), -- Mogu Pumpkin
    (123535, 'spell_sunsong_plant_seed'), -- Red Blossom Leek
    (123565, 'spell_sunsong_plant_seed'), -- Pink Turnip
    (123568, 'spell_sunsong_plant_seed'), -- White Turnip
    (123773, 'spell_sunsong_plant_seed'), -- Snakeroot
    (123774, 'spell_sunsong_plant_seed'), -- Enigma
    (123775, 'spell_sunsong_plant_seed'), -- Magebulb
    (129623, 'spell_sunsong_plant_seed'), -- Windshear Cactus
    (129628, 'spell_sunsong_plant_seed'), -- Raptorleaf
    (129863, 'spell_sunsong_plant_seed'), -- Songbell
    (129974, 'spell_sunsong_plant_seed'), -- Witchberries
    (129976, 'spell_sunsong_plant_seed'), -- Jade Squash
    (129978, 'spell_sunsong_plant_seed'); -- Striped Melon
