#include "HmSOSwarm.h"
#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
#endif

HmSO::HmSO(ParamMap &v) :MultiPopulationCont<HmSOSubSwarm>(), Algorithm(-1,string(""))
{
    //ctor
    setDefaultPar();
	// m_subPop[0] is a parent swarm
	HmSOSubSwarm * hm=new HmSOSubSwarm(m_parentSize,1,true);
	addPopulation(*hm);
}


void HmSO::setDefaultPar()
{
    if(Global::msp_global->mp_problem->m_name=="DYN_CONT_MovingPeak")
	{
        m_parentSize=5;
        m_childSize=10;
        m_Rexcl=30;
        m_Rconv=0.1;//1.0;
        m_epsilon=5;
    }else if(Global::msp_global->mp_problem->m_name=="DYN_CONT_CompositionDBG"||Global::msp_global->mp_problem->m_name=="DYN_CONT_RotationDBG")
	{
        m_parentSize=5;
        m_childSize=10;
        m_Rexcl=3;
        m_Rconv=0.5;
        m_epsilon=5;
    }
}

ReturnFlag HmSO::responseChange()
{
	ReturnFlag rf=Return_Normal;
	for(int i=0;i<m_subPop[0]->getPopSize();i++)
		m_subPop[0]->getPop()[i]->representative()=m_subPop[0]->getPop()[i]->self();
	m_subPop[0]->getBest()[0]->self()=m_subPop[0]->getPop()[m_subPop[0]->findBest()[0]]->representative();

     //waken all child populations and do a random local search
	for(size_t i=1;i<m_subPop.size();i++)
    {
		rf=m_subPop[i]->waken();
		handleReturnFlagAll(rf);
		HANDLE_RETURN_FLAG(rf)
	}
	return rf;
}

void HmSO::checkConvergence()
{
     for(int j=1;j<m_subPop.size();j++)
		 if(m_subPop[j]->isConverged(m_Rconv)&&fabs(m_subPop[j]->getBest()[0]->obj(0)-m_gbest)<m_epsilon)
            m_subPop[j]->getFlag()[0]=false;
}

void HmSO::checkCllision()
{
    for(int i=1;i<m_subPop.size();i++){
        for(int j=i+1;j<m_subPop.size();j++){
           if( m_subPop[i]->getBest()[0]->getDistance(*m_subPop[j]->getBest()[0])<m_Rexcl){
                if(*m_subPop[i]->getBest()[0]>*m_subPop[j]->getBest()[0]){
					deletePopulation(j);
                    j--;
                }else{
					deletePopulation(i);
                    i--;
                    break;
                }
           }
        }
    }
}

void HmSO::updateGBest()
{
    m_gbest=m_subPop[0]->getBest()[0]->obj(0);

	for(size_t j=1;j<m_subPop.size();j++)
	{
		if(Global::msp_global->mp_problem->getOptType()==MIN_OPT)
		{
			if(m_subPop[j]->getBest()[0]->obj(0)<m_gbest) 
				m_gbest=m_subPop[j]->getBest()[0]->obj(0);
		}
		else 
		{
			if(m_subPop[j]->getBest()[0]->obj(0)>m_gbest) 
				m_gbest=m_subPop[j]->getBest()[0]->obj(0);
		}		
	}
}

