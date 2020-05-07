#include "MenuPlanning.h"

extern "C" {
	Individual *maker(){
		return new MenuPlanning();
	}
}
