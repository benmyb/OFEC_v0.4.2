#include "HmSOSubSwarm.h"

HmSOSubSwarm::HmSOSubSwarm(const int popsize,const int id, bool mode) :Swarm<CodeVReal,Particle>(popsize,mode)
{
    initializePara(0.729844,1.496180,1.496180);
    m_flag[0]=true;       // true for being in a active state; false for being in hibernation state
    setID(id);
     if(m_popID==1){
		 if(Global::msp_global->mp_problem->m_name=="DYN_CONT_MovingPeak"){ 
            for(int i=0;i<m_popsize;i++) m_pop[i]->setVmax(-50,50);
				setRadius(30);
         }else if(Global::msp_global->mp_problem->m_name=="DYN_CONT_CompositionDBG"||Global::msp_global->mp_problem->m_name=="DYN_CONT_RotationDBG"){
            for(int i=0;i<m_popsize;i++) m_pop[i]->setVmax(-3,3);
				setRadius(3);
         }		 
    }else{
        if(Global::msp_global->mp_problem->m_name=="DYN_CONT_MovingPeak"){
            for(int i=0;i<m_popsize;i++) m_pop[i]->setVmax(-10,10);
				setRadius(30);
         }else if(Global::msp_global->mp_problem->m_name=="DYN_CONT_CompositionDBG"||Global::msp_global->mp_problem->m_name=="DYN_CONT_RotationDBG"){
            for(int i=0;i<m_popsize;i++) m_pop[i]->setVmax(-1,1);
				setRadius(3);
         }
    }
}

HmSOSubSwarm::HmSOSubSwarm(const Solution<CodeVReal> & center, double radius,const int rPopsize,const int id,bool mode) :Swarm<CodeVReal,Particle>(center,radius,rPopsize,mode)
{
	setDefaultPar();
	initializePara(0.729844,1.496180,1.496180);
    setID(id);
    m_flag[0]=true;        // true for being in a active state; false for being in hibernation state
     if(m_popID==1){
         if(Global::msp_global->mp_problem->m_name=="DYN_CONT_MovingPeak"){
            for(int i=0;i<m_popsize;i++) m_pop[i]->setVmax(-50,50);
				setRadius(30);
         }else if(Global::msp_global->mp_problem->m_name=="DYN_CONT_CompositionDBG"||Global::msp_global->mp_problem->m_name=="DYN_CONT_RotationDBG"){
            for(int i=0;i<m_popsize;i++) m_pop[i]->setVmax(-3,3);
				setRadius(3);
         }
    }else{
        if(Global::msp_global->mp_problem->m_name=="DYN_CONT_MovingPeak"){
            for(int i=0;i<m_popsize;i++) m_pop[i]->setVmax(-10,10);
				setRadius(30);
         }else if(Global::msp_global->mp_problem->m_name=="DYN_CONT_CompositionDBG"||Global::msp_global->mp_problem->m_name=="DYN_CONT_RotationDBG"){
            for(int i=0;i<m_popsize;i++) m_pop[i]->setVmax(-1,1);
				setRadius(3);
         }
    }
}

void HmSOSubSwarm::setDefaultPar()
{
    if(Global::msp_global->mp_problem->m_name=="DYN_CONT_MovingPeak")
        m_Rsearch=0.5;
    else if(Global::msp_global->mp_problem->m_name=="DYN_CONT_CompositionDBG"||Global::msp_global->mp_problem->m_name=="DYN_CONT_RotationDBG")
        m_Rsearch=0.1;
}

void HmSOSubSwarm::updateCurRadius(bool mode)
{
	// mode=true for distance between representatives; false for pself
    // the distance between two furthest particles
    m_curRadius=0;
    for(int i=0;i<m_popsize;i++){
        for(int j=i+1;j<m_popsize;j++){
            if(mode){
                double dis=m_pop[i]->representative().getDistance(m_pop[j]->representative());
                if(dis>m_curRadius) m_curRadius=dis;
            }else{
				double dis=m_pop[i]->self().getDistance(m_pop[j]->self());
                if(dis>m_curRadius) m_curRadius=dis;
            }
        }
    }
}

ReturnFlag HmSOSubSwarm::waken()
{
	ReturnFlag rf=Return_Normal;
	for(int i=0;i<m_popsize;i++)
	{
		if(!dynamic_cast<DynamicProblem *>(Global::msp_global->mp_problem.get())->predictChange(1))  
		{
			rf=m_pop[i]->initialize(*m_best[0],m_Rsearch,i,i+1,true);
			if(rf!=Return_Normal) return rf;
		}
        else 
		{
			rf=m_pop[i]->initialize(*m_best[0],m_Rsearch,i,i+1,false);
			if(rf!=Return_Normal) return rf;
		}
    }
	*m_best[0]=m_pop[findBest()[0]]->representative();
     m_flag[0]=true;
	 return rf;
}

