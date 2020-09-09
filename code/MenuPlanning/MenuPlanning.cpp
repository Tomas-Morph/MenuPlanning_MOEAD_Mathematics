

/**
 *
 * author: Alejandro Marrero
 * contact: alejandro.marrero.10@ull.edu.es
 * Created on Sun Jul 05 2020
 *
 * Copyright (c) 2020 Universidad de La Laguna
 *
 * Summary:
 *
 *
 **/

#include "MenuPlanning.h"

#include <bits/stdc++.h>

#include <array>
#include <chrono>
#include <cmath>

// Constantes del problema
const int MenuPlanning::N_ARGS = 2;
const int MenuPlanning::N_OBJS = 2;
const int MenuPlanning::MAX_INT = std::numeric_limits<int>::max();

// Variables estaticas del problema
int MenuPlanning::nDias;
int MenuPlanning::nParam;
string MenuPlanning::pathToDB;
double MenuPlanning::minCost;
double MenuPlanning::maxCost;
double MenuPlanning::minRepetition;
double MenuPlanning::maxRepetition;

vector<infoPlatos> MenuPlanning::v_primerosPlatos;
vector<infoPlatos> MenuPlanning::v_segundosPlatos;
vector<infoPlatos> MenuPlanning::v_postres;
vector<pair<int, int>> MenuPlanning::gruposAl;
vector<vector<vector<double>>> MenuPlanning::v3_compatibilidadPlatos;
vector<string> MenuPlanning::alergenosPlan;
vector<string> MenuPlanning::incompatibilidadesPlan;
vector<int> MenuPlanning::gruposAlPlan;
vector<double> MenuPlanning::infoNPlan;

/**
 *
 * Constructor por defecto de una instancia de MenuPlanning
 */
MenuPlanning::MenuPlanning() {
  this->setFeasibility(0.0);
  originalCost = 0.0;
  originalRepetition = 0.0;
  partial = false;
  requiredFeasibility = std::numeric_limits<double>::max();
}

MenuPlanning::~MenuPlanning() {
  badDays.clear();
  badDays.shrink_to_fit();
}

/**
 * Metodo para inicializar un individuo MenuPlanning
 *
 * @param vector de parametros
 **/
bool MenuPlanning::init(const vector<string> &params) {
  if (params.size() != N_ARGS) {
    cout << "Error Menu Planning: numero incorrecto de parametros." << endl;
    cout << "- Number of days expected" << endl;
    cout << "- Path to DB expected" << std::endl;
    return false;
  }
  nDias = atoi((params[0]).c_str());
  nParam = nDias * num_tipoPlato;  // Multiplicar nDias por tipos de platos (3)
  setNumberOfVar(nParam);
  setNumberOfObj(N_OBJS);
  pathToDB = params[1];
  set_gruposAl();
  set_Platos();
  set_vectorCompatibilidad();
  infoNPlan.assign(num_nutr, 0);
  alergenosPlan.assign(num_alerg, "0");
  incompatibilidadesPlan.assign(num_incomp, "0");
  // Definimos las variables de normalizacion segun el numero de dias
  int idx = 0;
  if (nDias == 40) {
    idx = 1;
  } else if (nDias == 60) {
    idx = 2;
  }
  setObjectivesRanges(idx);
  return true;
}

/**
 *
 *
 */
void MenuPlanning::setObjectivesRanges(const int &idx) {
  minCost = get<0>(objectivesRanges[idx]);
  maxCost = get<1>(objectivesRanges[idx]);
  minRepetition = get<2>(objectivesRanges[idx]);
  maxRepetition = get<3>(objectivesRanges[idx]);
}

void MenuPlanning::set_Platos() {
  string mainCourses = pathToDB + "/primerosplatos.txt";
  string secondCourses = pathToDB + "/segundosplatos.txt";
  string desserts = pathToDB + "/postres.txt";
  set_VectoresPlatos(mainCourses.c_str(), v_primerosPlatos);
  set_VectoresPlatos(secondCourses.c_str(), v_segundosPlatos);
  set_VectoresPlatos(desserts.c_str(), v_postres);
}

/**
 * Método para definir los grupos de alimentos
 */
void MenuPlanning::set_gruposAl(void) {
  pair<int, int> ga;
  ga.first = MAX_INT;
  ga.second = MAX_INT;
  gruposAl.assign(num_gAl, ga);
}

