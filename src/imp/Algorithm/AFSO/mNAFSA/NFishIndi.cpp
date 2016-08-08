#include "NFishIndi.h"
#include "../../../Utility/definition.h"
#include "../../../Global/global.h"
ReturnFlag NFish::prey(double visual){
	Solution<CodeVReal> t;

	for(int i=0;i<GET_NUM_DIM;++i){
		t.data()[i]=m_x[i]+visual*Global::msp_global->getRandFloat(-1,1);
	}
	t.validate();
	ReturnFlag rf=t.evaluate();
	if(rf==Return_Normal&&t>*this) Solution<CodeVReal>::operator=(t);
	return rf;
}
ReturnFlag NFish::follow(const Solution<CodeVReal> & best,double visual){
	double dis=best.getDistance(*this);
	if(dis==0) return Return_Normal;
	for(int i=0;i<GET_NUM_DIM;++i){
		double x=best.data()[i]-m_x[i];
		m_x[i]+=x/dis*visual*Global::msp_global->getRandFloat(0,1);
	}
	validate();
	return evaluate();
}
ReturnFlag NFish::swarm(const Solution<CodeVReal> & center,double visual){
	double dis=center.getDistance(*this);
	if(dis==0) return Return_Normal;
	for(int i=0;i<GET_NUM_DIM;++i){
		double x=center.data()[i]-m_x[i];
		m_x[i]+=x/dis*visual*Global::msp_global->getRandFloat(0,1);
	}
	validate();
	return evaluate();
}