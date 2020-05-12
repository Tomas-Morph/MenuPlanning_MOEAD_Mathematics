"""
    Menu Planning Problem Feasibility Checker
    Alejandro Marrero - alu0100825008@ull.edu.es
    ---
    Nutrients Requirements
"""

import argparse
from mpp_feasibility.mpp_data import compute_requirements, read_database
from mpp_feasibility.feasibility_calculator import \
    compute_configuration_feasibility
from mpp_feasibility.result_files_parser import file_list, parse_all_files
from mpp_feasibility.results_to_table import FeasibilityTable

# Definimos los parametros que vamos a usar en el script
parser = argparse.ArgumentParser('Menu Planning Problem Feasibility Checker')
parser.add_argument('path_to_files')
parser.add_argument('num_of_days', type=int)
parser.add_argument('output_filename')
parser.add_argument('-v', '--verbose', action='store_true')
args = parser.parse_args()

# Obtenemos los datos necesarios
requirements = compute_requirements(args.num_of_days, args.verbose)
main_df, second_df, dess_df = read_database(args.verbose)

# Leemos los ficheros del directorio que recibimos como parametro
files_to_parse = file_list(args.path_to_files)
# Extraemos las soluciones de cada configuracion
configurations = parse_all_files(files_to_parse)

table = FeasibilityTable(args.output_filename)

for config in configurations:
    if args.verbose:
        print(f'Configuration: {config}')
    ind_feas = compute_configuration_feasibility(configurations[config],
                                                 requirements,
                                                 args.num_of_days,
                                                 main_df, second_df, dess_df,
                                                 args.verbose)
    table.add_row_to_table(config, ind_feas)
# Por ultimo guardamos los resultados de las comprobaciones en una tabla
table.write_table_to_file()
