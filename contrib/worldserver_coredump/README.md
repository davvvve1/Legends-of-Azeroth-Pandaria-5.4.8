# Worldserver core dumps

The service runs as `server` with `/usr/local/bin` as its working directory.
The default relative `core` pattern therefore cannot create a file because
that directory is not writable by the service account.

Install the persistent core-dump configuration:

```bash
sudo contrib/worldserver_coredump/install.sh
```

No worldserver restart is needed when its existing `LimitCORE` is already
`infinity`. Future starts retain the explicit systemd limit. Dumps are named
`core.worldserver.PID.TIMESTAMP` in `/var/lib/mop-world/coredumps` and files
older than 14 days are eligible for the normal systemd-tmpfiles cleanup.

After a crash, generate a full backtrace from the newest dump:

```bash
core_file="$(find /var/lib/mop-world/coredumps -maxdepth 1 -type f -name 'core.worldserver.*' -printf '%T@ %p\n' | sort -nr | head -1 | cut -d' ' -f2-)"
gdb -q /usr/local/bin/worldserver "$core_file" -ex 'set pagination off' -ex 'thread apply all bt full' -ex quit
```

Keep the exact `worldserver` binary that generated a dump until it has been
analysed; replacing it can make symbols and addresses disagree with the core.
