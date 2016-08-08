#include "DynPopDESubPop.h"


DynPopDESubPop::DynPopDESubPop(int size, int numBrowian,bool mode):DEPopulation<CodeVReal,DynDEIndividual>(size,mode),m_numBrownian(numBrowian){

    m_flag[f_reinit]=(false);
	assignType();
	setMutationStrategy(DE_best_2);
	m_bestLastIter=m_best[0]->obj(0);
	m_penalty=0;
	m_performance=0;
	m_improvement =0;
	m_sigma=0.2;
}

void DynPopDESubPop::assignType(){

	// for the version with the suggested settings
	// select the worst m_numBrownian indis as brownian indis
	for(int i=0;i<m_popsize;i++){
		m_pop[i]->m_type=DynDEIndividual::IndividualType::TYPE_DE;
	}
	for(int k=0;k<m_numBrownian;k++){
		int wst=0;
		while(m_pop[wst]->m_type!=DynDEIndividual::IndividualType::TYPE_DE) wst++;

		for(int i=wst+1;i<m_popsize;i++){
			if(m_pop[i]->m_type!=DynDEIndividual::IndividualType::TYPE_DE) continue;
			if(m_pop[i]->self()<m_pop[wst]->self()) wst=i;
		}
		m_pop[wst]->m_type=DynDEIndividual::IndividualType::TYPE_BROWNIAN;
	}
}

ReturnFlag DynPopDESubPop::evolve(){
	ReturnFlag rf=Return_Normal;
     if(this->m_popsize<1){
		 return rf;
     }

    for(int i=0;i<this->m_popsize;i++){
		if(m_pop[i]->m_type==DynDEIndividual::IndividualType::TYPE_DE){
			mutate(i);
			this->m_pop[i]->recombine(m_CR);
		}else if(m_pop[i]->m_type==DynDEIndividual::IndividualType::TYPE_BROWNIAN){
			rf=m_pop[i]->brownian(m_best[0]->self(),m_sigma);
			
			if(rf!=Return_Normal) return rf;
	
		}		
			
    }

    this->updateIDnIndex();
    for(int i=0;i<this->m_popsize;i++){
		if(m_pop[i]->m_type==DynDEIndividual::IndividualType::TYPE_DE){
			rf=m_pop[i]->select();
			if(rf!=Return_Normal) return rf;
		}
				
        updateBestArchive(this->m_pop[i]->self());	
			
    }
    this->m_iter++;
	
    return rf;

}
void DynPopDESubPop::setInitialRadius(double r){
	this->m_initialRadius=r;
}
ReturnFlag DynPopDESubPop::initialize(bool mode){
	ReturnFlag rf=Population<CodeVReal,DynDEIndividual>::initialize(false,mode,true);
	assignType();
	setMutationStrategy(DE_best_2);
	m_bestLastIter=m_best[0]->obj(0);
	m_penalty=0;
	m_performance=0;
	m_improvement =0;
	return rf;
}

void DynPopDESubPop::updateInfor(DynPopDESubPop *worstPop){
	// update perforance, penalty 
	m_improvement=fabs(m_bestLastIter-m_best[0]->obj(0));
	m_performance=(m_improvement+1)*(1.+fabs(m_best[0]->obj(0)-worstPop->m_best[0]->obj(0)));
	
	//penalty factor
	if(m_improvement>0) m_penalty=0;
	else m_penalty+=1;

	//performacne with penalty 
	if(m_penalty>0) m_performance/=m_penalty;
	 
	m_bestLastIter=m_best[0]->obj(0);
	
}