# Playerbot instance-mechanics regression

Run `python3 contrib/playerbot_instance_mechanics/run_tests.py` from the
repository root.  It emits a machine-readable JSON result and fails when the
shared strategy is no longer loaded for every bot, scenario maps fall outside
the instance gate, or Gekkan's target order changes.

The Gekkan order is `Ironhide -> Hexxer -> Skulker -> Oracle -> Gekkan`.
Ironhide removes the 50% nearby damage reduction first; Hexxer removes the
casting-speed debuff next.  Cleansing Flame remains covered by the existing
group-wide coordinated interrupt system.

The shared layer provides reusable reactions (hazard avoidance, frontal
avoidance, spread/stack, kiting, tank swaps, defensives, critical-add switches
and encounter-unit healing) in normal, heroic, challenge, LFR, flex, raid and
scenario maps.  This is baseline coverage, not evidence that every boss has a
dedicated encounter script.  A fight needing vehicle controls, clicks,
platform transitions or a unique puzzle still requires a specific strategy
and live verification.
