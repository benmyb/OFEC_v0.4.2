#include "SAMOSwarm.h"

#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
#endif

using namespace std; 

//#define OFEC_DEBUG_ 


SAMOSwarm::SAMOSwarm(ParamMap &v):Algorithm(-1,(v[param_algName])),MultiPopulationCont<MQSOSubSwarm>((v[param_subPopSize]),(v[param_overlapDgre])),\
	m_ConvFactor((v[param_convFactor]))
{
	initialize();
	
}

void SAMOSwarm::updateRadius(){
	double u,l;
    int d=Global::msp_global->mp_problem->getNumDim();
    CAST_PROBLEM_CONT->getSearchRange(l,u,0);

    m_Rexcl=0.5*(u-l)/pow((double)m_M, 1./d);

	if(m_ConvFactor==-1)    m_Rconv=2.*m_Rexcl;
	else m_Rconv=m_ConvFactor*m_Rexcl;
}

void SAMOSwarm::initialize()
{
	 m_M=1;
	 m_Nexcess=3;
	 m_Nfree=1;
	 updateRadius();
	 for(int i=0;i<m_M;i++){
		MQSOSubSwarm *s=new MQSOSubSwarm(m_maxSubSize,true);
		s->initializePara(0.729843788,2.05,2.05);
        addPopulation(*s);
		if(m_Nexcess<m_Nfree) throw myException("error@construct SAMOSwarm");
    }
}

