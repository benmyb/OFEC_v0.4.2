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
// Created: 5 Nov 2011
// Last modified:

#include "MQSOSwarm.h"

#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
#endif

using namespace std;

MQSOSwarm::MQSOSwarm(ParamMap &v):Algorithm(-1,(v[param_algName])),MultiPopulationCont<MQSOSubSwarm>((v[param_subPopSize]),(v[param_overlapDgre]))
{

	double u,l;
	int d=Global::msp_global->mp_problem->getNumDim();
	int peaks = 0;
	if(CAST_PROBLEM_DYN)peaks=CAST_PROBLEM_DYN->getNumberofPeak();
	else if (CAST_PROBLEM_DYN_ONEPEAK)peaks = CAST_PROBLEM_DYN_ONEPEAK->getNumPeak();
	CAST_PROBLEM_CONT->getSearchRange(l,u,0);

	if(m_overlapDegree==-1)
    m_Rexcl=0.5*(u-l)/pow((double)peaks, 1./d);
	else m_Rexcl=m_overlapDegree;
	m_Rconv=m_Rexcl;

	m_exclusion=true;

	if(v[param_popSize]==100) m_M=10;
	else m_M=v[param_popSize]/10; 

	initialize();

	if(IS_ALG_NAME(Global::ms_curAlgId,"ALG_mCPSO")) m_algPar<<m_name<<" M:"<<m_M<<"; Rexcl:"<<m_Rexcl<<"; Rconv:"<<m_Rconv;
	else	m_algPar<<m_name<<" M:"<<m_M<<"; Rexcl:"<<m_Rexcl<<"; Rconv:"<<m_Rconv;

}

void MQSOSwarm::initialize()
{
	 for(int i=0;i<m_M;i++)
	 {
		 MQSOSubSwarm *s=new MQSOSubSwarm(m_maxSubSize,true);
		s->initializePara(0.729843788,2.05,2.05);
        addPopulation(*s);
    }
}

bool MQSOSwarm::checkConvergenceAll(){
    bool flag=true;
    for(int i=0;i<m_M&&flag;i++){
        for(int j=0;j<m_subPop[i]->m_popsize&&flag;j++){
            for(int k=j+1;k<m_subPop[i]->m_popsize&&flag;k++){
				if(m_subPop[i]->m_pop[j]->m_type!=MQSOParticle::PARTICLE_NEUTRAL||m_subPop[i]->m_pop[k]->m_type!=MQSOParticle::PARTICLE_NEUTRAL ) continue;
                for(int d=0;d<m_subPop[i]->m_numDim;d++){
                    if(fabs(double(m_subPop[i]->m_pop[j]->data()[d]- m_subPop[i]->m_pop[k]->data()[d]))>m_Rconv){
                        flag=false;
                        break;
                    }
                }
            }
        }
    }

    return flag;
}
void MQSOSwarm::exclude(){
    for(int i=0;i<m_M;i++){
        for(int j=i+1;j<m_M;j++){
            if(m_subPop[i]->m_flag[0]==false&&m_subPop[j]->m_flag[0]==false&&m_subPop[i]->m_best[0]->getDistance(*m_subPop[j]->m_best[0])<m_Rexcl){
                if(*m_subPop[i]->m_best[0]>*m_subPop[j]->m_best[0]){
                    m_subPop[j]->m_flag[0]=true;
                }else{
                    m_subPop[i]->m_flag[0]=true;
                }
            }
        }
    }
}

ReturnFlag MQSOSwarm::run_(){

	#ifdef OFEC_CONSOLE
	if(Global::msp_global->m_runId==0){
		mSingleObj::getSingleObj()->setProgrOutputFlag(true);
	if(mMultiModal::getPopInfor())
		mMultiModal::getPopInfor()->setOutProgFlag(true);
	}
	#endif // OFEC_CONSOLE

	ReturnFlag r_flag=Return_Normal;
    while(r_flag!=Return_Terminate){
      
		if(checkConvergenceAll()) 	m_subPop[findWorstPop()]->m_flag[0]=true;
        //exclusion
		if(m_exclusion)		exclude();
		
        for(int i=0;i<m_M;i++){
			bool changeFlag=false;
			 
			Solution<CodeVReal> x(*m_subPop[i]->m_best[0]);
			 vector<double> objOld=x.obj(); 
			if(!m_subPop[i]->m_flag[0])   r_flag=m_subPop[i]->evolve();
			handleReturnFlagAll(r_flag);
			HANDLE_RETURN_FLAG(r_flag)
			
			
			if(CAST_PROBLEM_DYN&&!CAST_PROBLEM_DYN->predictChange( 1) || CAST_PROBLEM_DYN_ONEPEAK&&!CAST_PROBLEM_DYN_ONEPEAK->predictChange(1))    r_flag=x.evaluate(true);
			handleReturnFlagAll(r_flag);
			HANDLE_RETURN_FLAG(r_flag)

				if(x.obj(0)!=objOld[0]){
				for(int i=0;i<m_M;i++)
					m_subPop[i]->m_flag[0]=false;		
			}
			
			#ifdef OFEC_DEMON
					vector<Algorithm*> vp;
					for(auto &it:m_subPop){
						vp.push_back(it.get());
					}
					msp_buffer->updateBuffer_(&vp);
			#endif

			m_subPop[i]->m_initialRadius=m_Rexcl;
			
        }

		if(r_flag==Return_Terminate) break;

		measureMultiPop();

        for(int i=0;i<m_M;i++){
			if(m_subPop[i]->m_flag[0]&&(CAST_PROBLEM_DYN&&!CAST_PROBLEM_DYN->predictChange(m_subPop[i]->m_popsize) || CAST_PROBLEM_DYN_ONEPEAK&&!CAST_PROBLEM_DYN_ONEPEAK->predictChange(m_subPop[i]->m_popsize))){
				if(CAST_PROBLEM_DYN&&CAST_PROBLEM_DYN->predictChange(m_subPop[i]->m_popsize) || CAST_PROBLEM_DYN_ONEPEAK&&CAST_PROBLEM_DYN_ONEPEAK->predictChange(m_subPop[i]->m_popsize)){
				r_flag=m_subPop[i]->Population::initialize(false,false,true);
				}else{
				r_flag=m_subPop[i]->Population::initialize(false,true,true);
				}
				m_subPop[i]->m_flag[0]=false;
				handleReturnFlagAll(r_flag);
				HANDLE_RETURN_FLAG(r_flag)
			}
        }
     }

    return r_flag;
}

