# Test and VIP item cleanup — 2026-09-25

The user authorized removal from the auction house, item definitions and all
owners, explicitly including custom VIP equipment. The live databases are
`mop_world_test` and `mop_characters_test`. Worldserver was stopped by the user.

`manifest.json` records the 3,517 explicitly identified test/GM/VIP template IDs
and their original names/levels. Classification uses explicit test/debug/GM
markers plus confirmed GM items and the custom VIP range. Deprecated/unused
names alone were not enough for removal. Normal quest items 8523, 8527, 19971
and 69217 were explicitly protected after checking quest references. Names
such as Battle Tested Blade, Tester's Notes, Stinging Viper and The Greatest
Race of Hunters are not internal test items.

The finalized cleanup and checksummed row backups are in
`sql/backup/test_item_cleanup_20260925/final/`. Earlier snapshots in its parent
folder are pre-review snapshots, not the final cleanup plan. `plan.json` lists
every affected table and predicate; `apply.sql` contains the transaction;
`verification.tsv` records the post-commit checks. `restore.sql` restores the
original rows from those backups and must only be used with worldserver stopped.
Backups contain account/character data and should not be published publicly.

Before deletion, the same SQL was exercised in a transaction ending in ROLLBACK.
All selected templates, copies and auctions reached zero; eleven known legitimate
control templates remained. There were no paid bids on the removed auctions and
no unrelated items inside removed bags. Mail text/money survives removal of its
attachments. Gear replacements are not granted to human characters by this cleanup.

The retired VIP item-only shop products and entries were removed, along with
vendor/loot offers, locale/addon rows, saved item references and appearance
collection entries. The Promotion service remains; its test item association is
removed. Audit ID/GUID tables remain as rollback/audit metadata, not usable items.

`prepare.py` is the one-off preparation utility. It requires private client
configuration files in `/tmp`, the captured schema, and the audited ID tables;
it refuses to overwrite the verified backup and does not execute `apply.sql`.

`TestItemFilter.h` prevents AHBot from restocking these kinds of items after a
future import. Test with:

```
c++ -std=c++11 -Wall -Wextra -Werror contrib/test_item_cleanup/filter_regression.cpp -o /tmp/test-item-filter
/tmp/test-item-filter
```

The worldserver build passed. Installation and restart are left to the user.
