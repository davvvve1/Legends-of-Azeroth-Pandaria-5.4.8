# Magisters' Terrace escape orb

Object 188173 casts spell 46841. The installed 5.4.8 SpellEffect.dbc supplies
only TARGET_DEST_DB for effect 0. The destination exists in spell_target_position
(map 530), but the destination LOS exemption in Spell::CheckEffectTarget requires
an explicit TARGET_UNIT_CASTER as well. The correction sets self as TargetA and
keeps the database destination as TargetB, matching the existing LOS exemption.

The correction applies to all difficulty spell stores. The escape object is
spawned with mask 6 (normal and heroic). No database change is required.

Run `python3 contrib/magisters_terrace/run_escape_test.py`. This compiles the
production correction and LOS exemption and checks that unrelated spells retain
their target handling. After installing/restarting worldserver, verify clicking
the orb teleports the player to Quel'Danas on both normal and heroic.
