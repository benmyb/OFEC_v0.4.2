#include "MQSOSubSwarm.h"
#include<float.h>

MQSOSubSwarm::MQSOSubSwarm(int size, bool flag):Swarm<CodeVReal,MQSOParticle>(size,flag),m_Nplus(5),m_Nq(5),\
    m_N(5),m_Q(pow(1./4.9,1./0.6)),m_Rcloud(1.0){

	assignType();
	m_swarmType=2;

}

void MQSOSubSwarm::assignType(){
	if(IS_ALG_NAME(Global::ms_curAlgId,"ALG_mCPSO"))	{
        for(int i=0;i<m_popsize;i++){
			if(i<m_N) m_pop[i]->m_type=MQSOParticle::PARTICLE_NEUTRAL;
            else  m_pop[i]->m_type=MQSOParticle::PARTICLE_CHARGED;       
		}
    }else{
        for(int i=0;i<m_popsize;i++){
            if(i<m_N) m_pop[i]->m_type=MQSOParticle::PARTICLE_NEUTRAL;
            else  m_pop[i]->m_type=MQSOParticle::PARTICLE_QUANTUM;
        }
    }
}

void MQSOSubSwarm::computeRepulsion(const int idx){
	for(int d=0;d<m_numDim;d++){
		m_pop[idx]->mv_repulse[d]=0;
	}
	for(int i=0;i<m_popsize;i++){
		if(i!=idx&&m_pop[i]->m_type==MQSOParticle::PARTICLE_CHARGED){
			double m=0,x,y;
			for(int d=0;d<m_numDim;d++){
				x=m_pop[idx]->data()[d];
				y=m_pop[i]->data()[d];
				m+=(x-y)*(x-y);
			}
			m=sqrt(m);
			m=pow(m,3.);
			for(int d=0;d<m_numDim;d++){
				x=m_pop[idx]->data()[d];
				y=m_pop[i]->data()[d];
				m_pop[idx]->mv_repulse[d]+=(x-y)*m_Q*m_Q/m;
			}
		}
	}

	// clamp to max<double>
	long double m=0;
	for(int d=0;d<m_numDim;d++){
		m+=m_pop[idx]->mv_repulse[d]*m_pop[idx]->mv_repulse[d];
	}
	m=sqrt(m);
	if(m>DBL_MAX ){
		for(int d=0;d<m_numDim;d++){
			m_pop[idx]->mv_repulse[d]=m_pop[idx]->mv_repulse[d]*DBL_MAX /m;
		}
	}
}

ReturnFlag MQSOSubSwarm::evolve(){
    if(this->m_popsize<1) return Return_Normal;

	ReturnFlag rf=Return_Normal;
	for(int i=0;i<this->m_popsize;i++){       
		if(m_pop[i]->m_type==MQSOParticle::PARTICLE_CHARGED) computeRepulsion(i);
		rf=m_pop[i]->move(neighborBest(i),m_W,m_C1,m_C2,m_Rcloud);

        if(rf!=Return_Normal) break;
		if(this->m_pop[i]->self()>this->m_pop[i]->m_pbest){
			this->m_pop[i]->m_pbest=this->m_pop[i]->self();
			this->updateBestArchive(this->m_pop[i]->self());
		}
	}
	m_iter++;
    return rf;
}