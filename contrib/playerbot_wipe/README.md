# Dungeon wipe recovery

A real master can release and revive at the instance entrance before a dead bot
checks whether to release. The old proximity rule then made the bot wait for
resurrection from its living master, even though the party had wiped.

Dungeon auto-release now waits for the instance encounter to finish and for all
living members in the same map instance to leave combat. A living healer bot then
gets the first chance to resurrect each corpse; only a party without one submits
the normal release request. The core's existing instance-entrance recovery still
handles complete wipes without resetting dungeon progress. A real resurrection
request takes precedence, and ghosts do not send duplicate release packets. The
check runs in both action eligibility and execution. Outdoor and battleground
policies remain separate.

Resurrection target discovery is intentionally independent of cast range and
line of sight. The healer's reach prerequisite walks toward a remote corpse and
routes around blocked sight before casting. Active `gotank` leadership pauses
while a corpse remains recoverable instead of starting the next pull.

Run:

```
python3 contrib/playerbot_wipe/run_tests.py
python3 contrib/instance_release/run_tests.py
```

After installation, check a wipe on normal and heroic, including master releasing
before the bots and a surviving player still fighting. Bots should return after
combat resets and resume their normal follow/buff behavior.