void SAMOSwarm::checkConvergenceAll(){
    bool converging;
    for(int i=0;i<m_M;i++){
		converging=true;
        for(int j=0;j<m_subPop[i]->m_popsize;j++){
            for(int k=j+1;k<m_subPop[i]->m_popsize;k++){
				if(m_subPop[i]->m_pop[j]->m_type!=MQSOParticle::PARTICLE_NEUTRAL||m_subPop[i]->m_pop[k]->m_type!=MQSOParticle::PARTICLE_NEUTRAL ) continue;
               
				if(fabs(m_subPop[i]->m_pop[j]->self().getDistance(m_subPop[i]->m_pop[k]->self()))>m_Rconv){
                        // a free swarm becomes a converging swarm when the distance of two farthest particles is less than ms_Rconv
						converging=false;
                        break;
                 }

            }
			if(!converging) break;
        }
		if(converging&&m_subPop[i]->m_swarmType==2) {
			m_subPop[i]->m_swarmType=1;
			m_Nfree--;
		}
    }

}
void SAMOSwarm::exclude(){
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
int SAMOSwarm::findWorstFreeSwarm(){
	if(m_subPop.size()<1) return -1;
    int idx=0;
	while(m_subPop[idx]->m_swarmType!=2) idx++;

    for(unsigned int i=idx+1;i<m_subPop.size();i++){
		if(m_subPop[i]->m_swarmType!=2) continue;
		if(*m_subPop[i]->m_best[0]<(*m_subPop[idx]->m_best[0])) idx=i;
	}
	return idx;
}
ReturnFlag SAMOSwarm::run_(){
	#ifdef OFEC_CONSOLE
	if(Global::msp_global->m_runId==0){
		mSingleObj::getSingleObj()->setProgrOutputFlag(true);
		if(mMultiModal::getPopInfor())
		mMultiModal::getPopInfor()->setOutProgFlag(true);
	}
	#endif // OFEC_CONSOLE
	#ifdef OFEC_DEBUG_
	static vector<vector<double>> distri(30,vector<double>(1000,0));
	#endif
	ReturnFlag r_flag=Return_Normal;
    while(!ifTerminating()){	
		//cout<<Global::msp_global->mp_problem->getEvaluations()<<" "<<m_M<<endl;
        for(int i=0;i<m_M;i++){
			Solution<CodeVReal> x(*m_subPop[i]->m_best[0]);
			 vector<double> objOld=x.obj(); 

			if(!m_subPop[i]->m_flag[0])   r_flag=m_subPop[i]->evolve();
			
			handleReturnFlagAll(r_flag);
			HANDLE_RETURN_FLAG(r_flag)

			#ifdef OFEC_DEMON
					vector<Algorithm*> vp;
					for(auto &it:m_subPop){
						vp.push_back(it.get());
					}
					msp_buffer->updateBuffer_(&vp);
			#endif
					
			if(CAST_PROBLEM_DYN&&!CAST_PROBLEM_DYN->predictChange(1) || CAST_PROBLEM_DYN_ONEPEAK&&!CAST_PROBLEM_DYN_ONEPEAK->predictChange(1))    r_flag=x.evaluate(true);
			handleReturnFlagAll(r_flag);
			HANDLE_RETURN_FLAG(r_flag)

				if(x.obj(0)!=objOld[0]){
				for(int i=0;i<m_M;i++)
					m_subPop[i]->m_flag[0]=false;		
			}

			m_subPop[i]->m_initialRadius=m_Rexcl;
        }
		if(r_flag==Return_Terminate) break;

		measureMultiPop();
		// test for convergence
		checkConvergenceAll();
        //exclusion
        exclude();

        for(int i=0;i<m_M;i++){
			if(m_subPop[i]->m_flag[0]&&(CAST_PROBLEM_DYN&&!CAST_PROBLEM_DYN->predictChange(m_subPop[i]->m_popsize) || CAST_PROBLEM_DYN_ONEPEAK&&!CAST_PROBLEM_DYN_ONEPEAK->predictChange(m_subPop[i]->m_popsize))){
				if(CAST_PROBLEM_DYN&&CAST_PROBLEM_DYN->predictChange(m_subPop[i]->m_popsize) || CAST_PROBLEM_DYN_ONEPEAK&&CAST_PROBLEM_DYN_ONEPEAK->predictChange(m_subPop[i]->m_popsize)){
					r_flag=m_subPop[i]->Population::initialize(false,false,true);
				}else{
					r_flag=m_subPop[i]->Population::initialize(false,true,true);
				}
				m_subPop[i]->m_flag[0]=false;
				
				// if pop i is a converging swarm then change its status to free swarm
				if(m_subPop[i]->m_swarmType==1){
					m_Nfree++;
				}
				m_subPop[i]->m_swarmType=2;
				handleReturnFlagAll(r_flag);
				HANDLE_RETURN_FLAG(r_flag)
			}
        }

		// check the number of free swarms
		if(m_Nfree<=0){
		
			if(CAST_PROBLEM_DYN&&!CAST_PROBLEM_DYN->predictChange(m_subPop[0]->getPopSize()) || CAST_PROBLEM_DYN_ONEPEAK&&!CAST_PROBLEM_DYN_ONEPEAK->predictChange(m_subPop[0]->getPopSize())){
				m_subPop.push_back(move(unique_ptr<MQSOSubSwarm>(new MQSOSubSwarm(m_subPop[0]->getPopSize(),true))));
				
			}else{
				m_subPop.push_back(move(unique_ptr<MQSOSubSwarm>(new MQSOSubSwarm(m_subPop[0]->getPopSize(),false))));
			}
			m_Nfree++;
			m_M++;
			#ifdef OFEC_DEBUG_
			g_mutex.lock();
			distri[Global::msp_global->m_runId][m_M]+=1;
			g_mutex.unlock();
			#endif
			if(ifTerminating()) break;
		}
		while(m_Nfree>m_Nexcess){
			unsigned int w=findWorstFreeSwarm();
			m_subPop.erase(m_subPop.begin()+w);
			m_Nfree--;
			m_M--;
			#ifdef OFEC_DEBUG_
			g_mutex.lock();
			distri[Global::msp_global->m_runId][m_M]+=1;
			g_mutex.unlock();
			#endif
		}
		updateRadius();
     }

#ifdef OFEC_DEBUG_
	g_mutex.lock();
	
	string ss=Global::g_arg[param_workingDir];
	ss+="Result/";
	ss+=mSingleObj::getSingleObj()->m_fileName.str();
	ss+="distr.txt";
	vector<double> res(1000,0);
	vector<double> tnum(MAX_NUM_RUN,0);

	for(int i=0;1000>i;i++){
		for(int j=0;MAX_NUM_RUN>j;j++)		tnum[j]+=distri[j][i];
	}

	for(int i=0;1000>i;i++){
		for(int j=0;MAX_NUM_RUN>j;j++)		res[i]+=distri[j][i]/tnum[j];
		res[i]/=(int)MAX_NUM_RUN;
	}
	ofstream out(ss.c_str());
	for(int i=0;1000>i;i++){
		out<<i<<" "<<res[i]<<endl;
	}
	out.close();
	g_mutex.unlock();
#endif

	return Return_Terminate;

}
