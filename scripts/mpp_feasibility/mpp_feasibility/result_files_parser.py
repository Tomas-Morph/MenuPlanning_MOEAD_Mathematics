"""
    Menu Planning Problem Feasibility Checker
    Alejandro Marrero - alu0100825008@ull.edu.es
"""

from os import listdir
from os.path import isfile, join, basename
from typing import List
from pprint import pprint

FRONT = 'Front'
FILE = 'file'
NUM_OF_OBJS = 2


def file_list(path_to_files: str = None) -> List:
    """
        Función para recolectar todos los ficheros que hay en el directorio
        que nos pasan como argumento
    :param path_to_files:
    :return:
    """
    return [join(path_to_files, f) for f in listdir(path_to_files) if
            isfile(join(path_to_files, f))]


def parse_file(file: str = None, verbose: bool = False) -> List:
    """
        Función para leer un único fichero de resultado.
        Creado con vistas a una posible paralelizacion
    :param file:
    :param verbose:
    :return:
    """
    individuals = []
    for line in reversed(list(open(file))):
        if FRONT in line:
            break
        elif len(line) > 1:
            individual = list(map(int, line.split()[:-NUM_OF_OBJS]))
            individuals.append(individual)
    if verbose:
        print(f'Parsing File: {file}')
        pprint(f'- {individuals}')

    return individuals


def parse_all_files(list_of_files: List = [str],
                    verbose: bool = False) -> dict:
    """
        Función que lee todos los ficheros de resultados recolectados
        y crea un diccionario configuracion/individuos para posteriormente
        calcular la factibilidad
    :param list_of_files:
    :param verbose:
    :return:
    """
    individuals = {}
    for file in list_of_files:
        results = []
        for line in reversed(list(open(file))):
            if FRONT in line:
                break
            elif len(line) > 1:
                individual = list(map(int, line.split()[:-NUM_OF_OBJS]))
                results.append(individual)
        if verbose:
            print(f'Parsing File: {file}')
        individuals[basename(file)] = results

    return individuals
