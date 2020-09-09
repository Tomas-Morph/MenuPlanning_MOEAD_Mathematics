/***********************************************************************************
 * AUTHORS
 *   - Alejandro Marrero
 *   - Eduardo Segredo
 *
 * DATE
 *   August 2020
 * *********************************************************************************/

#include "MOEAD_MPP.h"

#include <algorithm>
#include <chrono>
#include <limits>

#include "MOFrontVector.h"

const int MOEAD_MPP::INITIAL_GENERATION = 0;
const int MOEAD_MPP::NUM_PARAMS = 6;
const double MOEAD_MPP::EPSILON = 1e-9;
// Constructor
MOEAD_MPP::MOEAD_MPP() { secondPopulation = new vector<Individual *>; }

// Destructor
MOEAD_MPP::~MOEAD_MPP(void) {
  for (int i = 0; i < secondPopulation->size(); i++) {
    if ((*secondPopulation)[i] != nullptr) {
      delete (*secondPopulation)[i];
      (*secondPopulation)[i] = nullptr;
    }
  }
  delete (secondPopulation);
  secondPopulation = nullptr;

  for (int i = 0; i < neighbourhood.size(); i++) {
    neighbourhood[i].clear();
  }
  neighbourhood.clear();
  neighbourhood.shrink_to_fit();

  for (int i = 0; i < weightVector.size(); i++) {
    weightVector[i].clear();
  }
  weightVector.clear();
  weightVector.shrink_to_fit();
  delete (referencePoint);
  referencePoint = nullptr;
}

void MOEAD_MPP::fillPopWithNewIndsAndEvaluate() {
  if (getPopulationSize() == -1) {
    cerr << "Warning: fillPopWithNewInds called but pSize has not been "
            "fixed. Using default value: 25"
         << endl;
    setPopulationSize(25);
  }
  const int NUM_OBJS = 2;
  for (int i = population->size(); i < getPopulationSize(); i++) {
    Individual *ind = getSampleInd()->internalClone();
    ind->setAuxDataSize(NUM_OBJS * NUM_OBJS);
    ind->setAuxData(0, weightVector[i][0]);
    ind->setAuxData(1, weightVector[i][1]);

    ind->restart();
    ind->dependentLocalSearch();
    updateReferencePoint(ind);
    population->push_back(ind);
  }
}

/**
 * Método principal del algoritmo MOEA/D que ejecuta una iteración completa del
 * algoritmo
 * Esta versión incluye las modoficaciones propuestas en la versión del MOEA/D
 * para la gestión de restricciones en problemas multi-objetivo.
 */
void MOEAD_MPP::runGeneration() {
  // For each individual (subproblem) in the current population
  for (int i = 0; i < getPopulationSize(); i++) {
    // Creates a new offspring by applying the variation operators
    Individual *offSpring = createOffspring(i);
    // Updates the state of the algorithm
    updateReferencePoint(offSpring, true);
    updateParentSolution(offSpring, i);
    if (useArchive) {
      updateSecondPopulation(offSpring);
    }
    delete (offSpring);
    offSpring = nullptr;
  }
#ifdef __MOEAD_MPP_DEBUG__
  std::cout << "\tGeneration: " << getGeneration()
            << "\tEvals: " << getPerformedEvaluations() << std::endl;
  std::cout << "\tPopulation size = " << getPopulationSize() << std::endl;
  for (unsigned i = 0; i < getPopulationSize(); i++) {
    std::cout << "\t" << i << ") ";
    (*population)[i]->print(std::cout);
  }
  std::cout << "\tArchive size = " << secondPopulation->size() << std::endl;
  for (unsigned i = 0; i < secondPopulation->size(); i++) {
    std::cout << "\t" << i << ") ";
    (*secondPopulation)[i]->print(std::cout);
  }
  std::cout << "\t========================================" << std::endl;
#endif
}

