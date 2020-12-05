lines = []
with open(input('Input file: '), 'r') as f:
    lines = [x.split(' ') for x in f.read().split('\n')[:-1]]

vs, es = [], []
for line in lines:
    if line[0] == 'v':
        vs.append(line[1:])
    elif line[0] == 'f':
        f = [x.split('/')[0] for x in line[1:]]
        for i in range(1, len(f)):
            es.append([int(f[i - 1]) - 1, int(f[i]) - 1])
        es.append([int(f[len(f) - 1]) - 1, int(f[0]) - 1])

with open(input('Output file: '), 'w') as f:
    f.write('%d %d\n' % (len(vs), len(es)))
    for v in vs:
        f.write('%s %s %s\n' % (v[0], v[1], v[2]))
    for e in es:
        f.write('%d %d\n' % (e[0], e[1]))
