#!/usr/bin/env bash
set -euo pipefail

if (( EUID != 0 )); then
    echo "Run this installer as root: sudo $0" >&2
    exit 1
fi

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
dump_dir=/var/lib/mop-world/coredumps

install -d -o server -g server -m 0750 "$dump_dir"
install -o root -g root -m 0644 \
    "$script_dir/60-mop-world-coredump.conf" \
    /etc/sysctl.d/60-mop-world-coredump.conf
install -d -o root -g root -m 0755 \
    /etc/systemd/system/mop-world.service.d
install -o root -g root -m 0644 \
    "$script_dir/coredump-systemd.conf" \
    /etc/systemd/system/mop-world.service.d/coredump.conf
install -o root -g root -m 0644 \
    "$script_dir/mop-world-coredump.tmpfiles" \
    /etc/tmpfiles.d/mop-world-coredump.conf

/usr/sbin/sysctl -p /etc/sysctl.d/60-mop-world-coredump.conf
systemd-tmpfiles --create /etc/tmpfiles.d/mop-world-coredump.conf
systemctl daemon-reload

echo "Worldserver core dumps are enabled."
echo "Crash files will be written to: $dump_dir"
echo "The current service already has LimitCORE=infinity; no restart is required."
