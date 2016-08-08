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
// Created: 25 Sep. 2014
// Last modified:

#include "DynDEPopulation.h"

#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
#endif

DynDEPopulation::DynDEPopulation(ParamMap &v):DEPopulation<CodeVReal,DynDEIndividual>(){
	setDefault(v);
	m_name="ALG_DynDE";

    for(int i=0;i<m_M;i++){
		DynDESubPop *pop=new DynDESubPop(10);
		pop->m_initialRadius=m_Rexcl;
		addPopulation(*pop);
		
    }
	m_algPar<<"M(N,Nbrownian):"<<m_M<<"("<<5<<","<<5<<"); Rexcl:"<<m_Rexcl<<"; Rcloud:"<<0.2;
}

DynDEPopulation::~DynDEPopulation()
{
    //dtor
}

void DynDEPopulation::setDefault(ParamMap &v){
    // for global population size =100
	if((v[param_popSize])==100) m_M=10;
    else m_M=(v[param_popSize])/10;

    double u,l;
	int d=GET_NUM_DIM;
	int peaks = 0;
	if (CAST_PROBLEM_DYN)peaks = CAST_PROBLEM_DYN->getNumberofPeak();
	else if (CAST_PROBLEM_DYN_ONEPEAK) peaks = CAST_PROBLEM_DYN_ONEPEAK->getNumPeak();
	CAST_PROBLEM_CONT->getSearchRange(l,u,0);

  
	if((v[param_exlRadius])<=0.0)
     m_Rexcl=0.5*(u-l)/pow((double)peaks, 1./d);
	else m_Rexcl=(v[param_exlRadius]);

}

void DynDEPopulation::exclude(){
    for( int i=0;i<m_M;i++){
        for( int j=i+1;j<m_M;j++){
			if(m_subPop[i]->m_flag[f_reinit]==false&&m_subPop[j]->m_flag[f_reinit]==false&&m_subPop[i]->m_best[0]->getDistance(*m_subPop[j]->m_best[0])<m_Rexcl){
                if(*m_subPop[i]->m_best[0]>*m_subPop[j]->m_best[0]){
                    m_subPop[j]->m_flag[f_reinit]=true;
                }else{
                    m_subPop[i]->m_flag[f_reinit]=true;
                }
            }
        }
    }
}
ReturnFlag DynDEPopulation::run_(){
	ReturnFlag rf=Return_Normal;

	#ifdef OFEC_CONSOLE
	if(Global::msp_global->m_runId==0){
		mSingleObj::getSingleObj()->setProgrOutputFlag(true);
	if(mMultiModal::getPopInfor())
		mMultiModal::getPopInfor()->setOutProgFlag(true);
	}
	#endif // OFEC_CONSOLE

	while(!ifTerminating()){
		   
		#ifdef OFEC_DEMON
					vector<Algorithm*> vp;
					for(auto &it:m_subPop){
						vp.push_back(it.get());
					}
					msp_buffer->updateBuffer_(&vp);
		#endif
        for( int i=0;i<m_M;i++){
			//m_subPop[i]->printToScreen();
			//getchar();
			
			if(!m_subPop[i]->m_flag[f_reinit])       rf=m_subPop[i]->evolve();
			//cout<<m_subPop[i]->m_flag<<" "<<endl;
			handleReturnFlag(rf);
			handleReturnFlagAll(rf);
			HANDLE_RETURN_FLAG(rf)
			//cout<<"Evals: "<<Global::msp_global->mp_problem->getEvaluations()<<endl;
			//m_subPop[i]->printToScreen();
			//getchar();
        }
		
		/* for( int i=0;i<m_M;i++){
			 cout<<"Evals: "<<Global::msp_global->mp_problem->getEvaluations()<<endl;
			 m_subPop[i]->printToScreen();
		 }
		 getchar();*/
		if(rf==Return_Terminate) break;
		//exclusion
       exclude();

        for( int i=0;i<m_M;i++){
			if(m_subPop[i]->m_flag[f_reinit]&&(CAST_PROBLEM_DYN&&!CAST_PROBLEM_DYN->predictChange(m_subPop[i]->m_popsize)|| CAST_PROBLEM_DYN_ONEPEAK&&!CAST_PROBLEM_DYN_ONEPEAK->predictChange(m_subPop[i]->m_popsize))){
				//cout<<"Reinitial Evals: "<<i<<" "<<Global::msp_global->mp_problem->getEvaluations()<<endl;
				//m_subPop[i]->m_best.clear();
				m_subPop[i]->initialize(false,true,true);	
				
			//m_subPop[i]->printToScreen();
			//getchar();
			}
			if(ifTerminating()) break;
            m_subPop[i]->m_flag[f_reinit]=false;
        }
		measureMultiPop();
		//cout<<Global::gp_problem->getEvaluations()<<endl;
     }
	

    return rf;

}
