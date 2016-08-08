#include "CPSOHSwarm.h"


CPSOH::CPSOH(ParamMap &v) :MultiPopulationCont<CooperativeSubSwarm>(), SwarmGBest(v)
{
	int numPops;
	if(Global::msp_global->mp_problem->getNumDim()<6)
		numPops=Global::msp_global->mp_problem->getNumDim()-1;
	else  
		numPops=6;
	
	if(numPops<1)
		throw myException("the number of subpops must be greater than 0");
	if(numPops>Global::msp_global->mp_problem->getNumDim())
		throw myException("the number of subpops must not be larger than the number of Dims");

	for(int j=0;j<numPops;j++){
		CooperativeSubSwarm *newp=new CooperativeSubSwarm(Global::g_arg[param_popSize],j,numPops,true);
		this->addPopulation(*newp);
	}

	CPSOH::msp_gbest.reset(new Solution<CodeVReal>());

	updateGlobalBest();
}

CPSOH & CPSOH::operator =(CPSOH &s){
	if(this==&s) return *this;
	*msp_gbest=*s.msp_gbest;
	return *this;

}
CPSOH::~CPSOH(){
	msp_gbest.reset();
}


ReturnFlag CPSOH::updateGlobalBest(){
	ReturnFlag rf=Return_Normal;
	int k=0;
	int pop_num=getNumPops();

	for(int i=0;i<pop_num;i++){
		for(int j=0;j<m_subPop[i]->m_numbers;j++) 
			(*msp_gbest).data()[k++]=(*m_subPop[i]->m_best[0]).data()[m_subPop[i]->mv_members[j]];
	}
	rf=msp_gbest->evaluate();
	if(rf!=Return_Normal) return rf;

	for(int i=0;i<pop_num;i++)
		*m_subPop[i]->m_best[0]=*msp_gbest;
	for(int i=0;i<pop_num;i++){
		for(int j=0;j<m_subPop[i]->getPopSize();j++){
			for( k=0;k<m_subPop[i]->mv_members[0];k++)
				m_subPop[i]->m_pop[j]->data()[k]=(*msp_gbest).data()[k];
			for(k=m_subPop[i]->mv_members[m_subPop[i]->m_numbers-1]+1;k<Global::msp_global->mp_problem->getNumDim();k++)
				m_subPop[i]->m_pop[j]->data()[k]=(*msp_gbest).data()[k];
			rf=m_subPop[i]->m_pop[j]->evaluate();
			if(rf!=Return_Normal) return rf;

			m_subPop[i]->m_pop[j]->representative()=m_subPop[i]->m_pop[j]->self();
		}
	}

	return rf;
}

ReturnFlag CPSOH::evolve(){

	ReturnFlag rf=Return_Normal;

	 for(int k=0;k<getNumPops();k++)
	 {
		 rf=m_subPop[k]->evolve(*msp_gbest);
		 if(rf!=Return_Normal) return rf;
	 }
	 
	int r=Global::msp_global->getRandInt(0,this->getPopSize()/2);
	int t=0;
	while(this->m_pop[r]->representative()==*this->m_best[0]){
		r=Global::msp_global->getRandInt(0,this->getPopSize()/2);
			t++;
		if(t>=this->getPopSize()/2) break;
	}
	this->m_pop[r]->self()=*msp_gbest;
	rf=this->SwarmGBest::evolve();
	if(rf!=Return_Normal) return rf;

	for(int k=0;k<getNumPops();k++){
		r=Global::msp_global->getRandInt(0,m_subPop[k]->getPopSize()/2);
		int t=0;
		while(m_subPop[k]->m_pop[r]->representative()==*m_subPop[k]->m_best[0]){
			r=Global::msp_global->getRandInt(0,m_subPop[k]->getPopSize()/2);
			t++;
			if(t>=m_subPop[k]->getPopSize()/2) break;
		}
		m_subPop[k]->m_pop[r]->self()=*this->m_best[0];
	}
	return rf;
}


ReturnFlag CPSOH::run_()
{
	ReturnFlag rf=Return_Normal;

	#ifdef OFEC_CONSOLE
	if(Global::msp_global->m_runId==0){
		mSingleObj::getSingleObj()->setProgrOutputFlag(true);
		if(mMultiModal::getPopInfor())
		mMultiModal::getPopInfor()->setOutProgFlag(true);
	}
	#endif // OFEC_CONSOLE

	while(!ifTerminating())
	{
		//cout<<"Run: "<<Global::msp_global->m_runId<<" "<<Global::msp_global->mp_problem->getEvaluations()<<endl;
		rf=evolve();

		#ifdef OFEC_CONSOLE
		if(mMultiModal::getPopInfor()){
			int peaksf;		
			peaksf=CAST_PROBLEM_CONT->getGOpt().getNumGOptFound();
			mMultiModal::getPopInfor()->input(Global::msp_global.get(), Global::msp_global->mp_problem->getEvaluations(),\
				Global::msp_global->m_totalNumIndis,1,peaksf,\
				CAST_PROBLEM_CONT->getGOpt().getNumOpt(),0,0,0,0,\
				0,0,CAST_PROBLEM_CONT->getGOpt().isAllFound());
		}
		#endif

		if(rf==Return_Terminate) return rf;
	}
	return rf;
}