#include "MQSOParticle.h"
#include "../../../Global/global.h"

MQSOParticle::MQSOParticle():Particle(),mv_repulse(GET_NUM_DIM){
}

MQSOParticle::~MQSOParticle(){
	mv_repulse.clear();
}

MQSOParticle::MQSOParticle(const MQSOParticle& other)
{
	mv_repulse.resize(GET_NUM_DIM);
    m_type=other.m_type;
	copy(other.mv_repulse.begin(),other.mv_repulse.end(),mv_repulse.begin());
}


void MQSOParticle::increaseDimension(){
    Particle::increaseDimension();
	mv_repulse.resize(GET_NUM_DIM);
}

void MQSOParticle::decreaseDimension(){
    Particle::decreaseDimension();
    mv_repulse.resize(GET_NUM_DIM);

}

ReturnFlag MQSOParticle::move(const Solution<CodeVReal> &lbest,double w, double c1, double c2,double r)
{
    double u,l;
	for( int j=0;j<GET_NUM_DIM;j++){
		double x=data()[j];
		if(m_type==PARTICLE_NEUTRAL){
            CAST_PROBLEM_CONT->getSearchRange(l,u,j);
            m_vel[j]=w*(m_vel[j]+c1*Global::msp_global->mp_uniformAlg->Next()*((m_pbest.data()[j])-x)+c2*Global::msp_global->mp_uniformAlg->Next()*((lbest.data()[j])-x));
            // note that, here uses the max vel to clamp the velocity for neutral particles even though not mentioned in the paper
            if(m_vel[j]>m_vMax[j].m_max)	m_vel[j]=m_vMax[j].m_max;
            else if(m_vel[j]<m_vMax[j].m_min)		m_vel[j]=m_vMax[j].m_min;
            data()[j]=x+m_vel[j];
        }else if(m_type==PARTICLE_QUANTUM){
			data()[j]= lbest.data()[j]+r*Global::msp_global->getRandFloat(-1,1);
        }else{
            CAST_PROBLEM_CONT->getSearchRange(l,u,j);
            m_vel[j]=w*(m_vel[j]+c1*Global::msp_global->mp_uniformAlg->Next()*((m_pbest.data()[j])-x)+c2*Global::msp_global->mp_uniformAlg->Next()*((lbest.data()[j])-x));
            m_vel[j]+=mv_repulse[j];
            // note that, here uses the search domain to clamp the velocity for charged particles
            if(fabs(m_vel[j])>u-l)	m_vel[j]=gSign(m_vel[j])*(u-l);

        }
    }
    self().validate();
	return self().evaluate();
}

void MQSOParticle::setType(ParticleType t){
    m_type=t;
}

