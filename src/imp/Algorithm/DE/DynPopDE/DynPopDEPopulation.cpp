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
// Created: 01 Nov 2012
// Last modified:
#include "DynPopDEPopulation.h"
#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
#endif
bool DynPopDEPopulation::isStagnation(){
	for(unsigned int i=0;i<m_subPop.size();i++){
		if(m_subPop[i]->m_improvement>0) return false;
	}
	return true;
}
void DynPopDEPopulation::updateRadius(){
	 double u,l;
	 int d=Global::msp_global->mp_problem->getNumDim();
	 CAST_PROBLEM_CONT->getSearchRange(l,u,0);   
	 m_exclRadius=0.5*(u-l)/pow((double)m_subPop.size(), 1./d);

}

void DynPopDEPopulation::setDefault(){
    //default configuration with 4 normal indis and 2 brownian indis for each pop
    m_numNormal=6;
    m_numBrownian=2;
    updateRadius();
	
}

DynPopDEPopulation::DynPopDEPopulation(ParamMap &v):Algorithm(-1,"ALG_DynPopDE"){
 
    DynPopDEPopulation::setDefault(); 
    m_subPop.push_back(move(unique_ptr<DynPopDESubPop>(new DynPopDESubPop(m_numNormal+m_numBrownian,m_numBrownian))));
	m_algPar<<"(NNormal,Nbrownian):"<<"("<<m_numNormal<<","<<m_numBrownian<<")";
}


DynPopDEPopulation::~DynPopDEPopulation(){
    //dtor
}