bool MOEAD_MPP::init(const vector<string> &params) {
  // Check for the algorithm parameters
  if (params.size() < NUM_PARAMS) {
    cerr << "Error MOEA/D: incorrect parameters" << endl;
    cerr << "Number of subproblems N" << endl;
    cerr << "Neighbourhood size T" << endl;
    cerr << "Weight vectors file name" << endl;
    cerr << "Mutation rate pm" << endl;
    cerr << "Crossover rate pc" << endl;
    cerr << "Using archive < 0 = NO, 1 = YES>" << endl;
    return false;
  }

  // Initialisation of parameters and all data structures required
  setNumberOfSubProblems(atoi(params[0].c_str()));
  setNeighbourhoodSize(atoi(params[1].c_str()));
  setFileName(params[2].c_str());
  setMutationRate(atof(params[3].c_str()));
  setCrossoverRate(atof(params[4].c_str()));
  setNumberOfObj(getSampleInd()->getNumberOfObj());
  setPopulationSize(getNumberOfSubProblems());
  initialiseReferencePoint();
  initialiseWeightVector();
  initialiseNeighbourhood();
  useArchive = (atoi(params[5].c_str()) == 1) ? true : false;
  return true;
}

/**
 * Generamos el frente de soluciones con los individuos factibles
 * - Archivo o población principal según configuracion
 */
void MOEAD_MPP::getSolution(MOFront *p) {
  if (useArchive) {
    for (unsigned int i = 0; i < secondPopulation->size(); i++) {
      if (abs((*secondPopulation)[i]->getFeasibility() - 0.0) < EPSILON) {
        p->insert((*secondPopulation)[i]);
      }
    }
  } else {
    for (unsigned int i = 0; i < getPopulationSize(); i++) {
      if (abs((*population)[i]->getFeasibility() - 0.0) < EPSILON) {
        p->insert((*population)[i]);
      }
    }
  }
}

/**
 * Generamos el frente de soluciones con los individuos factibles
 * - Archivo o población principal según configuracion
 */
MOFront *MOEAD_MPP::getLocalSolution() {
  if (secondPopulation->size() == 0) {
    return nullptr;
  }
  MOFront *p = new MOFrontVector(getSampleInd(), false, false);
  for (unsigned int i = 0; i < secondPopulation->size(); i++) {
    if (abs((*secondPopulation)[i]->getFeasibility() - 0.0) < EPSILON) {
      p->insert((*secondPopulation)[i]);
    }
  }
  return p;
}

/**
 * Método que inicializa el punto de referencia que guía la búsqueda.
 * Se emplea en el cálculo de la función de Tchebycheff *
 */
void MOEAD_MPP::initialiseReferencePoint() {
  referencePoint = getSampleInd()->internalClone();
  if (getSampleInd()->getOptDirection(0) == MINIMIZE) {
    referencePoint->setObj(0, std::numeric_limits<double>::max());
  } else {
    referencePoint->setObj(0, std::numeric_limits<double>::min());
  }
  if (getSampleInd()->getOptDirection(1) == MINIMIZE) {
    referencePoint->setObj(1, std::numeric_limits<double>::max());
  } else {
    referencePoint->setObj(1, std::numeric_limits<double>::min());
  }
}

/**
 * Método empleado para leer el fichero de pesos que determina el vecindario
 * del algoritmo
 **/
void MOEAD_MPP::initialiseWeightVector() {
  weightVector = vector<vector<double>>(getPopulationSize(),
                                        vector<double>(getNumberOfObj(), 0));

  ifstream inputFile;
  inputFile.open(fileName.c_str());
  if (!inputFile) {
    cerr << "Error MOEA/D: file containing weight vectors could not be opened"
         << endl;
    exit(1);
  }

  int numWeightVectors;
  inputFile >> numWeightVectors;

  if (numWeightVectors != getPopulationSize()) {
    cerr << "Error MOEA/D: the number of weight vectors does not match with "
            "the number of subproblems specified"
         << endl;
    inputFile.close();
    exit(1);
  }

  for (int i = 0; i < getPopulationSize(); i++) {
    for (int j = 0; j < getNumberOfObj(); j++) {
      inputFile >> weightVector[i][j];
    }
  }
  inputFile.close();
}

