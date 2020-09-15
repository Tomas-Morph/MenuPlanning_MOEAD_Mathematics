#!/usr/bin/python3

from os import listdir
from os.path import isfile, join
import pandas as pd
import numpy as np
import argparse
import re

FRONT = 'Front'
EVAL_RE = r"Current Evaluation = (\d+)"
FRONT_RE = r"Front Size = (\d+)\n"


def get_mean_distance(individuals, days):
    """
        Método empleado para calcular la distancia media
        por frente en cada checkpoint de las ejecuciones
    """
    sum_distance = 0.0
    for ind_ix in range(len(individuals)):
        sum_distance += get_distance_to_closest(ind_ix, individuals, days)

    return sum_distance / len(individuals)


def get_distance_to_closest(ind_ix, individuals, days):
    """
        Método para calcular la distancia al vecino más cercano
    """
    if len(individuals) == 1:
        return 0

    min_distance = np.Inf
    for other_ix in range(len(individuals)):
        if ind_ix == other_ix:
            continue
        distance = compute_distance(
            individuals[ind_ix], individuals[other_ix], days)
        if distance < min_distance:
            min_distance = distance

    return min_distance


def compute_distance(ind_1, ind_2, days):
    """
        Metodo que calcula la distancia
        de un individuo resultado de la
        ejecución del problema MenuPlanning en METCO
    """
    distance = 0
    foods = {}
    # Evaluamos el primer individuo
    for day_ix in range(days):
        day_food = (
            ind_1[day_ix * 3],
            ind_1[day_ix * 3 + 1],
            ind_1[day_ix * 3 + 2]
        )
        if day_food in foods:
            foods[day_food] += 1
        else:
            foods[day_food] = 1
    # Evaluamos el segundo
    for day_ix in range(days):
        day_food = (
            ind_2[day_ix * 3],
            ind_2[day_ix * 3 + 1],
            ind_2[day_ix * 3 + 2]
        )
        # Si esta le restamos una unidad
        if day_food in foods:
            foods[day_food] -= 1
            if foods[day_food] == 0:
                foods.pop(day_food)
        else:
            distance += 1

    return distance


def parse_files(path, num_of_objs=2, stepsize=10, final_step=3600, days=20, verbose=False):
    """
        Parseamos los ficheros de resultados de METCO
        y nos quedamos con los mejores valores para los
        objetivos alcanzados en cada una de las repeticiones
    """
    average_distance = {}
    for file in [join(path, f) for f in listdir(path) if isfile(join(path, f))]:
        if verbose:
            print(f'Parsing file: {file}')
        current_step = stepsize

        lines = []
        with open(file) as f:
            lines = f.readlines()
        line_idx = 0
        while line_idx < len(lines):
            result = re.match(EVAL_RE, lines[line_idx])
            checkpoint = int(result.group(1))
            if checkpoint == current_step and checkpoint <= final_step:
                if current_step not in average_distance:
                    average_distance[current_step] = 0

                line_idx += 1
                # Parseamos el frente
                result = re.match(FRONT_RE, lines[line_idx])
                front_size = int(result.group(1))
                individuals = []
                for _ in range(front_size):
                    line_idx += 1
                    individuals.append(list(
                        map(float, lines[line_idx].split()[:-num_of_objs])))

                # Tenemos que saltar el retorno de carro
                line_idx += 2
                # Calculamos la distancia y actualizamos el step
                average_distance[current_step] += get_mean_distance(
                    individuals, days)

                if verbose:
                    print(f'{current_step} {average_distance[current_step]}')
                current_step += stepsize
                if current_step > final_step:
                    break

    return average_distance


def results_to_file(filename, average_distance, repetitions):
    with open(filename, 'w') as file:
        for checkpoint in average_distance:
            distance = average_distance[checkpoint] / repetitions
            file.write(f'{checkpoint}, {distance}\n')


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('path')
    parser.add_argument('objs', type=int)
    parser.add_argument('filename')
    parser.add_argument('repetitions', type=int)
    parser.add_argument('stepsize', type=int)
    parser.add_argument('final_step', type=int)
    parser.add_argument('days', type=int)
    parser.add_argument('-v', '--verbose', action='store_true')
    args = parser.parse_args()
    average_distance = parse_files(args.path, args.objs,
                                   args.stepsize, args.final_step, args.days, args.verbose)
    results_to_file(args.filename, average_distance, args.repetitions)
