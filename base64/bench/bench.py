#!/bin/env python3
import argparse
import csv
import functools
import io
import itertools
import os
import subprocess
import sys

import matplotlib
import matplotlib.pyplot

SIZES = ','.join(
    ','.join((str(2 ** x), f'2^{x}'))
    for x in range(20, 31, 5))

def main(*args):
    args = parse_args(args)
    out = sys.stdout
    if out.isatty():
        print('refusing to write SVG data to a TTY', file=sys.stderr)
        return 1
    encode_data = list(exec_test(args, ('base64_encode', 'rfc_encode')))
    decode_data = list(exec_test(args, ('base64_decode', 'rfc_decode')))
    exec_data = list(flatten(
        exec_test(args, (n,), env={'PATH': os.path.join(*p)})
        for n, p in (
            ('system_exec', (os.curdir, 'base64', 'bench', 'system')),
            ('base64_exec', (args.build_dir, 'base64')),
            ('rfc_exec', (args.build_dir, 'base64', 'rfc')))))
    matplotlib.use('svg')
    matplotlib.pyplot.rcParams['figure.autolayout'] = True
    fig = matplotlib.pyplot.figure(figsize=(8, 8))
    plot_tests('encode', encode_data, '_encode', 2, 0)
    plot_tests('decode', decode_data, '_decode', 2, 1)
    plot_tests('exec', exec_data, '_exec', 3, 2)
    matplotlib.pyplot.savefig(out)

def parse_args(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument('build_dir', type=str)
    parser.add_argument('-v', '--verbose', action='count', default=0)
    ret = parser.parse_args(argv)
    if ret.verbose:
        ret.log = lambda *x: print(*x, file=sys.stderr)
    else:
        ret.log = lambda *x: None
    return ret

def exec_test(args, names, env=None):
    env = dict(env or {})
    env['CODEX_BENCHMARK_SIZES'] = SIZES
    bench = os.path.join(args.build_dir, 'base64', 'bench', 'bench')
    args.log('executing:', *names)
    out = subprocess.check_output(
        (bench, '-iterations', '8', '-csv', *names), env=env)
    return csv.reader(out.decode('utf-8').splitlines())

def size(x): return x[1]
def label(x): return ' '.join((x[0], size(x)))
def duration(x): return float(x[3])

def trimmed_test(suffix, x):
    return remove_suffix(suffix, x[0])
def trimmed_label(suffix, x):
    return ' '.join((remove_suffix(suffix, x[0]), size(x)))
def remove_suffix(suffix, s):
    return s[:-len(suffix)] if s.endswith(suffix) else s

def zip_n(l, n):
    assert(not (len(l) % n))
    size = len(l) // n
    return zip(*(
        itertools.islice(l, i * size, (i + 1) * size)
        for i in range(n)))

def flatten(l):
    return (y for x in l for y in x)

def plot_tests(title, data, suffix, n, row):
    for i, l in enumerate(zip_n(data, n)):
        ax = matplotlib.pyplot.subplot2grid((3, 3), (row, i))
        if not i:
            ax.set_ylabel(title)
        ax.set_title(size(l[0]))
        ax.bar(
            list(map(functools.partial(trimmed_test, suffix), l)),
            list(map(duration, l)))

if __name__ == '__main__':
    sys.exit(main(*sys.argv[1:]))
