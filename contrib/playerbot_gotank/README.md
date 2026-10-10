# Playerbot `gotank` regression

Run `python3 contrib/playerbot_gotank/run_tests.py` from the repository root.
The check fails if the accepted chat spellings, map-thread transition reset,
Mogu'shan Palace route restart, dead-master regrouping, combat-safe recovery,
or the generic 160-yard mmap-gated corridor scan is removed.

The selected tank leads only while the real master is alive and the party is
ready.  Repeating `gotank`, `go tank`, or `go-tank` clears leadership and makes
the bots follow the real master again.  Enabling it later creates a fresh route
generation rather than resuming stale movement from the previous activation.
