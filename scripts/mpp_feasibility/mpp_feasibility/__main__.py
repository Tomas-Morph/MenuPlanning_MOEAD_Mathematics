"""
    Menu Planning Problem Feasibility Checker
    Alejandro Marrero - alu0100825008@ull.edu.es
    ---
    Nutrients Requirements
"""

from mpp_feasibility.nutrients_req import compute_requirements
from mpp_feasibility.menu_parser import create_datasets

recomendations = compute_requirements(10, verbose=True)
main_courses_df, second_courses_df, desserts_df = create_datasets(verbose=True)
