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
// Created: 12 Nov 2011
// Last modified:

#include "SOSPopulation.h"
#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
#endif

SOSPopulation::SOSPopulation(ParamMap &v):GAPopulation(){
    
    //create an initial main population with size of m_popsize
    m_name="ALG_SOS";
	m_algPar<<"Population size: "<<v[param_popSize];
    defaultParSet(v[param_popSize]);
	addPopulation(*new SOSSubPop(v[param_popSize],v[param_evalCountFlag]));
}


SOSPopulation::~SOSPopulation()
{
    //dtor
}
void SOSPopulation::defaultParSet(int size){
    // parameter settings are not provided in the original paper, here are for used values
	m_maxSubSize=static_cast<int>(size*0.2);
	m_minSubSize=static_cast<int>(size*0.05);
	m_ratioToBestFit=0.5;
	m_alpha=0.5;
	m_forkingIter=static_cast<int>(log(size*1.0)/log(2.));
	m_range=0.05;
}
void SOSPopulation::computeDesiredPopSize(){

    // calculate the dynamism of each population
	for(unsigned i=0;i<m_subPop.size();i++){
		if(m_subPop[i]->m_popsize>0){
			double curbest=m_subPop[i]->m_best[0]->obj(0);
			double prebest=m_subPop[i]->m_bestFitPreIter;
			double improve;
			vector<double> gOpt;
			Global::msp_global->mp_problem->getObjGlobalOpt(gOpt);
			if(Global::msp_global->mp_problem->getOptType()==MIN_OPT){
				//different from original paper, should consider negtive values
				improve=(prebest-curbest)/(prebest-gOpt[0]);

			}else{
				improve=(curbest-prebest)/(gOpt[0]-prebest);
			}
		
			m_subPop[i]->m_dynamism=(0>improve?0:improve);
		}else{
			m_subPop[i]->m_dynamism=0;
		}

    }
    // compute the quality of each population
    double sumD=0,sumF=0,sumQ=0,worstBestFit;
	unsigned worstBestIdx=0;
	while(worstBestIdx<m_subPop.size()&&m_subPop[worstBestIdx]->m_popsize==0) worstBestIdx++;
	for(unsigned i=0;i<m_subPop.size();i++){
		if(m_subPop[i]->m_popsize==0) continue;
        sumD+=m_subPop[i]->m_dynamism;
		//different from original paper, should consider negtive values
		if(m_subPop[worstBestIdx]->m_best[0]>m_subPop[i]->m_best[0]) worstBestIdx=i;
    }
	worstBestFit=m_subPop[worstBestIdx]->m_best[0]->obj(0);

	for(unsigned i=0;i<m_subPop.size();i++){
		if(m_subPop[i]->m_popsize==0) continue;
		//different from original paper, should consider negtive values
		sumF=m_subPop[i]->m_best[0]->obj(0)-worstBestFit;
    }

    if(sumD>0){
        for(unsigned i=0;i<m_subPop.size();i++){
			if(m_subPop[i]->m_popsize==0) m_subPop[i]->m_quality=0;
			else     m_subPop[i]->m_quality=m_alpha*m_subPop[i]->m_dynamism/sumD+(1-m_alpha)*(m_subPop[i]->m_best[0]->obj(0)-worstBestFit)/sumF;
            sumQ+=m_subPop[i]->m_quality;
        }

    }else{
		for(unsigned i=0;i<m_subPop.size();i++){
			if(m_subPop[i]->m_popsize==0) m_subPop[i]->m_quality=0;
			else  m_subPop[i]->m_quality= (m_subPop[i]->m_best[0]->obj(0)-worstBestFit)/sumF;
           sumQ+=m_subPop[i]->m_quality;
        }

    }
    // compute desired population size, the restrictions on the minimum and maximum population size have to be respected
    for(unsigned i=0;i<m_subPop.size();i++){
		m_subPop[i]->m_desiredSize=static_cast<int>(Global::g_arg[param_popSize]*1*m_subPop[i]->m_quality/sumQ);
		 if(i!=0){
			if(m_subPop[i]->m_desiredSize<m_minSubSize)m_subPop[i]->m_desiredSize=m_minSubSize;
			 else if(m_subPop[i]->m_desiredSize>m_maxSubSize)m_subPop[i]->m_desiredSize=m_maxSubSize;
		 }else{
			if(m_subPop[i]->m_desiredSize<m_maxSubSize)m_subPop[i]->m_desiredSize=m_maxSubSize;
			else if(Global::g_arg[param_popSize]<m_subPop[i]->m_desiredSize)m_subPop[i]->m_desiredSize=Global::g_arg[param_popSize];
		 }
    }

}
ReturnFlag SOSPopulation::adjustPopulation(){
	ReturnFlag rf=Return_Normal;
	for(unsigned i=0;i<m_subPop.size();i++){
        int desiredsize=m_subPop[i]->m_desiredSize;
        if(desiredsize>m_subPop[i]->m_popsize){
        //add individuals
            if(i==0){
                // the main population			
                if(CAST_PROBLEM_DYN&&CAST_PROBLEM_DYN->predictChange(desiredsize - m_subPop[i]->m_popsize) || CAST_PROBLEM_DYN_ONEPEAK&&CAST_PROBLEM_DYN_ONEPEAK->predictChange(desiredsize - m_subPop[i]->m_popsize))
                    rf=m_subPop[i]->add(desiredsize-m_subPop[i]->m_popsize,false,false);
                else
                     rf=m_subPop[i]->add(desiredsize-m_subPop[i]->m_popsize,true,false);
            }else{
                // child population
                if(CAST_PROBLEM_DYN&&CAST_PROBLEM_DYN->predictChange(desiredsize - m_subPop[i]->m_popsize) || CAST_PROBLEM_DYN_ONEPEAK&&CAST_PROBLEM_DYN_ONEPEAK->predictChange(desiredsize - m_subPop[i]->m_popsize))
                   rf=m_subPop[i]->add(desiredsize-m_subPop[i]->m_popsize,false,true);
                else
                   rf=m_subPop[i]->add(desiredsize-m_subPop[i]->m_popsize,true,true);

            }
        }else if(desiredsize<m_subPop[i]->m_popsize){
            //remove worst individuals
            m_subPop[i]->sort(false,true);
            int *p_remove=new int[m_subPop[i]->m_popsize-desiredsize];
			for(int j=0;j<m_subPop[i]->m_popsize-desiredsize;j++) p_remove[j]= m_subPop[i]->m_pop[m_subPop[i]->m_orderList[j]]->getId();

            m_subPop[i]->remove(m_subPop[i]->m_popsize-desiredsize,p_remove);
            delete [] p_remove;
			p_remove=0;

        }
		if(rf!=Return_Normal) return rf;
    }
	return rf;
}
int SOSPopulation::removeOverlapping(){

    for(int i=0;i<m_subPop[0]->m_popsize;i++){
		for(unsigned j=1;j<m_subPop.size();j++){
			if(m_subPop[0]->m_pop[i]->self().getDistance(*m_subPop[j]->m_best[0],DIS_MANHATTAN)<m_subPop[j]->m_initialRadius){
                if(CAST_PROBLEM_DYN&&CAST_PROBLEM_DYN->predictChange(1) || CAST_PROBLEM_DYN_ONEPEAK&&CAST_PROBLEM_DYN_ONEPEAK->predictChange(1))
                    m_subPop[0]->m_pop[i]->self().initialize(false);
                else
                    m_subPop[0]->m_pop[i]->self().initialize(true);
                break;
            }
        }
    }
     for(unsigned i=1;i<m_subPop.size();i++){
         for(unsigned j=i+1;j<m_subPop.size();j++){
			 if(m_subPop[i]->m_best[0]->getDistance(*m_subPop[j]->m_best[0],DIS_MANHATTAN)<m_subPop[i]->m_initialRadius){
               deletePopulation(j);
                j--;
            }else if(m_subPop[i]->m_best[0]->getDistance(*m_subPop[j]->m_best[0],DIS_MANHATTAN)<m_subPop[j]->m_initialRadius){
                 deletePopulation(i);
                 i--;
                 break;
            }
        }
     }
	 return -1;
}



