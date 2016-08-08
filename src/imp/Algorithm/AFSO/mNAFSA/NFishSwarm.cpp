#include "NFishSwarm.h"
#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
#endif
extern mutex g_mutexStream;
NFishSwarm::NFishSwarm(ParamMap & v):PopulationCont<CodeVReal, NFish>(v[param_popSize],v[param_evalCountFlag]),m_tryNumber(2),m_limit(0.75),\
m_bestLastEnvir(GET_NUM_DIM,Global::msp_global->mp_problem->getNumObj()){
	setVisual(CAST_PROBLEM_CONT->getSearchRange().getDomainSize()/4);
	m_flag.resize(3);
	m_flag[NFishSwarm::f_sleeping]=m_flag[NFishSwarm::f_est_s]=m_flag[NFishSwarm::f_excl]=false;

}
NFishSwarm::NFishSwarm(int size,bool flag):PopulationCont<CodeVReal, NFish>(size,flag),m_tryNumber(2),m_limit(0.75),\
m_bestLastEnvir(GET_NUM_DIM,Global::msp_global->mp_problem->getNumObj()){
	setVisual(CAST_PROBLEM_CONT->getSearchRange().getDomainSize()/4);
	m_flag.resize(3);
	m_flag[0]=m_flag[1]=m_flag[2]=false;
}
ReturnFlag NFishSwarm::run_(){
	ReturnFlag rf=Return_Normal;
	Global::msp_global->mp_problem->setValidateMode(VALIDATION_SETTOBOUND);
	while(!ifTerminating()){
		g_mutexStream.lock();
	//	cout<<Global::msp_global->m_runId<<" "<<Global::msp_global->mp_problem->getEvaluations()<<" "<<m_best[0]->obj(0)<<endl;
		g_mutexStream.unlock();
		#ifdef OFEC_DEMON
			vector<Algorithm*> vp;	
			vp.push_back(this);	
			msp_buffer->updateBuffer_(&vp);
		#endif
        rf=evolve();
    }
	 
     return rf;
}
ReturnFlag NFishSwarm::evolve(){
	ReturnFlag rf=Return_Normal;
	//new prey behavior
	for(auto &fish:m_pop){
		for(int i=0;i<m_tryNumber;++i){
			rf=fish->prey(m_visual);
			if(rf!=Return_Normal) return rf;
			updateBestArchive(fish->self());
		}
	}
	
	//new follow behavior
	for(auto &fish:m_pop){
		rf=fish->follow(*m_best[0],m_visual);
		if(rf!=Return_Normal) return rf;
		updateBestArchive(fish->self());
	}

	//new swarm behevior
	computeCenter();
	findBest();
	for(auto &fish:m_pop){
		if(m_center>fish->self()){
			if(fish->m_index!=m_bestIdx[0]){
				rf=fish->swarm(m_center,m_visual);
				if(rf!=Return_Normal) return rf;
			}else{
				fish->self()=m_center;	
			}
			updateBestArchive(fish->self());
		}
	}
	updateVisual();
	updateCurRadius();
	m_iter++;
	return rf;
}
void NFishSwarm::updateVisual(){
	m_visual*=m_limit+Global::msp_global->mp_uniformAlg->Next()*(1-m_limit);
}
void NFishSwarm::setVisual(double visual){
	m_visual=visual;
}
void NFishSwarm::sleep(double dis,double r){
	if(dis<=r)	m_flag[NFishSwarm::f_sleeping]=true;
}
void NFishSwarm::wakeup(){
	m_flag[NFishSwarm::f_sleeping]=false;
}
bool NFishSwarm::isConverged(int count,double r){
	if(m_iter>count){
		if(m_best[0]->getDistance(*m_preBest.begin())<r) return true;
	}
	return false;
}
void NFishSwarm::updatePreBest(int count){
	if(m_iter>=count) m_preBest.pop_front();
	m_preBest.push_back(*m_best[0]);
}

void NFishSwarm::computeCenter(){
	if(this->m_popsize<1) return;
		
	if(Global::msp_global->mp_problem->isProTag(CONT)){
		for( int i=0;i<GET_NUM_DIM;i++){
			double x=0.;
			for(int j=0;j<this->m_popsize;j++){
				Solution<CodeVReal> & chr=this->m_pop[j]->representative();
				x=chr.data()[i]+x;
			}
			m_center.data()[i]=x/this->m_popsize;

		}			
		m_center.evaluate(false);
	}
}