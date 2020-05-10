"""
    Menu Planning Problem Feasibility Checker
    Alejandro Marrero - alu0100825008@ull.edu.es
    ---
    Nutrients Requirements
"""

NUTRIENTS = 21
NAME = 'name'
RECOMMENDED_VALUE = 'rec_value'
K_NUTR = 'key_nutrient'
MIN_VALUE = 'min'
MAX_VALUE = 'max'

# Columns for the datasets
COURSE_NAME = 'course_name'
PRICE = 'price'
NUM_OF_COURSES = 3

# Min and Max nutrients ranges
K_NUTR_MIN = 0.9
K_NUTR_MAX = 1.10
NUTR_MIN = 0.7
NUTR_MAX = 1.3

nutrient_recommendations = {
    0: {
        NAME: 'FOLIC ACID',
        RECOMMENDED_VALUE: 135
    },
    1: {
        NAME: 'CALCIUM',
        RECOMMENDED_VALUE: 585
    },
    2: {
        NAME: 'ENERGY',
        RECOMMENDED_VALUE: 1012,
        K_NUTR: True
    },
    3: {
        NAME: 'PHOSPHORUS',
        RECOMMENDED_VALUE: 562.5
    },
    4: {
        NAME: 'FATS',
        RECOMMENDED_VALUE: 31.72,
        K_NUTR: True
    },
    5: {
        NAME: 'IRON',
        RECOMMENDED_VALUE: 8.55
    },
    6: {
        NAME: 'MAGNESIUM',
        RECOMMENDED_VALUE: 112.5
    },
    7: {
        NAME: 'POTASIUM',
        RECOMMENDED_VALUE: 2025
    },
    8: {
        NAME: 'PROTEINS',
        RECOMMENDED_VALUE: 27.08,
        K_NUTR: True
    },
    9: {
        NAME: 'SELENIUM',
        RECOMMENDED_VALUE: 25.75
    },
    10: {
        NAME: 'SODIUM',
        RECOMMENDED_VALUE: 870
    },
    11: {
        NAME: 'VITAMIN_A',
        RECOMMENDED_VALUE: 450
    },
    12: {
        NAME: 'VITAMIN_B1',
        RECOMMENDED_VALUE: 0.41
    },
    13: {
        NAME: 'VITAMIN_B2',
        RECOMMENDED_VALUE: 0.63
    },
    14: {
        NAME: 'VITAMIN_B6',
        RECOMMENDED_VALUE: 0.54
    },
    15: {
        NAME: 'VITAMIN_B12',
        RECOMMENDED_VALUE: 2.28
    },
    16: {
        NAME: 'VITAMIN_C',
        RECOMMENDED_VALUE: 27
    },
    17: {
        NAME: 'VITAMIN_D',
        RECOMMENDED_VALUE: 4.65
    },
    18: {
        NAME: 'VITAMIN_E',
        RECOMMENDED_VALUE: 6.3
    },
    19: {
        NAME: 'IODINE',
        RECOMMENDED_VALUE: 67.5
    },
    20: {
        NAME: 'ZINC',
        RECOMMENDED_VALUE: 6.75
    }
}


def compute_requirements(days=10, verbose=False):
    if verbose:
        print(f'Computing nutrients requirements for {days} days')
    for nutri_key in nutrient_recommendations:
        nutrient_recommendations[nutri_key][MIN_VALUE] = \
            nutrient_recommendations[nutri_key][RECOMMENDED_VALUE] * days * (
                K_NUTR_MIN if K_NUTR in nutrient_recommendations[
                    nutri_key] else NUTR_MIN)

        nutrient_recommendations[nutri_key][MAX_VALUE] = \
            nutrient_recommendations[nutri_key][RECOMMENDED_VALUE] * days * (
                K_NUTR_MAX if K_NUTR in nutrient_recommendations[
                    nutri_key] else NUTR_MAX)
        if verbose:
            print(
                f'Nutrient {nutri_key + 1}: {nutrient_recommendations[nutri_key]}')
    return nutrient_recommendations

