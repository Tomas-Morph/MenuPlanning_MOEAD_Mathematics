"""
    Menu Planning Problem Feasibility Checker
    Alejandro Marrero - alu0100825008@ull.edu.es
    ---
    Nutrients Requirements
"""

from mpp_feasibility.mpp_data import *


def compute_feasibility(individual: list = None, requirements: dict = None,
                        num_of_days: int = None,
                        main_df: pd.DataFrame = None,
                        second_df: pd.DataFrame = None,
                        dess_df: pd.DataFrame = None,
                        verbose: bool = False):
    """
        Método empleado para calcular la factibilidad de un individuo
        resultante de la ejecución del MPP con METCO
    :param num_of_days:
    :param dess_df:
    :param main_df:
    :param second_df:
    :param requirements:
    :param individual:
    :param verbose:
    :return:
    """
    if individual is None:
        raise RuntimeError('Individual is None!')

    individual_feasibility = np.zeros(NUTRIENTS)
    verbose_info = []

    for n_idx in range(NUTRIENTS):
        nutr_feas = 0.0
        for d_idx in range(num_of_days):
            # Extraemos los indices de los platos de cada dia
            main_idx = individual[d_idx * NUM_OF_COURSES]
            second_idx = individual[d_idx * NUM_OF_COURSES + 1]
            dess_idx = individual[d_idx * NUM_OF_COURSES + 2]

            if verbose and len(verbose_info) < num_of_days:
                verbose_info.append(
                    [
                        main_df.iloc[main_idx][COURSE_NAME].iloc[0],
                        second_df.iloc[second_idx][COURSE_NAME].iloc[0],
                        dess_df.iloc[dess_idx][COURSE_NAME].iloc[0]
                    ])

            # Buscamos los valores de los nutrientes
            nutr_feas += main_df.iloc[main_idx][n_idx]
            nutr_feas += second_df.iloc[second_idx][n_idx]
            nutr_feas += dess_df.iloc[dess_idx][n_idx]
            nutr_feas = nutr_feas.iloc[0]

            if nutr_feas < requirements[n_idx][MIN_VALUE]:
                nutr_feas = nutr_feas - requirements[n_idx][MIN_VALUE]

            elif nutr_feas > requirements[n_idx][MAX_VALUE]:
                nutr_feas = nutr_feas - requirements[n_idx][
                    MAX_VALUE]

        individual_feasibility[n_idx] = nutr_feas

    if verbose:
        print(f'Individual: {individual}')
        print(f'- Nutrients feasibilities:\n{individual_feasibility}')
        for day in range(num_of_days):
            print(f'Day #{day}/{num_of_days}')
            print(f'- Main course: {verbose_info[day][0]}')
            print(f'- Second course: {verbose_info[day][1]}')
            print(f'- Dessert: {verbose_info[day][2]}')


    return individual_feasibility
