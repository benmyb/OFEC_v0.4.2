#ifndef SOSSUBPOP_H
#define SOSSUBPOP_H

#include "../GAIndividual.h"
#include "../GAPopulation.h"
#include "../../PopulationCont.h"

class SOSPopulation;
class SOSSubPop:public PopulationCont<CodeVReal,GAIndividual<CodeVReal>,GAPopulation<CodeVReal,GAIndividual<CodeVReal>> >{
	friend class SOSPopulation;
public:
	using GAPopulation<CodeVReal,GAIndividual<CodeVReal>>::add;
	SOSSubPop();
	SOSSubPop(int size, bool mode);
	SOSSubPop & operator=(const SOSSubPop & rhs);
	ReturnFlag add(int num,  bool mode,  bool insize);
protected:
	ReturnFlag evolve();
private:
	double m_dynamism;
    double m_quality;
    int m_desiredSize;
    double m_bestFitPreIter;
    int m_unimpr;
};
#endif