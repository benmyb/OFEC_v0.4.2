#ifndef FTSWARM_H
#define FTSWARM_H

#include "FTSubSwarm.h"
#include "../../MultiPopulationCont.h"

/*D. Yazdani, B. Nasiri, A. Sepas-Moghaddam, and M. R. Meybodi. A novel multi-swarm algorithm for optimization in dynamic environments
based on particle swarm optimization. Applied Soft Computing, 13(4):2144 ¨C 2158, 2013.

issues: no actions on inactive swarms causing the number of inactive swarms getting larger and larger. 
In this implementation, all inactive swarms are taken as asleep swarms
*/

class FTSwarm:  public Algorithm, public MultiPopulationCont<FTSubSwarm>{
private:
	double m_rExcl,m_convLimit;
	int m_k;
	int m_numActive;
	deque<Solution<CodeVReal>> m_preFinderBest;
public:
	FTSwarm(ParamMap &v);
	ReturnFlag run_();
private:
	void excludeFinder();
	void excludeTracker();
	bool detectChange();
	void responseChange();
	int isFinderInTracker();
	void activate();
	void printActive();
	void removeInactive();
};
#endif