/**
 *  Metodo que inicializa el vecindario de cada uno de los individuos
 *  Define la relacion de vecindad segun la distancia
 **/
void MOEAD_MPP::initialiseNeighbourhood() {
  if (getNeighbourhoodSize() > getPopulationSize()) {
    cerr << "Error MOEA/D: the neighbourhood size (T) cannot be greather than "
            "the number of subproblems (N)"
         << endl;
    exit(1);
  }
  neighbourhood = vector<vector<int>>(getPopulationSize(), vector<int>());
  for (int i = 0; i < getPopulationSize(); i++) {
    vector<int> indx;
    vector<double> dist;
    for (int j = 0; j < getPopulationSize(); j++) {
      indx.push_back(j);
      double tp = getEuclideanDistance(weightVector[i], weightVector[j]);
      dist.push_back(tp);
    }
    minFastSort(dist, indx, getPopulationSize(), getNeighbourhoodSize() + 1);
    for (int k = 0; k < getNeighbourhoodSize(); k++) {
      neighbourhood[i].push_back(indx[k]);
    }
    indx.clear();
    dist.clear();
  }
}

/***
 * Método que aplica los operadores geneticos ç
 * para generar un nuevo individuo
 **/
Individual *MOEAD_MPP::createOffspring(const int &i) {
  // Selects two neighboring solutions randomly
  int idx1 = (int)(getNeighbourhoodSize()) * (rand() / (RAND_MAX + 1.0));
  int idx2 = (int)(getNeighbourhoodSize()) * (rand() / (RAND_MAX + 1.0));
  idx1 = neighbourhood[i][idx1];
  idx2 = neighbourhood[i][idx2];
  Individual *p1 = (*population)[idx1]->internalClone();
  Individual *p2 = (*population)[idx2]->internalClone();
  // Crossover
  double vcross = rand() / (RAND_MAX + 1.0);
  if (vcross < pc) {
    p1->crossover(p2);
  }
  // Mutation
  // Potential improvement: mutate p2 and select the best individual from both
  // p1 and p2
  p1->mutation(pm);
  // Copiamos los pesos para poder realizar la ILS
  p1->setAuxData(0, weightVector[i][0]);
  p1->setAuxData(1, weightVector[i][1]);
  p1->setAuxData(2, referencePoint->getObj(0));
  p1->setAuxData(3, referencePoint->getObj(1));
  evaluate(p1);
  p1->dependentLocalSearch();
  // Free memory
  delete (p2);
  p2 = nullptr;
  return p1;
}

/**
 * Método que actualiza el punto de referencia
 * si hemos encontrado nuevos optimos
 **/
void MOEAD_MPP::updateReferencePoint(Individual *ind, bool copy) {
  // Si no es factible lo descartamos directamente
  if (ind->getFeasibility() != 0.0) {
    return;
  } else {
    for (unsigned i = 0; i < ind->getNumberOfObj(); i++) {
      if (ind->getObj(i) < referencePoint->getObj(i)) {
        referencePoint->setObj(i, ind->getObj(i));
      }
    }
    // Copiamos el RP para cada individuo
    if (copy) {
      for (unsigned int i = 0; i < getPopulationSize(); i++) {
        (*population)[i]->setAuxData(2, referencePoint->getObj(0));
        (*population)[i]->setAuxData(3, referencePoint->getObj(1));
      }
    }
  }
}

/**
 * Metodo para actualizar la poblacion secundaria eliminando los individuos
 * dominados por el individuo que recibe como parametro
 **/
