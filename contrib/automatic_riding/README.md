# Automatic riding and account mounts

`AutomaticRiding.cpp` grants riding ranks and flight licenses from level 1 and repairs riding skill to 375. World migrations 27/28 lower mount item level requirements; migration 29 sets every mount item and riding trainer minimum to 1. The core also overrides riding/mount spell levels and skill race/class minimums in server memory, including account mount spells. Account mounts still load through the existing `account_spell` login query.

`Player::AddSpell` preserves active riding ranks instead of superseding apprentice/journeyman when expert/artisan/master are learned. `SendInitialSpells` omits inactive spells, so previously only the highest riding rank was advertised at login. The login script now reactivates previously hidden riding ranks. Other spell rank behavior is unchanged.

Build, run `sudo make install`, restart worldserver, then log out and in. Apply world migration 29 for the trainer/item policy; the active-rank correction itself requires no SQL.

In-game verification still required:

- Log in on a level 1/11 character with previously learned master riding; verify apprentice and journeyman are known client-side (`IsSpellKnown(33388)` and `IsSpellKnown(33391)`).
- Summon an owned Black Wolf and Traveler's Tundra Mammoth from the mount collection outdoors, out of combat, in normal form.
- Repeat after relogging and after a level increase; verify saved riding remains 375/375.
- Check an ordinary ranked combat spell still supersedes its lower rank.
- Confirm unowned or opposite-faction mounts are not granted by this change.

The code and build are verified locally. Mount collection usability on an actual 5.4.8 client has not been verified. A client DBC patch was considered but not shipped; differing client behavior must be checked in game before concluding that a client patch is necessary.
