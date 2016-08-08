#include "mNAFSASwarm.h"

mNAFSASwarm::mNAFSASwarm(ParamMap &v):Algorithm(-1,v[param_algName]),
	m_sleepRadius(0.4),m_convRadius(0.5),m_count(10){
	
	m_maxSubSize=v[param_subPopSize];
	double u,l;
	int d=GET_NUM_DIM;
	int peaks = 0;
	if(CAST_PROBLEM_DYN)peaks= CAST_PROBLEM_DYN->getNumberofPeak();
	else if (CAST_PROBLEM_DYN_ONEPEAK) peaks = CAST_PROBLEM_DYN_ONEPEAK->getNumPeak();

	CAST_PROBLEM_CONT->getSearchRange(l,u,0);
	m_exlRadius=0.5*(u-l)/pow((double)peaks, 1./d);

	m_shiftLength=1.0;
	m_subPop.push_back(move(unique_ptr<NFishSwarm>( new NFishSwarm(m_maxSubSize,true))));
	m_subPop[m_subPop.size()-1]->setVisual(25);
}

ReturnFlag mNAFSASwarm::exlude(){
	if(m_subPop.size()<=1) return Return_Normal;
	ReturnFlag rf=Return_Normal;
	for(auto i=m_subPop.begin();i!=m_subPop.end()-1;++i){
		if(m_subPop[m_subPop.size()-1]->m_best[0]->getDistance( *(*i)->m_best[0])<m_exlRadius){
			if((*m_subPop[m_subPop.size()-1]->m_best[0])>*(*i)->m_best[0]){
				if(CAST_PROBLEM_DYN&&!CAST_PROBLEM_DYN->predictChange(m_maxSubSize)||CAST_PROBLEM_DYN_ONEPEAK&&!CAST_PROBLEM_DYN_ONEPEAK->predictChange(m_maxSubSize)){
					rf=(*i)->initialize(false,true,true);
				}else{
					rf=(*i)->initialize(false,false,true);
				}
				(*i)->m_flag[NFishSwarm::f_excl]=true;// exclusion flag
			}else{
				if(CAST_PROBLEM_DYN&&!CAST_PROBLEM_DYN->predictChange(m_maxSubSize) || CAST_PROBLEM_DYN_ONEPEAK&&!CAST_PROBLEM_DYN_ONEPEAK->predictChange(m_maxSubSize)){
					rf=m_subPop[m_subPop.size()-1]->initialize(false,true,true);
				}else{
					rf=m_subPop[m_subPop.size()-1]->initialize(false,false,true);
				}
				m_subPop[m_subPop.size()-1]->m_flag[NFishSwarm::f_excl]=true;// exclusion flag
			}
			if(rf!=Return_Normal){
				HANDLE_RETURN_FLAG(rf)
			}
			break;
		}
	}
	return rf;
}
bool mNAFSASwarm::isConvergingAll(){
	for(auto& s:m_subPop){
		if(s->m_flag[NFishSwarm::f_sleeping]) continue; // check only active swarms
		if(!s->isConverged(m_count,m_convRadius)) return false;
	}
	return true;
}
void mNAFSASwarm::estimateShiftLength(){
	
	double maxShift=0;
	for(auto &s:m_subPop){
		if(!s->m_flag[NFishSwarm::f_est_s]|| s->m_flag[NFishSwarm::f_excl]) continue;
		double dis=s->m_best[0]->getDistance(s->m_bestLastEnvir);
		if(dis>maxShift) maxShift=dis;
	}
	if(maxShift!=0) m_shiftLength=maxShift;
	else m_shiftLength=1.0;
		

}
ReturnFlag mNAFSASwarm::run_(){
	#ifdef OFEC_CONSOLE
	if(Global::msp_global->m_runId==0){
		mSingleObj::getSingleObj()->setProgrOutputFlag(true);
	if(mMultiModal::getPopInfor())
		mMultiModal::getPopInfor()->setOutProgFlag(true);
	}
	#endif // OFEC_CONSOLE

	ReturnFlag rf=Return_Normal;
	Solution<CodeVReal> test;
	test.initialize();

	while(!ifTerminating()){
		//cout<<Global::msp_global->mp_problem->getEvaluations()<<" "<<m_subPop.size()<<" "<<m_subPop[findBestPop()]->m_best[0]->getObjDistance(Global::msp_global->mp_problem->getGOpt()[0].data().m_obj)<<endl;
		// excute NAFSA procedure
		for(auto &s:m_subPop){
			if(s->m_flag[NFishSwarm::f_sleeping])		continue;
			
			s->updatePreBest(m_count);
			rf=s->evolve();
			handleReturnFlagAll(rf);
			HANDLE_RETURN_FLAG(rf)	
		}
		if(rf==Return_Terminate) break;
		measureMultiPop();
		if(isConvergingAll()&&rf==Return_Normal){
			if(CAST_PROBLEM_DYN&&!CAST_PROBLEM_DYN->predictChange(m_maxSubSize) || CAST_PROBLEM_DYN_ONEPEAK&&!CAST_PROBLEM_DYN_ONEPEAK->predictChange(m_maxSubSize)){
				m_subPop.push_back(move(unique_ptr<NFishSwarm>( new NFishSwarm(m_maxSubSize,true))));
			}else{
				m_subPop.push_back(move(unique_ptr<NFishSwarm>( new NFishSwarm(m_maxSubSize,false))));
			}	
			m_subPop[m_subPop.size()-1]->setVisual(25);
		}
		
		if(rf==Return_Normal) rf=exlude();

		int bestIdx=findBestPop();
		for(decltype(m_subPop.size()) i=0;i<m_subPop.size();++i){ 
			if(bestIdx!=i){
				m_subPop[i]->sleep(m_subPop[i]->m_curRadius, m_sleepRadius);
			}
		}

		//test for changes
		double oldFit=test.obj(0);
		if(CAST_PROBLEM_DYN&&!CAST_PROBLEM_DYN->predictChange(1) || CAST_PROBLEM_DYN_ONEPEAK&&!CAST_PROBLEM_DYN_ONEPEAK->predictChange(1))		rf=test.evaluate(true);
		else rf=test.evaluate(false);

		if(oldFit!=test.obj(0)){
			estimateShiftLength();

			//wakeup all swarms
			for(auto &s:m_subPop){
				s->m_flag[NFishSwarm::f_sleeping]=false; //set sleeping falg false
				s->m_bestLastEnvir=*s->m_best[0];
				s->m_flag[NFishSwarm::f_est_s]=true;  //set estimation shiftlength flag true
				s->m_flag[NFishSwarm::f_excl]=false; // set exclusion flag false
			}

			for(auto &s:m_subPop){
				if(s->isConverged(m_count,m_convRadius)){
					if(CAST_PROBLEM_DYN&&!CAST_PROBLEM_DYN->predictChange(m_maxSubSize) || CAST_PROBLEM_DYN_ONEPEAK&&!CAST_PROBLEM_DYN_ONEPEAK->predictChange(m_maxSubSize)) s->initialize(*s->m_best[0],m_shiftLength,true,false);
					else s->initialize(*s->m_best[0],m_shiftLength,false,false);
				}
				s->setVisual(0.4*m_shiftLength);
			}
		}
			
	}

	return rf;
}