#!/usr/bin/python3

from os import listdir
from os.path import isfile, join
import pandas as pd
import numpy as np
import argparse

FRONT = 'Front'
MIN = 'min'
MAX = 'max'


def parse_files(path, num_of_objs=2, algorithm_name='', verbose=False):
    """
        Parseamos los ficheros de resultados de METCO
        y nos quedamos con los mejores valores para los
        objetivos alcanzados en cada una de las repeticiones
    """
    data = []
    for file in [join(path, f) for f in listdir(path) if isfile(join(path, f))]:
        file_objs = [np.Inf, np.Inf]
        if verbose:
            print(f'Parsing File: {file}')
        for line in reversed(list(open(file))):
            if FRONT in line:
                break
            elif len(line.split()) > 1:

                objs = list(map(float, line.split()[-num_of_objs:]))
                for idx in range(num_of_objs):
                    if objs[idx] < file_objs[idx]:
                        file_objs[idx] = objs[idx]

        data.append((algorithm_name, *file_objs))

    return data


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('path')
    parser.add_argument('objs', type=int)
    parser.add_argument('filename')
    parser.add_argument('algorithm_name')
    parser.add_argument('-v', '--verbose', action='store_true')
    args = parser.parse_args()
    ranges = parse_files(args.path, args.objs,
                         args.algorithm_name, args.verbose)
    df = pd.DataFrame(ranges, columns=['Algorithm', 'Cost', 'Repetition'])
    df.to_csv(args.filename, index=False)
