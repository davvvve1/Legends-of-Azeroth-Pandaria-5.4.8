# The Plains of Nasam tank fixes

Requires migration 2026_09_27_23 and matching installed worldserver.

Manual checks (not yet performed in-game):

1. Accept 11652, board Horde Siege Tank, and select an Injured Warsong Mage within 15 yards. Use Rescue Injured Soldier (slot 3). Verify one rescued-soldier credit, soldier removal, and respawn after 60 seconds. Repeat for warrior, shaman and engineer. An enemy, out-of-range target or already rescued soldier must give no rescue credit.
2. Two tanks attempt to rescue the same soldier: only the first successful rescue should credit.
3. Select a hostile target within 100 yards with line of sight, fire The Demoralizer, and verify impact at that target position. Try facing away and different camera angles. Clear selection and verify original ground aiming. Dead, friendly, out-of-range and obstructed targets must reject the selected-target cast.
4. Kill Scourge at existing Nasam positions: verify Scourge Unit credit and 30-second respawn. Verify unrelated Scourge outside the bounded Nasam region keep existing respawn settings.
5. Check Meatpounder still works and boarding, movement and dismount work both in Borean Tundra and battlegrounds.

This patch restores rescue and selected-target cannon aim; it does not implement missing landmine, fuel or central-structure mechanics.
