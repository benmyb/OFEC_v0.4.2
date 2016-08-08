#include "APSO.h"
#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
#endif

APSO::APSO(ParamMap &v):Swarm<CodeVReal,AdaptParticle>(v[param_popSize],true),m_CurGen(0),m_CurState(Exploration),m_PreState(Exploration){
	//CalculateEF();
	if(Global::g_arg.find(param_maxEvals)==Global::g_arg.end()) 
		Global::g_arg[param_maxEvals]=10000*Global::msp_global->mp_problem->getNumDim();
}

void APSO::calculateEF(){

	m_MinDis=0;
	double l,u;
	for(int i=0;i<Global::msp_global->mp_problem->getNumDim();i++){
        CAST_PROBLEM_CONT->getSearchRange(l,u,i);
        m_MinDis+=(u-l)*(u-l);
    }
    m_MinDis=sqrt(m_MinDis);

	m_MaxDis=0;
	double dis;
	int indexBest=findBest()[0];
	for(int i=0;i<this->m_popsize;i++){
		dis=0;
		for(int j=0;j<this->m_popsize;j++){
			if(i==j) continue;
			dis+=this->m_pop[i]->self().getDistance(this->m_pop[j]->self());
		}
		dis/=(this->m_popsize-1);
		if(dis>m_MaxDis) m_MaxDis=dis;
		if(dis<m_MinDis) m_MinDis=dis;
		if(indexBest==i) m_BestDis=dis;
	}
	m_ef=(m_BestDis-m_MinDis)/(m_MaxDis-m_MinDis);

	if(m_ef>=0&&m_ef<=0.2){
		m_PreState=m_CurState;
		m_CurState=Convergence;
	}else if(m_ef>0.2&&m_ef<=0.3){
		m_PreState=m_CurState;
		if(m_PreState==Exploration||m_PreState==Exploitation)
			m_CurState=Exploitation;
		else
			m_CurState=Convergence;
	}else if(m_ef>0.3&&m_ef<=0.4){
		m_PreState=m_CurState;
		m_CurState=Exploitation;

	}else if(m_ef>0.4&&m_ef<=0.6){
		m_PreState=m_CurState;
		if(m_PreState==Exploration||m_PreState==JumpingOut)
			m_CurState=Exploration;
		else
			m_CurState=Exploitation;

	}else if(m_ef>0.6&&m_ef<=0.7){
		m_PreState=m_CurState;
		m_CurState=Exploration;
	}else if(m_ef>0.7&&m_ef<=0.8){
		m_PreState=m_CurState;
		if(m_PreState==Convergence||m_PreState==JumpingOut)
			m_CurState=JumpingOut;
		else
			m_CurState=Exploration;

	}else{
		m_PreState=m_CurState;
		m_CurState=JumpingOut;
	}
}

ReturnFlag APSO::updateAcceleration(){
	ReturnFlag rf=Return_Normal;
	if(m_CurState==Convergence){
		increaseC1(0.5);
		increaseC2(0.5);
		rf=elitismLearning();
		if(rf!=Return_Normal) return rf;
	}else if(m_CurState==Exploitation){
		increaseC1(0.5);
		decreaseC2(0.5);
	}else if(m_CurState==Exploration){
		increaseC1();
		decreaseC2();

	}else{
		decreaseC1();
		increaseC2();
	}
	return rf;
}

ReturnFlag APSO::elitismLearning(){
	ReturnFlag rf=Return_Normal;
	Solution<CodeVReal> p;
	p=*m_best[0];
	int d=Global::msp_global->getRandInt(0,Global::msp_global->mp_problem->getNumDim());
	double sigma=1.0-0.9*m_CurGen/(Global::g_arg[param_maxEvals]/m_popsize);
	double l,u,x;
	CAST_PROBLEM_CONT->getSearchRange(l,u,0);
	x=p.data()[d];
	x=x+(u-l)*Global::msp_global->mp_normalAlg->NextNonStand(0,sigma);
	
	if(x>u||x<l) return rf;

	p.data()[d]=x;
	rf=p.evaluate();
	if(rf!=Return_Normal) return rf;

	if(p>*m_best[0]) *m_best[0]=p;
	else m_pop[findWorst()[0]]->self()=p.self();

	return rf;
}
ReturnFlag APSO::evolve(){
	ReturnFlag rf=Return_Normal;
	m_CurGen=Global::msp_global->mp_problem->getEvaluations()/Global::g_arg[param_maxEvals];
	calculateEF();

	updateW(m_ef);
	rf=updateAcceleration();
	if(rf!=Return_Normal) return rf;

	if(m_C1+m_C2>4){
		m_C1=m_C1*4/(m_C1+m_C2);
		m_C2=m_C2*4/(m_C1+m_C2);
	}

	for(int i=0;i<m_popsize;i++){

		rf=m_pop[i]->move(*m_best[0],m_W,m_C1,m_C2);
		if(rf!=Return_Normal) return rf;

		if(m_pop[i]->isValid() && m_pop[i]->self()>m_pop[i]->representative())
		{
			m_pop[i]->representative()=m_pop[i]->self();
			if(*m_pop[i]>*m_best[0]) *m_best[0]=*m_pop[i];
		}		  
	}
	return rf;
}
void APSO::increaseC1(double weight){
	m_C1+=weight*Global::msp_global->getRandFloat(0.05,0.1);
}
 void APSO::decreaseC1(double weight){
	 m_C1-=weight*Global::msp_global->getRandFloat(0.05,0.1);
 }
 void APSO::increaseC2(double weight){
	 m_C2+=weight*Global::msp_global->getRandFloat(0.05,0.1);
 }
 void APSO::decreaseC2(double weight){
	 m_C2-=weight*Global::msp_global->getRandFloat(0.05,0.1);
 }
 void APSO::updateW(double f){

	 m_W=1/(1+1.5*exp(-2.6*f));
 }

 ReturnFlag APSO::run_(){

	 ReturnFlag rf=Return_Normal;

	 #ifdef OFEC_CONSOLE
	if(Global::msp_global->m_runId==0){
		mSingleObj::getSingleObj()->setProgrOutputFlag(true);
		if(mMultiModal::getPopInfor())
		mMultiModal::getPopInfor()->setOutProgFlag(true);
	}
	#endif // OFEC_CONSOLE

	 while(!ifTerminating()){

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

		#ifdef OFEC_DEMON
			vector<Algorithm*> vp;	
			vp.push_back(this);	
			msp_buffer->updateBuffer_(&vp);
		#endif

		if(rf==Return_Terminate) break;
    }
        return rf;
}
