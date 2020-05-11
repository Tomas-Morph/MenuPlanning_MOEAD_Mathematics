"""
    Menu Planning Problem Feasibility Checker
    Alejandro Marrero - alu0100825008@ull.edu.es
    ---
    Nutrients Requirements
"""
import argparse
from mpp_feasibility.mpp_data import compute_requirements, read_database
from mpp_feasibility.feasibility_calcultator import compute_feasibility
from mpp_feasibility.result_files_parser import file_list, parse_all_files

parser = argparse.ArgumentParser('Menu Planning Problem Feasibility Checker')
parser.add_argument('path_to_files')
parser.add_argument('num_of_days', type=int)
parser.add_argument('output_filename')
parser.add_argument('-v', '--verbose', action='store_true')
args = parser.parse_args()

requirements = compute_requirements(args.num_of_days, args.verbose)
main_df, second_df, dess_df = read_database(args.verbose)
files_to_parse = file_list(args.path_to_files)
results = parse_all_files(files_to_parse)

for config in results:
    print(f'Configuration: {config}')
    for individual in results[config]:
        compute_feasibility(individual, requirements, args.num_of_days,
                            main_df, second_df, dess_df, args.verbose)
