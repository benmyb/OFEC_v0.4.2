#include "CooperativeSubSwarm.h"

CooperativeSubSwarm::CooperativeSubSwarm() :Swarm()
{
}

CooperativeSubSwarm::CooperativeSubSwarm(int popsize,int ID,int popNum,bool mode) :Swarm(popsize,mode)
{
	initializePara(0.7298,1.49445,1.49445);
	m_popID=ID;

	int k=Global::msp_global->mp_problem->getNumDim()%popNum;
	if(m_popID<k)
		m_numbers=static_cast<int>(ceil(Global::msp_global->mp_problem->getNumDim()/static_cast<double>(popNum)));
	else
		m_numbers=static_cast<int>(floor(Global::msp_global->mp_problem->getNumDim()/static_cast<double>(popNum)));
	mv_members.resize(m_numbers);
	for(int i=0;i<m_numbers;i++){
		if(m_popID<k) mv_members[i]=static_cast<int>(ceil(Global::msp_global->mp_problem->getNumDim()/static_cast<double>(popNum)))*m_popID+i;
		else mv_members[i]=static_cast<int>(ceil(Global::msp_global->mp_problem->getNumDim()/static_cast<double>(popNum)))*k+static_cast<int>(floor(Global::msp_global->mp_problem->getNumDim()/static_cast<double>(popNum)))*(m_popID-k)+i;
	}
}

ReturnFlag CooperativeSubSwarm::evolve(Solution<CodeVReal> & msp_gbest)
{
	ReturnFlag rf=Return_Normal;
	for(int i=0;i<m_popsize;i++)
	{
		rf=m_pop[i]->move(*m_best[0],mv_members,m_numbers,msp_gbest,m_W,m_C1,m_C2);
		if(rf!=Return_Normal) return rf;

		if(m_pop[i]->self()>m_pop[i]->representative())
		{
			m_pop[i]->representative()=m_pop[i]->self();
		
			if(m_pop[i]->self()>*m_best[0]) {
				*m_best[0]=m_pop[i]->self();
				if(m_pop[i]->self()>msp_gbest){
					msp_gbest=m_pop[i]->self();
				}
			}

		}
	}
	return rf;
}

