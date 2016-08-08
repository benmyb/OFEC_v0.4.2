/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com 
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 29 Oct. 2011
// Last modified:
#include "SpeciesSwarm.h"

#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
#endif

SpeciesSwarm::SpeciesSwarm():Swarm<CodeVReal,SpeciesParticle>()
{
    //ctor
}

SpeciesSwarm::SpeciesSwarm(ParamMap& v):Swarm<CodeVReal,SpeciesParticle>(v[param_popSize],v[param_evalCountFlag]),m_numSeeds(0), mv_seedList(m_popsize){
	m_name="ALG_SPSO";
	initializePara(0.729844,2.05,2.05);
	setRadius(v[param_exlRadius]);
	setPMax(v[param_subPopSize]);
}
SpeciesSwarm::SpeciesSwarm( const SpeciesSwarm& other):Swarm<CodeVReal,SpeciesParticle>(other),m_numSeeds(other.m_numSeeds),mv_seedList(other.mv_seedList)
{
    //copy ctor   
}

SpeciesSwarm& SpeciesSwarm::operator=( const SpeciesSwarm& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
	Swarm<CodeVReal,SpeciesParticle>::operator=(rhs);

    m_numSeeds=rhs.m_numSeeds;
    mv_seedList=rhs.mv_seedList;

    return *this;
}

