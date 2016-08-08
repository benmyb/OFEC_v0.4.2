#ifndef FISHSWARM_H
#define FISHSWARM_H

#include "NFishIndi.h"
#include "../../PopulationCont.h"
class mNAFSASwarm;
class NFishSwarm:public PopulationCont<CodeVReal, NFish>{
	friend class mNAFSASwarm;
	enum{f_sleeping=0,f_est_s,f_excl};
private:
	double m_visual;
	int m_tryNumber;
	double m_limit;
	Solution<CodeVReal> m_bestLastEnvir;
	list<Solution<CodeVReal>> m_preBest;
public:
	NFishSwarm(ParamMap & v);
	NFishSwarm(int size,bool flag);
	ReturnFlag run_();
	ReturnFlag evolve();
	void updateVisual();
	void setVisual(double);
	void sleep(double dis,double r);
	void wakeup();
	bool isConverged(int count,double r);
	void updatePreBest(int);
	void computeCenter();
};

#endif