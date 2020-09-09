#!/usr/bin/python3

import sys
import operator as op
from functools import reduce
from itertools import product
from typing import List


# Function that calculates a combinatorial number
def calculateCombNumber(n: int, r: int) -> float:
    r = min(r, n - r)
    num = reduce(op.mul, range(n, n - r, -1), 1)
    denom = reduce(op.mul, range(1, r + 1), 1)
    return num / denom


# Function that obtains the list of weights given a parameter H and
# the number of objective functions to be considered
def getWeightList(paramH: int, numObj: int) -> List[float]:
    weightList = []
    for hValue in range(0, paramH + 1):
        weightList.append(hValue / paramH)
    return weightList

# Function that returns the sum of a list


def sumList(myList):
    if not all(isinstance(n, (float, int)) for n in myList):
        print("Function sumList: wrong type of arguments")
        sys.exit(-1)
    return reduce((lambda x, y: x + y), myList)

# Function that returns a list of weight vectors given a weight list and
# the number of objective functions


def getWeightVectorList(weightList, numObj):
    if not all(isinstance(n, (float, int)) for n in weightList) or type(numObj) not in (int, ):
        print("Function getWeightVectorList: wrong type of arguments")
        sys.exit(-1)

    productList = product(weightList, repeat=numObj)
    weightVectorList = []
    for prod in productList:
        weightVector = []
        for weight in prod:
            weightVector.append(weight)
        if sumList(weightVector) == 1.0:
            weightVectorList.append(weightVector)
    return weightVectorList

# Functions that writes the weight vector list into a file


def writeToFile(weightVectorList, output_filename):

    with open(output_filename, 'w') as output:
        output.write(f"{len(weightVectorList)}\n")
        for weightVector in weightVectorList:
            for weight in weightVector:
                output.write(f"{weight} ")
            output.write('\n')


if __name__ == "__main__":
    # Main program
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} H m outputFileName")
        sys.exit(-1)
    else:
        paramH = int(sys.argv[1])
        numObj = int(sys.argv[2])
        outputFileName = sys.argv[3]
        paramN = int(calculateCombNumber(paramH + numObj - 1, numObj - 1))
        print(f"Parameter H: {paramH}")
        print(f"Number of objective functions (m): {numObj}")
        print(f"Number of weight vectors: {paramN}")

        weightList = getWeightList(paramH, numObj)
        print(f"List of weights ({len(weightList)}): {weightList}")

        weightVectorList = getWeightVectorList(weightList, numObj)
        #print(f"List of weight vectors ({len(weightVectorList)}): {weightVectorList}")
        writeToFile(weightVectorList, outputFileName)