void SpeciesSwarm::findSeeds(){
    //the population must be sorted before this procedure
	for(int i=0;i<m_popsize;i++) {
		m_pop[i]->m_flag=0;
		m_pop[i]->m_seedship=false;
	}

	m_numSeeds=0;

	m_pop[m_orderList[0]]->m_flag=1;
	m_pop[m_orderList[0]]->m_lbestIdx=m_orderList[0];
	m_pop[m_orderList[0]]->m_seedship=true;

	mv_seedList[m_numSeeds]=m_orderList[0];
	m_numSeeds++;
	for(int i=0;i<m_popsize;i++){
		bool found=false;
		if(m_pop[m_orderList[i]]->m_flag==1) continue;
		for(int j=0;j<m_numSeeds;j++){
		    //here, we use pself as basis to calculate seeds rather than pbest since no special note regarding this issue in the paper
			if(m_pop[m_orderList[i]]->self().getDistance(m_pop[mv_seedList[j]]->self())<m_initialRadius){
				found=true;
				m_pop[m_orderList[i]]->m_lbestIdx=mv_seedList[j];
				break;
			}
		}
		if(!found){
			mv_seedList[m_numSeeds]=m_orderList[i];
			m_pop[m_orderList[i]]->m_seedship=true;
			m_pop[m_orderList[i]]->m_lbestIdx=m_orderList[i];
			m_numSeeds++;
		}
		m_pop[m_orderList[i]]->m_flag=1;
	}

}
bool SpeciesSwarm::removeConvergence(){
// if it has convered particles, return true, otherwise return false

	bool flag=false;
	for(int j=0;j<m_numSeeds ;j++){
		for(int i=0;i<m_popsize;i++ ){
			if(m_pop[i]->m_lbestIdx==mv_seedList[j]&&m_pop[i]->m_lbestIdx!=i){
				if(m_pop[i]==m_pop[mv_seedList[j]]){
					flag=true;
					//ignore the reinitialization if this will trigger environmental change
					if(CAST_PROBLEM_DYN&&!CAST_PROBLEM_DYN->predictChange(1) || CAST_PROBLEM_DYN_ONEPEAK&&!CAST_PROBLEM_DYN_ONEPEAK->predictChange(1))	m_pop[i]->initialize();
				}
			}
		}
	}
	return flag;

}
void SpeciesSwarm::checkOvercrowd(){
    //the population must be sorted before this procedure
    for(int i=0;i<m_popsize;i++) 	m_pop[i]->m_speciesship=true;

	int *temp=new int[m_popsize];
	int t_num;
	for(int j=0;j<m_numSeeds ;j++){
		t_num=0;
		for(int i=0;i<m_popsize;i++ ){
			if(m_pop[i]->m_lbestIdx==mv_seedList[j]){
				temp[t_num++]=i;
			}
		}

		if(t_num>m_pMax){
		    // find out the particles needed to be reinitialized
			//cout<<"overcrowing"<<endl;
		    for(int i=0;i<t_num;i++) 	m_pop[temp[i]]->m_flag=0;
		    int num=0;
		    for(int i=0;i<m_popsize;i++ ){
                if(m_pop[m_orderList[i]]->m_lbestIdx==mv_seedList[j]&&num<m_pMax){
                    m_pop[m_orderList[i]]->m_flag=1;
                    num++;
                }
		    }
			for(int i=0;i<t_num;i++){
				if(m_pop[temp[i]]->m_flag==0){
					m_pop[temp[i]]->m_speciesship=false;
					//ignore the reinitialization if this will trigger environmental change
					if(CAST_PROBLEM_DYN&&!CAST_PROBLEM_DYN->predictChange(1) || CAST_PROBLEM_DYN_ONEPEAK&&!CAST_PROBLEM_DYN_ONEPEAK->predictChange(1))  m_pop[temp[i]]->initialize();
				}
			}
		}
	}
	delete []temp;
	temp=0;

}
ReturnFlag SpeciesSwarm::evolve(){

	ReturnFlag rf=Return_Normal;
    if(m_popsize<1) return rf;

	for(int i=0;i<m_popsize;i++){
		if(!m_pop[i]->m_speciesship)		continue;

		rf=m_pop[i]->move(m_pop[m_pop[i]->m_lbestIdx]->m_pbest,m_W,m_C1,m_C2);

		handleReturnFlag(rf);
		HANDLE_RETURN_FLAG(rf)
	
       /*double fit=m_pop[i]->m_pbest.obj(0);

        if(!CAST_PROBLEM_DYN->predictChange(1))      rf=m_pop[i]->m_pbest.evaluate();

		handleReturnFlag(rf);
		HANDLE_RETURN_FLAG(rf)
      
        if(fit!=m_pop[i]->m_pbest.obj(0)){
            m_pop[i]->self().evaluate(false);
			m_pop[i]->m_pbest=m_pop[i]->self();
			return rf;
        }
		*/
		if(m_pop[i]->self()>m_pop[i]->m_pbest){
			m_pop[i]->m_pbest=m_pop[i]->self();
			updateBestArchive(m_pop[i]->self());
		}

	}
    return rf;
}
ReturnFlag SpeciesSwarm::run_(){
	#ifdef OFEC_CONSOLE
	if(Global::msp_global->m_runId==0){
		mSingleObj::getSingleObj()->setProgrOutputFlag(true);
		if(mMultiModal::getPopInfor())
		mMultiModal::getPopInfor()->setOutProgFlag(true);
	}
	#endif // OFEC_CONSOLE

     while(!ifTerminating()){
		// cout<<Global::msp_global->mp_problem->getEvaluations()<<" "<<m_numSeeds<<" "<<m_best[0]->obj(0)<<endl;

		 sort(true,true);
         findSeeds();
         checkOvercrowd();
         removeConvergence();
         if(Return_Terminate== evolve()) break;
		
		 #ifdef OFEC_DEMON
			vector<Algorithm*> vp;	
			vp.push_back(this);	
			msp_buffer->updateBuffer_(&vp);
		#endif

		if(mMultiModal::getPopInfor()){
			int peaksf;
			if(CAST_PROBLEM_DYN_CONT){
				if (IS_PROBLEM_NAME(Global::ms_curProId, "DYN_CONT_MovingPeak") || IS_PROBLEM_NAME(Global::ms_curProId, "DYN_CONT_RotationDBG")) {
					peaksf = CAST_PROBLEM_DYN_CONT->getPeaksFound();

				}
				else if (Global::msp_global->mp_problem->m_name.find("FUN_") != string::npos) {
					peaksf = CAST_PROBLEM_CONT->getGOpt().getNumGOptFound();
				}
				mMultiModal::getPopInfor()->input(Global::msp_global.get(), Global::msp_global->mp_problem->getEvaluations(), \
					Global::msp_global->m_totalNumIndis, m_numSeeds, peaksf, \
					CAST_PROBLEM_DYN_CONT->getNumofVisablePeaks(), 0, 0, 0, 0, \
					CAST_PROBLEM_DYN_CONT->getPeaksTracedQaulity(), 0, CAST_PROBLEM_DYN_CONT->isGOptTracked());
			}
			else if (CAST_PROBLEM_DYN_ONEPEAK) {
				FFreePeak_D_OnePeak*pro = dynamic_cast<FFreePeak_D_OnePeak*>(Global::msp_global->mp_problem.get());
				int peaksf = pro->getPT();
				mMultiModal::getPopInfor()->input(Global::msp_global.get(), Global::msp_global->mp_problem->getEvaluations(), \
					Global::msp_global->m_totalNumIndis, m_numSeeds, peaksf, \
					pro->getNumPeak(), 0,0, 0, 0, 0, 0, pro->isAllGOptTraced());
			}
			
		}
		 
     }
	 
	 return Return_Terminate;
}

void SpeciesSwarm::setPMax(int rPmax){
    m_pMax=rPmax;

	size_t start, end;
    start=this->m_algPar.str().find("pmax: ");
    for(size_t i=start;i<this->m_algPar.str().size();i++){
        if(this->m_algPar.str()[i]==';') {
            end=i;
            break;
        }
    }
    stringstream ss;
    ss<<"pmax:  "<<m_pMax<<";";
	if(start!=string::npos){
		string result=this->m_algPar.str();
		result.replace(start,end-start+1, ss.str());
		 this->m_algPar.str(result);
	}else this->m_algPar<<ss.str();

}



