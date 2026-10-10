# Playerbot `gotank` regression

Run `python3 contrib/playerbot_gotank/run_tests.py` from the repository root.
The check fails if the accepted chat spellings, map-thread transition reset,
Mogu'shan Palace route restart, combat-safe recovery,
the generic 160-yard mmap-gated trash scan, post-combat cleanup and combat
engine recovery, leadership-owned formation movement without a living-master
distance leash, uninterrupted pack progression, or boss-to-boss mmap
navigation is removed.

Entering a dungeon or raid automatically elects the deterministic living main
bottank and starts leadership. The leader action runs as a persistent controller
before ordinary idle actions, so it cannot be starved between pulls and no
initial chat command is required.

The selected tank keeps leading without regroup or resurrection pauses.
Repeating `gotank`, `go tank`, or `go-tank` is the only in-instance operation
which clears leadership, suppresses automatic restart for the current instance,
and makes the bots follow the real master again. Enabling it later creates a
fresh route generation rather than resuming stale movement from the previous
activation.

After every pack the leader discards the dead pull target and the preceding
combat movement delay, exits a stale combat engine, then resumes the same
route. Core combat flags do not hold the party indefinitely when no live
nearby enemy is actually fighting. While leadership is active, the tank and
followers use the tank-led formation instead of the normal 140-yard leash to
the living real master.
An unengaged living trash target cached by the combat engine is also discarded;
only a live target belonging to an encounter which is still in progress keeps
the party in combat for a boss intermission.
Outside Mogu'shan Palace, the next incomplete encounter supplies a persistent
destination and the live mmap is traversed in short steps so intervening trash
becomes visible and is pulled normally. Finishing all registered encounters
does not clear leadership: the tank continues through remaining reachable
packs, and only the master's `gotank` (or leaving the instance) restores
real-master following.
