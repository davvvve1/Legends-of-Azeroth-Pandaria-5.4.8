# Automatic riding and account mounts

`AutomaticRiding.cpp` grants riding ranks and flight licenses from level 1 and repairs riding skill to 375. World migrations 27/28 lower mount item level requirements; migration 29 sets every mount item and riding trainer minimum to 1. The core also overrides riding/mount spell levels and skill race/class minimums in server memory, including account mount spells. Account mounts still load through the existing `account_spell` login query.

`ObjectMgr::LoadItemTemplates` sets `RequiredLevel` to 1 only for miscellaneous mount items after loading both DB2 defaults and database overrides. This also covers mounts absent from `item_template`. Other item requirements and normal spell, ability, and quest levels are unchanged. Mount capability checks still enforce riding skill, map, area, swimming state, and required licenses; spell validation still enforces dungeon and no-fly restrictions.

Heart of the Aspects (spell 110051) is the shared ground/flying mount. The login/level-change script grants it to both factions, including new accounts. Character migration `2026_09_27_00_characters_shared_level_one_mount.sql` grants it to every existing account with characters. No bag space is needed. Below level 20, use `/cast Heart of the Aspects` or put that macro on the action bar: the stock mount journal has a separate local `PLAYER_MOUNT_LEVEL = 20` check that disables its buttons and shows `MOUNT_JOURNAL_CANT_USE` before sending a cast to the server. The macro still uses normal spell and location validation.

`Player::AddSpell` preserves active riding ranks instead of superseding apprentice/journeyman when expert/artisan/master are learned. `SendInitialSpells` omits inactive spells, so previously only the highest riding rank was advertised at login. The login script now reactivates previously hidden riding ranks. Other spell rank behavior is unchanged.

Build, run `sudo make install`, restart worldserver, then log out and in. Apply world migration 29 for the trainer/item policy; the active-rank correction itself requires no SQL.

In-game verification still required:

- Log in on a level 1/11 character with previously learned master riding; verify apprentice and journeyman are known client-side (`IsSpellKnown(33388)` and `IsSpellKnown(33391)`).
- Summon an owned Black Wolf and Traveler's Tundra Mammoth from the mount collection outdoors, out of combat, in normal form.
- Repeat after relogging and after a level increase; verify saved riding remains 375/375.
- Check an ordinary ranked combat spell still supersedes its lower rank.
- Confirm Heart of the Aspects is granted on both factions and other unowned mounts are not granted.

The code and build are verified locally. In-game casting still needs verification. The stock client mount journal's level-20 UI gate was confirmed by inspecting the local client Lua; changing server DBC data does not change that UI gate. No client patch is shipped.