ReturnFlag SOSPopulation::run_(){
	ReturnFlag rf=Return_Normal;

	#ifdef OFEC_CONSOLE
	if(Global::msp_global->m_runId==0){
		mSingleObj::getSingleObj()->setProgrOutputFlag(true);
	if(mMultiModal::getPopInfor())
		mMultiModal::getPopInfor()->setOutProgFlag(true);
	}
	#endif // OFEC_CONSOLE

	while(rf!=Return_Terminate){
		//evovle each population
		for(unsigned int i=0;i<m_subPop.size();i++){
			rf=m_subPop[i]->evolve();
			if(rf==Return_Terminate)			break;
			
			handleReturnFlag(rf);
			handleReturnFlagAll(rf);
			HANDLE_RETURN_FLAG(rf)
			
			//update the best indis so far from all pops
			updateBestArchive(*m_subPop[i]->m_best[0]);
		}
		if(rf==Return_Terminate)			break;
		#ifdef OFEC_CONSOLE
		measureMultiPop();
		#endif
		#ifdef OFEC_DEMON
		vector<Algorithm*> vp;
		for(auto &it:this->m_subPop){
			vp.push_back(it.get());
		}
		msp_buffer->updateBuffer_(&vp);
		#endif
		//cout<<Global::msp_global->mp_problem->getEvaluations()<<" "<<m_subPop.size()<<endl;

		// check the condition for creating new pop; all indis within m_range around the m_best of pop[0] form a new pop, if not
		// enough indis to meet the restriction on  m_minSubSize, further close indis are included
		if(m_subPop[0]->m_unimpr>m_forkingIter){
			double u,l;
			int newPopsize=0;  //makesure the main pop is not empty
			CAST_PROBLEM_CONT->getSearchRange(l,u,0);
			
			addPopulation(* new SOSSubPop());
			vector<unique_ptr<GAIndividual<CodeVReal>>> indis;
			vector<int>idx;
			for(int j=0;j<m_subPop[0]->m_popsize;j++){
				if(indis.size()>=m_maxSubSize) break;
				if(m_subPop[0]->m_best[0]->getDistance(m_subPop[0]->m_pop[j]->self(),DIS_MANHATTAN)/GET_NUM_DIM<m_range*(u-l)){
					idx.push_back(m_subPop[0]->m_pop[j]->getId());
					indis.push_back(move(m_subPop[0]->m_pop[j]));	
				}
			}
			if(indis.size()==0) throw myException("No indivdiduals of a new pop@ SOSPopulation::run_()");
			m_subPop[m_subPop.size()-1]->add(indis);
			m_subPop[m_subPop.size()-1]->m_initialRadius=m_subPop[m_subPop.size()-1]->m_curRadius;
			(m_subPop[m_subPop.size()-1])->m_bestFitPreIter=m_subPop[m_subPop.size()-1]->m_best[0]->obj(0);
			indis.clear();	

			m_subPop[0]->remove(idx);
			
			if(m_subPop[0]->m_popsize==0) {
				if(CAST_PROBLEM_DYN&&CAST_PROBLEM_DYN->predictChange(m_maxSubSize) || CAST_PROBLEM_DYN_ONEPEAK&&CAST_PROBLEM_DYN_ONEPEAK->predictChange(m_maxSubSize))
                    rf=m_subPop[0]->add(m_maxSubSize,false,false);
                else
                     rf=m_subPop[0]->add(m_maxSubSize,true,false);
			
			}
		}

		//discard a pop if it its best indi's fitness is smaller than a minimum required fitness
		for(unsigned int i=1;i<m_subPop.size();i++){
			if(m_subPop[i]->m_best[0]->obj(0)<m_best[0]->obj(0)*m_ratioToBestFit){
				deletePopulation(i);
				i--;
			}

		}
		if(m_subPop.size()>1){
			computeDesiredPopSize();
			rf=adjustPopulation();
			if(rf==Return_Terminate)		break;
			removeOverlapping();
		}
     }
	
	return rf;
}
