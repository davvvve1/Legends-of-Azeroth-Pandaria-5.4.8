#!/usr/bin/env python3
"""Read-only, allowlisted installation export. Never modifies the source DBs."""
import argparse
import gzip
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--config', required=True, type=Path)
    parser.add_argument('--output', required=True, type=Path)
    args = parser.parse_args()
    policy = json.loads(Path(__file__).with_name('tables.json').read_text())
    config = args.config.read_text()
    args.output.mkdir(parents=True, exist_ok=True)
    report = {}
    for name, spec in policy.items():
        match = re.search(r'^' + spec['config'] + r'\s*=\s*"([^"]+)"', config, re.M)
        if not match:
            raise RuntimeError('Missing configuration: ' + spec['config'])
        host, port, user, password, database = match.group(1).split(';')
        for identifier in [database] + spec['schema']:
            if not re.fullmatch(r'[A-Za-z0-9_]+', identifier):
                raise RuntimeError('Invalid SQL identifier')
        with tempfile.TemporaryDirectory(prefix='clean-db-export-') as temporary:
            cnf = Path(temporary) / 'client.cnf'
            def quote(value):
                return '"' + value.replace('\\', '\\\\').replace('"', '\\"') + '"'
            cnf.write_text('[client]\nhost=' + quote(host) + '\nport=' + port +
                           '\nuser=' + quote(user) + '\npassword=' + quote(password) + '\n')
            os.chmod(cnf, 0o600)
            base = ['mariadb-dump', '--defaults-extra-file=' + str(cnf),
                    '--single-transaction', '--skip-lock-tables', '--skip-add-locks',
                    '--skip-comments', '--skip-triggers', '--hex-blob',
                    '--default-character-set=utf8mb4']
            raw = Path(temporary) / 'export.sql'
            with raw.open('wb') as output:
                subprocess.run(base + ['--no-data', database] + spec['schema'], stdout=output, check=True)
                if spec['data']:
                    subprocess.run(base + ['--no-create-info', database] + spec['data'], stdout=output, check=True)
            destination = args.output / (name + '.sql.gz')
            with raw.open('rb') as source, destination.open('wb') as compressed:
                with gzip.GzipFile(filename='', fileobj=compressed, mode='wb', mtime=0, compresslevel=6) as output:
                    for line in source:
                        # Runtime sequence counters reveal activity and are not seeds.
                        line = re.sub(rb' AUTO_INCREMENT=\d+', b'', line)
                        if re.match(rb'\s*(?:CREATE\s+USER\b|GRANT\s|/\*!\d+\s+DEFINER=)', line, re.I):
                            raise RuntimeError('Unexpected privileged statement')
                        output.write(line)
                    if name == 'auth':
                        output.write(b"\nINSERT INTO `realmlist` (`id`,`name`,`address`,`localAddress`,`port`,`icon`,`flag`,`timezone`,`allowedSecurityLevel`,`population`,`gamebuild`) VALUES (1,'Legends of Azeroth','127.0.0.1','127.0.0.1',8085,0,0,1,0,0,18414);\n")
            files = [destination]
            if destination.stat().st_size >= 48 * 1024 * 1024:
                files = []
                with destination.open('rb') as source:
                    while chunk := source.read(48 * 1024 * 1024):
                        part = destination.with_name(destination.name + '.%03d' % (len(files) + 1))
                        part.write_bytes(chunk)
                        files.append(part)
                destination.unlink()
            report[name] = {'files': [path.name for path in files],
                            'schema_tables': len(spec['schema']),
                            'data_tables': spec['data'],
                            'sha256': {path.name: hashlib.sha256(path.read_bytes()).hexdigest() for path in files}}
            print(name + ': exported allowlisted schema/data')
    (args.output / 'manifest.json').write_text(json.dumps(report, indent=2) + '\n')


if __name__ == '__main__':
    main()
