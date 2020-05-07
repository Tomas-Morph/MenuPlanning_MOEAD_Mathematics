#!/usr/bin/python3.6

import argparse
from weight_generator_MOEAD import generate_weights
import os

# Constantes que vamos a usar en la generacion de los ficheros
ALGORITHM = "MOEAD"
PROBLEM = "MenuPlanning"
OUTPUT_DIRECTORY = f"experimentsResults/{PROBLEM}_{ALGORITHM}"
METCO_PATH = "/home/amarrero/oplink/algorithms/team"
PREACTION = f"Preaction: mkdir -p {OUTPUT_DIRECTORY}\n"
EXTENSION = ".sexp"
MACHINE_FILE = "experimentConfigFiles/MPP_MOEAD/machinefile\n"


def configure_experiment(name, reps):
    mutation_prob = [0.1, 0.05, 0.02]
    crossover_prob = [0.9, 0.8, 0.7]
    population_sizes = [50, 100, 150, 200]
    neighbourhood_size = 10
    evaluations = 400000000
    number_of_days = [5, 10, 20, 40]

    for pop_size in population_sizes:
        config_name = f'{name}_{ALGORITHM}_ps{pop_size}'
        # Antes de cargar los datos al fichero de configuracion
        # Vamos a crear un vector de pesos para la configuracion
        config_weights_file = f"{os.getcwd()}/{config_name}.weights"
        generate_weights(pop_size + 1, 2, config_weights_file)

        output_file_pattern = f'%NameOfExperiment_%Algorithms_PopSize_%PopSize_Neihbs_%Neihb_M_%ProbMut_CR_%ProbCross_%NumDays_Days_%Individual_%Evaluations'

        path = f"MetcoPath: {METCO_PATH}\n"
        repetitions = f"Repetitions: {reps}\n"
        experiment = f"Name_of_experiment: {name}\n"
        experimentCamel = f"NameOfExperiment: {name}\n"
        algorithm = f"Algorithms: {ALGORITHM}\n"
        problem = f"Individual: {PROBLEM}\n"
        evals = f"Evaluations: {evaluations}\n"
        population_size = f"PopSize: {pop_size}\n"
        neihb = f"Neihb: {neighbourhood_size}\n"
        weights_file = f"WeightsFile: {config_weights_file}\n"
        prob_mut = f"ProbMut: {', '.join(map(str, mutation_prob))}\n"
        prob_cross = f"ProbCross: {', '.join(map(str, crossover_prob))}\n"
        num_days = f"NumDays: {', '.join(str(number_of_days))}\n"
        individual_params = f"IndividualParams: {', '.join(str(number_of_days))}"
        execution = f"Execution:  %MetcoPath/src/skeleton/main/metcoSeq %MetcoPath/scripts/run/{OUTPUT_DIRECTORY}/ " \
            "%MetcoPath/src/skeleton/main/ PlainText " \
            f"{output_file_pattern}.%rep " \
            "%Algorithms %Individual EVALUATIONS %Evaluations 10000000 0 %PopSize %Neihb %WeightsFile %ProbMut %ProbCross ! %NumDays $ NoOp \n\n"

        machine_file = f"machinefile: {MACHINE_FILE}\n"
        output_file = f"OutputFile: {output_file_pattern}"

        with open(f"{config_name}{EXTENSION}", "w") as output:
            output.write(path)
            output.write(repetitions)
            output.write(experiment)
            output.write(experimentCamel)
            output.write(PREACTION)
            output.write(algorithm)
            output.write(problem)
            output.write(evals)
            output.write(population_size)
            output.write(neihb)
            output.write(weights_file)
            output.write(prob_mut)
            output.write(prob_cross)
            output.write(num_days)
            output.write(individual_params)
            output.write("\n")
            output.write(execution)
            output.write(machine_file)
            output.write(output_file)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Configuring MMP Experiments")
    parser.add_argument("name", help="Name of the experiment", type=str)
    parser.add_argument(
        "repetitions", help="Repetitions for each configuration", type=int)
    args = parser.parse_args()
    configure_experiment(args.name, args.repetitions)
