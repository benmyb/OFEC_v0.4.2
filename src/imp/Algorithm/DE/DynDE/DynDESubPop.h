#ifndef DYNDESUBPOP_H
#define DYNDESUBPOP_H
#include "DynDEIndividual.h"
#include "../DEPopulation.h"
class DynDEPopulation;
class DynDESubPop:public DEPopulation<CodeVReal,DynDEIndividual> {
public:
	friend class DynDEPopulation;
	DynDESubPop(int size,bool flag=true);
	void assignType();
protected:
	ReturnFlag evolve();
private:
	    int m_NumNormal;    // the number of normal individuals of each swarm
        int m_NumBrownian;       // the number of brownian individuals of each swarm
        int ms_NumQuantum;    // the number of quantum individuals of each swarm
		double m_Rcloud; // radius of quantum swarms
		double m_sigma;	// deviation for generation of Brownian individuals
};
#endif