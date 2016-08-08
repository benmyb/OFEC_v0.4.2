#include "AdaptParticle.h"
#include "../../../Global/global.h"


AdaptParticle::AdaptParticle(): Particle()
{
}

ReturnFlag AdaptParticle::move( const Solution<CodeVReal> &lbest,double w, double c1, double c2)
{
	double u,l;
	for(int j=0;j<Global::msp_global->mp_problem->getNumDim();j++)
	{
		CAST_PROBLEM_CONT->getSearchRange(l,u,j);
		double x=data()[j];
		m_vel[j]=w*m_vel[j]+c1*Global::msp_global->mp_uniformAlg->Next()*(m_pbest.data()[j]-x)+c2*Global::msp_global->mp_uniformAlg->Next()*(lbest.data()[j]-x);
								
		if(fabs(m_vel[j])>m_vMax[j].m_max)
			m_vel[j]=gSign(m_vel[j])*m_vMax[j].m_max;

		data()[j]+=m_vel[j];
	}
	 self().validate();
	 // In the original paper, the new created individual is not be validated. It may lead to create many unvalidated individuals in the population and the algorithm 
	 // is at a standstill. So here we modified it.
	//if(!Global::msp_global->mp_problem->isValid(self().m_x))		
	return self().evaluate();
}