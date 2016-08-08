#include "CooperativeParticle.h"
#include "../../../Global/global.h"

CooperativeParticle::CooperativeParticle() :Particle()
{
}

ReturnFlag CooperativeParticle::move(Solution<CodeVReal> &lbest,const vector<int> &members, int num,const Solution<CodeVReal> &global_best,double w, double c1, double c2)
{
	double l,u;
    int k,j;
    for( k=0;k<num;k++)
	{
        j=members[k];
        CAST_PROBLEM_CONT->getSearchRange(l,u,0);
		double x=data()[j];    
		m_vel[j]=w*m_vel[j]+c1*Global::msp_global->mp_uniformAlg->Next()*(m_pbest.data()[j]-x)+c2*Global::msp_global->mp_uniformAlg->Next()*(lbest.data()[j]-x);

		if(m_vel[j]>m_vMax[j].m_max)	m_vel[j]=m_vMax[j].m_max;
		else if(m_vel[j]<m_vMax[j].m_min)	m_vel[j]=m_vMax[j].m_min;

		data()[j]+=m_vel[j];
		if(data()[j]>u||data()[j]<l) data()[j]= gSign(data()[j])*u;
    }

    for( k=0;k<members[0];k++)
        data()[k]=global_best.data()[k];
	for(k=members[num-1]+1;k<Global::msp_global->mp_problem->getNumDim();k++)
        data()[k]=global_best.data()[k];
            	
	return self().evaluate();
}