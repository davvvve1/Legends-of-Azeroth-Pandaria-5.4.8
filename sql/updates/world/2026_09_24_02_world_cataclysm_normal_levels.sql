-- Correct base (normal-mode) levels in Cataclysm dungeons and their scripted summons.
-- The imported templates contained heroic levels or mixed normal/heroic ranges.
-- Normal difficulty falls back to creature_template when no creature_difficulty
-- row exists; these entries have no DUNGEON_NORMAL overrides.
-- Source (normal/base templates, not difficulty_entry_1 heroic templates):
-- https://github.com/FirelandsProject/firelands-cata/blob/b250899d4fdf68603e4d8a4d05b4b522d8319eb0/data/sql/base/db_world/creature_template.sql
-- Covers Blackrock Caverns, Throne of the Tides, Stonecore, Vortex Pinnacle,
-- Grim Batol and Lost City of the Tol'vir. Halls of Origination was checked
-- and its spawned creature levels already matched the reference.
-- Existing creature_difficulty rows and all other creature stats are preserved.
-- Reload affected templates and respawn creatures, or restart worldserver.
START TRANSACTION;
-- Crimsonborne Guardian
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 39381 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Twilight Hatchling
UPDATE `creature_template` SET `minlevel` = 85, `maxlevel` = 85 WHERE `entry` = 39388 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Faceless Corruptor
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 39392 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Crimsonborne Seer
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 39405 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Ascended Windwalker
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 39414 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Ascended Flameseeker
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 39415 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Trogg Dweller
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 39450 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Naz'jar Invader
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 39616 AND `minlevel` = 85 AND `maxlevel` = 85;
-- General Umbriss
UPDATE `creature_template` SET `minlevel` = 86, `maxlevel` = 86 WHERE `entry` = 39625 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Crimsonborne Warlord
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 39626 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Corla, Herald of Twilight
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 39679 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Karsh Steelbender
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 39698 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Beauty
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 39700 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Ascendant Lord Obsidius
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 39705 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Twilight Flame Caller
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 39708 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Azureborne Guardian
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 39854 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Azureborne Seer
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 39855 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Twilight Firecatcher
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 39870 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Twilight Wyrmcaller
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 39873 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Twilight Earthshaper
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 39890 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Azureborne Warlord
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 39909 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Twilight Shadow Weaver
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 39954 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Twilight Enforcer
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 39956 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Lady Naz'jar
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 39959 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Deep Murloc Drudge
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 39960 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Twilight Stormbreaker
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 39962 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Twilight Torturer
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 39978 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Twilight Sadist
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 39980 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Crazed Mage
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 39982 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Malignant Trogg
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 39984 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Mad Prisoner
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 39985 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Evolved Twilight Zealot
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 39987 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Twilight Zealot
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 39990 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Conflagration
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 39994 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Quicksilver
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 40004 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Lucky
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 40008 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Buster
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 40013 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Runty
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 40015 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Twilight Element Warden
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 40017 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Twilight Obsidian Borer
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 40019 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Incendiary Spark
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 40021 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Defiled Earth Rager
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 40023 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Ground Siege Stalker
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 40030 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Blitz Stalker
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 40040 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Bellows Slave
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 40084 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Enslaved Gronn Brute
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 40166 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Twilight Beguiler
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 40167 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Forgemaster Throngus
UPDATE `creature_template` SET `minlevel` = 86, `maxlevel` = 86 WHERE `entry` = 40177 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Cave In Stalker
UPDATE `creature_template` SET `minlevel` = 85, `maxlevel` = 85 WHERE `entry` = 40228 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Twilight War-Mage
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 40268 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Twilight Thundercaller
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 40270 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Ascended Rockbreaker
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 40272 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Ascended Waterlasher
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 40273 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Searing Light
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 40283 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Crimsonborne Seer
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 40290 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Azureborne Seer
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 40291 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Twilight Armsmaster
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 40306 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Drahga Shadowburner
UPDATE `creature_template` SET `minlevel` = 86, `maxlevel` = 86 WHERE `entry` = 40319 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Valiona
UPDATE `creature_template` SET `minlevel` = 86, `maxlevel` = 86 WHERE `entry` = 40320 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Invocation of Flame Stalker
UPDATE `creature_template` SET `minlevel` = 85, `maxlevel` = 85 WHERE `entry` = 40355 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Invoked Flaming Spirit
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 40357 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Seeping Twilight
UPDATE `creature_template` SET `minlevel` = 85, `maxlevel` = 85 WHERE `entry` = 40365 AND `minlevel` = 85 AND `maxlevel` = 87;
-- Chains of Woe
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 40447 AND `minlevel` = 81 AND `maxlevel` = 82;
-- Twilight Enforcer
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 40448 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Erudax
UPDATE `creature_template` SET `minlevel` = 86, `maxlevel` = 86 WHERE `entry` = 40484 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Deep Murloc Hunter
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 40579 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Naz'jar Invader
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 40584 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Geyser
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 40597 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Faceless Corruptor
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 40600 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Naz'jar Tempest Witch
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 40634 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Dark Fissure
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 40784 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Neptulon
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 40792 AND `minlevel` = 86 AND `maxlevel` = 86;
-- Shadow of Obsidius
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 40817 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Erunak Stonespeaker
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 40825 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Mind Fog
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 40861 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Khaaphom
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 40953 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Twilight Armsmaster
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 41073 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Twilight Drake
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 41095 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Naz'jar Spiritmender
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 41096 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Naz'jar Spiritmender
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 41139 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Ozruk
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 42188 AND `minlevel` = 84 AND `maxlevel` = 87;
-- Devout Follower
UPDATE `creature_template` SET `minlevel` = 83, `maxlevel` = 83 WHERE `entry` = 42428 AND `minlevel` = 83 AND `maxlevel` = 84;
-- Gravity Well
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 42499 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Stonecore Rift Conjurer
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 42691 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Stonecore Bruiser
UPDATE `creature_template` SET `minlevel` = 83, `maxlevel` = 83 WHERE `entry` = 42692 AND `minlevel` = 86 AND `maxlevel` = 86;
-- Stonecore Sentry
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 42695 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Stonecore Warbringer
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 42696 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Stonecore Magmalord
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 42789 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Stonecore Flayer
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 42808 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Crystalspawn Giant
UPDATE `creature_template` SET `minlevel` = 83, `maxlevel` = 83 WHERE `entry` = 42810 AND `minlevel` = 86 AND `maxlevel` = 86;
-- Rock Borer
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 42845 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Imp
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 43014 AND `minlevel` = 81 AND `maxlevel` = 84;
-- Slabhide
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 43214 AND `minlevel` = 84 AND `maxlevel` = 87;
-- Stalactite Trigger - Trash, On Ground
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 43357 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Millhouse Manastorm
UPDATE `creature_template` SET `minlevel` = 83, `maxlevel` = 83 WHERE `entry` = 43391 AND `minlevel` = 83 AND `maxlevel` = 86;
-- Stonecore Berserker
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 43430 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Corborus
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 43438 AND `minlevel` = 84 AND `maxlevel` = 87;
-- Stonecore Earthshaper
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 43537 AND `minlevel` = 85 AND `maxlevel` = 85;
-- High Prophet Barim
UPDATE `creature_template` SET `minlevel` = 86, `maxlevel` = 86 WHERE `entry` = 43612 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Lockmaw
UPDATE `creature_template` SET `minlevel` = 86, `maxlevel` = 86 WHERE `entry` = 43614 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Dust Flail
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 43655 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Frenzied Crocolisk
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 43658 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Unbound Earth Rager
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 43662 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Altairus
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 43873 AND `minlevel` = 84 AND `maxlevel` = 87;
-- Asaad
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 43875 AND `minlevel` = 84 AND `maxlevel` = 87;
-- Grand Vizier Ertan
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 43878 AND `minlevel` = 84 AND `maxlevel` = 87;
-- Rock Borer
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 43917 AND `minlevel` = 81 AND `maxlevel` = 83;
-- Soul Fragment
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 43934 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Sharptalon Eagle
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 44261 AND `minlevel` = 85 AND `maxlevel` = 85;
-- General Husam
UPDATE `creature_template` SET `minlevel` = 86, `maxlevel` = 86 WHERE `entry` = 44577 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Ozumat Vehicle
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 44581 AND `minlevel` = 86 AND `maxlevel` = 86;
-- Bad Intentions Target
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 44586 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Unyielding Behemoth
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 44648 AND `minlevel` = 84 AND `maxlevel` = 84;
-- Deep Murloc Invader
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 44658 AND `minlevel` = 81 AND `maxlevel` = 84;
-- Minion of Siamat
UPDATE `creature_template` SET `minlevel` = 83, `maxlevel` = 83 WHERE `entry` = 44713 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Vicious Mindlasher
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 44715 AND `minlevel` = 84 AND `maxlevel` = 84;
-- Tol'vir Land Mine
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 44796 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Blight of Ozumat
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 44801 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Siamat
UPDATE `creature_template` SET `minlevel` = 86, `maxlevel` = 86 WHERE `entry` = 44819 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Blight of Ozumat
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 44834 AND `minlevel` = 86 AND `maxlevel` = 86;
-- Blight Beast
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 44841 AND `minlevel` = 81 AND `maxlevel` = 84;
-- Pygmy Brute
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 44896 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Pygmy Scout
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 44897 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Pygmy Firebreather
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 44898 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Oathsworn Axemaster
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 44922 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Oathsworn Myrmidon
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 44924 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Oathsworn Wanderer
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 44926 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Oathsworn Pathfinder
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 44932 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Unyielding Behemoth (Leap Vehicle)
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 44949 AND `minlevel` = 86 AND `maxlevel` = 86;
-- Neferset Torturer
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 44977 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Neferset Theurgist
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 44980 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Oathsworn Skinner
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 44981 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Neferset Darkcaster
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 44982 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Ozumat Vehicle, Big
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 45030 AND `minlevel` = 86 AND `maxlevel` = 86;
-- Oathsworn Scorpid Keeper
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 45062 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Bonesnapper Scorpid
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 45063 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Tamed Tol'vir Prowler
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 45096 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Oathsworn Tamer
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 45097 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Servant of Siamat
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 45269 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Trogg Dweller
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 45467 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Earth Shards
UPDATE `creature_template` SET `minlevel` = 81, `maxlevel` = 81 WHERE `entry` = 45469 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Gust Soldier
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 45477 AND `minlevel` = 82 AND `maxlevel` = 85;
-- Naz'jar Soldier
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 45620 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Naz'jar Soldier
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 45672 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Lurking Tempest
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 45704 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Wild Vortex
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 45912 AND `minlevel` = 82 AND `maxlevel` = 85;
-- Armored Mistral
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 45915 AND `minlevel` = 82 AND `maxlevel` = 85;
-- Cloud Prince
UPDATE `creature_template` SET `minlevel` = 83, `maxlevel` = 83 WHERE `entry` = 45917 AND `minlevel` = 83 AND `maxlevel` = 85;
-- Young Storm Dragon
UPDATE `creature_template` SET `minlevel` = 83, `maxlevel` = 83 WHERE `entry` = 45919 AND `minlevel` = 83 AND `maxlevel` = 85;
-- Empyrean Assassin
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 45922 AND `minlevel` = 82 AND `maxlevel` = 85;
-- Turbulent Squall
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 45924 AND `minlevel` = 82 AND `maxlevel` = 85;
-- Servant of Asaad
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 45926 AND `minlevel` = 82 AND `maxlevel` = 85;
-- Executor of the Caliph
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 45928 AND `minlevel` = 82 AND `maxlevel` = 85;
-- Minister of Air
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 45930 AND `minlevel` = 82 AND `maxlevel` = 85;
-- Skyfall Star
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 45932 AND `minlevel` = 82 AND `maxlevel` = 85;
-- Temple Adept
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 45935 AND `minlevel` = 82 AND `maxlevel` = 85;
-- Inferno Leap
UPDATE `creature_template` SET `minlevel` = 85, `maxlevel` = 85 WHERE `entry` = 47040 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Whipping Wind
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 47238 AND `minlevel` = 82 AND `maxlevel` = 84;
-- Wind Tunnel
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 48092 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Waterspout
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82 WHERE `entry` = 48571 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Fire Patch
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 48711 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Faceless Corruptor
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 48844 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Blaze of the Heavens
UPDATE `creature_template` SET `minlevel` = 85, `maxlevel` = 85 WHERE `entry` = 48904 AND `minlevel` = 87 AND `maxlevel` = 87;
-- Burning Soul
UPDATE `creature_template` SET `minlevel` = 84, `maxlevel` = 84 WHERE `entry` = 49219 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Naz'jar Spiritmender
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 50276 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Naz'jar Invader
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 50278 AND `minlevel` = 85 AND `maxlevel` = 85;
-- Twilight Zealot
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80 WHERE `entry` = 50284 AND `minlevel` = 85 AND `maxlevel` = 85;
COMMIT;