/**
 * Método para cargar los platos a partir de los datos
 * dentro del fichero que se recibe como parametro
 **/
void MenuPlanning::set_VectoresPlatos(const char *c_filename,
                                      vector<infoPlatos> &v_vecP) {
  ifstream ifs;
  int i;
  int cont = 1;

  ifs.open(c_filename, ifstream::in);
  if (ifs.is_open()) {
    while (ifs.good()) {
      string str;
      infoPlatos ip;

      getline(ifs, str, ',');
      ip.nombre = str;  // Nombre del plato
      getline(ifs, str, ',');
      ip.precio = atof(str.c_str());  // Precio
      getline(ifs, str, ',');
      ip.cantidad = atof(str.c_str());  // Cantidad
      ip.diasRep = std::numeric_limits<int>::max();

      for (i = 0; i < num_alerg; i++) {
        getline(ifs, str, ',');
        ip.alg.push_back(str);
      }  // Alergenos
      for (i = 0; i < num_incomp; i++) {
        getline(ifs, str, ',');
        ip.inc.push_back(str);
      }  // Incompatibilidades
      for (i = 0; i < num_nutr; i++) {
        getline(ifs, str, ',');
        ip.infoN.push_back(atof(str.c_str()));
      }  // Informacion nutricional

      getline(ifs, str, ',');
      while (str != "*" && !ifs.eof()) {
        ip.gruposAl.push_back(atoi(str.c_str()));
        getline(ifs, str, ',');

        for (int x = 0; x < str.size(); x++)
          if (str.at(x) == '\n') str = "*";
      }

      v_vecP.push_back(ip);
    }
    ifs.close();
  } else {
    cout << "\n\nError. No se han podido leer los archivos de platos: "
         << c_filename << endl;
    exit(0);
  }
}

/*--------------------------------------------*/
/*---------- COMPATIBILIDAD PLATOS -----------*/
/*--------------------------------------------*/
void MenuPlanning::set_vectorCompatibilidad(void) {
  // Vector de postres
  vector<double> sz(v_postres.size(), 0.0);
  // Plano de postres y segundos platos
  vector<vector<double>> sz2(v_segundosPlatos.size(), sz);
  // Prisma rectangular de primeros, segundos platos y postres
  v3_compatibilidadPlatos.assign(v_primerosPlatos.size(), sz2);
  vector<bool> gaElegidos;
  for (int y = 0; y < v_primerosPlatos.size(); y++)
    for (int x = 0; x < v_segundosPlatos.size(); x++)
      for (int z = 0; z < v_postres.size(); z++) {
        gaElegidos.clear();
        gaElegidos.assign(num_gAl, false);
        set_GAElegidos(v_primerosPlatos[y].gruposAl, gaElegidos);
        v3_compatibilidadPlatos[y][x][z] =
            set_penalizacionVC(v_segundosPlatos[x].gruposAl, gaElegidos);
        set_GAElegidos(v_segundosPlatos[x].gruposAl, gaElegidos);
        v3_compatibilidadPlatos[y][x][z] +=
            set_penalizacionVC(v_postres[z].gruposAl, gaElegidos);
      }
}

void MenuPlanning::set_GAElegidos(vector<int> gal, vector<bool> &galE) {
  for (int i = 0; i < gal.size(); i++) galE[gal[i]] = true;
}

double MenuPlanning::set_penalizacionVC(vector<int> &gal, vector<bool> galE) {
  double resultado = 0.0;
  for (int i = 0; i < gal.size(); i++) {
    if (galE[gal[i]]) switch (gal[i]) {
        case 0:
          resultado += penalizaciones[0];
          break;
        case 1:
          resultado += penalizaciones[1];
          break;
        case 2:
          resultado += penalizaciones[2];
          break;
        case 3:
          resultado += penalizaciones[3];
          break;
        case 4:
          resultado += penalizaciones[4];
          break;
        case 5:
          resultado += penalizaciones[5];
          break;
        case 6:
          resultado += penalizaciones[6];
          break;
        case 7:
          resultado += penalizaciones[7];
          break;
        case 8:
          resultado += penalizaciones[8];
          break;
        case 9:
          resultado += penalizaciones[9];
          break;
      }
  }
  return resultado;
}

/*--------------------------------------------------------*/
/*---------- GENERACION ALEATORIA DE INDIVIDUO -----------*/
/*--------------------------------------------------------*/
void MenuPlanning::restart(void) {
  for (int i = 0; i < nDias; i++) {
    for (int j = 0; j < 3; j++) {
      setVar(i * 3 + j, random() % NPLATOS[j]);
    }
  }
}

