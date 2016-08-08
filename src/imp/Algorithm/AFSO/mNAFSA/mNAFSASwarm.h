#ifndef MNFISHSWARM_H
#define MNFISHSWARM_H
#include "NFishSwarm.h"
#include "../../MultiPopulationCont.h"

/* Danial Yazdani, Babak Nasiri, Alireza Sepas-Moghaddam, Mohammadreza Meybodi, Mohammadreza Akbarzadeh-Totonchi, mNAFSA: 
A novel approach for optimization in dynamic environments with global changes, Swarm and Evolutionary Computation, 
Volume 18, October 2014, Pages 38-53, ISSN 2210-6502, http://dx.doi.org/10.1016/j.swevo.2014.05.002. */


class mNAFSASwarm: public Algorithm,public MultiPopulationCont<NFishSwarm>{
private:
	double m_sleepRadius,m_convRadius;
	int m_count;	// for checking covergence
	double m_exlRadius;
	double m_shiftLength;
public:
	mNAFSASwarm(ParamMap &v);
	ReturnFlag run_();
	ReturnFlag exlude();
	bool isConvergingAll();
protected:
	void estimateShiftLength();
};
#endif