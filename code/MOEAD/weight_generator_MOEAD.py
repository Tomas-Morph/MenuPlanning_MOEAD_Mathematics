#!/usr/bin/python3

import argparse
import sys
from itertools import product
from functools import reduce
from scipy.special import comb


# Function that calculates a combinatorial number
def combinatorial_number(n, r):
    if type(n) is not int or type(r) is not int:
        print("Function combinatorial_number: wrong type of arguments")
        sys.exit(-1)
    r = min(r, n - r)
    return comb(n, r, exact=True)


# Function that obtains the list of weights given a parameter H and
# the number of objective functions to be considered
def get_weight_list(param_h, n_objectives):
    if type(param_h) not in (int, ) or type(n_objectives) not in (int, ):
        print("Function getListWeights: wrong type of arguments")
        sys.exit(-1)

    weights = [(h_value / param_h) for h_value in range(0, param_h)]
    return weights


# Function that returns the sum of a list
def check_vector(weights):
    if not all(isinstance(n, (float, int)) for n in weights):
        print("Function check_vector: wrong type of arguments")
        sys.exit(-1)
    return reduce((lambda x, y: x + y), weights)


# Function that returns a list of weight vectors given a weight list and
# the number of objective functions
def get_weight_vector_list(weights, n_objectives):
    if not all(isinstance(n, (float, int)) for n in weights) or type(n_objectives) is not int:
        print("Function get_weight_vector_list: wrong type of arguments")
        sys.exit(-1)

    product_list = product(weights, repeat=n_objectives)
    weight_list = []
    for prod in product_list:
        weight_vector = [weight for weight in prod]
        if check_vector(weight_vector) == 1.0:
            weight_list.append(weight_vector)

    return weight_list


# Functions that writes the weight vector list into a file
def write_to_file(weight_list, output_filename):
    with open(output_filename, 'w') as file:
        file.write(f"{len(weight_list)}\n")
        for weight_vector in weight_list:
            for weight in weight_vector:
                file.write(f'{weight} ')
            file.write('\n')


def generate_weights(h, num_objectives, output_filename):
    param_n = combinatorial_number(h - num_objectives - 1, num_objectives - 1)
    weights = get_weight_list(h, num_objectives)
    list_of_vector_weights = get_weight_vector_list(
        weights, num_objectives)
    #print(f"List of weight vectors ({len(list_of_vector_weights)}): {list_of_vector_weights}")
    write_to_file(list_of_vector_weights, output_filename)


# Main program
if __name__ == "__main__":
    # Definimos los argumentos necesarios para la ejecucion del programa
    parser = argparse.ArgumentParser('MOEA/D Weight Vector Generator')
    parser.add_argument('H', type=int)
    parser.add_argument('objectives', type=int)
    parser.add_argument('filename', type=str)
    # Obtenemos los valores de los argumentos
    args = parser.parse_args()
    h = args.H
    num_objectives = args.objectives
    output_filename = args.filename
    generate_weights(h, num_objectives, output_filename)