/**
 * Clonacion del individuo
 **/
Individual *MenuPlanning::clone(void) const {
  MenuPlanning *mpp = new MenuPlanning();
  mpp->badDays = {badDays};
  mpp->heaviestNut = heaviestNut;
  mpp->heaviestType = heaviestType;
  mpp->originalCost = originalCost;
  mpp->originalRepetition = originalRepetition;
  mpp->partial = partial;
  mpp->requiredFeasibility = requiredFeasibility;
  return mpp;
}

/*----------------------------------------*/
/*---------- OPERADOR DE CRUCE -----------*/
/*----------------------------------------*/
void MenuPlanning::dependentCrossover(Individual *ind) {
  // uniformCrossover(ind);
  pairBasedCrossover(ind);
}

/**
 * Uniform Crossover Based on MPP CEC 2019
 *
 **/
void MenuPlanning::uniformCrossover(Individual *ind2) {
  for (int i = 0; i < nDias; i++) {
    if (rand() > (RAND_MAX / 2)) {
      double vIdx = var[i * 3];
      double vIdx1 = var[i * 3 + 1];
      double vIdx2 = var[i * 3 + 2];
      this->var[i * 3] = ind2->getVar(i * 3);
      this->var[i * 3 + 1] = ind2->getVar(i * 3 + 1);
      this->var[i * 3 + 2] = ind2->getVar(i * 3 + 2);
      ind2->setVar(i * 3, vIdx);
      ind2->setVar(i * 3 + 1, vIdx1);
      ind2->setVar(i * 3 + 2, vIdx2);
    }
  }
}

/**
 * Pair Based Crossover basado en CEC 2019
 */
void MenuPlanning::pairBasedCrossover(Individual *i2) {
  vector<Food> pendingI1, pendingI2;

  map<Food, int> f1;
  int dist = 0;
  for (int i = 0; i < nDias; i++) {
    Food f;
    f.p1 = round(getVar(i * 3));
    f.p2 = round(getVar(i * 3 + 1));
    f.p3 = round(getVar(i * 3 + 2));
    f1[f]++;
  }

  int fixed = 0;
  for (int i = 0; i < nDias; i++) {
    Food f;
    f.p1 = round(i2->getVar(i * 3));
    f.p2 = round(i2->getVar(i * 3 + 1));
    f.p3 = round(i2->getVar(i * 3 + 2));
    if (f1.count(f)) {  // Comida en ambos
      setVar(fixed * 3, f.p1);
      setVar(fixed * 3 + 1, f.p2);
      setVar(fixed * 3 + 2, f.p3);
      i2->setVar(fixed * 3, f.p1);
      i2->setVar(fixed * 3 + 1, f.p2);
      i2->setVar(fixed * 3 + 2, f.p3);
      fixed++;
      f1[f]--;
      if (f1[f] == 0) {
        f1.erase(f);
      }
    } else {
      pendingI2.push_back(f);
      dist++;
    }
  }
  for (map<Food, int>::iterator it = f1.begin(); it != f1.end(); it++) {
    for (int j = 0; j < it->second; j++) {
      pendingI1.push_back(it->first);
    }
  }
  if (pendingI1.size() != pendingI2.size()) {
    cerr << "Error interno. PendingI1 != PendingI2" << endl;
    exit(-1);
  }
  random_shuffle(pendingI1.begin(), pendingI1.end());
  int next = nDias - pendingI1.size();
  for (int i = 0; i < pendingI1.size(); i++) {
    Food f1 = pendingI1[i];
    Food f2 = pendingI2[i];
    if (rand() < RAND_MAX / 2.0) {
      swap(f1, f2);
    }
    setVar(next * 3, f1.p1);
    setVar(next * 3 + 1, f1.p2);
    setVar(next * 3 + 2, f1.p3);
    i2->setVar(next * 3, f2.p1);
    i2->setVar(next * 3 + 1, f2.p2);
    i2->setVar(next * 3 + 2, f2.p3);
    next++;
  }
}

/**
 * Buscamos mejoras en el punto de referencia
 * tras aplicar ILS
 */
