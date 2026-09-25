# Continuous playerbot following

`MovementAction::Follow` previously tested a `FOLLOW_MOTION_TYPE` generator
with `ServerFacade::GetChaseTarget`, which only handles `CHASE_MOTION_TYPE`.
The duplicate-follow guard therefore never matched. Each eligible AI decision
called `MoveFollow` again; MotionMaster replaced the existing generator and
restarted its path. The guard also ran after spell interruption.

The new `GetFollowTarget` reads the proper player/creature follow generator.
A request to keep following the same target now returns before interrupting
spells or submitting movement. The active generator continues tracking the
moving leader. New targets and transitions out of chase still submit follow.
Pathfinding, collision checks, movement speed and reaction settings are unchanged.

Run `python3 contrib/playerbot_movement/run_follow_test.py`. The regression
compiles the production lookup and movement-submission code. It verifies that
200 repeated decisions preserve one follow command and do not stop spells,
while retargeting, chase transitions and creature lookup remain functional.
A worldserver build checks integration; movement smoothness needs a live-client
check in The Slave Pens after installation and restart.
