#ifndef MQSOSUBSWARM_H
#define MQSOSUBSWARM_H

#include "../Swarm.h"
#include "MQSOParticle.h"

class MQSOSwarm;
class SAMOSwarm;
class MQSOSubSwarm: public Swarm<CodeVReal,MQSOParticle>{

public:
	friend class MQSOSwarm;
	friend class SAMOSwarm;
	MQSOSubSwarm(int size,bool flag);
	~MQSOSubSwarm(){}
	
protected:
	void assignType();
	void computeRepulsion(const int idx);
	ReturnFlag evolve(); 

private:
	int m_Nplus;    // the number of charged particles of each swarm
    int m_Nq;       // the number of quantum particles of each swarm
    int m_N;        // the number of neutral particles of each swarm
	double m_Q;     // charge of particles
	double m_Rcloud; // radius of quantum swarms
	int m_swarmType;		//0 for converged swarm, 1 for converging swarm, 2 for free swarm
};


#endif