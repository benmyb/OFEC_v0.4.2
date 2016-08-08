#include "FTSwarm.h"
#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
#endif

FTSwarm::FTSwarm(ParamMap &v):Algorithm(-1,"ALG_FTMPSO"), m_k(2),m_convLimit(1.0),m_numActive(1){

	double u,l;
	int d=GET_NUM_DIM;
	int peaks = 0;
	if(CAST_PROBLEM_DYN)	peaks=CAST_PROBLEM_DYN->getNumberofPeak();
	else if(CAST_PROBLEM_DYN_ONEPEAK) peaks = CAST_PROBLEM_DYN_ONEPEAK->getNumPeak();


	CAST_PROBLEM_CONT->getSearchRange(l,u,0);
	m_rExcl=0.5*(u-l)/pow((double)peaks, 1./d);
	
	//creat a finder swarm with 10 particles
	FTSubSwarm * s=new FTSubSwarm(10);
	m_preFinderBest.push_back(*s->m_best[0]);
	addPopulation(*s);
	m_algPar<<"Finder size: 10, Tracker size: 5, E_try: 20 conv_limit: 1";
}

void FTSwarm::excludeFinder(){
	// check if the finder swarm (m_subPop[0]) is within an active tracker swarm
	for(decltype(m_subPop.size())  i=1;i<getNumPops();i++){
		if(m_subPop[i]->m_flag[0]) continue;
		if(m_subPop[0]->m_best[0]->getDistance(*m_subPop[i]->m_best[0])<=m_rExcl){
			if(CAST_PROBLEM_DYN&&CAST_PROBLEM_DYN->predictChange(m_subPop[0]->m_popsize + 1) || CAST_PROBLEM_DYN_ONEPEAK&&CAST_PROBLEM_DYN_ONEPEAK->predictChange(m_subPop[0]->m_popsize + 1))			m_subPop[0]->reInitialize(true,false);
			else		m_subPop[0]->reInitialize(true,true);
			break;
		}
	}
	
}
void FTSwarm::excludeTracker(){
	// check exclusion between tracker active swarms
	for(decltype(m_subPop.size()) i=1;i<m_subPop.size();i++){
		if(m_subPop[i]->m_flag[0]) continue;
		for(unsigned j=i+1;j<getNumPops();j++){
			if(m_subPop[j]->m_flag[0]) continue;
			if(m_subPop[i]->m_best[0]->getDistance(*m_subPop[j]->m_best[0])<=m_rExcl){
				if(*(m_subPop[i]->m_best[0])>*(m_subPop[j]->m_best[0]))		m_subPop[j]->m_flag[0]=true;
				else		m_subPop[i]->m_flag[0]=true;
			}
		}
	}
}
int FTSwarm::isFinderInTracker(){
		// check if the finder swarm (m_subPop[0]) is within an active tracker swarm
	for(decltype(m_subPop.size()) i=1;i<getNumPops();i++){
		if(m_subPop[i]->m_flag[0]) continue;
		if(m_subPop[0]->m_best[0]->getDistance(*m_subPop[i]->m_best[0])<=m_rExcl){
			return i;
		}
	}
	return -1;
}
bool FTSwarm::detectChange(){
	
	for(auto &swarm:m_subPop){
		vector<double> objOld=swarm->m_best[0]->obj();
		if(CAST_PROBLEM_DYN&&CAST_PROBLEM_DYN->predictChange(2) || CAST_PROBLEM_DYN_ONEPEAK&&CAST_PROBLEM_DYN_ONEPEAK->predictChange(2))  swarm->m_best[0]->evaluate(false);
		else swarm->m_best[0]->evaluate(true);
		vector<double> objNew=swarm->m_best[0]->obj();
		if(objOld[0]!=objNew[0]) return true;
	}
	return false;

}
void FTSwarm::responseChange(){
	vector<unique_ptr<FTSubSwarm>>::iterator it=m_subPop.begin()+1;
	for(;it!=m_subPop.end();++it){
		(*it)->m_flag[0]=false;
		(*it)->m_flag[1]=false;
		(*it)->afterChange();
	}
	m_numActive=m_subPop.size();
}
ReturnFlag FTSwarm::run_(){

	ReturnFlag rf=Return_Normal;
	#ifdef OFEC_CONSOLE
	if(Global::msp_global->m_runId==0){
		mSingleObj::getSingleObj()->setProgrOutputFlag(true);
	if(mMultiModal::getPopInfor())
		mMultiModal::getPopInfor()->setOutProgFlag(true);
	}
	#endif // OFEC_CONSOLE

	while(!ifTerminating()){
		rf=m_subPop[0]->evolve();
		if(rf==Return_Normal){
			excludeFinder();
			activate();
			if(m_subPop.size()>1){
				vector<unique_ptr<FTSubSwarm>>::iterator it=m_subPop.begin()+1;
				int bestIdx=findBestPop(1);
				//cout<<Global::msp_global->mp_problem->getEvaluations()<<" "<<getNumPops()<<" "<<m_subPop[bestIdx]->m_best[0]->obj(0)<<endl;
				for(;it!=m_subPop.end();++it){
					if((*it)->m_flag[1]) continue;
					rf=(*it)->evolve();
					if(rf!=Return_Normal) break;
					rf=(*it)->localSearch();
					if(rf!=Return_Normal) break;
					if(bestIdx!=it-m_subPop.begin()) 	if((*it)->sleep()) --m_numActive;
					//printActive();
				}
				if(rf==Return_Normal)		excludeTracker();
			}
		}
		if(rf==Return_Terminate) break;

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
		
		if(detectChange()){
				responseChange();
		}
		if(rf!=Return_Normal){
			handleReturnFlagAll(rf);
			HANDLE_RETURN_FLAG(rf)
		}
		removeInactive();
	}
	return Return_Normal;
}

