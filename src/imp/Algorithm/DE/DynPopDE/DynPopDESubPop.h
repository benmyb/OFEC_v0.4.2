#ifndef DYNPOPDESUBPOP_H
#define DYNPOPDESUBPOP_H
#include "../DynDE/DynDEIndividual.h"
#include "../DEPopulation.h"

class DynPopDEPopulation;
class DynPopDESubPop: public DEPopulation<CodeVReal,DynDEIndividual>{
	friend class DynPopDEPopulation;
	enum{f_reinit=0};
private:
	
	double m_sigma;	// deviation for generation of Brownian individuals
	double m_bestLastIter;
	double m_performance;
	double m_improvement;
	double m_penalty;
	int m_numBrownian;       // the number of brownian individuals of each swarm
protected:
	ReturnFlag evolve();
	void updateInfor(DynPopDESubPop *worstPop);
	void assignType();
	void setInitialRadius(double r);
public: 
	ReturnFlag initialize(bool mode=true);
	DynPopDESubPop(int size, int,bool mode=true);
};
#endif