#include "DynDESubPop.h"

DynDESubPop::DynDESubPop(int size,bool flag):DEPopulation<CodeVReal,DynDEIndividual>(size,flag),m_NumNormal(5), m_NumBrownian(5), ms_NumQuantum(5),\
 m_Rcloud(1.0 ),m_sigma(0.2){
// set m_Rcloud to shift lenght, default for shift length =1.0 if not known 
//default configuration (N,N+) or (N,Nq)= (5,5)
	assignType();
	setMutationStrategy(DE_best_2);
}
void DynDESubPop::assignType(){

	// for the version with the suggested settings 
    for(int i=0;i<m_popsize;i++){
        if(i<m_NumNormal) m_pop[i]->m_type=DynDEIndividual::IndividualType::TYPE_DE;
        else  m_pop[i]->m_type=DynDEIndividual::IndividualType::TYPE_BROWNIAN;
    }
 
}


ReturnFlag DynDESubPop::evolve(){
     if(this->m_popsize<1)		 return Return_Normal;
        
	 ReturnFlag rf=Return_Normal;
    for(int i=0;i<this->m_popsize;i++){
		if(m_pop[i]->m_type==DynDEIndividual::IndividualType::TYPE_DE||m_pop[i]->m_type==DynDEIndividual::IndividualType::TYPE_ENTROPY_DE){
			//m_pop[i]->printToScreen();
			mutate(i);
			this->m_pop[i]->recombine(m_CR);
			//m_pop[i]->printToScreen();
		}else if(m_pop[i]->m_type==DynDEIndividual::IndividualType::TYPE_BROWNIAN){
			rf=m_pop[i]->brownian(m_best[0]->self() ,m_sigma);
	
		}else if(m_pop[i]->m_type==DynDEIndividual::IndividualType::TYPE_QUANTUM){
			rf=m_pop[i]->quantum(m_best[0]->self(),m_Rcloud);
		}
		if(rf!=Return_Normal)  return rf;				
    }
	
    this->updateIDnIndex();
	

    for(int i=0;i<this->m_popsize;i++){
		if(m_pop[i]->m_type==DynDEIndividual::IndividualType::TYPE_DE||m_pop[i]->m_type==DynDEIndividual::IndividualType::TYPE_ENTROPY_DE){
			//m_pop[i]->printToScreen();
			rf=m_pop[i]->select();
			//m_pop[i]->printToScreen();
			//getchar();
			if(rf!=Return_Normal) return rf;
		}

		if(m_pop[i]->m_type==DynDEIndividual::IndividualType::TYPE_ENTROPY_DE){
			// add entropy if necessary 
			rf=m_pop[i]->entropy(m_sigma);
			if(rf!=Return_Normal) return rf;		
		}
		updateBestArchive(this->m_pop[i]->self());			
    }
	if(rf==Return_Normal){
		this->m_iter++;
	}
    return rf;
}
