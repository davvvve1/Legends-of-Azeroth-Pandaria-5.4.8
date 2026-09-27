# World database patches

These reviewed patches update existing installations. The `sql/install` world
archive is a snapshot and may not include newer patches. Do not reimport that
archive over an existing database.

Apply the files in `world/` in filename order to your world database using your
local database credentials. Back up first. These patches are safe to repeat and
contain no player records, accounts, passwords, or server addresses.

- Durnholde: quest sparkle on diversion barrels.
- Sethekk Halls: Lakka rescue for the current quest and Cobalt Eye quest drops.
- Steamvault: Irradiated Gear quest drop from Steamrigger.
- Magisters' Terrace: Volatile Essence quest drop from Vexallus.
- Survival Ring: start, progress and hazards for Flame and Blades (requires the matching code update).
- The Root of the Problem: disable 30 duplicate permanent Coldbite Spawn in the burrow; the existing egg traps still summon them. Rows are retained with `spawnMask=0`. Restart worldserver to unload the old spawns.
- Coldbite Spawn and Hatchlings: reduce weapon damage to 25 percent as a local solo-quest balance adjustment (not a verified retail value). Matriarch and fixed spell damage are unchanged. Restart worldserver after applying.
- Evie Stormstout: Chen (67138) starts the eulogy for nearby quest holders and grants both objectives. Requires the matching code update and restart.
- Han Stormstout: inspect Han (62776) through gossip to earn the discovery objective. Restores quest progression; the amber transport scene is not recreated. Requires matching code and restart.
- Han quest turn-in: add a stationary Chen at the completion marker in Morrowchamber, preserving the shared Brewgarden quest giver. Restart to load the new spawn; cave placement needs live verification.
- The Poisoned Mind: Xaril boarding gossip, private bombing circuit and vehicle abilities. Requires matching code and restart. The route is reconstructed; targets grant normal kill credit, and completion/early exit returns to the boarding point.
- Black Morass / Opening the Dark Portal: remove the Return to Andormu quest requirement for both factions on normal and heroic. Reload access requirements or restart worldserver.
- Sa'at: offer The Opening of the Dark Portal (10297) without the old attunement chain. Companion code fixes beacon validation, event wipe/reset timers, saved completion and explicit heroic boss spells. Install the rebuilt server and restart.
- Xaril: add the missing quest giver outside the Amber Womb at 28.7,42.2. Matching code limits flight starts to this location, offers travel there from Klaxxi'vess, and uses a faster outdoor bombing route within range.

Loot fixes cover both normal and heroic. The object and gossip fixes share the
same templates on both difficulties. Restart worldserver after upgrading code
and applying patches. For loot-only changes, a GM can use
`.reload creature_loot_template` before the next kill; existing corpses keep
already-generated loot.

Steamvault panel interaction/sparkle, the Magisters' Terrace exit teleport, bot
formation and bot wipe recovery are code fixes and require rebuilding and
installing worldserver. Build with `make -j16` from the `build` directory.

- `2026_09_26_03_world_dark_skies_daily.sql`: unlock Dark Skies after The Poisoned Mind, remove the incorrect reputation ceiling, and preserve the daily rotation. Requires matching flight script and restart.
- `2026_09_26_04_world_thunder_pvp_dailies.sql`: Captive Audience and Tactical Mana Bombs for both factions; bind prison/bomb interactions and remove their four NYI blockers. Requires matching code and restart.
- `2026_09_26_06_world_thunder_hold_cannons.sql`: make Paint it Red! cannons passive and pacified. They remain attackable for quest credit. Restart worldserver to reload their template.
- `2026_09_26_07_world_dalaran_portals.sql`: restore the eight missing Dalaran portals to the other capitals and Shattrath. Stormwind, Orgrimmar, and Caverns of Time remain as they are. Restart worldserver to load the spawns.
- `2026_09_26_08_world_nordrassil_summit.sql`: give The Nordrassil Summit from Thrall, complete its ceremony objective through his gossip, and turn it in to Aggra. Requires matching code and a worldserver restart.
- `2026_09_26_09_world_elemental_bonds_doubt.sql`: add Cyclonas's ride to Thrall and make the air encounter completable by defeating 20 Essences of Doubt. Requires matching code and a worldserver restart.

- `2026_09_26_05_world_daily_relations.sql`: restore missing daily quest giver and turn-in relations and enable the corresponding quest menus.

Reviewed patches are mirrored with identical filenames and contents in
`sql/updates/world/`, the standard world database updater directory. Apply each
migration once, using either directory. Keep filename order: the level-78 Wrath
policy in update 12 is superseded by random Wrath from level 68 in update 13.
The published configuration has `Updates.EnableDatabases = 0`, so updates need
manual application unless database updates are explicitly enabled.

Historical migrations are available in `sql/updates/`. Select migrations for your installed database version.

- `2026_09_26_10_world_azjol_nerub_hadronox_duplicate_spawns.sql`: disable duplicate permanent Hadronox and initial pack spawns; the instance script creates them after Krik'thir. Requires matching code and restart to unload existing spawns.

- `2026_09_26_11_world_jade_forest_final_blow_barricades.sql`: hide stair barricades after The Final Blow is complete or rewarded. Requires matching code and restart.
- `2026_09_26_12_world_wrath_dungeons_min_level_78.sql`: former normal Wrath entrance policy; superseded by update 13.
- `2026_09_26_13_world_wrath_random_from_level_68.sql`: restore individual normal entrance limits with a level-68 floor. Matching code opens random normal Wrath from level 68 and retains individual dungeon brackets.
- `2026_09_26_14_world_gorrok_regroup_credit.sql`: restore Gorrok rescue gossip credit without the area-restricted spell.
- `2026_09_26_15_world_strongarm_alive_quest_mobs.sql`: remove permanent feign death from Strongarm Private and Medic and allow NPC allies to attack the Private.
- `2026_09_26_16_world_strongarm_airstrip_quest_audit.sql`: award Doren kill credit after his transformation and require Unreliable Allies for volunteer rescue.
- `2026_09_26_17_world_konk_seein_red_phase.sql`: hide Konk after Seein' Red is complete or rewarded.

Updates 14–17 need a worldserver restart to load changed templates, scripts and
phasing data. These SQL-only changes do not require recompilation.

- `2026_09_27_04_world_acid_rain_gyrocopter.sql`: enable boarding from the Recovered Gyrocopter (or Recovered Supplies), summon a private flight vehicle, and enable Throw Star and Poison Blossom against Acid Rain's Hozen targets. Requires the matching rebuilt server, `sudo make install`, and restart. The patrol is reconstructed from the saved target positions; the original cinematic is not reproduced. Completion or early vehicle exit returns to the boarding position. Needs in-game flight and targeting verification.
- `2026_09_27_05_world_acid_rain_autocomplete.sql`: supersede the Acid Rain flight requirement with immediate completion by removing its kill objectives and setting the autocomplete flag. Existing active quests can also be turned in. Prerequisites and rewards stay intact. Restart worldserver to load the SQL change; no rebuild required.

- `2026_09_27_06_world_young_and_vicious_delivery.sql`: restore Swiftclaw delivery credit and dismount at the raptor pens for Young and Vicious (24626); correct the swapped capture/delivery map markers. Requires worldserver restart, no rebuild. Needs in-game verification.

- `2026_09_27_07_world_echo_isles_spirit_healer.sql`: add the missing Spirit Healer at Echo Isles graveyard 1700. The accompanying core change makes ordinary Spirit Healers accessible across quest phases while preserving ghost visibility and battleground Spirit Guide phases. Build, sudo make install, and restart required.
