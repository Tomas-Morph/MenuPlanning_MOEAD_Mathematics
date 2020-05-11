"""
    Menu Planning Problem Feasibility Checker
    Alejandro Marrero - alu0100825008@ull.edu.es
    ---
    Nutrients Requirements
"""
import pandas as pd
import numpy as np

main_courses = './data/primerosplatos.txt'
second_courses = './data/segundosplatos.txt'
desserts = './data/postres.txt'
USE_COLS = 37
NUTRIENTS = 21
NUTRIENT_NAME = 'nutrient'
DAYS = 'days'
RECOMMENDED_VALUE = 'rec_value'
K_NUTR = 'key_nutrient'
MIN_VALUE = 'min'
MAX_VALUE = 'max'

# Columns for the datasets
COURSE_NAME = 'course_name'

NUM_OF_COURSES = 3

# Min and Max nutrients ranges
K_NUTR_MIN = 0.9
K_NUTR_MAX = 1.10
NUTR_MIN = 0.7
NUTR_MAX = 1.3

# Nutrient Recomendations
nutrient_recommendations = {
    0: {
        NUTRIENT_NAME: 'FOLIC_ACID',
        RECOMMENDED_VALUE: 135
    },
    1: {
        NUTRIENT_NAME: 'CALCIUM',
        RECOMMENDED_VALUE: 585
    },
    2: {
        NUTRIENT_NAME: 'ENERGY',
        RECOMMENDED_VALUE: 1012,
        K_NUTR: True
    },
    3: {
        NUTRIENT_NAME: 'PHOSPHORUS',
        RECOMMENDED_VALUE: 562.5
    },
    4: {
        NUTRIENT_NAME: 'FATS',
        RECOMMENDED_VALUE: 31.72,
        K_NUTR: True
    },
    5: {
        NUTRIENT_NAME: 'IRON',
        RECOMMENDED_VALUE: 8.55
    },
    6: {
        NUTRIENT_NAME: 'MAGNESIUM',
        RECOMMENDED_VALUE: 112.5
    },
    7: {
        NUTRIENT_NAME: 'POTASIUM',
        RECOMMENDED_VALUE: 2025
    },
    8: {
        NUTRIENT_NAME: 'PROTEINS',
        RECOMMENDED_VALUE: 27.08,
        K_NUTR: True
    },
    9: {
        NUTRIENT_NAME: 'SELENIUM',
        RECOMMENDED_VALUE: 25.75
    },
    10: {
        NUTRIENT_NAME: 'SODIUM',
        RECOMMENDED_VALUE: 870
    },
    11: {
        NUTRIENT_NAME: 'VITAMIN_A',
        RECOMMENDED_VALUE: 450
    },
    12: {
        NUTRIENT_NAME: 'VITAMIN_B1',
        RECOMMENDED_VALUE: 0.41
    },
    13: {
        NUTRIENT_NAME: 'VITAMIN_B2',
        RECOMMENDED_VALUE: 0.63
    },
    14: {
        NUTRIENT_NAME: 'VITAMIN_B6',
        RECOMMENDED_VALUE: 0.54
    },
    15: {
        NUTRIENT_NAME: 'VITAMIN_B12',
        RECOMMENDED_VALUE: 2.28
    },
    16: {
        NUTRIENT_NAME: 'VITAMIN_C',
        RECOMMENDED_VALUE: 27
    },
    17: {
        NUTRIENT_NAME: 'VITAMIN_D',
        RECOMMENDED_VALUE: 4.65
    },
    18: {
        NUTRIENT_NAME: 'VITAMIN_E',
        RECOMMENDED_VALUE: 6.3
    },
    19: {
        NUTRIENT_NAME: 'IODINE',
        RECOMMENDED_VALUE: 67.5
    },
    20: {
        NUTRIENT_NAME: 'ZINC',
        RECOMMENDED_VALUE: 6.75
    }
}


def compute_requirements(days: int = 10, verbose: bool = False):
    """
        Método encargado de calcular las restricciones.
        Calcula los valores mínimos y máximos para cada uno de los 21 nutrientes
    """
    if verbose:
        print(f'Computing nutrients requirements for {days} days')
    for nutri_key in nutrient_recommendations:
        nutrient_recommendations[nutri_key][MIN_VALUE] = \
            nutrient_recommendations[nutri_key][
                RECOMMENDED_VALUE] * days * (K_NUTR_MIN if K_NUTR in
                                                           nutrient_recommendations[
                                                               nutri_key] else NUTR_MIN)

        nutrient_recommendations[nutri_key][MAX_VALUE] = \
            nutrient_recommendations[nutri_key][RECOMMENDED_VALUE] * days * (
                K_NUTR_MAX if K_NUTR in nutrient_recommendations[
                    nutri_key] else NUTR_MAX)
        if verbose:
            print(f'{nutrient_recommendations[nutri_key]}')

    return nutrient_recommendations


def read_database(verbose: bool = False):
    """
        Método que lee los ficheros de la DB que contienen la informacion sobre los platos
    :param verbose:
    :return:
    """
    if verbose:
        print(f'Loading main courses')
    main_courses_df = read_db_file(main_courses, verbose)
    if verbose:
        print(f'Loading second courses')
    second_courses_df = read_db_file(second_courses, verbose)
    if verbose:
        print(f'Loading desserts')
    desserts_df = read_db_file(desserts, verbose)
    return main_courses_df, second_courses_df, desserts_df


def read_db_file(path_to_file: str, verbose: bool = False):
    """
        Método que realiza la lectura y el preprocesamiento de los datos
        de un fichero de la DB y prescinde de los datos no necesarios
    :param path_to_file:
    :param verbose:
    :return:
    """
    df = pd.read_csv(path_to_file, delimiter=',', header=None,
                     names=list(range(USE_COLS)),
                     error_bad_lines=False, warn_bad_lines=False)
    df.drop(df.iloc[:, 1:15], axis=1, inplace=True)
    df = df.iloc[:, :-1]
    cols = [[COURSE_NAME] + [i for i in range(NUTRIENTS)]]
    df.columns = cols
    df.rename(columns={df.columns[0]: COURSE_NAME}, inplace=True)
    if verbose:
        print(df.head())
    return df