void MenuPlanning::updateInnerReference() {
  // Si no es factible lo descartamos directamente
  if (getFeasibility() != 0.0) {
    return;
  } else {
    if (getObj(0) < getAuxData(2)) {
      setAuxData(2, getObj(0));
    }
    if (getObj(1) < getAuxData(3)) {
      setAuxData(3, getObj(1));
    }
  }
}

/**
 * ILS Basada en MPP CEC 2019
 *
 **/
void MenuPlanning::dependentLocalSearch() {
  vector<Neighbor> neighbors;
  neighbors.reserve(nDias * (NPLATOS[0] + NPLATOS[1] + NPLATOS[2]));
  for (int i = 0; i < nDias; i++) {
    for (int j = 0; j < 3; j++) {
      for (int k = 0; k < NPLATOS[j]; k++) {
        Neighbor n;
        n.variable = i * 3 + j;
        n.newValue = k;
        neighbors.push_back(n);
      }
    }
  }
  vector<double> bestIndividual = var;
  evaluate();
  updateInnerReference();
  pair<double, double> bestResult =
      pair<double, double>(getFeasibility(), computingFitnessValue());
  for (int i = 0; i < 100; i++) {
    partial = false;
    evaluate();
    updateInnerReference();
    pair<double, double> currentResult =
        pair<double, double>(getFeasibility(), computingFitnessValue());

    bool improved = true;
    while (improved) {
      improved = false;
      random_shuffle(neighbors.begin(), neighbors.end());
      for (int i = 0, length = neighbors.size(); i < length; ++i) {
        int currentValue = var[neighbors[i].variable];
        var[neighbors[i].variable] = neighbors[i].newValue;
        partial = true;
        requiredFeasibility = currentResult.first;
        evaluate();
        updateInnerReference();
        partial = false;
        pair<double, double> newResult =
            pair<double, double>(getFeasibility(), computingFitnessValue());
        if (newResult >= currentResult) {
          var[neighbors[i].variable] = currentValue;
        } else {
          improved = true;
          currentResult = newResult;
        }
      }
    }
    if (currentResult >= bestResult) {
      var = bestIndividual;
    } else {
      bestResult = currentResult;
      bestIndividual = var;
    }
    evaluate();
    updateInnerReference();
    if (badDays.size() == 0) {
      if (heaviestNut != -1) {
        vector<pair<double, int>> infoNut;
        for (int i = 0; i < nDias; i++) {
          double total = v_primerosPlatos[var[i * 3]].infoN[heaviestNut] +
                         v_segundosPlatos[var[i * 3 + 1]].infoN[heaviestNut] +
                         v_postres[var[i * 3 + 2]].infoN[heaviestNut];
          infoNut.push_back(pair<double, int>(total, i));
        }
        sort(infoNut.begin(), infoNut.end());
        if (heaviestType == 1) reverse(infoNut.begin(), infoNut.end());
        int selectedDay = infoNut[random() % 6].second;
        var[selectedDay * 3] = (rand() % NPLATOS[0]);
        var[selectedDay * 3 + 1] = (rand() % NPLATOS[1]);
        var[selectedDay * 3 + 2] = (rand() % NPLATOS[2]);
      } else {
        int selectedDay = random() % nDias;
        var[selectedDay * 3] = (rand() % NPLATOS[0]);
        var[selectedDay * 3 + 1] = (rand() % NPLATOS[1]);
        var[selectedDay * 3 + 2] = (rand() % NPLATOS[2]);
      }
    } else {
      for (auto it = badDays.begin(); it != badDays.end(); it++) {
        int day = *it;
        int which = random() % 3;
        var[day * 3 + which] = (rand() % NPLATOS[which]);
      }
    }
  }
  var = bestIndividual;
  evaluate();
  neighbors.clear();
  neighbors.shrink_to_fit();
}

/**
 * Computes the fitness value of a particular individual by considering the
 * Tchebycheff approach
 *
 **/
double MenuPlanning::computingFitnessValue() {
  // getAuxData(2) es el obj0 del punto de referencia
  // getAuxData(3) es el obj1 del punto de referencia
  double f1 = (abs(getObj(0) - getAuxData(2))) * getAuxData(0);
  double f2 = (abs(getObj(1) - getAuxData(3))) * getAuxData(1);
  return (f1 > f2) ? f1 : f2;
}

