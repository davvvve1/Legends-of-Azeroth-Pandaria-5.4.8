# Clean installation snapshots

These snapshots contain the current world and bot templates as of 2026-09-25.
They replace the old SQL bases, update scripts and backup files. Do not import
old migrations over this snapshot.

| Database | Included data |
| --- | --- |
| world | Current gameplay tables; no backups, player reports or NPC-bot logs |
| auth | Empty accounts and account-related tables; client build/race/class definitions and a localhost realm |
| characters | Empty characters, inventories, mail, auctions and other player state; bot name templates only |
| playerbots | Bot templates, dialogue, routes, equipment and item caches; no character IDs, per-player settings or guild tasks |

No existing player or bot account is included. Bots are regenerated from the
configuration. There is no shared administrator login. Account passwords,
password hashes, session keys, email addresses and login IP records are not
exported. The table allowlist is recorded in `tables.json`.

## Import on a new installation

Install MariaDB and Python 3. Create a private MariaDB client option file, for
example `~/.loa-install.cnf`, with mode `600`:

```ini
[client]
host=127.0.0.1
port=3306
user=YOUR_DATABASE_ADMIN
password=YOUR_LOCAL_PASSWORD
```

From the repository root:

```bash
python3 contrib/clean_install/import.py --client-config ~/.loa-install.cnf
```

The importer verifies SHA-256 checksums and creates `world`, `auth`,
`characters` and `playerbots`. It **refuses to run if any of those databases
already exists**. Use `--prefix fresh_` for another set of new databases and
adjust the server configuration accordingly. It never drops an existing
database. A failed import leaves its new databases for inspection.

World is split into numbered gzip parts to stay below GitHub's file-size
limit. The importer joins these automatically; do not extract parts separately.

Create a local database user with a new password and access to these four
databases. Copy `etc/authserver.conf.example`, `etc/worldserver.conf.example`
and `etc/playerbots.conf.example` to your installation's configuration
directory, removing `.example`. Replace every `CHANGE_ME`, configure data
paths, and set `auth.realmlist.address` locally if clients connect remotely.
The shipped address is loopback; no deployment address is published.

The examples preserve the server's gameplay/bot preferences. Account IDs are
not portable and have been reset. Fresh bot accounts use random passwords.
Keep actual `.conf` and `.cnf` files private; they are ignored by Git.

Auction House Bot selling is enabled. `AuctionHouseBot.Account = 0` uses the
core's account-free auction ownership, so no legacy account/character ID is
required. Auctions start empty and are generated after worldserver starts;
initial population takes time. Buying other players' auctions remains disabled
as in the current server settings.

Start authserver and worldserver, then create your own account and set its GM
level through the server console. The snapshots contain no administrator.

## Refresh snapshots

```bash
python3 contrib/clean_install/export.py \
  --config /path/to/private/worldserver.conf \
  --output /path/to/new-export
```

Export uses read-only MariaDB dumps with explicit table allowlists. It never
changes source tables. Unknown/new tables are excluded until reviewed.
Use an empty output directory, inspect the result, and test an import before
replacing public snapshots. Keep the server/database schema unchanged during
export; each database dump uses a transaction, not a cross-database snapshot.

## History and credentials

The public repository was rebuilt from a clean source snapshot to remove old
SQL backups and live configuration from reachable Git history. Existing clones
and forks are not erased by this operation. Rotate previously published real
credentials. GitHub may retain unreachable commits or cached views; removing
those requires GitHub Support. Do not push an old branch/history back into the
clean repository.
