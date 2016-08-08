#ifndef NFISHINDI_H
#define NFISHINDI_H
/*
Danial Yazdani, Babak Nasiri, Alireza Sepas-Moghaddam, Mohammadreza Meybodi, Mohammadreza Akbarzadeh-Totonchi, 
mNAFSA: A novel approach for optimization in dynamic environments with global changes, Swarm and Evolutionary 
Computation, Volume 18, October 2014, Pages 38-53, ISSN 2210-6502, http://dx.doi.org/10.1016/j.swevo.2014.05.002.
*/
#include "../../Individual.h"
class NFishSwarm;
class NFish:public Individual<CodeVReal>{  //new fish definition
	friend class NFishSwarm;
public:
	ReturnFlag prey(double visual);
	ReturnFlag follow(const Solution<CodeVReal> & best, double visual);
	ReturnFlag swarm(const Solution<CodeVReal> & center,double visual);
};

#endif