ReturnFlag HmSO::evolve()
{
	ReturnFlag rf=Return_Normal;

	Solution<CodeVReal> x;
	x=m_subPop[0]->getBest()[0]->self();
	for(size_t i=0;i<m_subPop.size();i++)
	{

		if(m_subPop[i]->getPopSize()<1) return Return_Normal;

		// hibernation state
		if(m_subPop[i]->getFlag()[0]==false) return Return_Normal;


		for(int j=0;j<m_subPop[i]->getPopSize();j++)
		{
			rf=m_subPop[i]->getPop()[j]->move(m_subPop[i]->getBest()[0]->self(),m_subPop[i]->getW(),m_subPop[i]->getC1(),m_subPop[i]->getC2());
			if(rf!=Return_Normal) break;

			if(m_subPop[i]->getPop()[j]->self()>m_subPop[i]->getPop()[j]->representative())
				m_subPop[i]->getPop()[j]->representative()=m_subPop[i]->getPop()[j]->self();

			if(m_subPop[i]->getID()==1)
			{
				for(size_t z=1;z<m_subPop.size();z++)
				{
					if(m_subPop[i]->getPop()[j]->self().getDistance(m_subPop[z]->getBest()[0]->self())<m_subPop[i]->getInitialRadius())
					{
						if(m_subPop[i]->getPop()[j]->self()>m_subPop[z]->getBest()[0]->self())
							*m_subPop[z]->getBest()[0]=m_subPop[i]->getPop()[j]->self();
                  
						if(!dynamic_cast<DynamicProblem *>(Global::msp_global->mp_problem.get())->predictChange(1))              
						{
							rf=m_subPop[i]->getPop()[j]->initialize();  
							if(rf!=Return_Normal) break;
						}
						else 
						{
							rf=m_subPop[i]->getPop()[j]->initialize(false);  
							if(rf!=Return_Normal) break;
						}
					}
				}
		    }
			if(m_subPop[i]->getPop()[j]->self()>m_subPop[i]->getBest()[0]->self())
			{
				m_subPop[i]->getBest()[0]->self()=m_subPop[i]->getPop()[j]->self();
				if(m_subPop[i]->getID()==1){
                vector<int> idex;
                for(int z=0;z<m_subPop[i]->getPopSize();z++){
					if(m_subPop[i]->getBest()[0]->getDistance(m_subPop[i]->getPop()[j]->self())<m_subPop[i]->getInitialRadius()) 
						idex.push_back(z); ////
				}
                if(idex.size()<m_childSize){
					if(!dynamic_cast<DynamicProblem *>(Global::msp_global->mp_problem.get())->predictChange(m_childSize-idex.size())){
						HmSOSubSwarm * hm=new HmSOSubSwarm(m_subPop[i]->getBest()[0]->self(),m_subPop[i]->getInitialRadius()/3,m_childSize-idex.size(),m_subPop.size()+1,true);
						addPopulation(*hm); 
                    }else{
						HmSOSubSwarm * hm=new HmSOSubSwarm(m_subPop[i]->getBest()[0]->self(),m_subPop[i]->getInitialRadius()/3,m_childSize-idex.size(),m_subPop.size()+1,false);
						addPopulation(*hm);
                    }
                 }else{
					HmSOSubSwarm * hm= new HmSOSubSwarm(0,m_subPop.size()+1,false);
					addPopulation(*hm);
                }
                int z=0;
				while(m_subPop[m_subPop.size()-1]->getPopSize()<m_childSize)
				{
					m_subPop[m_subPop.size()-1]->add(m_subPop[i]->getPop()[idex[z]].get(),false);
					if(!dynamic_cast<DynamicProblem *>(Global::msp_global->mp_problem.get())->predictChange(1)) 
					{
						rf=m_subPop[i]->getPop()[idex[z]]->initialize();
						if(rf!=Return_Normal) break;
					}
                    else 
					{
						rf=m_subPop[i]->getPop()[idex[z]]->initialize(false);
						if(rf!=Return_Normal) break;
					}
                    z++;
                }
                idex.clear();
            }
        }
	}
	handleReturnFlagAll(rf);
	HANDLE_RETURN_FLAG(rf)
	if(rf==Return_Normal)
		m_subPop[i]->updateCurRadius();
	if(rf==Return_Change_Dim){
		rf=responseChange();
		break;
	}
	else{
		double fit=x.obj(0);
		if(!dynamic_cast<DynamicProblem * >(Global::msp_global->mp_problem.get())->predictChange(1)) 
		{
			rf=x.evaluate();
			handleReturnFlagAll(rf);
			HANDLE_RETURN_FLAG(rf)
		}
		if(fit!=x.obj(0)){
			rf=responseChange();
			break;
		}
	}
	}
	return rf;

}
ReturnFlag HmSO::run_()
{
	ReturnFlag rf=Return_Normal;

	 #ifdef OFEC_CONSOLE
	if(Global::msp_global->m_runId==0){
		mSingleObj::getSingleObj()->setProgrOutputFlag(true);
		if(mMultiModal::getPopInfor())
		mMultiModal::getPopInfor()->setOutProgFlag(true);
	}
	#endif // OFEC_CONSOLE

    while(!ifTerminating())
	{
		rf=evolve();

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

		if(rf==Return_Terminate) break;
		if(m_subPop.size()==1) m_subPop[0]->getFlag()[0]=true;
		updateGBest();
		checkConvergence();
		checkCllision();
    }
    return rf;
}
