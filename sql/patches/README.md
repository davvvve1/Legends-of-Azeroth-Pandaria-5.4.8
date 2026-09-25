# World database patches

These reviewed patches update existing installations. The current `sql/install`
world archive already includes all five fixes. Do not reimport that archive over
an existing database.

Apply the files in `world/` in filename order to your world database using your
local database credentials. Back up first. These patches are safe to repeat and
contain no player records, accounts, passwords, or server addresses.

- Durnholde: quest sparkle on diversion barrels.
- Sethekk Halls: Lakka rescue for the current quest and Cobalt Eye quest drops.
- Steamvault: Irradiated Gear quest drop from Steamrigger.
- Magisters' Terrace: Volatile Essence quest drop from Vexallus.

Loot fixes cover both normal and heroic. The object and gossip fixes share the
same templates on both difficulties. Restart worldserver after upgrading code
and applying patches. For loot-only changes, a GM can use
`.reload creature_loot_template` before the next kill; existing corpses keep
already-generated loot.

Steamvault panel interaction/sparkle, the Magisters' Terrace exit teleport, bot
formation and bot wipe recovery are code fixes and require rebuilding and
installing worldserver. Build with `make -j16` from the `build` directory.