/*-------------------------------------------*/
/*---------- OPERADOR DE MUTACION -----------*/
/*-------------------------------------------*/
void MenuPlanning::dependentMutation(double pm) {
  int x;
  bool mod = false;
  for (int i = 0; i < nDias; i++) {
    x = rand() % 100;
    if (x < (pm * 100)) {
      setVar(i * num_tipoPlato, rand() % v_primerosPlatos.size());
      setVar(i * num_tipoPlato + 1, rand() % v_segundosPlatos.size());
      setVar(i * num_tipoPlato + 2, rand() % v_postres.size());
      mod = true;
    }
  }
}

/**
 * Método que se encarga de calcular la factibilidad de un
 *
 * individuo con respecto a las restricciones de nutrientes
 *
 * BASADO en MPP CEC 2019
 *
 * Alejandro Marrero - alu0100825008@ull.edu.es
 **/
double MenuPlanning::computeFeasibility() {
  feasibility = 0.0;
  std::array<double, num_nutr> infoNPlan{};
  badDays.clear();
  // Bucle para calcular did(S)
  for (int i = 0; i < nDias; i++) {
    int idx = i * num_tipoPlato;
    // Valores nutricionales para el dia I
    std::array<double, num_nutr> dayNutr{};

    // Primero obtenemos la cantidad de nutrientes de cada dia
    for (unsigned int j = 0; j < num_nutr; j++) {
      dayNutr[j] += v_primerosPlatos[getVar(idx)].infoN[j];
      dayNutr[j] += v_segundosPlatos[getVar(idx + 1)].infoN[j];
      dayNutr[j] += v_postres[getVar(idx + 2)].infoN[j];
      infoNPlan[j] += dayNutr[j];
    }
    // Calculamos los nutrientes forzados por dia
    for (int j = 0; j < FORCED_INDEXES_SIZE; j++) {
      int index = FORCED_INDEXES[j];
      if (dayNutr[index] < ingR[index] * FORCED_MIN[j]) {
        feasibility +=
            pow((ingR[index] * FORCED_MIN[j] - dayNutr[index]) / ingR[index],
                2) *
            1000000.0;
        badDays.push_back(i);

      } else if (dayNutr[index] > ingR[index] * FORCED_MAX[j]) {
        feasibility +=
            pow((dayNutr[index] - ingR[index] * FORCED_MAX[j]) / ingR[index],
                2) *
            1000000.0;
        badDays.push_back(i);
      }
    }
  }
  double heaviestValue = 0.0;
  heaviestNut = -1;
  // Ahora calculamos los nutrientes globales del plan gid(S)
  for (unsigned int i = 0; i < num_nutr; i++) {
    if ((i == CALCIUM_INDEX) || (i == POTASIUM_INDEX) || (i == IRON_INDEX))
      continue;
    if (infoNPlan[i] < (ingR[i] * minReq[i] * nDias)) {
      double value = pow(
          (ingR[i] * minReq[i] * nDias - infoNPlan[i]) / (ingR[i] * nDias), 2);
      feasibility += value;
      if (value > heaviestValue) {
        heaviestValue = value;
        heaviestNut = i;
        heaviestType = -1;
      }
    }
    if (infoNPlan[i] > (ingR[i] * maxReq[i] * nDias)) {
      double value = pow(
          (infoNPlan[i] - ingR[i] * maxReq[i] * nDias) / (ingR[i] * nDias), 2);
      feasibility += value;
      if (value > heaviestValue) {
        heaviestValue = value;
        heaviestNut = i;
        heaviestType = 1;
      }
    }
  }

  feasibility *= 1e12;

  // devolvemos id(S) = did(S) + gid(S)
  return feasibility;
}

/**
 * Metodo que evalua un individuo de la clase MenuPlanning
 * - Asigna los valores para los dos objetivos:
 *  - Precio
 *  - Nivel de Repeticion
 **/
