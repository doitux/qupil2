"""Reads ts/qupil_de.ts of Qupil 1.4.1 and writes raw.json (all messages) into the current directory.

Usage: python3 -I extract.py <path to 1.4.1 ts/qupil_de.ts>
"""
import xml.etree.ElementTree as ET, json, sys, collections
p = sys.argv[1]
r = ET.parse(p).getroot()
rows = []
for ctx in r.findall('context'):
    cname = ctx.find('name').text
    for m in ctx.findall('message'):
        src = m.find('source').text or ''
        tr = m.find('translation')
        t = tr.text or ''
        typ = tr.get('type','')
        locs = [ (l.get('filename'), l.get('line')) for l in m.findall('location')]
        rows.append(dict(ctx=cname, src=src, de=t, type=typ, numerus=m.get('numerus'), locs=locs))
print(len(rows), collections.Counter(r['type'] for r in rows))
json.dump(rows, open('raw.json','w'), ensure_ascii=False, indent=1)
# unique by (src,de)
u = collections.OrderedDict()
for r_ in rows:
    if r_['type'] in ('obsolete','vanished'): continue
    k=(r_['src'],r_['de'])
    u.setdefault(k, []).append(r_['ctx'])
print('unique pairs', len(u))
srcs = collections.defaultdict(set)
for (s,d) in u: srcs[s].add(d)
conf = {s:d for s,d in srcs.items() if len(d)>1}
print('conflicting src', len(conf))
for s,d in conf.items(): print(repr(s[:60]), '=>', [x[:50] for x in d])