void DynPopDEPopulation::exclude(){
	Solution<CodeVReal> chr; 
	ReturnFlag rf=Return_Normal;
	for(decltype(m_subPop.size()) i=0;i<m_subPop.size();i++){
        for(decltype(m_subPop.size()) j=i+1;j<m_subPop.size();j++){
            if(m_subPop[i]->m_flag[DynPopDESubPop::f_reinit]==false&&m_subPop[j]->m_flag[DynPopDESubPop::f_reinit]==false&&m_subPop[i]->m_best[0]->getDistance(*m_subPop[j]->m_best[0])<m_exclRadius){
				// exclusion subject to RMC, check the midpoint between the best indis of the two pops
				for(int d=0;d<GET_NUM_DIM;d++){
					chr.data()[d]=(m_subPop[i]->m_best[0]->data()[d]+m_subPop[j]->m_best[0]->data()[d])/2.;
				}
				
				if(CAST_PROBLEM_DYN&&!CAST_PROBLEM_DYN->predictChange(1) || CAST_PROBLEM_DYN_ONEPEAK&&!CAST_PROBLEM_DYN_ONEPEAK->predictChange(1)) chr.evaluate(false);
				else rf=chr.evaluate(true);
				
				handleReturnFlagAll(rf);
				HANDLE_RETURN_FLAG(rf)

					if(chr<*m_subPop[j]->m_best[0]&&chr<*m_subPop[i]->m_best[0]) continue;

                if(m_subPop[i]->m_best>m_subPop[j]->m_best){
                    m_subPop[j]->m_flag[DynPopDESubPop::f_reinit]=true;
                }else{
                    m_subPop[i]->m_flag[DynPopDESubPop::f_reinit]=true;
                }
            }
        }
		if(rf!=Return_Normal) return ;
    }
	
}
ReturnFlag DynPopDEPopulation::run_(){
	int count=0;
	#ifdef OFEC_CONSOLE
	if(Global::msp_global->m_runId==0){
		mSingleObj::getSingleObj()->setProgrOutputFlag(true);
		if(mMultiModal::getPopInfor())
		mMultiModal::getPopInfor()->setOutProgFlag(true);
	}
	#endif // OFEC_CONSOLE
	ReturnFlag rf=Return_Normal;
	while(!ifTerminating()){
		//cout<<"Evals: "<<Global::msp_global->mp_problem->getEvaluations()<<" "<<m_subPop.size()<<endl;
		#ifdef OFEC_DEMON
				vector<Algorithm*> vp;
				for(auto &it:m_subPop){
					vp.push_back(it.get());
				}
				msp_buffer->updateBuffer_(&vp);
		#endif
		//detect change
				Solution<CodeVReal> chr(*m_subPop[0]->m_best[0]);
	
		if(CAST_PROBLEM_DYN&&!CAST_PROBLEM_DYN->predictChange(1) || CAST_PROBLEM_DYN_ONEPEAK&&!CAST_PROBLEM_DYN_ONEPEAK->predictChange(1)){
			rf=chr.evaluate();
		}
		if(chr.obj(0)!=m_subPop[0]->m_best[0]->obj(0)){
			count=0;
		}
		handleReturnFlagAll(rf);
		HANDLE_RETURN_FLAG(rf)
		
		if(count<2){
			for(auto &pop:m_subPop){
				rf=pop->evolve();
				handleReturnFlagAll(rf);
				HANDLE_RETURN_FLAG(rf)
				pop->assignType();
			}
			m_worstPop=this->findWorstPop();
			for(auto &pop:m_subPop) pop->updateInfor(m_subPop[m_worstPop].get());
		}else{
			int highPer=getHighestPerformancePop();
			rf=m_subPop[highPer]->evolve(); 
			handleReturnFlagAll(rf);
			HANDLE_RETURN_FLAG(rf)
			m_worstPop=this->findWorstPop();
			m_subPop[highPer]->updateInfor(m_subPop[m_worstPop].get());
			m_subPop[highPer]->assignType();
		}
		if(ifTerminating()) break;
		if(isStagnation()){
			if(CAST_PROBLEM_DYN&&!CAST_PROBLEM_DYN->predictChange(m_numNormal + m_numBrownian) || CAST_PROBLEM_DYN_ONEPEAK&&!CAST_PROBLEM_DYN_ONEPEAK->predictChange(m_numNormal + m_numBrownian)){
				m_subPop.push_back(move(unique_ptr<DynPopDESubPop>(new DynPopDESubPop(m_numNormal+m_numBrownian,m_numBrownian))));
				//immediately envolve one iteration after created suggested by m_subPop
				rf=m_subPop[m_subPop.size()-1]->evolve();
				handleReturnFlagAll(rf);
				HANDLE_RETURN_FLAG(rf)
				updateRadius();
				m_worstPop=this->findWorstPop();
				for(auto &pop:m_subPop) pop->updateInfor(m_subPop[m_worstPop].get());
			}
		}

		measureMultiPop();

		//exclusion
		exclude();

		//removal of population
		bool removal=false;
		for(unsigned int i=0;i<m_subPop.size();i++){
			if(m_subPop[i]->m_flag[DynPopDESubPop::f_reinit]&&m_subPop[i]->m_improvement!=0) {
				deletePopulation(i);
				i--;
				removal=true;
			}
		}

		bool reinitialization=false;
		for(unsigned int i=0;i<m_subPop.size();i++){
			if(m_subPop[i]->m_flag[DynPopDESubPop::f_reinit]&&m_subPop[i]->m_improvement==0&&(CAST_PROBLEM_DYN&&!CAST_PROBLEM_DYN->predictChange(m_subPop[i]->m_popsize) || CAST_PROBLEM_DYN_ONEPEAK&&!CAST_PROBLEM_DYN_ONEPEAK->predictChange(m_subPop[i]->m_popsize))){
				rf=m_subPop[i]->initialize();
				reinitialization=true;
				if(rf==Return_Terminate) break;
			}
		}

		if(removal||reinitialization){
				m_worstPop=this->findWorstPop();
				for(auto &pop:m_subPop) pop->updateInfor(m_subPop[m_worstPop].get());
				updateRadius();
		}

		count++;
    }	
    return rf;
}



int DynPopDEPopulation::getHighestPerformancePop(){
	int idx=0;
	for(decltype(m_subPop.size()) i=1;i<m_subPop.size();i++){
		if(m_subPop[i]->m_performance>m_subPop[idx]->m_performance) idx=i;
	}

	return idx;
	
}
