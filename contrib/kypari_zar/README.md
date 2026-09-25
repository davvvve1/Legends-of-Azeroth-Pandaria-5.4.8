# Kypari Zar (31022)

Sonar Tower 212933 previously cast dummy spell 123531 without a script handler.
Neither the tower nor summoned Korven 63328 had an event script. Quest objectives
267931 and 268179 require creature credits 63286 and 63287 respectively.

The tower now starts a defense for the clicking player. Korven and his attackers
use that player's private-object ownership (group members may also see them).
Repeated clicks cannot start another event for the same player. Tower credit is
granted when Korven spawns; defense credit requires killing all three waves of
Ik'thik Towerguards. This implementation uses waves of three, four and five,
with five-second pauses; these timings/counts are a reconstruction, not verified
retail timings. The expected click/defense sequence is also described in the
[quest reference](https://www.wowhead.com/mop-classic/quest=31022/kypari-zar).

Death, logout, abandoning the quest, leaving an 80-yard radius, an attacker
despawning alive, failed summons, or the three-minute timeout clean up the event
without granting defense credit. Click the tower again to retry. Static Korven
quest givers are unchanged. Event dialogue is English.

## Validation

Run `python3 contrib/kypari_zar/run_tests.py`. The harness compiles the production
callbacks and exercises click gates, failed initial summons, duplicate clicks,
separate players, attack targets, kill-gated waves, single completion credit,
cleanup and retry after eight failure paths. Worldserver also builds successfully.
The harness does not verify client rendering, terrain navigation or live combat.

## Deployment

Apply `sql/updates/world/2026_09_22_02_world_kypari_zar.sql`, install the rebuilt
worldserver, and restart it. The migration is idempotent. In this workspace it
was applied to the configured world database; previous values were saved to
`/tmp/kypari_zar_before.tsv`. Installation and a live gameplay test remain pending.
