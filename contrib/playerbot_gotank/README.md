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

Leadership owns every safe non-combat AI tick, including ticks where the
current point movement is still in flight and no duplicate movement command is
needed. The ordinary follow/idle engine therefore cannot overwrite the route.
If a point move has stopped but its movement latch remains, the controller
clears that latch and recalculates the route without a new chat command.

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
an instance script's broad `IN_PROGRESS` state cannot pause the route between
waves or phases. Only a real nearby hostile interaction (victim or threat)
holds the tank in combat; as soon as that interaction ends the same leadership
controller advances toward the next route point without another command.
The elected tank owns the party kill order: its selected mob receives skull
before the pull, and skull moves to each new target the tank selects inside a
pack. DPS target selection ranks that skull above unmarked enemies.
Leadership is re-elected from the live instance group on every update while
automatic mode is enabled, so an LFG strategy/map reinitialization cannot drop
the controller. Already validated route steps are dispatched as exact motion
waypoints instead of being rejected by a second generic path search.
Outside Mogu'shan Palace, the next incomplete encounter supplies a persistent
destination and the live mmap is traversed in short steps so intervening trash
becomes visible and is pulled normally. Finishing all registered encounters
does not clear leadership: the tank continues through remaining reachable
packs, and only the master's `gotank` (or leaving the instance) restores
real-master following.