void FTSwarm::activate(){
	m_preFinderBest.push_back(*m_subPop[0]->m_best[0]);	
	if(m_subPop[0]->m_iter>=m_k){
		m_preFinderBest.pop_front();
	}
	if(m_subPop[0]->m_iter%2==0){
		// create a new tracker swarm
		if(m_preFinderBest.front().getDistance(m_preFinderBest.back())<m_convLimit){
			int idx=isFinderInTracker();
			int numPeak = 0;
			if (CAST_PROBLEM_DYN_CONT) numPeak = CAST_PROBLEM_DYN_CONT->getNumberofPeak();
			else if (CAST_PROBLEM_DYN_ONEPEAK) numPeak = CAST_PROBLEM_DYN_ONEPEAK->getNumPeak();
			if(m_numActive-1<numPeak){
				if(idx==-1){
					m_subPop[0]->sort(true,true);
					FTSubSwarm * s=0;
					if(CAST_PROBLEM_DYN&&CAST_PROBLEM_DYN->predictChange(6) || CAST_PROBLEM_DYN_ONEPEAK&&CAST_PROBLEM_DYN_ONEPEAK->predictChange(6)) s=new FTSubSwarm(5,false);
					else s=new FTSubSwarm(5);
					for(int i=0;i<s->m_popsize;i++){
						*s->m_pop[i]=*m_subPop[0]->m_pop[m_subPop[0]->m_orderList[i]];
					}
					*s->m_best[0]=*m_subPop[0]->m_best[0];
					addPopulation(*s);
					++m_numActive;
					//printActive();
				}
				if(CAST_PROBLEM_DYN&&CAST_PROBLEM_DYN->predictChange(m_subPop[0]->m_popsize + 1) || CAST_PROBLEM_DYN_ONEPEAK&&CAST_PROBLEM_DYN_ONEPEAK->predictChange(m_subPop[0]->m_popsize + 1)) m_subPop[0]->reInitialize(true,false);
				else m_subPop[0]->reInitialize(true,true);
			}else{
				if(idx!=-1){
					m_subPop[idx]->m_flag[0]=true;
				}
			}
		}
	}

}
void FTSwarm::printActive(){
	vector<unique_ptr<FTSubSwarm>>::iterator it=m_subPop.begin();
	cout<<m_numActive<<": ";
	for(;it!=m_subPop.end();++it){
		cout<<(*it)->m_flag[1]<<' ';
	}
	cout<<endl;
}

void FTSwarm::removeInactive(){
	if(m_subPop.size()<2) return;
	while(m_subPop.size()>1){
		vector<unique_ptr<FTSubSwarm>>::iterator it=m_subPop.begin()+1;
		int num=1;
		for(;it!=m_subPop.end();++it,++num){
			if((*it)->m_flag[0]) break; 
		}
		if(num==m_subPop.size()) break;
		if(!(*it)->m_flag[1]) --m_numActive;
		m_subPop.erase(m_subPop.begin()+num);
	}
}