
/**
 *  Alejandro Marrero - alu0100825008@ul.edu.es
 *  06-07-2020
 **/

#ifndef __MENUPLANNING_H__
#define __MENUPLANNING_H__

#include <stdlib.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "Constants.h"
#include "Individual.h"

#define __MPP_FEASIBILITY_DEBUG__
#define __MPP_PRINT_ID_S__

char *getcwd(char *buf, size_t size);

#define CROSSOVER_UNIFORM 0
#define CROSSOVER_HUX 1

// Struct que almacena informacion de los platos utilizada en
// el proceso de creacion del plan
struct infoPlatos {
  string nombre;   // Nombre del plato
  int diasRep;     // Numero de dias desde que se eligio el plato en el plan por
                   // ultima vez
  float precio;    // Precio del plato
  float cantidad;  // Cantidad en gramos
  vector<int> gruposAl;  // Grupos alimenticios correspondientes a los
                         // ingredientes principales del plato
  vector<float> infoN;   // Informacion nutricional del plato
  vector<string> alg;    // Alergenos del plato
  vector<string> inc;    // Incompatibilidades alimenticias del plato
};

/**
 * Struct empleada en el PairBasedCrossover
 **/
struct Food {
  int p1, p2, p3;
  bool operator<(const Food &i2) const {
    return (make_pair(p1, make_pair(p2, p3)) <
            make_pair(i2.p1, make_pair(i2.p2, i2.p3)));
  }
};

/**
 * Struct para definir Vecinos del MPP CEC 2019
 **/
struct Neighbor {
  int variable;
  int newValue;
};

class MenuPlanning : public Individual {
 private:
  // Variables comunes a todas las instancias de MenuPlanning
  static int nDias;   // Numero de dias que tenemos que planificar
  static int nParam;  // Numero de parametos = nDias * 3
  static vector<infoPlatos> v_primerosPlatos;  // Primeros platos
  static vector<infoPlatos> v_segundosPlatos;  // Segundos platos
  static vector<infoPlatos> v_postres;         // Postres
  static vector<pair<int, int> > gruposAl;     // Grupos de alimentos
  // Compatibilidad entre los platos
  static vector<vector<vector<double> > > v3_compatibilidadPlatos;
  static vector<string> alergenosPlan;           // Alergenos del plan
  static vector<string> incompatibilidadesPlan;  // Incomptabilidades del plan
  static vector<int> gruposAlPlan;  // Grupos de alimentos en el plan
  static vector<double> infoNPlan;  // Informacion Nutrional del plan

  // Variables empleadas en CEC 2019 para gestionar ID(S) y ILS
  vector<int> badDays;
  double heaviestNut;
  double heaviestType;
  double originalCost, originalRepetition;
  bool partial;
  double requiredFeasibility;

  // Variables para la normalizacion de objetivos en la ILS
  static double minCost, maxCost;
  static double minRepetition, maxRepetition;
  void dependentCrossover(Individual *ind);  // Operadro de cruce especifico
  void dependentMutation(double pm);         // Operador de mutacion específico

 public:
  MenuPlanning();
  virtual ~MenuPlanning();

  bool init(const vector<string> &params);  // METCO init
  void evaluate(void);  // Metodo de evaluacion de individuos MenuPlanning
  Individual *clone(void) const;  // Clonacion de un individuo MenuPlanning
  void restart(void);             // Generacion aleatoria del individuos
  void set_gruposAl(void);        // Metodo que define los grupos de alimentos
  double computeFeasibility();    // Calcula la factibilidad de los individuos
  double computingFitnessValue();

  static void setObjectivesRanges(const int &);

  // Operadores de cruce para MPP basados en MPP CEC 2019
  void uniformCrossover(Individual *ind2);
  void pairBasedCrossover(Individual *ind2);
  void updateInnerReference();

  // Búsqueda Local HillClimbing basada en MPP CEC 2019
  virtual void dependentLocalSearch();
  double getEuclideanDistance(Individual *ind) const;

#ifdef __MPP_PRINT_ID_S__
  // Imprime los datos de un individuo MenuPlanning
  virtual void print(std::ostream &os) const;
#endif

  // Lectura de ficheros de platos
  void set_Platos(void);
  void set_VectoresPlatos(const char *c_filename, vector<infoPlatos> &v_vecP);

  void set_vectorCompatibilidad(void);  // Calcula la compatibilidad
  void set_GAElegidos(vector<int> gal, vector<bool> &galE);
  double set_penalizacionVC(vector<int> &gal, vector<bool> galE);

  // Comprobar las restricciones del problema
  bool checkInfoN(const int i);
  bool checkInfoN2(void);

  // Metodos para el calculo del objetivo de grado de repeticion
  bool gaElegidosPorIteracion(vector<int> vec, int valor);
  int setValorPP(int id);
  int setValorSP(int id);
  int setValorP(int id);
  void sumValorPP(void);
  void sumValorSP(void);
  void sumValorP(void);
  void sumValorGA(void);
  void set_ultimos5GA(vector<vector<int> > &ultimos5GA, vector<int> vec);
  double set_ValorGAFirstAlternativa(vector<vector<int> > &ultimos5GA,
                                     vector<int> vec);

  unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }

  double inline getMinimum(const int i) const { return 1; }

  double inline getMaximum(const int i) const {
    if (i % num_tipoPlato == 0)
      return v_primerosPlatos.size();
    else if (i % num_tipoPlato == 1)
      return v_segundosPlatos.size();
    else if (i % num_tipoPlato == 2)
      return v_postres.size();

    else {
      std::cout << "Error in getMaximum. I = " << i << std::endl;
      exit(-1);
    }
  }

  // PRUEBA
  void mostrarPlatos(void);

 private:
  static string pathToDB;
  static const int N_OBJS;
  static const int MAX_INT;
  static const int N_ARGS;
};

#endif
