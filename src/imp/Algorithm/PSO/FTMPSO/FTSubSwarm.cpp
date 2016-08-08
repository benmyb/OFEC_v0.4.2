#include "FTSubSwarm.h"
#include "../../../Problem/DOP/MovingPeak.h"

FTSubSwarm::FTSubSwarm(int size,bool mode):Swarm<CodeVReal,FTParticle>(size,mode),m_etry(20),m_rCloud(0.2),m_minCF(0.8),m_L(0.4){

	if(IS_PROBLEM_NAME(Global::ms_curProId,"DYN_CONT_MovingPeak")){
		m_rCloud=0.2*dynamic_cast<MovingPeak*>(Global::msp_global->mp_problem.get())->getVLength();
	}
	setW(0.729843788);
	setC2(2.05);
	setC1(2.05);
	m_flag.resize(2);
}
ReturnFlag FTSubSwarm::localSearch(){
	if(m_flag[0]||m_flag[1]) return Return_Normal;
	int i(0);
	ReturnFlag rf=Return_Normal;
	while(i<m_etry){
		for(int d=0;d<GET_NUM_DIM;d++){
			m_exploiter.data()[d]=m_best[0]->data().m_x[d]+Global::msp_global->getRandFloat(-m_rCloud,m_rCloud);
		}
		rf=m_exploiter.evaluate();
		updateBestArchive(m_exploiter);
		if(rf!=Return_Normal) break;
		++i;
	}
	m_rCloud*=m_minCF*(Global::msp_global->mp_uniformAlg->Next()*(1-m_minCF));
	return rf;
}

ReturnFlag FTSubSwarm::evolve(){
	
	if(m_flag[0]||m_flag[1]) return Return_Normal;	// in asleep mode

	ReturnFlag r_flag=Return_Normal;

	for(int i=0;i<this->m_popsize;i++){
        r_flag=this->m_pop[i]->move(neighborBest(i),m_W,m_C1,m_C2);//
			
		if(this->m_pop[i]->self()>this->m_pop[i]->m_pbest){
			this->m_pop[i]->m_pbest=this->m_pop[i]->self();
			this->updateBestArchive(this->m_pop[i]->self());
		}

		if(r_flag!=Return_Normal) break;

	}
	
	++m_iter;
	return r_flag;
}

bool FTSubSwarm::sleep(){
	m_flag[1]=true;			
	for(int i=0;i<m_popsize;i++){
		for(auto &v:m_pop[i]->m_vel){
			if(v<-m_L||v>m_L) {
				m_flag[1]=false; // set it active status
				break;
			}
		}
		if(!m_flag[1]) break;
	}
	return m_flag[1];
}

void FTSubSwarm::afterChange(){
	
	for(auto &par:m_pop){
		for(int d=0;d<GET_NUM_DIM;d++){
			if(IS_PROBLEM_NAME(Global::ms_curProId,"DYN_CONT_MovingPeak")) 
				par->data()[d]=(*m_best[0]).data()[d]+Global::msp_global->getRandFloat(-1,1)*0.5*dynamic_cast<MovingPeak*>(Global::msp_global->mp_problem.get())->getVLength();
			else par->data()[d]=(*m_best[0]).data()[d]+Global::msp_global->getRandFloat(-1,1)*0.5;

			if(IS_PROBLEM_NAME(Global::ms_curProId,"DYN_CONT_MovingPeak")) 
				par->m_vel[d]=Global::msp_global->getRandFloat(-1,1)*0.5*dynamic_cast<MovingPeak*>(Global::msp_global->mp_problem.get())->getVLength();
			else par->m_vel[d]=Global::msp_global->getRandFloat(-1,1)*0.5;

			if(CAST_PROBLEM_DYN&&CAST_PROBLEM_DYN->predictChange(2) || CAST_PROBLEM_DYN_ONEPEAK&&CAST_PROBLEM_DYN_ONEPEAK->predictChange(2)) par->self().evaluate(false);
			else		par->self().evaluate(true);
			
			par->m_pbest=par->self();
		}
	}

	findBest();
	m_best.clear();
	for(auto &i:m_bestIdx){
		updateBestArchive(m_pop[i]->representative());
	}

}