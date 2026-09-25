# Legends of Azeroth Pandaria 5.4.8

World of Warcraft Mists of Pandaria emulator core for client build `5.4.8.18414`.

This project is based on SkyFire/TrinityCore-style server architecture and builds the usual `authserver`, `worldserver`, database tools, and map extraction tools.

The repository is an actively developed MoP test server rather than an untouched
upstream core. It contains source-backed core/database repairs, custom playerbot
automation, solo test systems and administrator tooling. Features marked
**experimental** below have passed build and clean-start checks, but still require
more in-game encounter testing before they should be treated as production-ready.

## Installation

- [Fresh Linux installation](#fresh-linux-installation)
- [Windows build](#configure-and-build-on-windows)
- [Clean database snapshots](contrib/clean_install/README.md)
- [Updating an existing Linux installation](#updating-an-existing-linux-installation)

## Project Features and Current Status

Installation defaults below refer to the published configuration examples as of
2026-09-25. Experimental features can be enabled separately.

| Component | Included | Current local status | Notes |
| --- | --- | --- | --- |
| MoP 5.4.8 core | Yes | Enabled | Client build `18414`; `authserver`, `worldserver`, scripts and extraction tools. |
| In-game Battle Pay shop | Yes | Enabled, catalog only | The client SHOP button is available after login and the catalog is loaded from the world database. It includes class-filtered VIP1/T14, VIP2/T15 and VIP3/T16 armor categories, discounted eight-piece bundles, individual armor pieces and separately purchased VIP weapons. This standalone repository does not include a website, donation checkout or vote-reward system, so normal players currently have no public way to earn the shop points required for purchases. |
| Playerbots | Yes | Enabled | `AiPlayerbot.Enabled = 1`; automatic random-bot login remains disabled. |
| Solo Arena bot fill | Yes | Disabled by default, experimental | Arena Battlemaster choices for 2v2, 3v3 and 5v5; role/faction-aware bot selection, temporary PvP loadouts, preparation buffs, rewards, exit/health recovery and cleanup. |
| Battleground bot fill and objectives | Yes | Enabled, experimental | Fills a real player's queue, builds both factions, applies temporary PvP loadouts and includes CTF, node, orb, cart, vehicle, resurrection, escort, mount and basic path/LoS handling. |
| LFG/LFR bot fill | Yes | Enabled, experimental | Stages missing tank/healer/damage roles for a real player, including the native 25-player Raid Finder 2/6/17 composition, and uses the normal 5.4.8 proposal flow. Filler dungeon locks are refreshed after equipment preparation; temporary LFR fillers inherit the real requester's progression access while retaining level, faction, season and item-level checks. Each filler gets one pre-combat class-buff attempt; there is no Arena-style waiting/retry stage once combat begins. Requester leave/logout pauses managed bot AI, removes the fillers from the instance group, returns them outside and logs them out. Dungeon-specific mechanics still require gameplay coverage. |
| World-boss raid bots | Yes | Available, experimental | Neutral Boss Bot Caller NPCs support 10/25-player preview/call, PvE role selection and gear, legendary cloaks, raid marks, summon, buffs/rebuff, wipe recovery, status and dismiss/cleanup. Encounter-specific AI is still being tuned. |
| Combat Assistant 5.4.8 | Yes | Server side enabled | One physical click/key press casts the server-recommended ability. All playable classes/specs are supported; the client addon must also be installed. |
| VIP hybrid/test vendors | Yes | SQL package included | VIP1-3 test equipment plus optional gems, enchants, riding, mounts and profession helpers. VIP1/T14, VIP2/T15 and VIP3/T16 include class/spec-labelled five-piece PvE sets with genuine 2/4-piece bonuses; all three ranks' real weapons accept one Sha-Touched legendary gem. |
| AuctionHouseBot | Yes | Seller enabled | Fresh auction stock is generated automatically; buying player auctions is disabled. |
| Warden client checks | Yes | **Disabled** | The Warden subsystem and check data are present, but `Warden.Enabled = 0` in the active configuration. |

### Major Custom Systems

- **In-game Battle Pay shop:** the MoP client SHOP window is enabled after login.
  Its categories, products and product items are loaded from the world database
  `battle_pay_group`, `battle_pay_entry`, `battle_pay_product` and
  `battle_pay_product_items` tables, while an account's purchasable balance is
  stored in the authentication database. The repository currently contains no
  public website integration for donations or voting, and no normal in-game
  point-earning flow is configured. The catalog can therefore be viewed, but a
  regular player cannot obtain purchase points in this standalone setup. The
  shop's 2/3/7/13-point vouchers are tradeable and redeem for exactly their
  purchase price on the recipient's account. The in-game-only `VIP 1-3 SETS`
  category is filtered to the logged-in character's class and exposes each
  applicable role/spec armor block as either
  one discounted eight-piece purchase or eight individual pieces. `VIP 1-3
  WEAPONS` keeps weapons and off-hands as separate choices. The 5.4.8 client has
  no nested BattlePay category protocol, so these are adjacent top-level shop
  categories rather than submenus.
- **Playerbot Arena automation:** a real player can use an Arena Battlemaster's
  custom solo 2v2/3v3/5v5 choices. The server selects the required teammate and
  opponents, loads only the managed bots, creates the teams, queues the match and
  restores managed bot state afterward. The connected real player's equipment is
  never replaced by the temporary bot loadout.
- **Playerbot Battleground automation:** specific and random BG queues can be
  populated only in response to a real player's queue. Objective routing exists
  for the playable local 5.4.8 BG templates, including Warsong Gulch, Twin Peaks,
  Arathi Basin, Battle for Gilneas, Eye of the Storm, Temple of Kotmogu, Deepwind
  Gorge, Silvershard Mines, Alterac Valley, Isle of Conquest and Strand of the
  Ancients. Score and rewards still go through normal battleground handlers.
- **Playerbot LFG automation:** missing dungeon roles are selected by faction,
  level, specialization and eligibility. Bot entry, proposal acceptance and group
  ownership use the core's normal LFG paths. Each filler gets one class-buff attempt
  after entering the dungeon and before combat; unlike Arena preparation, it is not
  retried after the requester or bot enters combat. Unsupported action target values
  are rejected safely and logged once with the bot, class, specialization, action and
  target-value names instead of crashing the map worker. The real requester remains
  every filler's pinned master for the complete run; fillers assist the requester's
  target and enemies already attacking the party, but cannot autonomously chain-pull
  unrelated packs merely because the requester is in combat. When the
  real requester leaves or logs out, only the automation-owned fillers are
  quiesced, removed from the abandoned instance group, returned outside and
  logged out; they are not left behind under a bot leader.
- **Pandaria world-boss raid staging:** visible neutral Boss Bot Callers are placed
  at supported outdoor bosses. They can assemble 10- or 25-player PvE raids around
  the requester, revive bots, apply role-appropriate builds/equipment, mark the
  main tank and primary healer, rebuff after deaths and safely dismiss the staged
  raid when the boss dies or the player cancels it. Headless playerbots are
  excluded from personal/bonus world-boss rewards regardless of whether they were
  called through this NPC or summoned normally; loot, money bags, currencies,
  legendary quest drops and loot lockouts remain reserved for real players.
- **Combat Assistant addon:** the tracked addon in
  [`contrib/combat_assistant_548`](contrib/combat_assistant_548) displays one
  recommended spell button and can bind it to key `2`. The server evaluates the
  active class/spec, talents, resources, cooldowns, crowd control, interrupts,
  dispels, emergency healing, defensives and supported ally protection. It never
  auto-casts and does not bypass GCD, range, facing, immunity or line of sight.
- **VIP PvE test equipment:** the package in
  [`vip_hybrid_solo_set_sql`](vip_hybrid_solo_set_sql) provides free test vendors
  and class/spec-aware VIP1 T14, VIP2 T15 and VIP3 T16 equipment for solo and
  encounter testing. The
  package keeps its generated SQL and source generator together so it can be
  reproduced on another database.

### Core, Quest and Database Work

The project also contains many bounded, source-backed fixes rather than blanket
deletions made only to silence logs. Notable completed work includes:

- Pandaren Alliance/Horde faction choice now refreshes the 5.4.8 client without a
  relog; it was verified with the 32-bit client for Alliance and the 64-bit client
  for Horde.
- The Death Knight `Eye of Acherus` quest vehicle can complete its scripted flight
  and then be flown/controlled by the player.
- Wandering Isle quest chains, dialogue, credits, transports, balloon flight,
  phasing and duplicate-spawn problems have received targeted repairs.
- Wandering Isle quest `29792`, `Bidden to Greatness`, is verified end to end.
  One closed Mandori and Pei-Wu world gate is visible before the quest. Accepting
  the quest hides those base spawns and creates exactly one personal gate at each
  scene; both gates open automatically, cannot be clicked manually, award the
  required credits and allow the quest to complete normally. The underlying
  SmartAI no-parameter event fallthrough that caused duplicate summons was fixed
  in the core.
- Missing `creature_text` diagnostics include the requested text group, full source
  GUID, map, position and owner identity so an invalid `Talk()` caller can be traced.
- Spell scripts, conditions, SmartAI, creature text, LFG destinations, item random
  suffix data, vendors, pools and gameobject/creature spawns have been audited
  against compatible local SkyFire/Trinity database sources with backups before
  destructive changes.
- Reforge restore/update handling and Void Storage slot/persistence safety include
  compatible fixes selectively ported from a reviewed SkyFire 5.4.8 update range.
- The 2026-08-24 fresh-start audit completed with an empty `DBErrors.log` and no
  startup error/warning/invalid/missing/crash diagnostics in `Server.log`.
- The extraction helper also copies client `cameras` data used by cinematic camera
  paths in addition to `dbc`, `maps`, `vmaps` and `mmaps`.

Detailed implementation history, exact backups, deferred work and verification
results are recorded in
[`doc/startup-log-fix-plan-2026-07-09.md`](doc/startup-log-fix-plan-2026-07-09.md).
SkyFire migration/source comparison notes are in
[`doc/SKYFIRE_548_MIGRATION_CHECKLIST.md`](doc/SKYFIRE_548_MIGRATION_CHECKLIST.md).

### Known Limits

- The visible in-game SHOP is database-backed, but this repository has no bundled
  donation/vote website or player-facing method for earning its purchase points.
- Playerbots and the Arena/BG/LFG/world-boss automation are development/test
  features. Individual class AI and encounter mechanics still need wider in-game
  verification.
- Generic world-boss tank swapping is not guessed without verified encounter data.
  Galleon has separate add-tank handling; other boss-specific positioning and
  handoff logic remains encounter-dependent.
- Some old database warnings remain deliberately unchanged where no exact compatible
  source exists. Data is not removed merely to make a warning disappear.

## Requirements

### Windows

- Windows 10/11 x64
- Visual Studio 2022 Community with `Desktop development with C++`
- Windows SDK 10.0.22621 or newer
- CMake 3.27 or newer
- Boost 1.85 x64 for MSVC 14.3/14.4
- MariaDB server/client and Python 3.9+ for the current clean database snapshots
- MySQL-compatible client development libraries for the CMake build
- OpenSSL 1.1.1 or OpenSSL 3.x

Historical Windows build layout (adjust the preset paths to your installation;
use MariaDB for the current database snapshots):

```txt
Wampserver: 3.4.2 64-bit
Apache:     2.4.67
PHP:        7.4.33
MySQL:      5.7.44 (client development library paths used by the preset)
MariaDB:    use the server/client installed for your clean database import
Boost:      C:/local/boost_1_85_0
OpenSSL:    C:/Program Files/OpenSSL-Win64
```

### Linux

- GCC 13+ with C++20 support (or a compatible Clang toolchain)
- CMake 3.16+ for command-line Linux builds; the Windows preset requires 3.27+
- Boost, OpenSSL, MariaDB client development libraries, Readline, BZip2 and zlib
- MariaDB server/client and Python 3.9+ for the installation importer
- A complete WoW 5.4.8.18414 client for extracting server data

## Fresh Linux Installation

This walkthrough uses Debian 13, MariaDB and the standard `/usr/local` install
prefix. The bundled snapshots were successfully imported into MariaDB 11.8.
Use a fresh checkout: the old repository history and SQL migrations were replaced
by the clean installation baseline on 2026-09-25.

### 1. Install dependencies and clone the repository

```bash
sudo apt update
sudo apt install -y build-essential cmake git python3 pkg-config \
  mariadb-server mariadb-client libmariadb-dev libmariadb-dev-compat \
  libboost-all-dev libssl-dev libreadline-dev libbz2-dev zlib1g-dev libncurses-dev
sudo systemctl enable --now mariadb

cd ~
git clone https://github.com/davvvve1/Legends-of-Azeroth-Pandaria-5.4.8.git
cd ~/Legends-of-Azeroth-Pandaria-5.4.8
```

### 2. Build and install with 16 jobs

```bash
cd ~/Legends-of-Azeroth-Pandaria-5.4.8
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local \
  -DTOOLS=ON -DPLAYERBOTS=ON -DELUNA=OFF
make -j16 && sudo make install
```

The server executables are installed in `/usr/local/bin`; configuration templates
are installed in `/usr/local/etc`. Reduce `-j16` if the build runs out of memory.
`ELUNA=OFF` matches this installation's Lua-disabled setup.

### 3. Import the four clean databases

The snapshots contain current world data and bot templates. Accounts, characters,
mail, auctions and private bot state start empty. No old SQL update files need to
be imported on top of this baseline.

Create a **private** client configuration for the importer:

```bash
touch ~/.loa-install.cnf
chmod 600 ~/.loa-install.cnf
nano ~/.loa-install.cnf
```

For Debian's local MariaDB root socket authentication, enter:

```ini
[client]
user=root
socket=/run/mysqld/mysqld.sock
```

Run the importer as root so socket authentication succeeds:

```bash
cd ~/Legends-of-Azeroth-Pandaria-5.4.8
sudo python3 contrib/clean_install/import.py --client-config "$HOME/.loa-install.cnf"
```

This creates `world`, `auth`, `characters` and `playerbots`. It verifies checksums
and joins the numbered world snapshot parts automatically. **It refuses to run
if any destination database already exists.** Do not delete a live database to
make the command pass. For a separate installation, add `--prefix fresh_` and
use the resulting names in the grants and server configuration below.

See the [snapshot guide](contrib/clean_install/README.md) for other authentication
methods, included tables and refreshing the exports.

### 4. Create a private runtime database user

Open MariaDB:

```bash
sudo mariadb
```

Replace the password placeholder before running this SQL. This creates a new
server login; it does not create any game account:

```sql
CREATE USER 'loa'@'127.0.0.1' IDENTIFIED BY 'REPLACE_WITH_A_NEW_DATABASE_PASSWORD';
GRANT ALL PRIVILEGES ON auth.* TO 'loa'@'127.0.0.1';
GRANT ALL PRIVILEGES ON world.* TO 'loa'@'127.0.0.1';
GRANT ALL PRIVILEGES ON characters.* TO 'loa'@'127.0.0.1';
GRANT ALL PRIVILEGES ON playerbots.* TO 'loa'@'127.0.0.1';
EXIT;
```

### 5. Install and edit the configuration files

Use the reviewed `.example` files to retain this server's gameplay and bot
settings. The following commands are for a **new installation**; do not overwrite
an existing server's private configuration when updating its binaries.

```bash
cd ~/Legends-of-Azeroth-Pandaria-5.4.8
sudo install -d /usr/local/etc
sudo install -m 600 -o "$(id -un)" etc/authserver.conf.example /usr/local/etc/authserver.conf
sudo install -m 600 -o "$(id -un)" etc/worldserver.conf.example /usr/local/etc/worldserver.conf
sudo install -m 600 -o "$(id -un)" etc/playerbots.conf.example /usr/local/etc/playerbots.conf
sudo install -d -m 750 -o "$(id -un)" -g "$(id -gn)" \
  /usr/local/var/loa /usr/local/var/loa/data /usr/local/var/loa/Logs
ln -s /usr/local/etc/playerbots.conf /usr/local/var/loa/playerbots.conf
nano /usr/local/etc/authserver.conf
nano /usr/local/etc/worldserver.conf
nano /usr/local/etc/playerbots.conf
```

Replace **every** `CHANGE_ME` with the runtime database password from step 4.
The connection format is `host;port;user;password;database`:

```ini
LoginDatabaseInfo = "127.0.0.1;3306;loa;CHANGE_ME;auth"
WorldDatabaseInfo = "127.0.0.1;3306;loa;CHANGE_ME;world"
CharacterDatabaseInfo = "127.0.0.1;3306;loa;CHANGE_ME;characters"
PlayerbotsDatabaseInfo = "127.0.0.1;3306;loa;CHANGE_ME;playerbots"
```

`LoginDatabaseInfo` also appears in `authserver.conf`, and
`PlayerbotsDatabaseInfo` also appears in `playerbots.conf`; keep them consistent.
In `worldserver.conf`, set:

```ini
RealmID = 1
DataDir = "/usr/local/var/loa/data"
LogsDir = "/usr/local/var/loa/Logs"
Console.Enable = 1
```

Set `LogsDir = "/usr/local/var/loa/Logs"` in `authserver.conf` as well.
Playerbots loads `playerbots.conf` from the **working directory**, which is why
this guide creates the symlink and runs both servers from `/usr/local/var/loa`.
Do not publish your real `.conf` or `.cnf` files.

### 6. Extract client data

Use your own complete WoW `5.4.8.18414` client. The data files are not included in
this repository. On Linux, run the installed tools from the client directory:

```bash
cd /path/to/your/WoW-client
/usr/local/bin/mapextractor -b 18273
/usr/local/bin/vmap4extractor
mkdir -p vmaps mmaps
/usr/local/bin/vmap4assembler Buildings vmaps
/usr/local/bin/mmaps_generator --threads 16
cp -a dbc maps vmaps mmaps cameras /usr/local/var/loa/data/
```

Wait for each extraction step to complete successfully. The extractor's target
build `18273` is intentional for this client's MPQ layout. For Windows extraction,
see [Extracting Client Data](#extracting-client-data) below.

### 7. Set the realm address and start the server

The imported realm defaults to `127.0.0.1`, port `8085`, build `18414`. Leave it
unchanged when the client runs on the same machine. For remote clients, use
`sudo mariadb auth` and set an address those clients can reach:

```sql
UPDATE realmlist SET address = 'YOUR_REACHABLE_SERVER_ADDRESS' WHERE id = 1;
```

Allow TCP ports `3724` and `8085` through your firewall/router when needed.
MariaDB can stay local to the server.

Start authserver in one terminal:

```bash
cd /usr/local/var/loa
/usr/local/bin/authserver -c /usr/local/etc/authserver.conf
```

Start worldserver in another terminal, using the same Linux user who owns the
configuration and runtime directories:

```bash
cd /usr/local/var/loa
/usr/local/bin/worldserver -c /usr/local/etc/worldserver.conf
```

The first startup can take longer while new bot accounts and characters are
created. Watch the console and `/usr/local/var/loa/Logs` for startup errors.
Create your own game account in the **worldserver console**, replacing the names
and password placeholders:

```text
account create YOUR_ACCOUNT YOUR_GAME_PASSWORD
account set addon YOUR_ACCOUNT 4
account set gmlevel YOUR_ACCOUNT 3 -1
```

Only grant GM level to an account that should administer the server. In the
client's `WTF/Config.wtf`, set the realm and portal to the address you configured:

```text
SET realmlist "YOUR_REACHABLE_SERVER_ADDRESS"
SET portal "YOUR_REACHABLE_SERVER_ADDRESS"
```

### 8. Verify bots and the Auction House

The examples enable playerbots and automatic LFG filling. Background random-bot
login, automatic BG/Arena filling and Combat Assistant are disabled by default
in these examples. Existing bot characters are not imported; the server builds a
new pool using `AiPlayerbot.RandomBotAccountCount` and the supplied name/templates.

Auction House Bot selling is enabled with `AuctionHouseBot.Account = 0`. This
uses the core's account-free auction ownership and does not depend on an old
account or character. Auction tables start empty and fill over update cycles
after worldserver starts. Buying players' auctions remains disabled.

```ini
AuctionHouseBot.Account = 0
AuctionHouseBot.Seller.Enabled = 1
AuctionHouseBot.Buyer.Enabled = 0
```

After logging in, check an auctioneer and test an LFG queue. Database import has
been tested; individual encounters and bot behavior still require in-game checks.

### Optional: run with systemd

After the foreground startup succeeds, stop both foreground servers and set
`Console.Enable = 0` in `worldserver.conf`. Create these two service files with
`sudoedit`, replacing `YOUR_LINUX_USER` with the user used in step 5.

`/etc/systemd/system/mop-auth.service`:

```ini
[Unit]
Description=Legends of Azeroth Authserver
After=mariadb.service network-online.target
Requires=mariadb.service

[Service]
User=YOUR_LINUX_USER
WorkingDirectory=/usr/local/var/loa
ExecStart=/usr/local/bin/authserver -c /usr/local/etc/authserver.conf
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
```

`/etc/systemd/system/mop-world.service`:

```ini
[Unit]
Description=Legends of Azeroth Worldserver
After=mariadb.service mop-auth.service
Requires=mariadb.service mop-auth.service

[Service]
User=YOUR_LINUX_USER
WorkingDirectory=/usr/local/var/loa
ExecStart=/usr/local/bin/worldserver -c /usr/local/etc/worldserver.conf
Restart=on-failure
RestartSec=5
LimitNOFILE=65535

[Install]
WantedBy=multi-user.target
```

Enable and inspect the services:

```bash
sudo systemctl daemon-reload
sudo systemctl enable --now mop-auth.service mop-world.service
sudo systemctl status mop-auth.service mop-world.service
journalctl -u mop-world.service -n 100 --no-pager
```

### Updating an existing Linux installation

Keep your current databases and private configuration. Do not reimport the clean
snapshots over a running installation. Follow any future release-specific database
migration instructions separately.

Update the source, then always build from the project's `build` directory:

```bash
cd ~/Legends-of-Azeroth-Pandaria-5.4.8
git pull --ff-only
cd ~/Legends-of-Azeroth-Pandaria-5.4.8/build
make -j16 && sudo make install && sudo systemctl restart mop-world.service
```

Restart `mop-auth.service` too when authserver changes. A restart disconnects
players. If your checkout still has the pre-cleanup history, preserve local work
and obtain a fresh clone instead of trying to merge the removed history.

## Configure and Build on Windows

The Linux walkthrough above is the complete fresh-install path. On Windows,
build using the following instructions, then use the same clean snapshots and
reviewed `.example` configurations. Put Python 3 and the MariaDB command-line
client on `PATH` for the importer. The older Wampserver paths below are build
examples; the current snapshots were validated with MariaDB, not MySQL 5.7.

The repository includes a CMake preset for a Wampserver-style Windows setup:

```powershell
cmake --preset windows-wamp-vs2022
cmake --build --preset windows-wamp-vs2022-relwithdebinfo
```

Or open the generated solution:

```txt
Build/Legends-of-Azeroth-Pandaria-5.4.8.sln
```

Recommended Visual Studio build configuration:

```txt
RelWithDebInfo x64
```

The built executables will be in:

```txt
Build/bin/RelWithDebInfo
```

Important executables:

```txt
authserver.exe
worldserver.exe
mapextractor.exe
vmap4extractor.exe
vmap4assembler.exe
mmaps_generator.exe
```

## OpenSSL 3 Legacy Provider

The core loads OpenSSL's legacy and default providers during startup. On Linux,
keep the OpenSSL runtime/provider packages installed alongside the development
libraries. The DLL instructions below apply to Windows.


For OpenSSL 3, the server needs the OpenSSL legacy provider because MoP authentication still uses RC4 through `AuthCrypt`.

If you use OpenSSL 3, copy this file:

```txt
C:/Program Files/OpenSSL-Win64/bin/legacy.dll
```

next to the server executables:

```txt
Build/bin/RelWithDebInfo/legacy.dll
```

Without this file, `worldserver.exe` can crash when a client connects to the realm. The crash stack usually points to:

```txt
Trinity::Crypto::ARC4::ARC4
AuthCrypt::AuthCrypt
WorldSocket::WorldSocket
```

Also keep these DLLs next to the executables:

```txt
libcrypto-3-x64.dll
libssl-3-x64.dll
libmysql.dll
```

## Database Setup

Use the current, privacy-filtered snapshots in [`sql/install`](sql/install).
They include world content and bot templates, with empty player/account data.
The old SQL migrations and database backups have been replaced by this baseline.

Follow the [clean installation guide](contrib/clean_install/README.md). Its importer
verifies checksums and refuses to overwrite any existing database:

```bash
python3 contrib/clean_install/import.py --client-config ~/.loa-install.cnf
```

## Configuration Files

Copy the reviewed examples from [`etc`](etc) to your installation's config
folder, remove `.example`, and fill in your own database credentials:

- `authserver.conf.example`
- `worldserver.conf.example`
- `playerbots.conf.example`

The examples retain gameplay and bot settings, but contain no live database
password or deployment IP. Set your local data path and realm address before
starting the server. Keep real `.conf` and `.cnf` files out of Git.

## Extracting Client Data

The server needs client data extracted from a clean World of Warcraft `5.4.8.18414` client:

```txt
dbc
maps
vmaps
mmaps
cameras
```

Build the project with `TOOLS=ON`, then copy or use:

```txt
extract_548_18414_maps_vmaps_mmaps.bat
```

Put the `.bat` file in the root of the WoW client folder and run it there. Before running, edit these paths inside the file if your project is not in the default local path:

```bat
set "TOOLS_DIR=C:\wamp64\www\Legends-of-Azeroth-Pandaria-5.4.8\Build\bin\RelWithDebInfo"
set "SERVER_DIR=C:\wamp64\www\Legends-of-Azeroth-Pandaria-5.4.8\Build\bin\RelWithDebInfo"
```

For build `5.4.8.18414`, the extractor uses:

```bat
set "TARGET_BUILD=18273"
```

This is expected for this extractor and client data layout.

## Running the Server on Windows

Start MariaDB first, copy and edit the three `.example` configurations beside
the executables, set your extracted data path, then run:

```txt
Build/bin/RelWithDebInfo/authserver.exe
Build/bin/RelWithDebInfo/worldserver.exe
```

Ports:

```txt
authserver:  3724
worldserver: 8085
```

Create an account from the `worldserver` console:

```txt
account create YOUR_ACCOUNT YOUR_GAME_PASSWORD
account set gmlevel YOUR_ACCOUNT 3 -1
account set addon YOUR_ACCOUNT 4
```

For a local client, set `WTF/Config.wtf`:

```txt
SET realmlist "127.0.0.1"
SET portal "127.0.0.1"
```

## Playerbots

Playerbots are included and enabled in the current local test setup, but remain
experimental. They may still expose class-, map- or encounter-specific gameplay
issues depending on database state and configuration.

Required file: `playerbots.conf` in the server working directory. The Linux
walkthrough links it from `/usr/local/etc`; on Windows place it beside the
executables and launch from that directory.

Basic enable/disable options:

```ini
AiPlayerbot.Enabled = 1
AiPlayerbot.RandomBotAutologin = 0
```

Automatic random-bot login is not required by the custom queue systems. Arena, BG,
LFG and world-boss staging load only the exact bots selected for the real player's
request and clean them up afterward.

Every random-bot login equips four `Royal Satchel` bags (28 slots each) before
specialization, Caller or loadout preparation begins. A pre-existing bag that
contains any item is never removed; an empty smaller bag may be upgraded. Gear
initialization repeats this capacity check before replacing equipment.

Hunter random bots also recover older pets that were accidentally persisted in
invalid slot `255`; the pet is moved into a valid active slot instead of making
the bot repeat `Call Pet 1` indefinitely. Raid preparation casts Arcane
Brilliance on the party rather than only checking the mage's own persistent
aura. Shaman raid cooldown totems are coordinated across nearby headless bots:
only one Mana Tide is active at once, and Spirit Link/Healing Tide are staggered
instead of being spent together. Personal damage totems and stackable healing
totems remain independent.

World-boss composition maximizes class diversity inside each role before
filling duplicate-class slots. A 25-player Alliance pool therefore uses one
Paladin, Priest, Shaman, Monk and Druid healer when all five are free, and takes
one representative of every available DPS class before adding further damage
bots. Low starting item level no longer excludes a valid class; selected bots
receive their specialization's five-piece item-level 566 T16 set and the
appropriate legendary cloak. Remaining slots prioritize missing raid buffs,
then lower PvP-piece count and higher starting item level, with random choice
between exact gear ties.

Random-bot hunter pets use the PvE Ferocity specialization. Every permanent
Playerbot pet—including Hunter, Warlock and Mage pets—uses passive reaction.
Taunt and direct threat-generating pet spells are disabled both during pet
initialization and later autocast maintenance, and a pet attack command is
refused until its target has already entered combat (or the owner is directly
attacking that target), preventing the pet from pulling for the raid.

The published configuration examples enable automatic LFG filling. BG/Arena
automation and Combat Assistant are available but disabled:

```ini
AiPlayerbot.AutoQueue.Enabled = 1
AiPlayerbot.AutoQueue.DryRun = 1
AiPlayerbot.AutoQueue.LFG = 1
AiPlayerbot.AutoQueue.LFG.Automatic = 1
AiPlayerbot.AutoQueue.Battleground = 0
AiPlayerbot.AutoQueue.Battleground.Automatic = 0
AiPlayerbot.AutoQueue.Arena = 0
AiPlayerbot.AutoQueue.Arena.Automatic = 0
AiPlayerbot.AutoQueue.Arena.AutomaticBattlemasterSolo = 0
AiPlayerbot.CombatAssistant.Enabled = 0
LFGSolo.Enabled = 0
```

`DryRun = 1` keeps the older generic observer protected. The newer explicitly
enabled request-driven Arena/BG/LFG paths have their own gates and are not disabled
by that observer setting. Use `etc/*.conf.example` for the installation defaults documented here;
the generic `.dist` templates can differ.

The legacy `LFGSolo` shortcut must remain disabled while playerbot LFG filling is
enabled. When `LFGSolo.Enabled = 1`, every dungeon queue is reduced to one damage
slot, the real player receives an immediate solo proposal, and there are no missing
tank/healer/damage slots for the bot automation to fill.

Implementation and test details are documented in
[`contrib/playerbot_auto_queue_548/README.md`](contrib/playerbot_auto_queue_548/README.md).

For debugging, disable them:

```ini
AiPlayerbot.Enabled = 0
AiPlayerbot.RandomBotAutologin = 0
```

The first playerbots startup can take longer because random bot accounts and characters are prepared.

## Troubleshooting

For Linux installations:

- **Database already exists during import:** the importer is protecting existing
  data. Use a separate database prefix for a second installation.
- **Access denied for the database user:** check the four connection strings and
  the matching MariaDB user/password/grants, including `playerbots.conf`.
- **Cannot load playerbots.conf:** start from `/usr/local/var/loa` and check its
  symlink to `/usr/local/etc/playerbots.conf` and file permissions.
- **Missing maps/DBC/vmaps/mmaps/cameras:** check `DataDir` and complete extraction
  from the correct client build before starting worldserver.
- **Empty Auction House immediately after startup:** allow seller update cycles
  to run, then verify the seller and house-ratio settings in the active config.
- **Build stops with a killed compiler process:** reduce `make -j16` to fewer jobs.

```bash
sudo systemctl status mariadb mop-auth.service mop-world.service
journalctl -u mop-world.service -n 100 --no-pager
ss -ltn | grep -E ':(3724|8085) '
```

For Windows installations:

If the client reaches realm selection but disconnects when selecting the realm:

- Check that `worldserver.exe` is still running.
- Check that port `8085` is listening.
- If using OpenSSL 3, make sure `legacy.dll` is next to `worldserver.exe`.
- Check `Build/bin/RelWithDebInfo/Logs/Server.log`.
- Check crash reports in `Build/bin/RelWithDebInfo/Crashes`.

PowerShell checks:

```powershell
Get-Process authserver,worldserver -ErrorAction SilentlyContinue
Get-NetTCPConnection -LocalPort 3724,8085 -ErrorAction SilentlyContinue
```

If `authserver.exe` says port `3724` is already in use, another `authserver` instance is already running:

```powershell
taskkill /F /IM authserver.exe
```

## License

GPL-2.0. See [COPYING.md](COPYING.md).

## Authors and Contributors

See [THANKS.md](THANKS.md).

## Build Status

[![windows-build](https://github.com/Legends-of-Azeroth/Legends-of-Azeroth-Pandaria-5.4.8/actions/workflows/windows-build-release.yml/badge.svg?branch=master)](https://github.com/Legends-of-Azeroth/Legends-of-Azeroth-Pandaria-5.4.8/actions/workflows/windows-build-release.yml)
[![linux-gcc-build](https://github.com/Legends-of-Azeroth/Legends-of-Azeroth-Pandaria-5.4.8/actions/workflows/linux_gcc.yml/badge.svg?branch=master)](https://github.com/Legends-of-Azeroth/Legends-of-Azeroth-Pandaria-5.4.8/actions/workflows/linux_gcc.yml)

<a href="https://scan.coverity.com/projects/legends-of-azeroth-mop">
  <img alt="Coverity Scan Build Status" src="https://scan.coverity.com/projects/26941/badge.svg"/>
</a>
