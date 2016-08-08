#include "FTParticle.h"
#include "../../../Global/global.h"
ReturnFlag FTParticle::move( const Solution<CodeVReal> &lbest,double w, double c1, double c2){
	double u,l;

	for( int j=0;j<GET_NUM_DIM;j++){
		CAST_PROBLEM_CONT->getSearchRange(l,u,j);
		double x=data()[j];
		m_vel[j]=w*(m_vel[j]+c1*Global::msp_global->mp_uniformAlg->Next()*((m_pbest.data()[j])-x)+c2*Global::msp_global->mp_uniformAlg->Next()*((lbest.data()[j])-x));

		if(m_vel[j]>m_vMax[j].m_max)	m_vel[j]=m_vMax[j].m_max;
		else if(m_vel[j]<m_vMax[j].m_min)		m_vel[j]=m_vMax[j].m_min;

		data()[j]=x+m_vel[j];
	}
	self().validate();
	return self().evaluate();

}