void MOEAD_MPP::updateSecondPopulation(Individual *ind) {
  // Removes from the external population all those individuals dominated by
  // individual ind
  if (ind->getFeasibility() != 0.0) {
    return;
  }
  unsigned int i = 0;
  // Removes from the external population all those individuals dominated by
  // individual ind
  while (i < secondPopulation->size()) {
    if (dominanceTest((*secondPopulation)[i], ind) == SECOND_DOMINATES) {
      delete ((*secondPopulation)[i]);
      (*secondPopulation)[i] =
          (*secondPopulation)[secondPopulation->size() - 1];
      secondPopulation->pop_back();
    } else
      i++;
  }
  // Adds individual ind to the external population if no individual in the said
  // population dominates it
  bool insert = true;
  i = 0;
  while (i < secondPopulation->size()) {
    if (dominanceTest((*secondPopulation)[i], ind) == FIRST_DOMINATES) {
      insert = false;
      break;
    }
    i++;
  }
  if (insert) secondPopulation->push_back(ind->internalClone());
}

/**
 * Metodo empleado para actualizar un individuo en caso de mejora de sus
 * descendientes
 *  - Mejora propuesta por Carlos Segura para evitar la perdida de diversidad
 **/
void MOEAD_MPP::updateParentSolution(Individual *offspring, const int &i) {
  // fitness of the offspring
  double f1 = computingFitnessValue(offspring, weightVector[i]);
  // fitness of the neighbour
  double f2 = computingFitnessValue((*population)[i], weightVector[i]);
  bool update = false;
  // First check if its feasibility
  if (offspring->getFeasibility() < (*population)[i]->getFeasibility()) {
    update = true;
  } else if ((abs(offspring->getFeasibility() -
                  (*population)[i]->getFeasibility()) < EPSILON) &&
             (f1 < f2)) {
    update = true;
  }

  if (update) {
    delete ((*population)[i]);
    (*population)[i] = offspring->internalClone();
    // Se copian los pesos para poder aplicar la ILS
    (*population)[i]->setAuxData(0, weightVector[i][0]);
    (*population)[i]->setAuxData(1, weightVector[i][1]);
    (*population)[i]->setAuxData(2, referencePoint->getObj(0));
    (*population)[i]->setAuxData(3, referencePoint->getObj(1));
  }
}

/**
 * Computes the fitness value of a particular individual by considering the
 * Tchebycheff approach
 *
 **/
double MOEAD_MPP::computingFitnessValue(Individual *ind,
                                        vector<double> &lambda) {
  double fitness = std::numeric_limits<double>::min();
  for (int i = 0; i < getNumberOfObj(); i++) {
    double eval = (abs(ind->getObj(i) - referencePoint->getObj(i))) * lambda[i];
    if (eval > fitness) {
      fitness = eval;
    }
  }
  return fitness;
}

/**
 * Metodo auxiliar que nos permite definir el orden de los individuos
 **/
void MOEAD_MPP::minFastSort(vector<double> &dist, vector<int> &indx,
                            int numberSubProblems, int neighbourhoodSize) {
  for (int i = 0; i < neighbourhoodSize; i++) {
    for (int j = i + 1; j < numberSubProblems; j++) {
      if (dist[i] > dist[j]) {
        double temp = dist[i];
        dist[i] = dist[j];
        dist[j] = temp;
        int id = indx[i];
        indx[i] = indx[j];
        indx[j] = id;
      }
    }
  }
}

/**
 * Metodo que calcula la distancia Euclidea entre dos vectores de pesos
 * */
double MOEAD_MPP::getEuclideanDistance(vector<double> weightA,
                                       vector<double> weightB) {
  double dist = 0;
  for (int i = 0; i < weightA.size(); i++) {
    dist += (weightA[i] - weightB[i]) * (weightA[i] - weightB[i]);
  }
  return sqrt(dist);
}

void MOEAD_MPP::printInfo(ostream &os) const {
  os << "Multi-objective Evolutionary Algorithm based on decomposition: MOEA/D"
     << endl;
  os << "Number of Evaluations = " << getPerformedEvaluations() << endl;
  os << "Number of subproblems (N) = " << getNumberOfSubProblems() << endl;
  os << "Neighbourhood size (T) = " << getNeighbourhoodSize() << endl;
  os << "Mutation rate = " << pm << endl;
  os << "Crossover rate = " << pc << endl;
  os << "Using Archive = " << std::boolalpha << useArchive << std::endl;
}
