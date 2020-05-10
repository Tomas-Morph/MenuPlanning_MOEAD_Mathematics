"""
    Menu Planning Problem Feasibility Checker
    Alejandro Marrero - alu0100825008@ull.edu.es
"""

import pandas as pd
from mpp_feasibility.nutrients_req import COURSE_NAME

# Main files
main_courses = './data/primerosplatos.txt'
second_courses = './data/segundosplatos.txt'
desserts = './data/postres.txt'


def read_file(path_to_file, verbose=False):
    df = pd.read_csv(path_to_file, delimiter=',', header=None,
                     error_bad_lines=False)
    df.drop(df.iloc[:, 1:15], axis=1, inplace=True)
    df.columns = [i for i in range(0, len(df.columns))]
    df.rename(columns={df.columns[0]: COURSE_NAME}, inplace=True)
    df = df.iloc[:, :-1]
    if verbose:
        print(df.head())
    return df


def create_datasets(verbose=False):
    if verbose:
        print(f'Loading main courses')
    main_courses_df = read_file(main_courses, verbose)
    if verbose:
        print(f'Loading second courses')
    second_courses_df = read_file(second_courses, verbose)
    if verbose:
        print(f'Loading desserts')
    desserts_df = read_file(desserts, verbose)
    return main_courses_df, second_courses_df, desserts_df


if __name__ == '__main__':
    create_datasets()
