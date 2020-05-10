"""
    Menu Planning Problem Feasibility Checker
    Alejandro Marrero - alu0100825008@ull.edu.es
"""

from mpp_feasibility.nutrients_req import *
import numpy as np


def compute_feasibility(individual, days, main_courses, second_courses,
                        desserts,
                        verbose=False):
    individual_feasibility = np.zeros(NUTRIENTS)
    verbose_info = []

    for n_idx in range(NUTRIENTS):
        nutrient_feasibility = 0.0
        for d_idx in range(days):
            # Extraemos los indices de los platos de cada dia
            main_course_idx = individual[d_idx * NUM_OF_COURSES]
            second_courses_idx = individual[d_idx * NUM_OF_COURSES + 1]
            dessert_idx = individual[d_idx * NUM_OF_COURSES + 2]
            if verbose:
                verbose_info.append(
                    [main_courses[main_course_idx][COURSE_NAME],
                     second_courses[second_courses_idx][COURSE_NAME],
                     desserts[dessert_idx][COURSE_NAME]])
            # Buscamos los valores de los nutrientes
            nutrient_feasibility += main_courses[main_course_idx][n_idx]
            nutrient_feasibility += second_courses[second_courses_idx][n_idx]
            nutrient_feasibility += desserts[dessert_idx][n_idx]
        if nutrient_feasibility < nutrient_recommendations[n_idx][MIN_VALUE]:
            nutrient_feasibility = nutrient_feasibility - \
                                   nutrient_recommendations[n_idx][MIN_VALUE]
        elif nutrient_feasibility > nutrient_recommendations[n_idx][MAX_VALUE]:
            nutrient_feasibility = nutrient_feasibility - \
                                   nutrient_recommendations[n_idx][MAX_VALUE]

        individual_feasibility[n_idx] = nutrient_feasibility

    if verbose:
        print(f'Individual:\n')
        for d_idx in range(days):
            print(f'Day #{d_idx}/{days}')
            print(f'- Main course: {verbose_info[d_idx][0]}')
            print(f'- Second course: {[verbose_info][d_idx][1]}')
            print(f'- Dessert: {verbose_info[d_idx][2]}')
            print(f'- Nutrients feasibilities:\n{individual_feasibility}')

    return individual_feasibility
