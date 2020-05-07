#!/usr/bin/python3

from os import listdir
from os.path import isfile, join
import numpy as np
import argparse

FRONT = 'Front'
MIN = 'min'
MAX = 'max'


def parse_files(path, num_of_objs=2, verbose=False):
    objs_range = [{MIN: np.inf,
                   MAX: -np.inf
                   } for _ in range(num_of_objs)]
    # Buscamos todos los ficheros y los parseamos
    for file in [join(path, f) for f in listdir(path) if isfile(join(path, f))]:
        if verbose:
            print(f'Parsing File: {file}')
        for line in reversed(list(open(file))):
            if FRONT in line:
                break
            elif len(line) > 1:
                objs = list(map(float, line.split()[-num_of_objs:]))
                for idx in range(num_of_objs):
                    if objs[idx] < objs_range[idx][MIN]:
                        objs_range[idx][MIN] = objs[idx]
                        if verbose:
                            print(f'New {idx} {MIN} in file: {file}')
                    if objs[idx] > objs_range[idx][MAX]:
                        objs_range[idx][MAX] = objs[idx]
                        if verbose:
                            print(f'New {idx} {MAX} in file: {file}')

    if verbose:
        print(f'Ranges are: {objs_range}')
    return objs_range

def write_results_to_file(filename, objs_range):
    with(open(filename, 'w')) as file:
       for objective in objs_range:
           file.write(f'{objective[MIN]} {objective[MAX]}\n')

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('path')
    parser.add_argument('objs', type=int)
    parser.add_argument('filename')
    parser.add_argument('-v', '--verbose', action='store_true')
    args = parser.parse_args()
    ranges = parse_files(args.path, args.objs, args.verbose)
    write_results_to_file(args.filename, ranges)