void MenuPlanning::evaluate(void) {
  double precioTotal = 0;
  double valPP, valSP, valP, valTabla, valGAFirst;
  double numPP = penalizaciones[15];
  double numSP = penalizaciones[16];
  double numP = penalizaciones[17];

  computeFeasibility();
  // partial = false;
  if (partial && (getFeasibility() > requiredFeasibility)) {
    return;
  }

  double valTotal = 0;
  // Vector que guarda los grupos alimenticios pertenecientes a los platos
  // elegidos
  vector<int> gaElegidos;
  // Vector que guarda los ga pertenecientes al menu de la iteracion anterior
  vector<int> gaElegidosAnterior;
  vector<vector<int>> ultimos5GA;
  int x = 0;
  for (int i = 0; i < nDias; i++) {
    x = i * num_tipoPlato;
    // Coste del menu
    precioTotal += v_primerosPlatos[round(getVar(x))].precio +
                   v_segundosPlatos[round(getVar(x + 1))].precio +
                   v_postres[round(getVar(x + 2))].precio;
    // Nutrientes
    for (int j = 0; j < infoNPlan.size(); j++)
      infoNPlan[j] += v_primerosPlatos[round(getVar(x))].infoN[j] +
                      v_segundosPlatos[round(getVar(x + 1))].infoN[j] +
                      v_postres[round(getVar(x + 2))].infoN[j];
  }
  // Nivel de repeticion
  for (int i = 0; i < nDias; i++) {
    x = i * num_tipoPlato;

    // PRIMER PLATO
    // Numero de dias desde que se repitio el plato seleccionado
    valPP = setValorPP(getVar(x));
    // Numero de dias desde que se repitio el grupo alimenticio
    for (int j = 0; j < v_primerosPlatos[getVar(x)].gruposAl.size(); j++) {
      // comprueba si ya habia aparecido en el menu el grupo alimenticio j, si
      // no lo a�ade al vector gaElegidos
      if (gaElegidosPorIteracion(gaElegidos,
                                 v_primerosPlatos[getVar(x)].gruposAl[j]))
        gaElegidos.push_back(v_primerosPlatos[getVar(x)].gruposAl[j]);
    }

    // SEGUNDO PLATO
    valSP = setValorSP(getVar(x + 1));
    for (int k = 0; k < v_segundosPlatos[getVar(x + 1)].gruposAl.size(); k++) {
      if (gaElegidosPorIteracion(gaElegidos,
                                 v_segundosPlatos[getVar(x + 1)].gruposAl[k]))
        gaElegidos.push_back(v_segundosPlatos[getVar(x + 1)].gruposAl[k]);
    }

    // POSTRE
    valP = setValorP(getVar(x + 2));
    for (int l = 0; l < v_postres[getVar(x + 2)].gruposAl.size(); l++) {
      if (gaElegidosPorIteracion(gaElegidos,
                                 v_postres[getVar(x + 2)].gruposAl[l]))
        gaElegidos.push_back(v_postres[getVar(x + 2)].gruposAl[l]);
    }

    // Obtener el valor de la tabla de platos de compatibilidad
    // entre primeros y segundos platos
    valTabla = v3_compatibilidadPlatos[getVar(x)][getVar(x + 1)][getVar(x + 2)];
    valGAFirst = set_ValorGAFirstAlternativa(ultimos5GA, gaElegidos);
    // Obtener el valor total del numero de dias
    // desde que se repitieron grupos alimenticios
    valTotal +=
        valTabla + numPP / valPP + numSP / valSP + numP / valP + valGAFirst;

    // Suma los valores de platos y grupos alimenticios
    // elegidos para el siguiente dia
    sumValorPP();
    sumValorSP();
    sumValorP();
    sumValorGA();

    set_ultimos5GA(ultimos5GA, gaElegidos);
    gaElegidosAnterior = gaElegidos;
    gaElegidos.clear();
  }
  // Normalizacion de los objetivos
  originalCost = precioTotal;
  originalRepetition = valTotal;
  precioTotal = (precioTotal - minCost) / (maxCost - minCost);
  valTotal = (valTotal - minRepetition) / (maxRepetition - minRepetition);
  setObj(0, precioTotal);
  setObj(1, valTotal);
  ultimos5GA.clear();
  gaElegidosAnterior.clear();
}

#ifdef __MPP_PRINT_ID_S__
/**
 * En esta version nos centramos únicamente en mostrar los
 * valores de cada nutriente para comprobar la factiblidad de los
 * individuos resultantes
 **/
void MenuPlanning::print(ostream &os) const {
  for (unsigned int i = 0; i < getNumberOfVar(); i++) os << getVar(i) << " ";
  os << originalCost << " " << originalRepetition << std::endl;
}
#endif

/**
 * Método empleado para calcular la distancia entre dos individuos
 *
 */
