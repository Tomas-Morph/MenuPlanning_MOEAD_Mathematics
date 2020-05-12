import pandas as pd
from mpp_feasibility.mpp_data import nutrient_recommendations, NUTRIENT_NAME
import numpy as np


class FeasibilityTable:
    CONFIGURATION = 'Configuration'

    def __init__(self, output_filename: str = None):
        self.header = [nutrient_recommendations[key][NUTRIENT_NAME]
                       for key in nutrient_recommendations]

        self.output_filename = output_filename
        self.data = {}

    def add_row_to_table(self, config_name: str = None,
                         feasibility: np.array = None):
        self.data[config_name] = feasibility

    def write_table_to_file(self):
        pd.DataFrame.from_dict(self.data, orient='index',
                               columns=self.header).to_csv(
            self.output_filename)

    def head(self):
        return pd.DataFrame.from_dict(self.data, orient='index',
                                      columns=self.header).head()
