#!/usr/bin/env python3
"""Prepare an offline, backed-up cleanup from the reviewed 2026-09-25 ID list.
Does not execute the generated apply.sql. Uses existing private client configs.
"""
import csv
import hashlib
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'sql/backup/test_item_cleanup_20260925/final'
WORLD, CHAR = 'mop_world_test', 'mop_characters_test'
IDS = f'SELECT entry FROM {WORLD}._cleanup_test_item_ids_20260925'
GUIDS = f'SELECT guid FROM {CHAR}._cleanup_test_item_guids_20260925'
CLIENT = '/tmp/test-item-world.cnf'

def query(sql):
    return subprocess.run(['mariadb', '--defaults-extra-file=' + CLIENT, '-N', '-B'],
                          input=sql, text=True, check=True, capture_output=True).stdout

def banned(col): return f'`{col}` IN ({IDS})'
def copies(col): return f'`{col}` IN ({GUIDS})'

def stopped():
    state = subprocess.run(['systemctl','show','mop-world.service','-p','ActiveState','--value'],
                           check=True, capture_output=True,text=True).stdout.strip()
    if state != 'inactive': raise RuntimeError('worldserver must be inactive: ' + state)

stopped()
if (OUT / 'backups.json').exists():
    raise RuntimeError('A verified backup already exists; never overwrite it')
assert query(f'SELECT COUNT(*) FROM {WORLD}._cleanup_test_item_ids_20260925;').strip() == '3517'
assert query(f'SELECT COUNT(*) FROM {CHAR}.character_inventory WHERE {copies("bag")} AND NOT ({copies("item")});').strip() == '0', 'Non-target items inside banned bags require relocation'
schema = {}
for row in csv.DictReader(open('/tmp/test-item-schema.tsv'), delimiter='\t'):
    schema.setdefault((row['table_schema'],row['table_name']),set()).add(row['column_name'])
plan=[]
def add(db, table, where, update=None):
    if (db,table) not in schema: return
    full=f'`{db}`.`{table}`'
    n=int(query(f'SELECT COUNT(*) FROM {full} WHERE {where};').strip())
    if n:
        plan.append(dict(database=db,table=table,where=where,count=n,
                         sql=(f'UPDATE {full} SET {update}' if update else f'DELETE FROM {full}')+' WHERE '+where+';'))
        print(db+'.'+table,n,flush=True)

# Child references first; preserve unrelated mail text/money and auction bids.
auction_ids=f'SELECT id FROM {CHAR}.auctionhouse WHERE {copies("itemguid")}'
add(CHAR,'auctionbidders',f'id IN ({auction_ids})')
add(CHAR,'mail',f'id IN (SELECT mail_id FROM {CHAR}.mail_items WHERE {copies("item_guid")}) AND id NOT IN (SELECT mail_id FROM {CHAR}.mail_items WHERE NOT ({copies("item_guid")}))','has_items=0, cod=0')
for table in ('character_aura','pet_aura'):
    # Cast item GUIDs may include their high-GUID type bits.
    pred=f'(`item_guid` & 4294967295) IN ({GUIDS})'
    add(CHAR,table+'_effect',f'(guid,slot) IN (SELECT guid,slot FROM {CHAR}.{table} WHERE {pred})')
    add(CHAR,table,pred)
for table,col in [('auctionhouse','itemguid'),('character_inventory','item'),('guild_bank_item','item_guid'),('mail_items','item_guid'),('item_refund_instance','item_guid'),('item_soulbound_trade_data','itemGuid'),('item_loot_money','container_id')]:
    add(CHAR,table,copies(col))
add(CHAR,'item_loot_items',copies('container_id')+' OR '+banned('item_id'))
add(CHAR,'character_gifts',copies('item_guid')+' OR '+banned('entry'))
add(CHAR,'character_void_storage',banned('itemEntry'))
add(CHAR,'item_deleted',banned('item_entry')+' OR '+copies('old_item_guid'))
add(CHAR,'solo_arena_loadout_backup',' OR '.join([banned('original_item_entry'),banned('temporary_item_entry'),copies('original_item_guid'),copies('temporary_item_guid')]))
for table,col in [('account_transmog_appearances','itemEntry'),('custom_transmog_collection','ItemEntry'),('custom_transmogrification','FakeEntry'),('fake_items','fakeEntry')]:
    add(CHAR,table,banned(col))
cols=['item'+str(i) for i in range(19)]
add(CHAR,'character_equipmentsets',' OR '.join(copies(c) for c in cols),', '.join(f'`{c}`=IF({copies(c)},0,`{c}`)' for c in cols))
# Serialized transmog sets are presets only; no physical items are removed here.
entries=[r['entry'] for r in json.loads((ROOT/'contrib/test_item_cleanup/manifest.json').read_text())]
pattern='(^|[^0-9])('+'|'.join(entries)+')([^0-9]|$)'
add(CHAR,'custom_transmogrification_sets',"SetData REGEXP '"+pattern+"'")
# Appearance-only override on otherwise legitimate item instances.
add(CHAR,'item_instance',banned('transmogrifyId')+' AND NOT ('+copies('guid')+')','transmogrifyId=0')
add(CHAR,'item_instance',copies('guid'))
# Templates and sources: remove sold/looted copies and item-specific metadata.
for (db,table),columns in sorted(schema.items()):
    if db==WORLD and table.endswith('_loot_template') and not table.startswith('_') and 'item' in columns:
        where=banned('item')
        if table=='item_loot_template': where+=' OR '+banned('entry')
        add(WORLD,table,where)