double MenuPlanning::getEuclideanDistance(Individual *ind2) const {
  map<Food, int> f1;
  int dist = 0;
  for (int i = 0; i < nDias; i++) {
    Food f;
    f.p1 = round(getVar(i * 3));
    f.p2 = round(getVar(i * 3 + 1));
    f.p3 = round(getVar(i * 3 + 2));
    f1[f]++;
  }
  for (int i = 0; i < nDias; i++) {
    Food f;
    f.p1 = round(ind2->getVar(i * 3));
    f.p2 = round(ind2->getVar(i * 3 + 1));
    f.p3 = round(ind2->getVar(i * 3 + 2));
    if (f1.count(f)) {
      f1[f]--;
      if (f1[f] == 0) {
        f1.erase(f);
      }
    } else {
      dist++;
    }
  }
  return dist;
}

/*----------------------------------------------------------------------*/
/*---------- METODOS PARA EL CALCULO DEL GRADO DE REPETICION -----------*/
/*----------------------------------------------------------------------*/

bool MenuPlanning::gaElegidosPorIteracion(vector<int> vec, int valor) {
  bool resultado = true;
  for (int i = 0; i < vec.size(); i++)
    if (vec[i] == valor) resultado = false;
  return resultado;
}

int MenuPlanning::setValorPP(int id) {
  int valor = MAX_INT;  // Tengo que retornar el numero de dias desde que se
                        // eligio el plato por ultima vez
  // Si el numero de dias es MAX_INT, significa que nunca se ha
  // elegido, por lo que retorno 0
  if (v_primerosPlatos[id].diasRep != MAX_INT)
    valor = v_primerosPlatos[id].diasRep;

  // Si el numero de dias es distinto a MAX_INT,
  // retorno el valor y reseteo el numero de dias a 0
  v_primerosPlatos[id].diasRep = 0;

  return valor;
}

int MenuPlanning::setValorSP(int id) {
  int valor = MAX_INT;
  if (v_segundosPlatos[id].diasRep != MAX_INT)
    valor = v_segundosPlatos[id].diasRep;
  v_segundosPlatos[id].diasRep = 0;

  return valor;
}

int MenuPlanning::setValorP(int id) {
  int valor = MAX_INT;
  if (v_postres[id].diasRep != MAX_INT) valor = v_postres[id].diasRep;
  v_postres[id].diasRep = 0;

  return valor;
}

void MenuPlanning::sumValorPP(void) {
  for (int i = 0; i < v_primerosPlatos.size(); i++)
    if (v_primerosPlatos[i].diasRep != MAX_INT) v_primerosPlatos[i].diasRep++;
}

void MenuPlanning::sumValorSP(void) {
  for (int i = 0; i < v_segundosPlatos.size(); i++)
    if (v_segundosPlatos[i].diasRep != MAX_INT) v_segundosPlatos[i].diasRep++;
}

void MenuPlanning::sumValorP(void) {
  for (int i = 0; i < v_postres.size(); i++)
    if (v_postres[i].diasRep != MAX_INT) v_postres[i].diasRep++;
}

void MenuPlanning::sumValorGA(void) {
  for (int i = 0; i < gruposAl.size(); i++) {
    if (gruposAl[i].first != MAX_INT) gruposAl[i].first++;
    gruposAl[i].second = MAX_INT;
  }
}

// METODO PARA ANIADIR LOS GA DE UN DIA EN EL VECTOR DE GAS DE LOS ULTIMOS 5
// DIAS
void MenuPlanning::set_ultimos5GA(vector<vector<int>> &ultimos5GA,
                                  vector<int> vec) {
  if (ultimos5GA.size() < 5)
    ultimos5GA.push_back(vec);
  else {
    ultimos5GA.erase(ultimos5GA.begin());
    ultimos5GA.push_back(vec);
  }
}

double MenuPlanning::set_ValorGAFirstAlternativa(
    vector<vector<int>> &ultimos5GA, vector<int> vec) {
  /*
   * 0 Otros, 1 Carne, 2 Cereal, 3 Fruta, 4 Lacteo, 5 Legumbre, 6 Marisco, 7
   * Pasta, 8 Pescado, 9 Verdura */
  double penalizacionPorGA[10] = {penalizaciones[0], penalizaciones[1],
                                  penalizaciones[2], penalizaciones[3],
                                  penalizaciones[4], penalizaciones[5],
                                  penalizaciones[6], penalizaciones[7],
                                  penalizaciones[8], penalizaciones[9]};
  double penalizacionPorDias[5] = {penalizaciones[10], penalizaciones[11],
                                   penalizaciones[12], penalizaciones[13],
                                   penalizaciones[14]};
  bool pen[5] = {false, false, false, false, false};
  double resultado = 0;

  if (ultimos5GA.size() > 0) {
    for (int i = 0; i < vec.size(); i++) {
      for (int j = 0; j < ultimos5GA.size(); j++)
        for (int k = 0; k < ultimos5GA[j].size(); k++) {
          if (vec[i] == ultimos5GA[j][k]) {
            pen[j] = true;
            resultado += penalizacionPorGA[vec[i]];
          }
        }
    }
    for (int x = 0; x < 5; x++)
      if (pen[x]) {
        resultado += penalizacionPorDias[x];
        pen[x] = false;
      }
  }

  return resultado;
}

