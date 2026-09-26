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

Historical migrations are available in `sql/updates/`. Select migrations for your installed database version; some older files are also mirrored in this patch directory.