# Retire item-only shop offers that would otherwise charge for an empty product.
# Service products (for example Promotion) remain services after their test
# placeholder item is removed.
empty_products = f"SELECT p.id FROM {WORLD}.battle_pay_product p WHERE p.type=0 AND EXISTS (SELECT 1 FROM {WORLD}.battle_pay_product_items i WHERE i.productId=p.id AND i.itemId IN ({IDS})) AND NOT EXISTS (SELECT 1 FROM {WORLD}.battle_pay_product_items i WHERE i.productId=p.id AND i.itemId NOT IN ({IDS}))"
add(WORLD,'battle_pay_entry',f'productId IN ({empty_products})')
add(WORLD,'battle_pay_product',f'id IN (SELECT id FROM ({empty_products}) retired_products)')
for table,col in [('npc_vendor','item'),('game_event_npc_vendor','item'),('playercreateinfo_item','itemid'),('battle_pay_boost_items','itemId'),('battle_pay_product_items','itemId'),('item_template_addon','Id'),('item_template_locale','ID'),('item_script_names','Id'),('battle_pet_item_to_species','itemId')]:
    # Vendor type 2 is a currency ID, not an item ID.
    suffix=' AND type=1' if table in ('npc_vendor','game_event_npc_vendor') else ''
    add(WORLD,table,banned(col)+suffix)
add(CHAR,'blackmarket_auctions',f'templateId IN (SELECT id FROM {WORLD}.blackmarket_template WHERE {banned("ItemEntry")})')
add(WORLD,'blackmarket_template',banned('ItemEntry'))
# Clear only the item references within shared NPC/quest rows.
for table,cols in [
 ('creature_equip_template',['ItemID1','ItemID2','ItemID3']),
 ('creature_template',['questItem'+str(i) for i in range(1,7)]),
 ('gameobject_template',['questItem'+str(i) for i in range(1,7)]),
 ('quest_template',['StartItem']+['RewardItem'+str(i) for i in range(1,5)]+['ItemDrop'+str(i) for i in range(1,5)]+['RewardChoiceItemID'+str(i) for i in range(1,7)]),
 ('achievement_reward',['item'])]:
    cols=[c for c in cols if c in schema.get((WORLD,table),set())]
    if cols: add(WORLD,table,' OR '.join(banned(c) for c in cols),', '.join(f'`{c}`=IF({banned(c)},0,`{c}`)' for c in cols))
add(WORLD,'item_template',banned('entry'))

# No accepted bids on targeted listings: deleting them cannot swallow escrow.
assert query(f'SELECT COUNT(*) FROM {CHAR}.auctionhouse WHERE {copies("itemguid")} AND buyguid>0 AND lastbid>0;').strip()=='0', 'Bid refunds needed before applying'
OUT.mkdir(parents=True,exist_ok=True)
# Back up each predicate before ANY mutation. Separate dumps restore updated rows too.
backup=[]
for index,p in enumerate(plan):
    name=f'{index:02d}_{p["database"]}_{p["table"]}.sql'
    path=OUT/name
    with path.open('wb') as f:
        f.write(('USE `'+p['database']+'`;\n').encode())
        subprocess.run(['mariadb-dump','--defaults-extra-file='+CLIENT,
                        '--no-create-info','--skip-triggers','--skip-lock-tables',
                        '--complete-insert','--skip-extended-insert','--hex-blob','--replace',
                        '--where='+p['where'],p['database'],p['table']],stdout=f,check=True)
    data=path.read_bytes()
    actual=sum(line.startswith(b'REPLACE INTO ') for line in data.splitlines())
    assert actual==p['count'], (name,actual,p['count'])
    backup.append(dict(file=name,rows=actual,sha256=hashlib.sha256(data).hexdigest()))
(OUT/'backups.json').write_text(json.dumps(backup,indent=2)+'\n')
(OUT/'plan.json').write_text(json.dumps(plan,indent=2)+'\n')
# SQL deliberately has no client --force; errors close the connection/rollback.
(OUT/'apply.sql').write_text('START TRANSACTION;\n'+'\n'.join(p['sql'] for p in plan)+'\nCOMMIT;\n')
(OUT/'restore.sql').write_text('START TRANSACTION;\n'+'\n'.join('SOURCE '+str(OUT/p['file'])+';' for p in reversed(backup))+'\nCOMMIT;\n')
stopped()
print('Prepared and verified backups for',sum(p['count'] for p in plan),'rows; apply.sql NOT executed',flush=True)
