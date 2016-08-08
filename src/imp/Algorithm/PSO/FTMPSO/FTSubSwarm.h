#ifndef FTSUBSWARM_H
#define FTSUBSWARM_H

#include "FTParticle.h"
#include "../Swarm.h"
/*D. Yazdani, B. Nasiri, A. Sepas-Moghaddam, and M. R. Meybodi. A novel multi-swarm algorithm for optimization in dynamic environments
based on particle swarm optimization. Applied Soft Computing, 13(4):2144 ¨C 2158, 2013.

issues: no actions on inactive swarms causing the number of inactive swarms getting larger and larger. 
In this implementation, all inactive swarms are taken as asleep swarms
*/

class FTSubSwarm: public Swarm<CodeVReal,FTParticle>{
	friend class FTSwarm;
private:
	int m_etry;
	FTParticle m_exploiter;
	double m_rCloud,m_minCF,m_L;
public:
	FTSubSwarm(int size,bool mode=true);
	ReturnFlag localSearch();
	ReturnFlag evolve();
	bool sleep();
	void afterChange();
};

#endif