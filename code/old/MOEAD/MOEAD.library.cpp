#include "MOEAD.h"

extern "C" {
	EA *maker(){
		return new MOEAD();
	}
}