void MenuPlanning::mostrarPlatos(void) {
  cout << "\n\nPRIMEROS PLATOS";
  for (int i = 0; i < v_primerosPlatos.size(); i++) {
    cout << "\nNombre: " << v_primerosPlatos[i].nombre;
    cout << "\ndiasrep: " << v_primerosPlatos[i].diasRep;
    cout << "\nprecio: " << v_primerosPlatos[i].precio;
    cout << "\ncantidad: " << v_primerosPlatos[i].cantidad;
    cout << "\ngrupos Al:";
    for (int x = 0; x < v_primerosPlatos[i].gruposAl.size(); x++) {
      cout << "\n- " << v_primerosPlatos[i].gruposAl[x];
    }
    cout << "\nInfo Nut:";
    for (int x = 0; x < v_primerosPlatos[i].infoN.size(); x++) {
      cout << "\n- " << v_primerosPlatos[i].infoN[x];
    }
    cout << "\nAlergenos:";
    for (int x = 0; x < v_primerosPlatos[i].alg.size(); x++) {
      cout << "\n- " << v_primerosPlatos[i].alg[x];
    }

    cout << "\nIncompatibilidades:";
    for (int x = 0; x < v_primerosPlatos[i].inc.size(); x++) {
      cout << "\n- " << v_primerosPlatos[i].inc[x];
    }
    cin.get();
  }

  cout << "\n\nSEGUNDOS PLATOS";
  for (int i = 0; i < v_segundosPlatos.size(); i++) {
    cout << "\nNombre: " << v_segundosPlatos[i].nombre;
    cout << "\ndiasrep: " << v_segundosPlatos[i].diasRep;
    cout << "\nprecio: " << v_segundosPlatos[i].precio;
    cout << "\ncantidad: " << v_segundosPlatos[i].cantidad;
    cout << "\ngrupos Al:";
    for (int x = 0; x < v_segundosPlatos[i].gruposAl.size(); x++) {
      cout << "\n- " << v_segundosPlatos[i].gruposAl[x];
    }
    cout << "\nInfo Nut:";
    for (int x = 0; x < v_segundosPlatos[i].infoN.size(); x++) {
      cout << "\n- " << v_segundosPlatos[i].infoN[x];
    }
    cout << "\nAlergenos:";
    for (int x = 0; x < v_segundosPlatos[i].alg.size(); x++) {
      cout << "\n- " << v_segundosPlatos[i].alg[x];
    }

    cout << "\nIncompatibilidades:";
    for (int x = 0; x < v_segundosPlatos[i].inc.size(); x++) {
      cout << "\n- " << v_segundosPlatos[i].inc[x];
    }
    cin.get();
  }

  cout << "\n\nPOSTRES";
  for (int i = 0; i < v_postres.size(); i++) {
    cout << "\nNombre: " << v_postres[i].nombre;
    cout << "\ndiasrep: " << v_postres[i].diasRep;
    cout << "\nprecio: " << v_postres[i].precio;
    cout << "\ncantidad: " << v_postres[i].cantidad;
    cout << "\ngrupos Al:";
    for (int x = 0; x < v_postres[i].gruposAl.size(); x++) {
      cout << "\n- " << v_postres[i].gruposAl[x];
    }
    cout << "\nInfo Nut:";
    for (int x = 0; x < v_postres[i].infoN.size(); x++) {
      cout << "\n- " << v_postres[i].infoN[x];
    }
    cout << "\nAlergenos:";
    for (int x = 0; x < v_postres[i].alg.size(); x++) {
      cout << "\n- " << v_postres[i].alg[x];
    }

    cout << "\nIncompatibilidades:";
    for (int x = 0; x < v_postres[i].inc.size(); x++) {
      cout << "\n- " << v_postres[i].inc[x];
    }
    cin.get();
  }
}
