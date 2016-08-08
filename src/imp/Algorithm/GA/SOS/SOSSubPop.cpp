#include "SOSSubPop.h"

SOSSubPop::SOSSubPop():PopulationCont<CodeVReal,GAIndividual<CodeVReal>,GAPopulation<CodeVReal,GAIndividual<CodeVReal>> >(),m_unimpr(0){
    setSelStrategy(SEL_TOURNAMENT);
    setMutationProbability(0.02);
    setMutationStrategy(MUTAT_NORMAL);
    setMutationStep(m_initialRadius);
    setXoverProbability(0.6);
}
SOSSubPop::SOSSubPop(int size, bool mode):PopulationCont<CodeVReal,GAIndividual<CodeVReal>,GAPopulation<CodeVReal,GAIndividual<CodeVReal>> >(size,mode),m_unimpr(0){
	setSelStrategy(SEL_TOURNAMENT);
    setMutationProbability(0.02);
    setMutationStrategy(MUTAT_NORMAL);
    setMutationStep(m_initialRadius);
    setXoverProbability(0.6);
	m_bestFitPreIter=m_best[0]->obj(0);
}
SOSSubPop & SOSSubPop::operator=(const SOSSubPop & rhs){
	if(this==&rhs) return *this;
	PopulationCont<CodeVReal,GAIndividual<CodeVReal>,GAPopulation<CodeVReal,GAIndividual<CodeVReal>> >::operator=(rhs);
	m_dynamism=rhs.m_dynamism;
	m_quality=rhs.m_quality;
	m_desiredSize=rhs.m_desiredSize;
	m_bestFitPreIter=rhs.m_bestFitPreIter;
	m_unimpr=rhs.m_unimpr;
	return *this;
}
ReturnFlag SOSSubPop::add(int num,  bool mode,  bool insize){
	ReturnFlag rf=PopulationCont<CodeVReal,GAIndividual<CodeVReal>,GAPopulation<CodeVReal,GAIndividual<CodeVReal>> >::add(num,mode,insize);
	m_pool.resize(this->m_popsize);
	return rf;
}
ReturnFlag SOSSubPop::evolve(){
	if(this->m_popsize<=0) return Return_Normal;

    m_bestFitPreIter=m_best[0]->obj(0);
    int i;
  //  selection
    switch(m_selStr){
        case SEL_ROULETTE_WHEEL:
        rouletteWheel();
        break;
        case SEL_TOURNAMENT:
        tSelectWoRep();
        break;
    }

	for( i=0; i< this->m_popsize; i++ )    this->m_pop[i]->m_backup=this->m_pop[i]->self();
	for( i=0; i< this->m_popsize; i++ ){
		this->m_pop[i]->self()=this->m_pop[m_pool[i]]->m_backup;
	}

 
	i=0;
	while (i+2<this->m_popsize){
		this->m_pop[i]->xover(*this->m_pop[i+1]);
		i+=2; // next pair of mates
	}
	for( i=0; i< this->m_popsize; i++ ) this->m_pop[i]->mutate();
	
	ReturnFlag rf=Return_Normal;
	for( i=0; i< this->m_popsize; i++ ){
		rf=this->m_pop[i]->self().evaluate();
		if(rf!=Return_Normal) break;
	}

	this->findBest();
	if(this->m_pop[this->m_bestIdx[0]]->self() > *this->m_best[0]){
		updateBestArchive(this->m_pop[this->m_bestIdx[0]]->self());
		m_unimpr=0;
	}else{
		m_unimpr++;
	}

	//replacement of the worst individual with the best one so far
	findWorst();
	if(*m_best[0]>m_pop[m_worstIdx[0]]->self())	m_pop[m_worstIdx[0]]->self()=*m_best[0];

	mapObj2Fitness();

	return rf;
}
