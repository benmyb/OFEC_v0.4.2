#ifndef MQSOSWARM_H
#define MQSOSWARM_H

/*
Blackwell, T.; Branke, J.; , "Multiswarms, exclusion, and anti-convergence in dynamic environments,"
Evolutionary Computation, IEEE Transactions on , vol.10, no.4, pp.459-472, Aug. 2006
*/

#include "../Swarm.h"
#include "../../MultiPopulationCont.h"
#include "MQSOSubSwarm.h"

class MQSOSwarm: public Algorithm, public MultiPopulationCont<MQSOSubSwarm>{	
public:
	
	MQSOSwarm(ParamMap &v);
    ReturnFlag run_();
protected:
	void initialize();
	bool checkConvergenceAll();
    void exclude();
private:
	int m_M;        // the number of populations
	double m_Rconv; // threshold radius of conveged swarms
	double m_Rexcl; // radius of exlusion radius 
	bool m_exclusion;	// 1 for with exclusion and 0 no exclustion	
		
};


#endif