#include "CPSORSubSwarm.h"

CPSORParticle::CPSORParticle():Particle(){

}
CPSORParticle::CPSORParticle( const Solution<CodeVReal> &chr):Particle(chr){
		
}

///**** Begin of definition of member functions of class CPSORSubSwarm ***///
CPSORSubSwarm::CPSORSubSwarm(int size, bool flag):Swarm<CodeVReal,CPSORParticle>(size,flag){

}
CPSORSubSwarm::CPSORSubSwarm(Group<CodeVReal,CPSORParticle> &g):Swarm<CodeVReal,CPSORParticle>(g){

}

ReturnFlag CPSORSubSwarm::evolve(){
	if(this->m_popsize<1) return Return_Normal;
	Solution<CodeVReal> t;
	ReturnFlag r_flag=Return_Normal;

	for(int i=0;i<this->m_popsize;i++){
		t=this->m_pop[i]->self();
		 bool flag=false;  
        r_flag=this->m_pop[i]->moveBound(neighborBest(i),m_W,m_C1,m_C2);//
			
		if(this->m_pop[i]->self()>this->m_pop[i]->m_pbest){
			this->m_pop[i]->m_pbest=this->m_pop[i]->self();
			flag=this->updateBestArchive(this->m_pop[i]->self());
		}

		if(r_flag!=Return_Normal) break;
		if(flag&&this->m_pop[i]->self()>t){
			r_flag=updateBest(i,1.0);
		}
		if(r_flag!=Return_Normal)  {  break;}
		
	}
	if(r_flag==Return_Normal){
		this->m_iter++;
	}
	computeCenter();
	updateCurRadius(true);

	return r_flag;
}