#!/usr/bin/env python3
"""Import public snapshots into NEW databases. Refuses any existing name."""
import argparse
import gzip
import hashlib
import json
from pathlib import Path
import re
import shutil
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--client-config', required=True, type=Path,
                        help='Private MariaDB [client] option file; never commit it')
    parser.add_argument('--prefix', default='', help='Optional new database name prefix')
    args = parser.parse_args()
    if args.prefix and not re.fullmatch(r'[A-Za-z0-9_]+', args.prefix):
        parser.error('Prefix must contain only letters, digits and underscores')
    folder = Path(__file__).resolve().parents[2] / 'sql/install'
    manifest = json.loads((folder / 'manifest.json').read_text())
    client = ['mariadb', '--defaults-extra-file=' + str(args.client_config.resolve()),
              '--default-character-set=utf8mb4']
    existing = set(subprocess.check_output(client + ['-N', '-e', 'SHOW DATABASES;'], text=True).splitlines())
    names = {name: args.prefix + name for name in manifest}
    if existing.intersection(names.values()):
        raise SystemExit('Refusing existing databases: ' + ', '.join(sorted(existing.intersection(names.values()))))
    for spec in manifest.values():
        for filename in spec['files']:
            if Path(filename).name != filename:
                raise SystemExit('Invalid snapshot filename')
            if hashlib.sha256((folder / filename).read_bytes()).hexdigest() != spec['sha256'][filename]:
                raise SystemExit('Checksum mismatch: ' + filename)
    for name, spec in manifest.items():
        database = names[name]
        if not re.fullmatch(r'[A-Za-z0-9_]+', database):
            raise SystemExit('Invalid database name')
        subprocess.run(client + ['-e', 'CREATE DATABASE `' + database + '` CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;'], check=True)
        with tempfile.TemporaryFile() as archive, tempfile.TemporaryFile() as sql:
            for filename in spec['files']:
                with (folder / filename).open('rb') as source:
                    shutil.copyfileobj(source, archive)
            archive.seek(0)
            with gzip.GzipFile(fileobj=archive, mode='rb') as source:
                shutil.copyfileobj(source, sql)
            sql.seek(0)
            subprocess.run(client + [database], stdin=sql, check=True)
        print('Imported ' + database, flush=True)
    print('Import complete. Configure local credentials and create your own administrator account.')


if __name__ == '__main__':
    main()
